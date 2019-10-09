#pragma once
#include "MainInclude.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>
#include "menu.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "Hitmarker.h"
#include "menu.hpp"
#include "LagComp.h"
#include "menu.hpp"
#include "hooks.hpp"
#include "imgui\directx9\imgui_impl_dx9.h"
#include "resource_system.h"
#include "panorama_resource.h"
#include <iosfwd>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <xstddef>
#include <crtdbg.h>
#include <random>
#include <Psapi.h>
#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

namespace Global
{
	extern char my_documents_folder[MAX_PATH];
	extern QAngle visualAngles;
	extern float hitmarkerAlpha;
	extern CUserCmd *userCMD;
	extern bool bSendPacket;
	extern int chokedticks;
}
namespace index
{
    constexpr auto EndScene                 = 42;
    constexpr auto Reset                    = 16;
    constexpr auto PaintTraverse            = 41;
    constexpr auto CreateMove               = 22;
    constexpr auto PlaySound                = 82;
    constexpr auto FrameStageNotify         = 37;
    constexpr auto DrawModelExecute         = 21;
    constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool          = 13;
	constexpr auto OverrideView             = 18;
	constexpr auto LockCursor               = 67;
	constexpr auto SceneEnd                 =  9;
	constexpr auto UnlockCursor             = 66;
	constexpr auto BeginFrame               = 42;
	constexpr auto FindMDL                  = 10;
	constexpr auto FireEventClientSide      = 9;        
}
struct StartSoundParams_t // apparently outdated thank sen66 for pointing it out.
{
	bool			staticsound;
	int			userdata;
	int			soundsource;
	int			entchannel;
	void		*pSfx;
	Vector		origin;
	Vector		direction;
	bool			bUpdatePositions;
	float			fvol;
	soundlevel_t	soundlevel;
	int			flags;
	int			pitch;
	bool			fromserver;
	float			delay;
	int			speakerentity;
	bool			suppressrecording;
	int			initialStreamPosition;
};

namespace Hooks
{
    void Initialize();
    void Shutdown();

    extern vfunc_hook hlclient_hook;
    extern vfunc_hook direct3d_hook;
    extern vfunc_hook vguipanel_hook;
    extern vfunc_hook vguisurf_hook;
    extern vfunc_hook mdlrender_hook;
	extern vfunc_hook enginesound_hook;
    //extern vfunc_hook viewrender_hook;


	using EndScene = long(__stdcall *)(IDirect3DDevice9*);
    using Reset               = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
    using CreateMove          = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
    using PaintTraverse       = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
    using FrameStageNotify    = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
    using PlaySound           = void(__thiscall*)(ISurface*, const char* name);
    using DrawModelExecute    = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
    using FireEvent           = bool(__thiscall*)(IGameEventManager2*, IGameEvent* pEvent);
    using DoPostScreenEffects = int(__thiscall*)(IClientMode*, int);
	using OverrideView = void(__thiscall*)(IClientMode*, CViewSetup*);
	using SceneEnd_t = void(__thiscall*)(void*);
	typedef void(__fastcall* LockCursor_t) (ISurface*, void*);
	typedef bool(__thiscall* IsPlayingTimeDemo_t) (IVEngineClient*);



	typedef void(__fastcall* hkEmitSound_t) (void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, StartSoundParams_t& params);
	void __fastcall hkEmitSound(void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, StartSoundParams_t& params);
    long __stdcall hkEndScene(IDirect3DDevice9* device);
    long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
    void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active);
    void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	//bool __fastcall netgraphtext_getbool(void* ecx, void* edx);
    void __stdcall hkPlaySound(const char* name);
    void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    void __stdcall hkFrameStageNotify(ClientFrameStage_t stage);
	void __stdcall hkOverrideView(CViewSetup * vsView);
    int  __stdcall hkDoPostScreenEffects(int a1);
	void __fastcall hkSceneEnd(void* thisptr, void* edx);
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx);
	void __fastcall hkLockCursor(ISurface* thisptr, void* edx);
	bool __fastcall IsPlayingTimeDemo(IVEngineClient* ecx, void* edx);
	bool __stdcall HookedCreateMove2(float SampleTime, CUserCmd* UserCmd);
	bool __fastcall Hooked_FireEventClientSide(void* thisptr, void* edx, IGameEvent* event);
}

