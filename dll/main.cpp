/*
	Mirv Demo Tool

*/

#ifndef MDT_DEBUG
//#define MDT_DEBUG
#endif

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")
#pragma comment(lib,"GLaux.lib")

#include <windows.h>
#include <winbase.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"

#include "detours.h"
#include "filming.h"
#include "aiming.h"
#include "zooming.h"
#include "cmdregister.h"
#include "ui.h"

#include "mdt_gltools.h" // we want g_Mdt_GlTools for having tools to force Buffers and Stuff like that
#include "dd_hook.h" // we have to call functions (inGetProcAddress) from here in order to init the hook

#include "hl_addresses.h" // address definitions

#include <map>
#include <list>

extern Filming g_Filming;
extern Aiming g_Aiming;
extern Zooming g_Zooming;

extern UI *gui;

extern const char *pszFileVersion;

typedef std::list <Void_func_t> VoidFuncList;
VoidFuncList &GetCvarList()
{
	static VoidFuncList CvarList;
	return CvarList;
}
VoidFuncList &GetCmdList()
{
	static VoidFuncList CmdList;
	return CmdList;
}

void CvarRegister(Void_func_t func) { GetCvarList().push_front(func); }
void CmdRegister(Void_func_t func) { GetCmdList().push_front(func); }

// Todo - Ini files for these?
cl_enginefuncs_s* pEngfuncs		= (cl_enginefuncs_s*)	HL_ADDR_CL_ENGINEFUNCS_S;//0x01EA0A08;
engine_studio_api_s* pEngStudio	= (engine_studio_api_s*)HL_ADDR_ENGINE_SUTDIO_API_S;//0x01EBC978;
playermove_s* ppmove			= (playermove_s*)		HL_ADDR_PLAYERMOVE_S;//0x02D590A0;

int		g_nViewports = 0;
bool	g_bIsSucceedingViewport = false;
bool	g_bMenu = false;

bool	g_bEnumDMcalled = false;



//
//  Cvars
//


REGISTER_CVAR(disableautodirector, "0", 0);
REGISTER_CVAR(fixforcehltv, "1", 0);

REGISTER_DEBUGCMD_FUNC(enumdm_called)
{
	if (g_bEnumDMcalled) pEngfuncs->Con_Printf("YES, got called since last check.\n");
	else  pEngfuncs->Con_Printf("NO, did not get called since last check.\n");

	g_bEnumDMcalled = false;
}
REGISTER_DEBUGCVAR(gl_noclear, "0", 0);

//
// Commands
//

#if 0
REGISTER_CMD_FUNC(menu)
{
	g_bMenu = !g_bMenu;

	// If we leave the gui then reset the mouse position so view doesn't jump
	if (!g_bMenu)
		SetCursorPos(pEngfuncs->GetWindowCenterX(), pEngfuncs->GetWindowCenterY());
}
#endif

void ToggleMenu()
{
	g_bMenu = !g_bMenu;

	// If we leave the gui then reset the mouse position so view doesn't jump
	if (!g_bMenu)
		SetCursorPos(pEngfuncs->GetWindowCenterX(), pEngfuncs->GetWindowCenterY());
}


REGISTER_CMD_FUNC(whereami)
{
	float angles[3];
	pEngfuncs->GetViewAngles(angles);
	pEngfuncs->Con_Printf("Location: %fx %fy %fz\nAngles: %fx %fy %fz\n", ppmove->origin.x, ppmove->origin.y, ppmove->origin.z, angles[0], angles[1], angles[2]);
}

// _mirv_info - Print some informations into the console that might be usefull. when people want to report problems they should copy the console output of the command.
REGISTER_DEBUGCMD_FUNC(info)
{
	pEngfuncs->Con_Printf(">>>> >>>> >>>> >>>>\n");
	pEngfuncs->Con_Printf("MDT_DLL_VERSION: v%s (%s)\n", pszFileVersion, __DATE__);
	pEngfuncs->Con_Printf("GL_VENDOR: %s\n",glGetString(GL_VENDOR));
	pEngfuncs->Con_Printf("GL_RENDERER: %s\n",glGetString(GL_RENDERER));
	pEngfuncs->Con_Printf("GL_VERSION: %s\n",glGetString(GL_VERSION));
	pEngfuncs->Con_Printf("GL_EXTENSIONS: %s\n",glGetString(GL_EXTENSIONS));
	pEngfuncs->Con_Printf("<<<< <<<< <<<< <<<<\n");
}

