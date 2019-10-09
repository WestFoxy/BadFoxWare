#include "sdk.hpp"
#include "../Helpers/Utils.hpp"
#include "csgostructs.hpp"
#include "../hooks.hpp"
#include "../features/CPanel.h"
#include "../Utils_fox.h"
#include "../GameEvents.h"
#include "../Bulletshow.h"
#include "../XorStr.h"

IMemAlloc*            g_MemAlloc = nullptr;
IVEngineClient*       g_EngineClient   = nullptr;
IBaseClientDLL*       g_CHLClient      = nullptr;
IClientEntityList*    g_EntityList     = nullptr;
CGlobalVarsBase*      g_GlobalVars     = nullptr;
IEngineTrace*         g_EngineTrace    = nullptr;
ICvar*                g_CVar           = nullptr;
IPanel*               g_VGuiPanel      = nullptr;
IClientMode*          g_ClientMode     = nullptr;
IVDebugOverlay*       g_DebugOverlay   = nullptr;
ISurface*             g_VGuiSurface    = nullptr;
CInput*               g_Input          = nullptr;
IVModelInfoClient*    g_MdlInfo        = nullptr;
IVModelRender*        g_MdlRender      = nullptr;
IVRenderView*         g_RenderView     = nullptr;
IMaterialSystem*      g_MatSystem      = nullptr;
IGameEventManager2*   g_GameEvents     = nullptr;
IMoveHelper*          g_MoveHelper     = nullptr;
IMDLCache*            g_MdlCache       = nullptr;
IPrediction*          g_Prediction     = nullptr;
CGameMovement*        g_GameMovement   = nullptr;
IEngineSound*         g_EngineSound    = nullptr;
CGlowObjectManager*   g_GlowObjManager = nullptr;
IViewRender*          g_ViewRender     = nullptr;
//IViewRenderBeams*     g_RenderBeams    = nullptr;
IDirect3DDevice9*     g_D3DDevice9     = nullptr;
CClientState*         g_ClientState    = nullptr;
IPhysicsSurfaceProps* g_PhysSurface    = nullptr;
ILocalize*            g_Localize       = nullptr;

C_BasePlayer*         g_LocalPlayer;

namespace Interfaces
{
    CreateInterfaceFn get_module_factory(HMODULE module)
    {
        return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, XorStr("CreateInterface")));
    }

    template<typename T>
    T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion)
    {
        auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

        if(!result) {
            throw std::runtime_error(std::string("Failed intf") + szInterfaceVersion);
        }

        return result;
    }
