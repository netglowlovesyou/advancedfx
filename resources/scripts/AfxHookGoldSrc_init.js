// WE SUGGEST TO NOT EDIT THIS FILE!
// This file carries out CRUCIAL operations and we might change it occasionally.


////////////////////////////////////////////////////////////////////////////////
//
// Documentation (in HLAE source code)
//
// [1] doc/notes_goldsrc/debug_cstrike_crosshair.txt
// *[2] doc/notes_goldsrc/debug_cstrike_deathmessage.txt
// [3] doc/notes_goldsrc/debug_cstrike_smoke.txt
// *[4] doc/notes_goldsrc/debug_valve_UpdateSpectatorPanel.txt
// *[5] doc/notes_goldsrc/debug_engine_ifaces.txt
// *[6] doc/notes_goldsrc/debug_sound.txt
// *[7] doc/notes_goldsrc/debug_SCR_UpdateScreen.txt
// *[8] doc/notes_goldsrc/debug_Host_Frame.txt
// *[9] doc/notes_goldsrc/debug_ClientFunctionTable
// *[10] doc/notes_goldsrc/debug_CL_ParseServerMessage.txt
// *[11] doc/notes_goldsrc/debug_R_DrawWorld_and_sky.txt

////////////////////////////////////////////////////////////////////////////////
//
// Address system:

function afx_OnHwDllLoaded()
{
	//
	// Engine-to-client interfaces:
	//
	
	addr.p_cl_enginefuncs_s = addr.hwDll + 0x133260; // *[5]
	addr.p_playermove_s = addr.hwDll + 0x1002DE0; // *[5]
	addr.p_engine_studio_api_s = addr.hwDll + 0x14E840; // *[5]
	
	//
	// General engine hooks:
	//
	
	addr.Host_Frame = addr.hwDll +0x566B0; // *[8]
	addr.Host_Frame_DSZ = 0x05; // *[8]
	
	addr.CL_EmitEntities = addr.hwDll + 0x148D0; // *[8]
	addr.CL_EmitEntities_DSZ = 0x05; // *[8]
	
	addr.ClientFunctionTable = addr.hwDll +0x122B620; // *[9]
	
	//
	// Rendering related:
	//
	
	addr.UnkDrawHud = addr.hwDll + 0xBB50; // *[7]
	addr.UnkDrawHudCall = addr.hwDll + 0xB4AF0; // *[7]
	addr.UnkDrawHudCallAfter = addr.UnkDrawHudCall + 0x5; // *[7]
	
	addr.R_RenderView = addr.hwDll + 0x45890; // *[7]
	addr.DTOURSZ_R_RenderView = 0x6; // *[7]
	
	addr.R_DrawViewModel = addr.hwDll +0x43E90; // *[7]
	addr.DTOURSZ_R_DrawViewModel = 0x09; // *[7]
	
	addr.R_PolyBlend = addr.hwDll +0x44400; // *[7]
	addr.DTOURSZ_R_PolyBlend  = 0x09; // *[7]
	
	addr.r_refdef = addr.hwDll + 0xEC28E0; // *[7]
	
	addr.Mod_LeafPVS = addr.hwDll + 0x28600; // *[7]
	addr.DTOURSZ_Mod_LeafPVS = 0x0e; // *[7]
	
	addr.R_DrawEntitiesOnList = addr.hwDll + 0x43C50; // *[7]
	addr.DTOURSZ_R_DrawEntitiesOnList = 0x0c; // *[7]
	
	addr.R_DrawParticles = addr.hwDll + 0x7B4B0; // *[7]
	addr.DTOURSZ_R_DrawParticles = 0x08; // *[7]
	
	addr.R_DrawSkyBoxEx = addr.hwDll + 0x4F74E; // *[11]
	addr.R_DrawSkyBoxEx_DSZ = 0x09; // *[11]
	
	addr.skytextures = addr.hwDll + 0x63C664; // *[11]
	
	
	//
	// Sound system related:
	//
	
	addr.GetSoundtime = addr.hwDll + 0x8BFE0; // *[6]
	addr.DTOURSZ_GetSoundtime = 0x07; // *[6]
	
	addr.S_PaintChannels = addr.hwDll + 0x8DC60; // *[6]
	addr.DTOURSZ_S_PaintChannels = 0x06; // *[6]
	
	addr.paintedtime = addr.hwDll + 0xA27F60; // *[6]
	addr.shm = addr.hwDll + 0x6B43F0; // *[6]
	addr.soundtime = addr.hwDll + 0xA27F5C; // *[6]
	
	addr.paintbuffer = addr.hwDll + 0xA1DE20; // *[6]
	
	addr.S_TransferPaintBuffer = addr.hwDll + 0x8D580; // *[6]
	addr.DTOURSZ_S_TransferPaintBuffer = 0x06; // *[6]
	
	addr.SND_PickChannel = addr.hwDll + 0x8B200; // *[6]
	addr.DTOURSZ_SND_PickChannel = 0x06; // *[6]
	
	
	//
	// Demo parsing related:
	//
	
	addr.CL_ParseServerMessage_CmdRead = addr.hwDll + 0x1CEF3; // *[10]
	addr.CL_ParseServerMessage_CmdRead_DSZ = 0x07; // *[10]
	addr.msg_readcount = addr.hwDll + 0x1001028; // *[10]
	addr.net_message = addr.hwDll +0xA9B930 - 0x10; // *[10]
	
	//
	// other
	//
	
	// tfc DemoEdit campath fix:
	// currently not supported // addr.HudSpectator_tfc = addr.hlExe + 0x509A00;
	// currently not supported // addr.HudSpectator_cmp_tfc = addr.HudSpectator_tfc + 0x23;
	
	// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
	// currently not supported // addr.UpdateSpectatorPanel_checkjmp_tfc = addr.hlExe + 0x5439A1; // Team Fortress Classic (tfc)
}
events.onHwDllLoaded = afx_OnHwDllLoaded;
delete afx_OnHwDllLoaded;