REGISTER_DEBUGCMD_FUNC(forcebuffers)
{
	const char* cBType_AppDecides = "APP_DECIDES";
	
	if (pEngfuncs->Cmd_Argc() != 3)
	{
		// user didn't supply 2 arguments, so give him some info about the command:
		pEngfuncs->Con_Printf("Useage: " DEBUG_PREFIX "forcebuffers <readbuffer_type> <drawbuffer_type\n");

		const char* cCurReadBuf = cBType_AppDecides; // when forcing is off that means the app decides
		const char* cCurDrawBuf = cBType_AppDecides; // .

		if (g_Mdt_GlTools.m_bForceReadBuff) cCurReadBuf = g_Mdt_GlTools.GetReadBufferStr();
		if (g_Mdt_GlTools.m_bForceDrawBuff) cCurDrawBuf = g_Mdt_GlTools.GetDrawBufferStr();

		pEngfuncs->Con_Printf("Current: " DEBUG_PREFIX "forcebuffers %s %s\n",cCurReadBuf,cCurDrawBuf);
		pEngfuncs->Con_Printf("Available Types: %s",cBType_AppDecides); // also add APP_DECIDES

		for (int i=0;i<SIZE_Mdt_GlTools_GlBuffs;i++)
		{
			const char* cBuffType=cMdt_GlTools_GlBuffStrings[i];
			pEngfuncs->Con_Printf(", %s",cBuffType); // is not first so add comma

		}

		pEngfuncs->Con_Printf("\n");
	} else {
		// user supplied 2 argument's try to set it

		const char* cReadBuffStr = pEngfuncs->Cmd_Argv(1);
		const char* cDrawBuffStr = pEngfuncs->Cmd_Argv(2);

		// the following code checks for each buffer if the user wants either let app decide (then it turns force off) or if he wants to set a specific type (on success it turns force on and otherwise prints an error):
		// ReadBuffer (for reading color buffers from GL):
		if (!stricmp(cBType_AppDecides,cReadBuffStr)) g_Mdt_GlTools.m_bForceReadBuff=false;
		else if (g_Mdt_GlTools.SetReadBufferFromStr(cReadBuffStr))  g_Mdt_GlTools.m_bForceReadBuff=true;
		else pEngfuncs->Con_Printf("Error: Failed to set ReadBuffer, supplied buffer type not valid.\n");
		// DrawBuffer (for definig the target GL color buffer for pixel writing functions etc.):
		if (!stricmp(cBType_AppDecides,cDrawBuffStr)) g_Mdt_GlTools.m_bForceDrawBuff=false;
		else if (g_Mdt_GlTools.SetDrawBufferFromStr(cDrawBuffStr))  g_Mdt_GlTools.m_bForceDrawBuff=true;
		else pEngfuncs->Con_Printf("Error: Failed to set DrawBuffer, supplied buffer type not valid.\n");
	}



	return;
}

REGISTER_DEBUGCVAR(deltatime, "1.0", 0);

void DrawActivePlayers()
{
	for (int i = 0; i <= pEngfuncs->GetMaxClients(); i++)
	{
		cl_entity_t *e = pEngfuncs->GetEntityByIndex(i);

		if (e && e->player && e->model && !(e->curstate.effects & EF_NODRAW))
		{
			float flDeltaTime = fabs(pEngfuncs->GetClientTime() - e->curstate.msg_time);

			if (flDeltaTime < deltatime->value)
				pEngfuncs->CL_CreateVisibleEntity(ET_PLAYER, e);
		}
	}
}

bool InMenu()
{
	// TODO - CHECK THAT WE'RE NOT IN MAIN MENU SCREEN
	return g_bMenu;
}

//
//
//

WNDPROC pWndProc;

LRESULT APIENTRY my_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int old_wParam = 0;

	if (InMenu())
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				ToggleMenu();
				return 0;
			}
			return 0;

		case WM_LBUTTONDOWN:
			gui->MouseDown(MOUSE_BUTTON1);
			return 0;

		case WM_LBUTTONUP:
			gui->MouseUp(MOUSE_BUTTON1);
			return 0;

		case WM_RBUTTONDOWN:
			gui->MouseDown(MOUSE_BUTTON2);
			return 0;

		case WM_RBUTTONUP:
			gui->MouseUp(MOUSE_BUTTON2);
			return 0;

		case WM_MOUSEMOVE:
			if ((old_wParam & MK_LBUTTON) ^ (wParam & MK_LBUTTON))
				(wParam & MK_LBUTTON) ? gui->MouseDown(MOUSE_BUTTON1) : gui->MouseUp(MOUSE_BUTTON1);
			if ((old_wParam & MK_RBUTTON) ^ (wParam & MK_RBUTTON))
				(wParam & MK_LBUTTON) ? gui->MouseDown(MOUSE_BUTTON1) : gui->MouseUp(MOUSE_BUTTON1);

			old_wParam = wParam;
			return 0;
		}
	}

	return CallWindowProc(pWndProc, hwnd, uMsg, wParam, lParam);
}

//
//	OpenGl Hooking
//

