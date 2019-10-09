#include "hooks.hpp"
#include <intrin.h>  
#include "SkinChanger.h"

#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "features/visuals.hpp"
#include "LagComp.h"
#include "options.hpp"
#include "valve_sdk\sdk.hpp"
#include "LagComp.h"
#include "features\GrenadeTra.hpp"
#include "features\NightMode.hpp"`
#include "features\Trig.hpp"
#include "features\LBOT.hpp"
#include "AA.h"

#include "helpers\VMT.h"
#include "FoxSystem.h"
#include "Rage.h"
#include "EnginePrediction.h"
#include "Utils_fox.h"
#include "Proxies.h"
#include <d3dx9.h>
#include <d3dx9core.h>
#include "AWMenu.h"
#include "FoxyMenu.h"
#include "Misc.h"
#include "GrenadesEsp.h"
#include "Bulletshow.h"
#include "AntiAim.h"
#include "imgdata.h"
#include "FakeWalk.h"
#include "FoxEvents.h"
#include "Resolver.h"
#include "ServerSounds.h"
#include "ValveMath.h"
#include "XorStr.h"

#pragma comment(lib, "d3dx9.lib")

#include <Psapi.h>
#pragma intrinsic(_ReturnAddress)  
ConVar* r_DrawSpecificStaticProp;
#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2	
extern unsigned long esp_font;


int DebugFovValue = 0;

namespace Global
{
	char my_documents_folder[MAX_PATH];
	QAngle visualAngles;
	float hitmarkerAlpha;
	CUserCmd *userCMD;
	bool bSendPacket;
	int chokedticks;
}
namespace Hooks
{
	vfunc_hook hlclient_hook;
	//vfunc_hook clientmode_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook clientmode_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook render_view;
	vfunc_hook sv_cheats;
	vfunc_hook gameevents_hook;
	vfunc_hook engineclient_hook;
	vfunc_hook enginesound_hook;

