#include "stdafx.h"

#include "d3d9Hooks.h"

#include <hooks/shared/detours.h>

typedef struct __declspec(novtable) Interface_s abstract {} * Interface_t;
typedef void * (__stdcall Interface_s::*InterfaceFn_t) (void *);
#define IFACE_PASSTHROUGH(iface,method,ifacePtr) \
	__pragma(warning( suppress : 4731 )) \
	virtual void __stdcall iface ##method(void) \
	{ \
		static InterfaceFn_t fn = (InterfaceFn_t)&iface::method; \
		{ \
		__asm POP EBP \
		__asm MOV EAX, ifacePtr \
		__asm MOV [ESP+4], EAX \
		__asm MOV EAX, fn \
		__asm JMP EAX \
		} \
	}

IDirect3DDevice9 * g_OldDirect3DDevice9;
struct NewDirect3DDevice9
{
    /*** IUnknown methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9, QueryInterface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, AddRef, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, Release, g_OldDirect3DDevice9);

    /*** IDirect3DDevice9 methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9, TestCooperativeLevel, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetAvailableTextureMem, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, EvictManagedResources, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDirect3D, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDeviceCaps, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDisplayMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetCreationParameters, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetCursorProperties, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetCursorPosition, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ShowCursor, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateAdditionalSwapChain, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetSwapChain, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetNumberOfSwapChains, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, Reset, g_OldDirect3DDevice9);
    
	STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
	{
		return g_OldDirect3DDevice9->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

    IFACE_PASSTHROUGH(IDirect3DDevice9, GetBackBuffer, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRasterStatus, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetDialogBoxMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetGammaRamp, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetGammaRamp, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVolumeTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateCubeTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVertexBuffer, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateIndexBuffer, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateRenderTarget, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateDepthStencilSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, UpdateSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, UpdateTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRenderTargetData, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetFrontBufferData, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, StretchRect, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ColorFill, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateOffscreenPlainSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetRenderTarget, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRenderTarget, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetDepthStencilSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDepthStencilSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, BeginScene, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, EndScene, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, Clear, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetTransform, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetTransform, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, MultiplyTransform, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetViewport, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetViewport, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetMaterial, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetMaterial, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetLight, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetLight, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, LightEnable, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetLightEnable, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetClipPlane, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetClipPlane, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetRenderState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRenderState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateStateBlock, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, BeginStateBlock, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, EndStateBlock, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetClipStatus, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetClipStatus, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetTextureStageState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetTextureStageState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetSamplerState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetSamplerState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ValidateDevice, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetPaletteEntries, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetPaletteEntries, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetCurrentTexturePalette, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetCurrentTexturePalette, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetScissorRect, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetScissorRect, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetSoftwareVertexProcessing, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetSoftwareVertexProcessing, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetNPatchMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetNPatchMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawPrimitive, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawIndexedPrimitive, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawPrimitiveUP, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawIndexedPrimitiveUP, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ProcessVertices, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVertexDeclaration, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetVertexDeclaration, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexDeclaration, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetFVF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetFVF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVertexShader, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetVertexShader, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShader, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetVertexShaderConstantF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShaderConstantF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetVertexShaderConstantI, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShaderConstantI, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetVertexShaderConstantB, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShaderConstantB, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetStreamSource, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetStreamSource, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetStreamSourceFreq, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetStreamSourceFreq, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetIndices, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetIndices, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreatePixelShader, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetPixelShader, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShader, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetPixelShaderConstantF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShaderConstantF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetPixelShaderConstantI, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShaderConstantI, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetPixelShaderConstantB, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShaderConstantB, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawRectPatch, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawTriPatch, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DeletePatch, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateQuery, g_OldDirect3DDevice9);
} g_NewDirect3DDevice9;


IDirect3D9 * g_OldDirect3D9;
struct NewDirect3D9
{
    /*** IUnknown methods ***/
	IFACE_PASSTHROUGH(IDirect3D9, QueryInterface, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, AddRef, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, Release, g_OldDirect3D9);

    /*** IDirect3D9 methods ***/
	IFACE_PASSTHROUGH(IDirect3D9, RegisterSoftwareDevice, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterCount, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterIdentifier, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterModeCount, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, EnumAdapterModes, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterDisplayMode, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceType, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceFormat, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceMultiSampleType, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDepthStencilMatch, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceFormatConversion, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetDeviceCaps, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterMonitor, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CreateDevice, g_OldDirect3D9);

    STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
	{
		HRESULT hRet = g_OldDirect3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
		if(ppReturnedDeviceInterface)
		{
			g_OldDirect3DDevice9 = *ppReturnedDeviceInterface;
			*ppReturnedDeviceInterface = reinterpret_cast<IDirect3DDevice9 *>(&g_NewDirect3DDevice9);
		}
		return hRet;
	}

} g_NewDirect3D9;


Direct3DCreate9_t old_Direct3DCreate9;
IDirect3D9 * WINAPI new_Direct3DCreate9(UINT SDKVersion)
{
	if(D3D_SDK_VERSION == SDKVersion)
	{
		g_OldDirect3D9 = old_Direct3DCreate9(SDKVersion);
		return reinterpret_cast<IDirect3D9 *>(&g_NewDirect3D9);
	}

	return old_Direct3DCreate9(SDKVersion);
}