#define RandomIntF(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin)
    void Initialize()
    {
        auto engineFactory    = get_module_factory(GetModuleHandleW(L"engine.dll"));
        auto clientFactory    = get_module_factory(GetModuleHandleW(L"client_panorama.dll"));
        auto valveStdFactory  = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
        auto vguiFactory      = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
        auto vgui2Factory     = get_module_factory(GetModuleHandleW(L"vgui2.dll"));
        auto matSysFactory    = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
        auto dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
        auto vphysicsFactory  = get_module_factory(GetModuleHandleW(L"vphysics.dll"));
		auto localizeFactory = get_module_factory(GetModuleHandleW(L"localize.dll"));
		auto matchmakingFactory = get_module_factory(GetModuleHandleW(L"matchmaking.dll"));

        
        g_CHLClient           = get_interface<IBaseClientDLL>      (clientFactory   , XorStr("VClient018"));
        g_EntityList          = get_interface<IClientEntityList>   (clientFactory   , XorStr("VClientEntityList003"));
        g_Prediction          = get_interface<IPrediction>         (clientFactory   , XorStr("VClientPrediction001"));
        g_GameMovement        = get_interface<CGameMovement>       (clientFactory   , XorStr("GameMovement001"));
        g_MdlCache            = get_interface<IMDLCache>           (dataCacheFactory, XorStr("MDLCache004"));
        g_EngineClient        = get_interface<IVEngineClient>      (engineFactory   , XorStr("VEngineClient014"));
        g_MdlInfo             = get_interface<IVModelInfoClient>   (engineFactory   , XorStr("VModelInfoClient004"));
        g_MdlRender           = get_interface<IVModelRender>       (engineFactory   , XorStr("VEngineModel016"));
		g_pEffects			  =	get_interface<IVEffects>		   (engineFactory   , XorStr("VEngineEffects001"));
        g_RenderView          = get_interface<IVRenderView>        (engineFactory   , XorStr("VEngineRenderView014"));
        g_EngineTrace         = get_interface<IEngineTrace>        (engineFactory   , XorStr("EngineTraceClient004"));
        g_DebugOverlay        = get_interface<IVDebugOverlay>      (engineFactory   , XorStr("VDebugOverlay004"));
        g_GameEvents          = get_interface<IGameEventManager2>  (engineFactory   , XorStr("GAMEEVENTSMANAGER002"));
        g_EngineSound         = get_interface<IEngineSound>        (engineFactory   , XorStr("IEngineSoundClient003"));
        g_MatSystem           = get_interface<IMaterialSystem>     (matSysFactory   , XorStr("VMaterialSystem080"));
        g_CVar                = get_interface<ICvar>               (valveStdFactory , XorStr("VEngineCvar007"));
        g_VGuiPanel           = get_interface<IPanel>              (vgui2Factory    , XorStr("VGUI_Panel009"));
        g_VGuiSurface         = get_interface<ISurface>            (vguiFactory     , XorStr("VGUI_Surface031"));
        g_PhysSurface         = get_interface<IPhysicsSurfaceProps>(vphysicsFactory , XorStr("VPhysicsSurfaceProps001"));
		g_Localize            = get_interface<ILocalize>           (localizeFactory, XorStr("Localize_001"));
		g_pFEngineEffects = get_interface<IFEffects>(clientFactory, XorStr("IEffects001"));
		g_MemAlloc = *(IMemAlloc**)(GetProcAddress(GetModuleHandle("tier0.dll"), XorStr("g_pMemAlloc")));
		
		g_MatchFramework = get_interface<CMatchFramework>(matchmakingFactory, XorStr("MATCHFRAMEWORK_001"));

        auto client = GetModuleHandleW(L"client_panorama.dll");
        auto engine = GetModuleHandleW(L"engine.dll");
        auto dx9api = GetModuleHandleW(L"shaderapidx9.dll");


		g_GlobalVars = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(g_CHLClient))[0] + 0x1Bu);
        //g_GlobalVars      =  **(CGlobalVarsBase***)(Utils::PatternScan(client, "A1 ? ? ? ? 5E 8B 40 10") + 1);
        //g_ClientMode      =        *(IClientMode**)(Utils::PatternScan(client, "A1 ? ? ? ? 8B 80 ? ? ? ? 5D") + 1);
		g_ClientMode = **reinterpret_cast<IClientMode***>    ((*reinterpret_cast<uintptr_t**>(g_CHLClient))[10] + 0x5u);
        g_Input           =  *(CInput**)(Utils::PatternScan(client, XorStr("B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85")) + 1);
        g_MoveHelper      =      **(IMoveHelper***)(Utils::PatternScan(client, XorStr("8B 0D ? ? ? ? 8B 46 08 68")) + 2);
        g_GlowObjManager  = *(CGlowObjectManager**)(Utils::PatternScan(client, XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 3);
        g_ViewRender      =        *(IViewRender**)(Utils::PatternScan(client, XorStr("A1 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? FF 10")) + 1);
		g_pViewRenderBeams = *(IViewRenderBeams**)  (Utils::PatternScan (client, XorStr("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 1); //  "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08"
        g_D3DDevice9      = **(IDirect3DDevice9***)(Utils::PatternScan(dx9api, XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
        g_ClientState     =     **(CClientState***)(Utils::PatternScan(engine, XorStr("A1 ? ? ? ? 8B 80 ? ? ? ? C3")) + 1);

        g_LocalPlayer     = (C_BasePlayer*)(Utils::PatternScan(client, XorStr("8B 0D ? ? ? ? 83 FF FF 74 07")) + 2);

		g_SmokeHitListener = new FoxEventListener(XorStr("smokegrenade_detonate"));
		g_MolotovHitListener = new FoxEventListener(XorStr("molotov_detonate"));
		g_HeGrenadeListener = new FoxEventListener(XorStr("hegrenade_detonate"));
		
		

		
		//g_EngineClient->ExecuteClientCmd("clear");
		/*for (int i = 0; i < 50; i++)
		{
			g_CVar->ConsoleColorPrintf(Color(255, 55, 0, 255), "************************************************************\n");
		}
		g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "LISA INITIALISED");*/
    }

    void Dump()
    {
        // Ugly macros ugh
       /* #define STRINGIFY_IMPL(s) #s
        #define STRINGIFY(s)      STRINGIFY_IMPL(s)
        #define PRINT_INTERFACE(name) Utils::ConsolePrint("%-20s: %p\n", STRINGIFY(name), name)

        PRINT_INTERFACE(g_CHLClient   );
        PRINT_INTERFACE(g_EntityList  );
        PRINT_INTERFACE(g_Prediction  );
        PRINT_INTERFACE(g_GameMovement);
        PRINT_INTERFACE(g_MdlCache    );
        PRINT_INTERFACE(g_EngineClient);
        PRINT_INTERFACE(g_MdlInfo     );
        PRINT_INTERFACE(g_MdlRender   );
        PRINT_INTERFACE(g_RenderView  );
        PRINT_INTERFACE(g_EngineTrace );
        PRINT_INTERFACE(g_DebugOverlay);
        PRINT_INTERFACE(g_GameEvents  );
        PRINT_INTERFACE(g_EngineSound );
        PRINT_INTERFACE(g_MatSystem   );
        PRINT_INTERFACE(g_CVar        );
        PRINT_INTERFACE(g_VGuiPanel   );
        PRINT_INTERFACE(g_VGuiSurface );
        PRINT_INTERFACE(g_PhysSurface );
		PRINT_INTERFACE(g_MatchFramework);*/
    }
}