	RecvProp* sequence_hook;
	void Initialize()
	{
		engineclient_hook.setup(g_EngineClient, XorStr("engine.dll"));
		engineclient_hook.hook_index(84, IsPlayingTimeDemo);
		
		hlclient_hook.setup(g_CHLClient, XorStr("client_panorama.dll"));
		direct3d_hook.setup(g_D3DDevice9, XorStr("shaderapidx9.dll"));
		enginesound_hook.setup(g_EngineSound, XorStr("engine.dll"));
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		gameevents_hook.setup(g_GameEvents);
		render_view.setup(g_RenderView);
		clientmode_hook.setup(g_ClientMode, XorStr("client_panorama.dll"));
		ConVar* sv_cheats_con = g_CVar->FindVar(XorStr("sv_cheats"));
		sv_cheats.setup(sv_cheats_con);
		render_view.hook_index(index::SceneEnd, hkSceneEnd);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);

		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		clientmode_hook.hook_index(24, HookedCreateMove2);
		//*(DWORD***)ClientMode


		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);

		g_Options.aimkey = VK_LBUTTON;

		vguisurf_hook.hook_index(index::PlaySound, hkPlaySound);
		vguisurf_hook.hook_index(67, hkLockCursor);
		mdlrender_hook.setup(g_MdlRender, XorStr("client_panorama.dll"));
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		enginesound_hook.hook_index(5, hkEmitSound);
		
		Visuals::CreateFonts();

		//SetupLobbyHooks();


		/*for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		{
			if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
				// Search for the 'm_nModelIndex' property.
				RecvTable* pClassTable = pClass->m_pRecvTable;

				for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
					RecvProp* pProp = &pClassTable->m_pProps[nIndex];

					if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
						continue;

					// Store the original proxy function.
					fnSequenceProxyFn = (RecvVarProxyFn)pProp->m_ProxyFn;

					// Replace the proxy function with our sequence changer.
					pProp->m_ProxyFn = SetViewModelSequence;

					break;
				}

				break;
			}
		}*/

		//std::cout << "Hooks initialized" << std::endl;
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		gameevents_hook.unhook_all();
		render_view.unhook_all();

		Glow::Get().Shutdown();

		Visuals::DestroyFonts();
	}
	//--------------------------------------------------------------------------------
	static auto random_sequence(const int low, const int high) -> int
	{
		return rand() % (high - low + 1) + low;
	}

	//-------------------------------------------------

	inline void DrawFillBox(int x, int y, int w, int h, Color color1, LPDIRECT3DDEVICE9 m_pDevice)
	{
		DWORD dxBoxColor = D3DCOLOR_RGBA(color1.r(), color1.g(), color1.b(), color1.a());

		struct Vertex
		{
			float x, y, z, ht;
			DWORD Color;
		}
		V[4] = { { x,y + h, 0.0f, 0.0f, dxBoxColor },{ x,y, 0.0f, 0.0f, dxBoxColor },{ x + w,y + h, 0.0f, 0.0f, dxBoxColor },{ x + w,y, 0.0f, 0.0f, dxBoxColor } };
		m_pDevice->SetTexture(0, NULL);
		m_pDevice->SetPixelShader(0);
		m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//D3DBLEND_INVSRCALPHA
		m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
		//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		/*D3DCOLOR rectColor = D3DCOLOR_XRGB(color1.r(), color1.g(), color1.b());	//No point in using alpha because clear & alpha dont work!
		D3DRECT BarRect = { x, y, x + w, y + h };

		m_pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, rectColor, 0, 0);*/
		return;
	}
	static struct _Keys
	{
		bool        bPressed;
		DWORD       dwStartTime;
	}PressingKeys[257];

	bool State_Key(int Key, DWORD dwTimeOut) {
		if (HIWORD(GetKeyState(Key))) {
			if (!PressingKeys[Key].bPressed || (PressingKeys[Key].dwStartTime && (PressingKeys[Key].dwStartTime + dwTimeOut) <= GetTickCount())) {
				PressingKeys[Key].bPressed = true;

				if (dwTimeOut > NULL)
					PressingKeys[Key].dwStartTime = GetTickCount();

				return true;
			}
		}
		else
			PressingKeys[Key].bPressed = false;
		return false;
	}
	void SetLocalPlayerReady()
	{
		static auto SetLocalPlayerReadyFn = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils_fox.FindPattern("client_panorama.dll", "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));
		HWND Hwnd;
		if ((Hwnd = FindWindow(NULL, XorStr("Counter-Strike: Global Offensive"))) && GetForegroundWindow() == Hwnd) // Gets the csgo window and checks if the active window is csgos window // EDIT: Changed from Valve001 to Counter-Strike: Global Offensive.
		{
			RECT lprect;
			GetClientRect(Hwnd, &lprect); // Gets the resolution of the window
			SendMessage(Hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(lprect.right / 2, lprect.bottom / 2)); // Moves the mouse into the middle of the window
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // click 
		}
		if (SetLocalPlayerReadyFn)
			SetLocalPlayerReadyFn("");
	}
	void __fastcall hkEmitSound(void * ecx, void * edx, void * filter, int iEntIndex, int iChannel, const char * pSoundEntry, unsigned int nSoundEntryHash, const char * pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector * pOrigin, const Vector * pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, StartSoundParams_t & params)
	{
		static auto oHkEmitSound = enginesound_hook.get_original<hkEmitSound_t>(5);

		//printf("called emit sound() %s\n", pSoundEntry);
		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep"))
		{
			SetLocalPlayerReady();
			FLASHWINFO fi;
			fi.cbSize = sizeof(FLASHWINFO);
			fi.hwnd = InputSys::Get().GetMainWindow();
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0;
			fi.dwTimeout = 0;
			FlashWindowEx(&fi);
		}
		
		/*if(pOrigin)
			g_CVar->ConsoleColorPrintf(Color(255, 70, 0), "{SND} %s x: %f y: %f z: %f ent index: %d\n", pSoundEntry, pOrigin->x, pOrigin->y, pOrigin->z, iEntIndex);*/

		
		oHkEmitSound(ecx, edx, filter, iEntIndex,  iChannel, pSoundEntry,  nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, params);
	}

	class FoxD3D9Draw
	{
	public:
		LPD3DXFONT m_font = 0;
		void DrawMessage(unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message)
		{
			D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, r, g, b);
			RECT rct; //Font
			rct.left = x;
			rct.right = 1680;
			rct.top = y;
			rct.bottom = rct.top + 200;
			m_font->DrawTextA(NULL, Message, -1, &rct, 0, fontColor);
			
			return;
		}

	}D3DDraw;

	bool sc_end_init = false;
	EndScene oEndScene = nullptr;

	const char *water_mark_or = "LISA beta test";
	char water_mark[] = "LISA beta test";
	IDirect3DTexture9* he_grenade_tex = 0;
	IDirect3DTexture9* flash_grenade_tex = 0;
	void DrawGrenades()
	{
		auto g_overlay = ImGui::GetOverlayDrawList();

		for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i)
		{
			auto entity = C_BasePlayer::GetPlayerByIndex(i);

			if (!entity)
				continue;

			if (entity == g_LocalPlayer)
				continue;

			auto classid = entity->GetClientClass()->m_ClassID;
			if ((classid == ClassID::CBaseCSGrenadeProjectile || classid == ClassID::CMolotovProjectile) && entity->m_vecOrigin() != Vector(0, 0, 0))
			{
				auto een = (C_BaseEntity*)entity;
				auto vec1 = een->m_vecOrigin();

				auto modelName = g_MdlInfo->GetModelName(entity->GetModel());
				if (modelName == nullptr) break;

				Vector scr;
				if (!Math::WorldToScreen(vec1, scr))
					continue;

				auto mdl = entity->GetModel();
				if (strstr(modelName, "w_eq_flashbang_dropped"))
					g_overlay->AddImage(flash_grenade_tex, ImVec2(scr.x - 6, scr.y - 10), ImVec2(scr.x + 7, scr.y + 10));
				else if (strstr(modelName, "w_eq_fraggrenade_dropped"))
					g_overlay->AddImage(he_grenade_tex, ImVec2(scr.x - 5, scr.y - 10), ImVec2(scr.x + 6, scr.y + 7), ImVec2(0, 0), ImVec2(1, 1), 0xff0000ff);
				else if (strstr(modelName, "w_eq_molotov_dropped") || strstr(modelName, "w_eq_incendiarygrenade_dropped"))
					g_overlay->AddCircle(ImVec2(scr.x, scr.y), mdl->radius, 0xff0000ff);
			}
		}
	}

	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		static int foxpussy = 0;
		if (!sc_end_init)
		{
			oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);
			D3DXCreateFontA(device, 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"), &D3DDraw.m_font);
			g_LisaMenu.Init(device);
			sc_end_init = true;
			D3DXCreateTextureFromFileInMemory(device, he_grenade_png, sizeof(he_grenade_png), &he_grenade_tex);
			D3DXCreateTextureFromFileInMemory(device, flash_grenade_png, sizeof(flash_grenade_png), &flash_grenade_tex);
			
			//g_FoxEvents->AddEventCustom("LISA Loaded", 1.0f,0,0, true);
		}
		if (foxpussy > 200)
		{
			g_FoxEvents->AddEventOrange("LISA LOADED", true);
			foxpussy = -666;
		}
		else if(foxpussy >= 0)
			foxpussy++;

		
		IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
		device->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
		device->GetVertexDeclaration(&vertDec);
		device->GetVertexShader(&vertShader);


		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		static auto crosshair_cvar = g_CVar->FindVar("crosshair");
		static auto LeftKnife = g_CVar->FindVar("cl_righthand");

		//viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Options.viewmodel_fov);
		mat_ambient_light_r->SetValue(g_Options.mat_ambient_light_r);
		mat_ambient_light_g->SetValue(g_Options.mat_ambient_light_g);
		mat_ambient_light_b->SetValue(g_Options.mat_ambient_light_b);
		DWORD dwOld_D3DRS_COLORWRITEENABLE;

		device->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		g_Options.pDev = device;
		
		if (g_Options.CreateMove_Crash_State != -1)
			D3DDraw.DrawMessage(5, 5, 255, 255, 0, 0, "client_panorama_fox: CreateMove crash");

		if (State_Key(g_Options.enable_legitbot_key, 1000))
			g_Options.enable_legitbot = !g_Options.enable_legitbot;

		if (State_Key(g_Options.forcebody_legitbot_key, 1000))
		{
			g_FoxEvents->AddEventCustom(g_Options.ForceBody ? "fox_body_force 0" : "fox_body_force 1", 0.34, 0.25, 0.86, true);
			g_Options.ForceBody = !g_Options.ForceBody;
		}

		if (State_Key(g_Options.misc_thirpers_key, 1000))
		{
			g_FoxEvents->AddEventCustom(g_Options.misc_thirdperson ? "msc_thirdperson 0" : "msc_thirdperson 1", 1.0f,1.0f,1.0f, false);
			g_Options.misc_thirdperson = !g_Options.misc_thirdperson;
		}

		if (State_Key(g_Options.misc_legitdesync_keyflip, 1000))
			g_Options.misc_legitdesync_flip = !g_Options.misc_legitdesync_flip;

		if (g_Options.rage_forcebaim_mode == 0)
		{
			if (GetAsyncKeyState(g_Options.rage_forcebaim_key))
				g_Options.rage_forcebaim_STATE = true;
			else
				g_Options.rage_forcebaim_STATE = false;
		}
		else if (g_Options.rage_forcebaim_mode == 1)
		{
			if (State_Key(g_Options.rage_forcebaim_key, 700))
				g_Options.rage_forcebaim_STATE = !g_Options.rage_forcebaim_STATE;
		}
		
		if (g_Options.rage_enabled_bind)
		{
			if (State_Key(g_Options.rage_enabled_bindkey, 1000))
				g_Options.rage_enabled = !g_Options.rage_enabled;
		}

		
		
		ImGui_ImplDX9_NewFrame();

		auto drawlst = ImGui::GetOverlayDrawList();

		g_FoxEvents->TickElements();
		g_FoxEvents->Render(drawlst);

		
		if (!g_EngineClient->IsInGame())
		{
			//ImGui::PushFont(g_LisaMenu.TahomaFont);
			drawlst->AddText(ImVec2(3, 50), ImGui::GetColorU32(ImVec4(1.0f, 0.2f, 0, 1.0f)), "Fox");
			//ImGui::PopFont();
		}
		/*ImGui::Begin("Debug window1");
		ImGui::Text("%d", DebugFovValue);

		ImGui::End();*/
		//-___________

		if (g_EngineClient->IsInGame())
		{
			if (g_Options.esp_grenades_esp)
				DrawGrenades();

		}
		g_LisaMenu.Run();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		//Menu::Get().Render();
		
		
		device->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);


		pixel_state->Apply();
		pixel_state->Release();
		device->SetVertexDeclaration(vertDec);
		device->SetVertexShader(vertShader);

		return oEndScene(device);
	}


	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto oReset = direct3d_hook.get_original<Reset>(index::Reset);
		Visuals::DestroyFonts();
		//Menu::Get().OnDeviceLost();
		g_LisaMenu.OnDeviceLost();
		D3DDraw.m_font->OnLostDevice();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0) {
			D3DDraw.m_font->OnResetDevice();
			g_LisaMenu.OnDeviceReset();
			Visuals::CreateFonts();
		}

		return hr;
	}
	
	void set_name(const char* name)
	{
		ConVar* nameConvar = g_CVar->FindVar(("name"));
		*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = NULL;
		nameConvar->SetValue(name);
	}

	void name_stealer()
	{
		if (!g_Options.misc_namestealer)
			return;
		static clock_t start_t = clock();
		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
		if (timeSoFar < g_Options.misc_namestealer_speed)
			return;
		const char* result;
		std::vector <std::string> names;

		for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			C_BaseEntity *entity = (C_BaseEntity*)g_EntityList->GetClientEntity(i);

			player_info_t pInfo;

			if (entity && g_LocalPlayer->m_iTeamNum() == entity->m_iTeamNum())
			{
				ClientClass* cClass = (ClientClass*)entity->GetClientClass();

				if (cClass->m_ClassID == (int)ClassID::CCSPlayer)
				{
					if (g_EngineClient->GetPlayerInfo(i, &pInfo))
					{
						if (!strstr(pInfo.szName, "GOTV"))
							names.push_back(pInfo.szName);
					}
				}
			}
		}
		if (g_Options.IsNewMap)
		{
			g_Options.IsNewMap = false;
			set_name("\n\xAD\xAD\xAD");
			start_t = clock();
			return;
		}
		int randomIndex = rand() % names.size();

		char buffer[128];
		sprintf_s(buffer, "%s ", names[randomIndex].c_str());
		result = buffer;

		set_name(result);
		start_t = clock();
	}

	static bool hooked = false;

	

	void safe_CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{

	}