void APIENTRY my_glBegin(GLenum mode)
{
	if (g_Filming.doWireframe(mode) == Filming::DR_HIDE)
		return;

	if (!g_Filming.isFilming())
	{
		glBegin(mode);
		return;
	}

	Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

	if (res == Filming::DR_HIDE)
		return;

	else if (res == Filming::DR_MASK)
		glColorMask(FALSE, FALSE, FALSE, TRUE);

	else
		glColorMask(TRUE, TRUE, TRUE, TRUE);

	glBegin(mode);
}

void APIENTRY my_glClear(GLbitfield mask)
{
	if (gl_noclear->value)
		return;
	
	// check if we want to clear (it also might set clearcolor and stuff like that):
	if (!g_Filming.checkClear(mask))
		return;

	glClear(mask);
}

SCREENINFO screeninfo;

void APIENTRY my_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	static bool bFirstRun = true;

	g_bIsSucceedingViewport = true;

	if (bFirstRun)
	{
		// Register the commands
		std::list<Void_func_t>::iterator i = GetCmdList().begin();
		while (i != GetCmdList().end())
			(*i++)();

		// Register the cvars
		i = GetCvarList().begin();
		while (i != GetCvarList().end())
			(*i++)();

		pEngfuncs->Con_Printf("Mirv Demo Tool v%s (%s) Loaded\nBy Mirvin_Monkey 02/05/2004\n\n", pszFileVersion, __DATE__);

		gui->Initialise();

		// TODO - use this for buffer size
		screeninfo.iSize = sizeof(SCREENINFO);
		pEngfuncs->pfnGetScreenInfo(&screeninfo);
		pEngfuncs->Con_DPrintf("%d %d %d %d\n", screeninfo.iWidth, screeninfo.iHeight, (int) screeninfo.charWidths, screeninfo.iCharHeight);

		g_Filming.setScreenSize(screeninfo.iWidth,screeninfo.iHeight);

		bFirstRun = false;
	}


	// Only on the first viewport
	if (g_nViewports == 0)
	{
		//g_Filming.setScreenSize(width, height);

		// Make sure we can see the local player if dem_forcehltv is on
		// dem_forcehtlv is not a cvar, so don't bother checking
		if (fixforcehltv->value != 0.0f && pEngfuncs->IsSpectateOnly() && ppmove->iuser1 != 4)
			DrawActivePlayers();

		// Always get rid of auto_director
		if (disableautodirector->value != 0.0f)
			pEngfuncs->Cvar_SetValue("spec_autodirector", 0.0f);

		// This is called whether we're zooming or not
		g_Zooming.handleZoom();

		// this is now done in doCapturePoint() called in swap
		//if (g_Filming.isFilming())
		//	g_Filming.recordBuffers();

		if (g_Aiming.isAiming())
			g_Aiming.aim();
	}

	// Not necessarily 5 viewports anymore, keep counting until reset
	// by swapbuffers hook.
	g_nViewports++;

	g_Zooming.adjustViewportParams(x, y, width, height);
	glViewport(x, y, width, height);

}

void APIENTRY my_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	g_Zooming.adjustFrustumParams(left, right, top, bottom);
	glFrustum(left, right, bottom, top, zNear, zFar);
}

//
// Hooking
//

#pragma warning(disable: 4312)
#pragma warning(disable: 4311)
#define MakePtr(cast, ptr, addValue) (cast)((DWORD)(ptr) + (DWORD)(addValue))

void *InterceptDllCall(HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect;
	DWORD dwOldProtect2;
	void *pOldFunction;

	if (!(pOldFunction = GetProcAddress(GetModuleHandle(szDllName), szFunctionName)))
		return 0;

	pDosHeader = (PIMAGE_DOS_HEADER) hModule;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE
	|| (pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)) == (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader)
		return NULL;

	while (pImportDesc->Name)
	{
		char *szModuleName = MakePtr(char *, pDosHeader, pImportDesc->Name);
		if (!stricmp(szModuleName, szDllName))
			break;
		pImportDesc++;
	}
	if (pImportDesc->Name == NULL)
		return NULL;

	pThunk = MakePtr(PIMAGE_THUNK_DATA, pDosHeader,	pImportDesc->FirstThunk);
	while (pThunk->u1.Function)
	{
		if (pThunk->u1.Function == (DWORD)pOldFunction)
		{
			VirtualProtect((void *) &pThunk->u1.Function, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect);
			pThunk->u1.Function = (DWORD) pNewFunction;
			VirtualProtect((void *) &pThunk->u1.Function, sizeof(DWORD), dwOldProtect, &dwOldProtect2);
			return pOldFunction;
		}
		pThunk++;
	}
	return NULL;
}