/**
* @remarks Not called when no client.dll is loaded (valve, tfc).
*/
function afx_OnClientDllLoaded()
{
	//
	// game: cstrike
	//
	
	// cstrike CrossHair fix related:
	addr.cstrike_UnkCrosshairFn = addr.clientDll + 0x41800; // *[1]
	addr.cstrike_UnkCrosshairFn_DSZ = 0x09; // at least 8 bytes req. // *[1]
	addr.cstrike_UnkCrosshairFn_add_fac = addr.clientDll + 0xC32B8; // *[1]
	addr.cstrike_UnkCrosshairFn_mul_fac = addr.clientDll + 0xCD520; // *[1]
	
	// cstrike EV_CreateSmoke:
	addr.cstrike_EV_CreateSmoke = addr.clientDll + 0xA080; // *[3]
	addr.cstrike_EV_CreateSmoke_DSZ = 0x0a; // *[3]
	
	// cstrike DeathMsg related (client.dll offsets):
	addr.cstrike_MsgFunc_DeathMsg = addr.clientDll + 0x44610; // *[2]
	addr.cstrike_MsgFunc_DeathMsg_DSZ = 0x08; // *[2]
	addr.cstrike_CHudDeathNotice_MsgFunc_DeathMsg = addr.clientDll + 0x44AF0; // *[2]
	addr.cstrike_CHudDeathNotice_MsgFunc_DeathMsg_DSZ = 0x08; // at least 8 bytes req. // *[2]
	addr.cstrike_rgDeathNoticeList = addr.clientDll + 0x124D88; // *[2]
	addr.cstrike_CHudDeathNotice_Draw = addr.clientDll + 0x44770; // *[2]
	addr.cstrike_CHudDeathNotice_Draw_DSZ = 0x0a; // at least 8 bytes req. // *[2]
	
	//
	// other games
	//
	
	// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
	addr.UpdateSpectatorPanel_checkjmp_ag = addr.clientDll + 0x028091; // Adrenaline-Gamer (ag)
	addr.UpdateSpectatorPanel_checkjmp_ns = addr.clientDll + 0x033F3D; // Natuaral Selection (ns)
	addr.UpdateSpectatorPanel_checkjmp_valve = addr.clientDll + 0x4A6A4; // Half-Life (valve) // *[4]
}
events.onClientDllLoaded = afx_OnClientDllLoaded;
delete afx_OnClientDllLoaded;


////////////////////////////////////////////////////////////////////////////////
//
// Provide additional OpenGL helpers:

ogl.GL_TRUE = 1;
ogl.GL_FALSE = 0;
ogl.GL_COLOR_BUFFER_BIT = 0x00004000;
ogl.GL_DEPTH_BUFFER_BIT = 0x00000100;
ogl.GL_ACCUM_BUFFER_BIT = 0x00000200;
ogl.GL_STENCIL_BUFFER_BIT = 0x00000400;