#include <algorithm>
	bool hook_init = false;
	float get_max_desync_delta() {
		auto local_player = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
		auto animstate = uintptr_t(local_player->AnimationState());

		float duckammount = *(float *)(animstate + 0xA4);
		float speedfraction = std::max(float(0), std::min(*reinterpret_cast<float*>(animstate + 0xF8), float(1)));

		float speedfactor = max(float(0), min(float(1), *reinterpret_cast<float*> (animstate + 0xFC)));

		float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
		float unk2 = unk1 + 1.f;
		float unk3;

		if (duckammount > 0)
			unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

		unk3 = *(float *)(animstate + 0x334) * unk2;

		return unk3;
	}
	
	void fake_duck(CUserCmd* ccmd) {
			
		static int fakeduckChokeAmount = 0;
			if (GetAsyncKeyState(g_Options.FakeDuckKey) && g_Options.FakeDuck)
			{
				g_Options.IsFakeDucking = true;
				if (fakeduckChokeAmount > 14)
				{
					fakeduckChokeAmount = 0;
					*g_Options.bSendPacketLink = true;
				}
				else
					*g_Options.bSendPacketLink = false;
				fakeduckChokeAmount++;

				//unsigned int chokegoal = g_Options.FakeDuckTestAmount;
				auto choke = *(int*)(uintptr_t(g_ClientState) + 0x4D28);

				if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
				{
					if (choke >= 7)
						ccmd->buttons |= IN_DUCK;
					else
						ccmd->buttons &= ~IN_DUCK;
				}
			}
			else
			{
				g_Options.IsFakeDucking = false;
				fakeduckChokeAmount = 0;
			}
		/*if (g_Options.FakeDuck && GetAsyncKeyState(g_Options.FakeDuckKey))
		{
			g_Options.IsFakeDucking = true;
			chocked >= 7 ? ccmd->buttons |= IN_DUCK : ccmd->buttons &= ~IN_DUCK;
		}
		else
		{
			g_Options.IsFakeDucking = false;
		}*/
	}