BOOL (APIENTRY *pwglSwapBuffers)(HDC hDC);
BOOL APIENTRY my_wglSwapBuffers(HDC hDC)
{
	static bool bHaveWindowHandle = false;

	if (!bHaveWindowHandle && hDC != 0)
	{
		HWND hWnd = WindowFromDC(hDC);
		pWndProc = (WNDPROC) SetWindowLong(hWnd, GWL_WNDPROC, (long) my_WndProc);
		bHaveWindowHandle = true;
	}

	// Next viewport will be the first of the new frame
	g_nViewports = 0;

	if (g_Filming.isFilming())
	{
		// we are filming, force buffers and capture our image:
		
		// save current buffers:
		g_Mdt_GlTools.SaveDrawBuffer();
		g_Mdt_GlTools.SaveReadBuffer();

		// force selected buffers(if any):
		g_Mdt_GlTools.AdjustReadBuffer();
		g_Mdt_GlTools.AdjustDrawBuffer();
		
		// record the selected buffer (capture):
		g_Filming.recordBuffers();
	}

	// Obviously use 
	if (InMenu())
	{
		gui->Update(pEngfuncs->GetClientTime());
		gui->Render(screeninfo.iWidth, screeninfo.iHeight);
	}

	// do the switching of buffers as requersted:
	bool bResWglSwapBuffers = (*pwglSwapBuffers)(hDC);

	// no we have captured the image (by default from backbuffer) and display it on the front, now we can prepare the new backbuffer image if required.

	if (g_Filming.isFilming())
	{
		// we are filming, do required clearing and restore buffers:

		// carry out preparerations on the backbuffer for the next frame:
		g_Filming.clearBuffers();

		// restore saved buffers:
		g_Mdt_GlTools.AdjustDrawBuffer(g_Mdt_GlTools.m_iSavedDrawBuff,false);
		g_Mdt_GlTools.AdjustReadBuffer(g_Mdt_GlTools.m_iSavedReadBuff,false);
	}

	return bResWglSwapBuffers;
}



// Don't let HL reset the cursor position if we're in our own gui
BOOL APIENTRY my_SetCursorPos(int x, int y)
{
	if (InMenu())
		return 1;

	return SetCursorPos(x, y);
}

// Get the mouse position for our menu and tell HL that the mouse is in the centre
// of the screen (to stop player from spinning while in menu)
BOOL APIENTRY my_GetCursorPos(LPPOINT lpPoint)
{
	if (!InMenu())
		return GetCursorPos(lpPoint);

	BOOL bRet = GetCursorPos(lpPoint);

	if (!bRet)
		return FALSE;

	int iMidx = pEngfuncs->GetWindowCenterX();
	int iMidy = pEngfuncs->GetWindowCenterY();

	gui->UpdateMouse((int) lpPoint->x - (iMidx - 800 / 2),
					 (int) lpPoint->y - (iMidy - 600 / 2));

	lpPoint->x = (long) iMidx;
	lpPoint->y = (long) iMidy;
	return TRUE;
}

FARPROC (WINAPI *pGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
FARPROC WINAPI newGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);
	if (HIWORD(lpProcName))
	{
		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &newGetProcAddress;

		if (!lstrcmp(lpProcName, "glBegin"))
			return (FARPROC) &my_glBegin;
		if (!lstrcmp(lpProcName, "glViewport"))
			return (FARPROC) &my_glViewport;
		if (!lstrcmp(lpProcName, "glClear"))
			return (FARPROC) &my_glClear;
		if (!lstrcmp(lpProcName, "glFrustum"))
			return (FARPROC) &my_glFrustum;
		if (!lstrcmp(lpProcName, "GetCursorPos"))
			return (FARPROC) &my_GetCursorPos;
		if (!lstrcmp(lpProcName, "SetCursorPos"))
			return (FARPROC) &my_SetCursorPos;
		if (!lstrcmp(lpProcName, "wglSwapBuffers"))
		{
			pwglSwapBuffers = (BOOL (APIENTRY *)(HDC hDC)) nResult;
			return (FARPROC) &my_wglSwapBuffers;
		}

		/*// some test infos:
		char tstString[10];
		int nLen =  strlen("Direct");;
		int lLen = strlen(lpProcName);
		if (lLen<nLen) nLen=lLen;
		memcpy(tstString,lpProcName,nLen);
		tstString[nLen+1]=0;

		if (!lstrcmpi("Direct",tstString))
		{
			//g_bEnumDMcalled = true;
			MessageBox(NULL,lpProcName,"MDT DLL Info",MB_OK);
			//return nResult;
		}*/

		if (!lstrcmp(lpProcName,"DirectDrawCreate"))
			return Hook_DirectDrawCreate(nResult); // give our hook original address and return new (it remembers the original one from it's first call, it also cares about the commandline options (if to force the res or not and does not install the hook if not needed))
	}

	return nResult;
}

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
			pGetProcAddress = (FARPROC(WINAPI *)(HMODULE, LPCSTR)) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress);
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
	}
	return true;
}