#define Square(x) ((x)*(x))
	void MinWalk(CUserCmd* get_cmd, float get_speed) 
	{
		if (get_speed <= 0.f)
			return;

		float min_speed = (float)(sqrt(Square(get_cmd->forwardmove) + Square(get_cmd->sidemove) + Square(get_cmd->upmove)));
		if (min_speed <= 0.f)
			return;

		if (get_cmd->buttons & IN_DUCK)
			get_speed *= 2.94117647f;

		if (min_speed <= get_speed)
			return;

		float kys = get_speed / min_speed;

		get_cmd->forwardmove *= kys;
		get_cmd->sidemove *= kys;
		get_cmd->upmove *= kys;
	}
	
	bool __stdcall HookedCreateMove2(float SampleTime, CUserCmd* UserCmd)
	{
		
			C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();
			//auto verified = g_Input->GetVerifiedCmd(sequence_number);
			auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
			auto cmd = UserCmd;//g_Input->GetUserCmd2(sequence_number);
			if (!UserCmd->command_number)
				return true;
			if (!UserCmd || !cmd->command_number || !pLocal)
				return true;

			const auto ebp = reinterpret_cast<uintptr_t*>(uintptr_t(_AddressOfReturnAddress()) - sizeof(void*));
			auto & send_packet = *reinterpret_cast<bool*>(*ebp - 0x1C);
			g_Options.bSendPacketLink = reinterpret_cast<bool*>(*ebp - 0x1C);

			if (pLocal->m_iHealth() > 0 && g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
			{
				fake_duck(UserCmd);
				if (!g_Options.IsFakeDucking && g_Options.FakeLagEnable)
				{
					static int chockedpackets = 0;
					if (g_Options.FakelagAmount < chockedpackets)
					{
						chockedpackets = 0;
						send_packet = true;
					}
					else
						send_packet = false;
					chockedpackets++;
				}
				if (g_Options.antiaim_enabled && !g_Options.FakeLagEnable)
				{
					
					static bool switch_aa_packet = true;
					switch_aa_packet = !switch_aa_packet;
					send_packet = switch_aa_packet;
				}
			}
			
			
			g_Options.GlobalSendPacket = send_packet;
			g_LocalPlayer = pLocal;
			g_Options.cmd = UserCmd;
			g_Options.should_send_packet = send_packet;

			

			if (g_Options.misc_legitdesync)
				desync::handle(cmd, send_packet);

			CCSGrenadeHint::Get().Tick(cmd->buttons);

			g_chMisc.cmd = cmd;
			g_chMisc.Run();
			
			g_EnginePrediction.Start();
			if (g_Options.rage_enabled && !g_Options.enable_legitbot)
			{

				
				g_Resolver.OnCreateMove();

				g_AimBot.Do();
				
				if (g_Options.antiaim_enabled)
					g_AntiAim.OnCreateMove();
			}
			g_EnginePrediction.End();

			if (g_Options.antiaim_enabled)
			{
				if (!send_packet)
				{
					g_Options.RealAngle = Vector(cmd->viewangles.pitch, cmd->viewangles.yaw, cmd->viewangles.roll);
					if (g_Options.desync == 3)
					{
						g_Options.RealAngle.y -= 180;
						ValveMath::NormalizeVectorM(g_Options.RealAngle);
					}
				}
				else
				{

					g_Options.FakeAngle = Vector(cmd->viewangles.pitch, cmd->viewangles.yaw, cmd->viewangles.roll);


					g_Options.LastNetworkedLocalOrigin = g_LocalPlayer->m_vecOrigin();

				}
			}
			else
			{
				g_Options.RealAngle = g_Options.FakeAngle = Vector(cmd->viewangles.pitch, cmd->viewangles.yaw, cmd->viewangles.roll);
			}
			if (g_Options.rage_SlowWalk && GetAsyncKeyState(g_Options.rage_SlowWalk_key))
				MinWalk(UserCmd, g_Options.rage_SlowWalk_speed);
			
			
			if (g_Options.enable_legitbot && weapon)
			{
				TimeWarp::Get().StoreRecords(cmd, pLocal);
				legitbot::Get().do_aimbot(pLocal, weapon, cmd);
				TimeWarp::Get().DoBackTrack(cmd, pLocal);
			}

			if (cmd->forwardmove > 450)
				cmd->forwardmove = 450;
			if (cmd->forwardmove < -450)
				cmd->forwardmove = -450;

			if (cmd->sidemove > 450)
				cmd->sidemove = 450;
			if (cmd->sidemove < -450)
				cmd->sidemove = -450;
			
			if (cmd->viewangles.pitch < -89.0f)
				cmd->viewangles.pitch = -89.0f;
			if (cmd->viewangles.pitch > 89.0f)
				cmd->viewangles.pitch = 89.0f;

			cmd->viewangles.roll = 0.0f;

			for (auto i = 0; i < 3; i++)
			{
				while (cmd->viewangles[i] < -180.0f) cmd->viewangles[i] += 360.0f;
				while (cmd->viewangles[i] > 180.0f) cmd->viewangles[i] -= 360.0f;
			}

			if (g_LocalPlayer->m_iHealth() > 0)
			{
				auto AnimState = g_LocalPlayer->AnimStatev2();
				
				if(AnimState)
					g_Options.fakeLBYValue = g_LocalPlayer->m_flLowerBodyYawTarget();
				//g_Options.FakeAngle.x = g_LocalPlayer->m_flLowerBodyYawTarget();
				//g_Options.FakeAngle.y = AnimState->current_feet_yaw;
				
			}
			//Math::ClampAngles(cmd->viewangles);
			
		return false;
	}

	CreateMove oCreateMove = nullptr;

	void __stdcall hkCreateMove(int sequence_number , float input_sample_frametime , bool active , bool& bSendPacket)
	{
		if (!hook_init || oCreateMove == nullptr)
		{
			oCreateMove = hlclient_hook.get_original<CreateMove>(22);
			hook_init = true;
		}
		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);
		
	}
	//--------------------------------------------------------------------------------


	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}

	}
	bool __fastcall IsPlayingTimeDemo(IVEngineClient* ecx, void* edx)
	{
		//static auto oIsPlayingTimeDemo = g_EngineClient->GetOriginal<IsPlayingTimeDemo_t>(VT_Indexes::IsPlayingTimeDemo);
		static auto oIsPlayingTimeDemo = engineclient_hook.get_original<IsPlayingTimeDemo_t>(84);
		if (!g_Options.rage_enabled) return oIsPlayingTimeDemo(ecx);

		//ida paste to make sure people think I know what im doing
		//.text:101E2957		mov     s_bInterpolate, bl
		//.text:101E295D		mov     eax, [ecx]
		//.text:101E295F		mov     eax, [eax+150h] ; IsPlayingTimeDemo (engine, 84)
		//.text:101E2965		call    eax
		//.text:101E2967		test    al, al

		static uintptr_t ret = 0;

		if (!ret)
			ret = Utils_fox.FindPattern("client_panorama.dll", "84 C0 75 14 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 07");

		if (_ReturnAddress() == (void*)ret)
			return true; // wow! disabled interpolation!!

		/*static ConVar* cl_interp = 0;
		if (!cl_interp)
		{
			cl_interp = g_CVar->FindVar("cl_interpolate 1");
		}
		cl_interp->GetBool();*/
		

		return oIsPlayingTimeDemo(ecx);
	}
	//--------------------------------------------------------------------------------

	void TickGrenades()
	{
		static clock_t start_t = clock();
		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
		if (timeSoFar < 0.1)
			return;

		for (auto i = 0; i < Smokes.size(); i++)
		{
			Smokes[i].time -= 0.1;
			if (Smokes[i].time < 0)
				Smokes.erase(Smokes.begin() + i);
		}

		for (auto i = 0; i < Molotovs.size(); i++)
		{
			Molotovs[i].time -= 0.1;
			if (Molotovs[i].time < 0)
				Molotovs.erase(Molotovs.begin() + i);
		}

		start_t = clock();
	}
	
	static bool initPaint = false;
	void RenderGrenades()
	{
		for (auto& faf : Smokes)
		{
			Vector screen_ps;
			Vector sm_pos;
			sm_pos.x = faf.x;
			sm_pos.y = faf.y;
			sm_pos.z = faf.z;
			if (!Math::WorldToScreen(sm_pos, screen_ps))
				continue;


			char in_buf[255];
			snprintf(in_buf, 255, "Smoke %.1f", faf.time);

			wchar_t buf[128];


			if (MultiByteToWideChar(CP_UTF8, 0, in_buf, -1, buf, 128) > 0) {
				int tw, th;
				g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

				g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 150));
				g_VGuiSurface->DrawFilledRect(screen_ps.x, screen_ps.y, screen_ps.x + tw, screen_ps.y + th);

				g_VGuiSurface->DrawSetTextFont(esp_font);
				g_VGuiSurface->DrawSetTextColor(Color(255, 255, 255, (int)(abs(sin(g_GlobalVars->realtime) * 255))));
				g_VGuiSurface->DrawSetTextPos(screen_ps.x, screen_ps.y);
				g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
				
				
			}

		}

		for (auto& faf : Molotovs)
		{
			Vector screen_ps;
			Vector sm_pos;
			sm_pos.x = faf.x;
			sm_pos.y = faf.y;
			sm_pos.z = faf.z;
			if (!Math::WorldToScreen(sm_pos, screen_ps))
				continue;


			char in_buf[255];
			snprintf(in_buf, 255, "Molotov %.1f", faf.time);

			wchar_t buf[128];


			if (MultiByteToWideChar(CP_UTF8, 0, in_buf, -1, buf, 128) > 0) {
				int tw, th;
				g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

				g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 150));
				g_VGuiSurface->DrawFilledRect(screen_ps.x, screen_ps.y, screen_ps.x + tw, screen_ps.y + th);

				g_VGuiSurface->DrawSetTextFont(esp_font);
				g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0, (int)(abs(sin(g_GlobalVars->realtime * 2) * 255))));
				g_VGuiSurface->DrawSetTextPos(screen_ps.x, screen_ps.y);
				g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
			}

		}
	}

	RECT GetBBoxH(C_BaseEntity* ent, Vector pointstransf[])
	{
		RECT rect{};
		auto collideable = ent->GetCollideable();

		if (!collideable)
			return rect;

		auto min = collideable->OBBMins();
		auto max = collideable->OBBMaxs();

		const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

		Vector points[] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			Math::VectorTransform(points[i], trans, pointsTransformed[i]);
		}

		Vector pos = ent->m_vecOrigin();
		Vector screen_points[8] = {};

		for (int i = 0; i < 8; i++)
			if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
				return rect;
			else
				pointstransf[i] = screen_points[i];

		auto left = screen_points[0].x;
		auto top = screen_points[0].y;
		auto right = screen_points[0].x;
		auto bottom = screen_points[0].y;

		for (int i = 1; i < 8; i++)
		{
			if (left > screen_points[i].x)
				left = screen_points[i].x;
			if (top < screen_points[i].y)
				top = screen_points[i].y;
			if (right < screen_points[i].x)
				right = screen_points[i].x;
			if (bottom > screen_points[i].y)
				bottom = screen_points[i].y;
		}
		return RECT{ (long)left, (long)top, (long)right, (long)bottom };
	}
	void DrawLinesAA(Color color)
	{
		Vector src3D, dst3D, forward, src, dst;
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;

		filter.pSkip = g_LocalPlayer;

		// LBY
		ValveMath::AngleVectorsM(Vector(0, g_Options.fakeLBYValue, 0), &forward);
		src3D = g_LocalPlayer->m_vecOrigin();
		dst3D = src3D + (forward * 50.f); //replace 50 with the length you want the line to have

		ray.Init(src3D, dst3D);

		g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

		if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
			return;

		g_VGuiSurface->DrawSetColor(Color(210, 105, 30, 255));
		g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);

		// REAL AGNEL
		ValveMath::AngleVectorsM(Vector(0, g_Options.RealAngle.y, 0), &forward);
		dst3D = src3D + (forward * 50.f); //replace 50 with the length you want the line to have

		ray.Init(src3D, dst3D);

		g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

		if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
			return;

		g_VGuiSurface->DrawSetColor(Color(0, 255, 0, 255));
		g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);

		// Fake AGNEL
		ValveMath::AngleVectorsM(Vector(0, g_Options.FakeAngle.y, 0), &forward);
		dst3D = src3D + (forward * 50.f); //replace 50 with the length you want the line to have

		ray.Init(src3D, dst3D);

		g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

		if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
			return;

		g_VGuiSurface->DrawSetColor(Color(255, 0, 0, 255));
		g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);

		//Render::Line(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
	}
	
	void PawsTrails()
	{
		
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
		{
			static clock_t start_t = clock();
			double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
			if (timeSoFar < g_Options.misc_foot_trails_delay_between)
				return;

			static float rainbow;
			static Vector last_pos = Vector(0,0,0);

			rainbow += g_Options.misc_foot_trails_rainbowdelta;
			if (rainbow > 1.f)
				rainbow = 0.f;

			auto rainbow_col = Color::FromHSB(rainbow, 1, 1);
			auto local_pos = g_LocalPlayer->m_vecOrigin();
			DrawBeamPaw(last_pos, Vector(local_pos.x + g_Options.misc_foot_trails_xshift,
				local_pos.y + g_Options.misc_foot_trails_yshift, local_pos.z + g_Options.misc_foot_trails_zshift), rainbow_col);
			last_pos = local_pos;

			start_t = clock();
		}

	}

	void __stdcall hkPaintTraverse(unsigned int panel, bool forceRepaint, bool allowForce)
	{
		if (!initPaint)
		{
			initPaint = true;

		}
		if (g_Options.esp_enabled && g_Options.Noscope && strcmp("HudZoom", g_VGuiPanel->GetName(panel)) == 0)
			return;
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);
		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel)
		{
			if(g_Options.misc_foot_trails)
				PawsTrails();
			TickGrenades();

			if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected()) {
				if (!g_LocalPlayer)
					return;
				if(g_LocalPlayer->m_iHealth() > 0 && g_Options.misc_fake_angles)
					DrawLinesAA(Color(255,70,0,255));

				//g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0, 255));
				

				if (g_Options.esp_enabled)
				{
					RenderGrenades();

					Visuals::Misc::Noscope();
					ServerSound::Get().Start();
					for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i)
					{
						auto entity = C_BasePlayer::GetPlayerByIndex(i);

						if (!entity)
							continue;

						if (entity == g_LocalPlayer)
							continue;

						/*if (entity->GetClientClass()->m_ClassID == ClassID::CHEGrenade || entity->GetClientClass()->m_ClassID == ClassID::CMolotovGrenade || entity->GetClientClass()->m_ClassID == ClassID::CBaseCSGrenade)
						{
							Vector onScr;
							
							auto collideable = entity->GetCollideable();
							Vector real = collideable->OBBMins();

							if (Math::WorldToScreen(real, onScr) && real != Vector(0, 0, 0))
							{

								g_VGuiSurface->DrawSetColor(Color(255, 0, 0, 255));
								g_VGuiSurface->DrawFilledRect(onScr.x - 3, onScr.y - 3, onScr.x + 3, onScr.y + 3);
								g_VGuiSurface->DrawOutlinedCircle(onScr.x, onScr.y, 25, 300);
							}
						}*/


						if (g_Options.FovArrrows && entity->IsAlive() && entity->m_iHealth() > 0  && entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())	Visuals::Player::DrawFovArrows(entity, Color(g_Options.color_arrows.r(), g_Options.color_arrows.g(), g_Options.color_arrows.b(), (int)(abs(sin(g_GlobalVars->realtime * 2) * 255))));

						if (i < 65 && entity->IsAlive()) {
							// Begin will calculate player screen coordinate, bounding box, etc
							// If it returns false it means the player is not inside the screen
							// or is an ally (and team check is enabled)
							if (Visuals::Player::Begin(entity)) {
								if (g_Options.esp_player_boxes)     Visuals::Player::RenderBox();
								if (!entity->IsDormant())
								{
									if (g_Options.esp_player_snaplines) Visuals::Player::RenderSnapline();
									
									if (g_Options.esp_player_weapons)   Visuals::Player::RenderWeapon();
									if (g_Options.esp_player_names)     Visuals::Player::RenderName();
									if (g_Options.esp_player_health)    Visuals::Player::RenderHealth();
									if (g_Options.esp_player_armour)    Visuals::Player::RenderArmour();
									if (g_Options.esp_player_Flash)     Visuals::Misc::Flash(entity);
									if (g_Options.HeadEsp)              Visuals::Player::HeadEsp(entity);
									if (g_Options.SniperX)				Visuals::Misc::SniperX(entity);
									if (g_Options.KevESP)				Visuals::Player::KevESP(entity);
									if (g_Options.esp_player_Skel)      Visuals::Misc::Skel(entity);
									if (g_Options.EyePosESP)            Visuals::Misc::EyePos(entity);
								}
								//DrawDLight(entity->EntIndex(), entity->m_vecOrigin(), entity->GetBonePos(8));

								//if (g_Options.boneESP)              Visuals::Player::BoneESP(entity);
							}
						}
						if (g_Options.esp_dropped_weapons && entity->IsWeapon()) {
							Visuals::Misc::RenderWeapon((C_BaseCombatWeapon*)entity);
						}
						if (g_Options.esp_defuse_kit && entity->IsDefuseKit()) {
							Visuals::Misc::RenderDefuseKit(entity);
						}
						if (strstr(entity->GetClientClass()->m_pNetworkName, "CPlantedC4"))
						{
							if (g_Options.esp_planted_c4)
								Visuals::Misc::RenderPlantedC4((C_BaseEntity*)entity);

							if (g_Options.BombInfo)
								Visuals::Misc::RenderC4Info((C_PlantedC4*)entity);
						}
						
						/*if (strstr(entity->GetClientClass()->m_pNetworkName, "Mol") && entity->m_vecOrigin() != Vector(0, 0, 0))
						{
							g_CVar->ConsolePrintf("%s", entity->GetClientClass()->m_pNetworkName);
						}*/
						
					}
					ServerSound::Get().Finish();

					if (g_Options.RecoilCrosshair)
						Visuals::Misc::RecoilCrosshair();
					if (g_Options.Radar)
						Visuals::Misc::Radar();

					HitMarkerEvent::Get().Paint();
					CCSGrenadeHint::Get().Paint();

					if (g_Options.Noscope)
						Visuals::Misc::Noscope();
				}

			}
			else
			{
				g_Options.IsNewMap = true;
			}

		}
	}

	IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;

	void SaveState(IDirect3DDevice9 * pDevice)
	{
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		//	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state); // This seam not to be needed anymore because valve fixed their shit
		pDevice->GetVertexDeclaration(&vertDec);
		pDevice->GetVertexShader(&vertShader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
	}

	void RestoreState(IDirect3DDevice9 * pDevice) // not restoring everything. Because its not needed.
	{
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		//pixel_state->Apply(); 
		//pixel_state->Release();
		pDevice->SetVertexDeclaration(vertDec);
		pDevice->SetVertexShader(vertShader);
	}
	//--------------------------------------------------------------------------------

	void __stdcall hkPlaySound(const char* name)
	{
		/*static auto oPlaySound = vguisurf_hook.get_original<PlaySound>(index::PlaySound);

		oPlaySound(g_VGuiSurface, name);

		// Auto Accept
		if (strstr(name, "UI/competitive_accept_beep.wav")) {
			static auto fnAccept =
				(void(*)())Utils::PatternScan("client_panorama.dll", "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12");

			fnAccept();

			//This will flash the CSGO window on the taskbar
			//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
			FLASHWINFO fi;
			fi.cbSize = sizeof(FLASHWINFO);
			fi.hwnd = InputSys::Get().GetMainWindow();
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0;
			fi.dwTimeout = 0;
			FlashWindowEx(&fi);
		}*/
	}
	
	//--------------------------------------------------------------------------------
	int __stdcall hkDoPostScreenEffects(int a1)
	{
		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>(index::DoPostScreenSpaceEffects);

		if (g_LocalPlayer && g_Options.glow_enabled)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, a1);
	}
	//--------------------------------------------------------------------------------\\

	
	bool end_scene_init = false;
	void SinCosRGA(float a, float* s, float* c)
	{
		*s = sin(a);
		*c = cos(a);
	}
	void AngleMatrix(const Vector& angles, matrix3x4_t& matrix)
	{

		

		float sr, sp, sy, cr, cp, cy;


		SinCosRGA(DEG2RAD(angles[1]), &sy, &cy);
		SinCosRGA(DEG2RAD(angles[0]), &sp, &cp);
		SinCosRGA(DEG2RAD(angles[2]), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp * crcy + srsy);
		matrix[1][2] = (sp * crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}
	void __fastcall hkSceneEnd(void* thisptr, void* edx)
	{


		if (!g_LocalPlayer)
			return hkSceneEnd(thisptr, edx);
		//models/inventory_items/cologne_prediction/cologne_prediction_glass


		if (g_Options.chams_player_enabled)
		{
			constexpr float color_normal[4] = { 255, 255, 255, 255 };

			IMaterial* mat = nullptr;
			IMaterial* regular = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
			IMaterial* regular_IgnoreZ = g_MatSystem->FindMaterial("chams_ignorez", TEXTURE_GROUP_MODEL);
			IMaterial* flat = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
			IMaterial* flat_IgnoreZ = g_MatSystem->FindMaterial("flat_ignorez", TEXTURE_GROUP_MODEL);
			IMaterial* wire = g_MatSystem->FindMaterial("chams_wire", TEXTURE_GROUP_MODEL);
			IMaterial* wire_IgnoreZ = g_MatSystem->FindMaterial("chams_wireignore", TEXTURE_GROUP_MODEL);

			if (!regular || regular->IsErrorMaterial())
				return;
			//static  IMaterial* wire_frame = CreateMaterial(false, true, false);
			//static  IMaterial* ignoreZ = CreateMaterial(true, false, false);
			//static  IMaterial* ignoreZ = CreateMaterial(false, false,true);
			for (int i = 1; i < g_GlobalVars->maxClients; ++i) {
				auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
				if (ent && ent->IsAlive() && !ent->IsDormant()) {

					//const char *opt_Chams[] = { " Textured", " Flat ", " WireFrame", " Textured XQZ","Flat XQZ ","WireFrame XQZ" };
					const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
					const auto front = enemy ? g_Options.esp_player_chams_color_t : g_Options.esp_player_chams_color_ct;
					const auto back = enemy ? g_Options.esp_player_chams_color_t_not_visible : g_Options.esp_player_chams_color_ct_not_visible;
					if (g_Options.theirchams && ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
					{
						if (g_Options.chams_player_regular)
						{
							regular->IncrementReferenceCount();

							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_MdlRender->ForcedMaterialOverride(regular);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_regular_IgnoreZ)
						{
							regular_IgnoreZ->IncrementReferenceCount();
							//regular_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(regular_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat)
						{
							flat->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(flat);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat_IgnoreZ)
						{
							flat_IgnoreZ->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(flat_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe)
						{
							wire->IncrementReferenceCount();
							wire->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(wire);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe_IgnoreZ)
						{
							wire_IgnoreZ->IncrementReferenceCount();
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(wire_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (g_Options.yourteamchams && ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
					{
						if (g_Options.chams_player_regular)
						{
							regular->IncrementReferenceCount();

							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_MdlRender->ForcedMaterialOverride(regular);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_regular_IgnoreZ)
						{
							regular_IgnoreZ->IncrementReferenceCount();
							//regular_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(regular_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat)
						{
							flat->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(flat);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat_IgnoreZ)
						{
							flat_IgnoreZ->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(flat_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe)
						{
							wire->IncrementReferenceCount();
							wire->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(wire);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe_IgnoreZ)
						{
							wire_IgnoreZ->IncrementReferenceCount();
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(wire_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}





					}



				}
			}
		}
		if (g_Options.glow_enabled)
		{
			Glow::Get().Run();
		}

		if (g_Input->m_fCameraInThirdPerson)
		{
			static IMaterial* mat = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_OTHER);
			if (mat && g_Options.FakeAng1type)
			{

				QAngle ang = g_LocalPlayer->m_angEyeAngles();

				//g_MdlRender->DrawModel()
				g_LocalPlayer->SetAngle2(QAngle(0, g_Options.FakeAngle.y, g_Options.FakeAngle.z)); // paste here ur AA.y value or pLocal->GetLby() (for example)
				bool LbyColor = false; // u can make LBY INDICATOR. When LbyColor is true. Color will be Green , if false it will be White
				float NormalColor[3] = { 1, 1, 1 };
				float lbyUpdateColor[3] = { 0, 1, 0 };
				g_RenderView->SetColorModulation(LbyColor ? lbyUpdateColor : NormalColor);
				g_MdlRender->ForcedMaterialOverride(mat);
				g_LocalPlayer->DrawModel(0x00000001, 255);

				g_LocalPlayer->SetAngle2(ang);
				g_MdlRender->ForcedMaterialOverride(nullptr);
				
			}



		}

	}

	

	inline bool ApplyCustomModel(C_BaseAttributableItem* pWeapon, const char* vMdl)
	{
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		// Get the view model of this weapon.
		C_BaseViewModel* pViewModel = pLocal->m_hViewModel().Get();

		if (!pViewModel)
			return false;

		// Get the weapon belonging to this view model.
		auto hViewModelWeapon = pViewModel->m_hWeapon();
		C_BaseAttributableItem* pViewModelWeapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(hViewModelWeapon);

		if (pViewModelWeapon != pWeapon)
			return false;

		// Check if an override exists for this view model.
		int nViewModelIndex = pViewModel->m_nModelIndex();

		// Set the replacement model.
		pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(vMdl);

		

		return true;
	}
	static char tag[] = "Foxy lover";
	//--------------------------------------------------------------------------------

	

	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);
		C_BasePlayer* me = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
		g_Resolver.FrameStage(stage);

		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{

			auto hWeapons = pLocal->m_hMyWeapons();
			if(g_Options.skinchanger_enabled)
				Skinchanger::Get().Work();
		}

		


		if (stage == FRAME_RENDER_START)
		{

			if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
			{
				if(g_Options.misc_thirdperson)
					g_Prediction->SetLocalViewAngles(Vector(g_Options.RealAngle.x, g_Options.RealAngle.y, 0));

				/*if(g_Options.flash_remove && g_LocalPlayer && (g_LocalPlayer->m_flFlashDuration() > 0.f))
					g_LocalPlayer->m_flFlashDuration() = 0.f;*/

			}
		}

		ofunc(g_CHLClient, stage);

	}
	//--------------------------------------------------------------------------------
	void __stdcall hkOverrideView(CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<OverrideView>(index::OverrideView);

		if (g_EngineClient->IsInGame() && vsView) {
			//grenade_prediction::Get().View(vsView);
			Visuals::Misc::ThirdPerson();
			if (g_EngineClient->IsInGame() && vsView) {
				CCSGrenadeHint::Get().View();
				//DebugFovValue = vsView->fov;
				if (g_Options.Noscope)
				{
					vsView->fov = 90;
					auto zoomsensration = g_CVar->FindVar("zoom_sensitivity_ratio_mouse");

					
					zoomsensration->SetValue("0");
				}
			}
		} 
		ofunc(g_ClientMode, vsView);

	}

	//--------------------------------------------------------------------------------
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);
		//chams are here
		Chams::Get().OnDrawModelExecute(ctx, state, info, pCustomBoneToWorld);

		if (g_Input->m_fCameraInThirdPerson && g_LocalPlayer->IsAlive())
		{
			auto entity = C_BasePlayer::GetPlayerByIndex(info.entity_index);
			if (entity == g_LocalPlayer && g_Options.rage_enabled)
			{
				auto matt = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
				Vector BonePos;
				Vector OutPos;
				matrix3x4_t BoneMatrix[128];
				for (int i = 0; i < 128; i++)
				{
					if(g_Options.desync != 3)
						ValveMath::AngleMatrixZZ(Vector(0, g_Options.RealAngle.y - g_Options.FakeAngle.y  ,0), BoneMatrix[i]);
					else
						ValveMath::AngleMatrixZZ(Vector(0, -g_LocalPlayer->DesyncValue(), 0), BoneMatrix[i]);
					ValveMath::MatrixMultiplyZZ(BoneMatrix[i], pCustomBoneToWorld[i]);
					BonePos = Vector(pCustomBoneToWorld[i][0][3], pCustomBoneToWorld[i][1][3], pCustomBoneToWorld[i][2][3]) - info.origin;
					if (g_Options.desync != 3)
						ValveMath::VectorRotateM(BonePos, Vector(0, g_Options.RealAngle.y - g_Options.FakeAngle.y, 0), OutPos);
					else
						ValveMath::VectorRotateM(BonePos, Vector(0, -g_LocalPlayer->DesyncValue(), 0), OutPos);
					OutPos += info.origin;

					BoneMatrix[i][0][3] = OutPos.x;
					BoneMatrix[i][1][3] = OutPos.y;
					BoneMatrix[i][2][3] = OutPos.z;
				}
				//g_MdlRender->ForcedMaterialOverride(matt);
				//g_RenderView->SetColorModulation(1.5, 1, 1);

				if (matt && !matt->IsErrorMaterial())
				{
					//g_Options.chams_arms_enabled
					g_MdlRender->ForcedMaterialOverride(matt);
					g_RenderView->SetBlend(g_Options.fake_chams_a);
					g_RenderView->SetColorModulation(g_Options.fake_chams_r, g_Options.fake_chams_g, g_Options.fake_chams_b);
					ofunc(g_MdlRender, ctx, state, info, BoneMatrix);
					g_MdlRender->ForcedMaterialOverride(nullptr, 0);
					g_RenderView->SetBlend(1);
					g_RenderView->SetColorModulation(1, 1, 1);
				}
				else
				{
					ofunc(g_MdlRender, ctx, state, info, BoneMatrix);
				}
				//g_RenderView->SetBlend(0.5);
				//g_MdlRender->ForcedMaterialOverride(matt);
				//g_RenderView->SetColorModulation(1, 1, 1);

				
				//g_MdlRender->ForcedMaterialOverride(nullptr, 0);
			}
		}
		
		ofunc(g_MdlRender, ctx, state, info, pCustomBoneToWorld);



		

		if (ofunc && Chams::Get().Do_BTChams(g_MdlRender, ctx, state, info))
			g_MdlRender->ForcedMaterialOverride(nullptr, 0);

		
		


	}

	auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");
	typedef bool(__thiscall *svc_get_bool_t)(PVOID);
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto ofunc = sv_cheats.get_original<svc_get_bool_t>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;

		return ofunc(pConVar);
	}

	void __fastcall hkLockCursor(ISurface* thisptr, void* edx)
	{
		static auto oLockCursor = Hooks::vguisurf_hook.get_original<LockCursor_t>(67);

		if (!g_LisaMenu.IsVisible())
			return oLockCursor(thisptr, edx);

		g_VGuiSurface->UnlockCursor();
	}
}