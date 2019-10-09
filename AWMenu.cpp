#include "AWMenu.h"
AWFoxMenu AFoxMenu;
#define NOMINMAX
#include <Windows.h>
#include <chrono>
#include <d3dx9tex.h>
#include "imgdata.h"
#include "KitParser.h"
#include "MainInclude.hpp"
#include "./features/NightMode.hpp"

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "droid.hpp"
#include "Lobby.h"
#include "FoxSystem.h"
#include "SDK_lobby.h"
#include "Bulletshow.h"
#include <fstream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"
struct ComboFilterState
{
	int  activeIdx;         // Index of currently 'active' item by use of up/down keys
	bool selectionChanged;  // Flag to help focus the correct item when selecting active item
};
namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

}
static bool ComboFilter__DrawPopup(ComboFilterState& state, int START, std::vector <Kit_t>ENTRIES, int ENTRY_COUNT)
{
	using namespace ImGui;
	bool clicked = 1;

	// Grab the position for the popup
	ImVec2 pos = GetItemRectMin(); pos.y += GetItemRectSize().y;
	ImVec2 size = ImVec2(GetItemRectSize().x - 60, GetItemsLineHeightWithSpacing() * 4);

	PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		0; //ImGuiWindowFlags_ShowBorders;

	SetNextWindowFocus();

	SetNextWindowPos(pos);
	SetNextWindowSize(size);
	Begin("##combo_filter", nullptr, flags);

	PushAllowKeyboardFocus(false);

	for (int i = 0; i < ENTRY_COUNT; i++) {
		// Track if we're drawing the active index so we
		// can scroll to it if it has changed
		bool isIndexActive = state.activeIdx == i;

		if (isIndexActive) {
			// Draw the currently 'active' item differently
			// ( used appropriate colors for your own style )
			PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 0, 1));
		}

		PushID(i);
		if (Selectable(ENTRIES[i].name.c_str(), isIndexActive)) {
			// And item was clicked, notify the input
			// callback so that it can modify the input buffer
			state.activeIdx = i;
			clicked = 1;
		}
		if (IsItemFocused() && IsKeyPressed(GetIO().KeyMap[ImGuiKey_Enter])) {
			// Allow ENTER key to select current highlighted item (w/ keyboard navigation)
			state.activeIdx = i;
			clicked = 1;
		}
		PopID();

		if (isIndexActive) {
			if (state.selectionChanged) {
				// Make sure we bring the currently 'active' item into view.
				SetScrollHere();
				state.selectionChanged = false;
			}

			PopStyleColor(1);
		}
	}

	PopAllowKeyboardFocus();
	End();
	PopStyleVar(1);

	return clicked;
}

static bool ComboFilter(const char* id, char* buffer, int bufferlen, vector <Kit_t>hints, int num_hints, ComboFilterState& s) {
	struct fuzzy {
		static int score(const char* str1, const char* str2) {
			int score = 0, consecutive = 0, maxerrors = 0;
			while (*str1 && *str2) {
				int is_leading = (*str1 & 64) && !(str1[1] & 64);
				if ((*str1 & ~32) == (*str2 & ~32)) {
					int had_separator = (str1[-1] <= 32);
					int x = had_separator || is_leading ? 10 : consecutive * 5;
					consecutive = 1;
					score += x;
					++str2;
				}
				else {
					int x = -1, y = is_leading * -3;
					consecutive = 0;
					score += x;
					maxerrors += y;
				}
				++str1;
			}
			return score + (maxerrors < -9 ? -9 : maxerrors);
		}
		static int search(const char* str, int num, std::vector <Kit_t>words) {
			int scoremax = 0;
			int best = -1;
			for (int i = 0; i < num; ++i) {
				int score = fuzzy::score(words[i].name.c_str(), str);
				int record = (score >= scoremax);
				int draw = (score == scoremax);
				if (record) {
					scoremax = score;
					if (!draw) best = i;
					else best = best >= 0 && strlen(words[best].name.c_str()) < strlen(words[i].name.c_str()) ? best : i;
				}
			}
			return best;
		}
	};
	using namespace ImGui;
	bool done = InputText(id, buffer, bufferlen, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
	bool hot = s.activeIdx >= 0 && strcmp(buffer, hints[s.activeIdx].name.c_str());
	if (hot) {
		int new_idx = fuzzy::search(buffer, num_hints, hints);
		int idx = new_idx >= 0 ? new_idx : s.activeIdx;
		s.selectionChanged = s.activeIdx != idx;
		s.activeIdx = idx;
		if (done || ComboFilter__DrawPopup(s, idx, hints, num_hints)) {
			int i = s.activeIdx;
			if (i >= 0) {
				strcpy(buffer, hints[i].name.c_str());
				done = true;
			}
		}
	}
	return done;
}
ImGuiID Colorpicker_Close = 0;
__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }

void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_ct_invis[], float col_t_invis[], bool alpha = true)
{

	bool switch_entity_teams = false;
	bool switch_color_vis = false;
	bool open_popup = ImGui::ColorButtonFloat(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(13, 0));

	if (open_popup) {
		ImGui::OpenPopup(picker_idname);
		Colorpicker_Close = ImGui::GetID(picker_idname);
	}

	if (ImGui::BeginPopup(picker_idname))
	{
		const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
		if (ImGui::Button(button_name0, ImVec2(-1, 0)))
			switch_entity_teams = !switch_entity_teams;

		const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
		if (ImGui::Button(button_name1, ImVec2(-1, 0)))
			switch_color_vis = !switch_color_vis;

		std::string id_new = picker_idname;
		id_new += "##pickeritself_";

		ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
		ImGui::EndPopup();
	}
}

void ColorPickerBox(const char* picker_idname, float col_n[], bool alpha = true)
{

	bool open_popup = ImGui::ColorButtonFloat(picker_idname, col_n, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
	if (open_popup)
	{
		ImGui::OpenPopup(picker_idname);
		Colorpicker_Close = ImGui::GetID(picker_idname);
	}

	if (ImGui::BeginPopup(picker_idname))
	{
		std::string id_new = picker_idname;
		id_new += "##pickeritself_";

		ImGui::ColorPicker4(id_new.c_str(), col_n, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
		ImGui::EndPopup();
	}
}

void DrawingSpectatorList()
{
	struct spec_player
	{
		string name;
		string target;
		bool isSpecLocal;
		spec_player(string n1, string n2, bool n3)
		{
			name = n1;
			target = n2;
			isSpecLocal = n3;
		}
	};
	vector <spec_player>spectating_players;
	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		int localIndex = g_EngineClient->GetLocalPlayer();
		C_BasePlayer* pLocalEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());//C_BasePlayer::GetPlayerByIndex(localIndex);
		if (pLocalEntity)
		{
			for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
			{
				C_BasePlayer* pBaseEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i); //C_BasePlayer::GetPlayerByIndex(i);
				if (!pBaseEntity)
					continue;
				if (pBaseEntity->m_iHealth() > 0)
					continue;
				if (pBaseEntity == pLocalEntity)
					continue;
				if (pBaseEntity->IsDormant())
					continue;
				//if (pBaseEntity->m_hObserverTarget() != pLocalEntity)
				//	continue;

				player_info_t pInfo, tInfo;
				g_EngineClient->GetPlayerInfo(pBaseEntity->EntIndex(), &pInfo);

				if (pInfo.ishltv)
					continue;

				auto tpTarget = pBaseEntity->m_hObserverTarget();

				if (tpTarget == 0 || tpTarget == nullptr)
					continue;
				//g_EngineClient->GetPlayerInfo(pBaseEntity->m_hObserverTarget()->EntIndex(), &tInfo);
				if (tpTarget == pLocalEntity)
					spectating_players.emplace_back(pInfo.szName, tpTarget->GetName().c_str(), true);
				else
					spectating_players.emplace_back(pInfo.szName, tpTarget->GetName().c_str(), false);


			}
		}
	}

	ImGui::SetNextWindowSize(ImVec2(230.0f, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSizeConstraints({ 230, -1 }, { 230, -1 });

	if (ImGui::Begin("Fox Spec List###SpecLST", &g_Options.spec_list, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		for (auto& a : spectating_players)
			if (a.isSpecLocal)
				ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "%s -> %s", a.name.c_str(), a.target.c_str());
			else
				ImGui::Text("%s -> %s", a.name.c_str(), a.target.c_str());


	ImGui::End();
}


void RenderSteamApiTab()
{
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Lobby info");
	ImGui::Separator();


	ImGui::Text("Local player xuid: %" PRIu64 "\n", local_player_xiud);
	ImGui::Text("Players in lobby: ");
	if (!players.empty())
	{
		local_player_xiud = players[0].xuid;
		for (const auto& player : players)
		{
			ImGui::Text("%" PRIu64 "\n", player.xuid);
		}
	}



	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "ISteamLobby[]");
	ImGui::Separator();

	static char stm_input_id[256] = { 0 };
	ImGui::InputText("SteamId##stm_id", stm_input_id, 256);

	static uint64 stm_last_invite = 666;

	if (ImGui::Button("Invite custom steam id"))
	{
		CSteamID stm_id;
		uint64 stm_id_uint;

		sscanf(stm_input_id, "%" PRIu64, &stm_id_uint);

		stm_id.SetFromUint64(stm_id_uint);
		SDK::Interfaces::SteamMatchmaking()->InviteUserToLobby(steam_id_global, stm_id);

		stm_last_invite = stm_id_uint;
	}

	ImGui::Text("Last invite to %" PRIu64 "\n", stm_last_invite);


	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "ISteamFriends[]");
	ImGui::Separator();

	static char stm_input_id_friend[256] = { 0 };
	ImGui::InputText("SteamId##stm_id_friend", stm_input_id_friend, 256);

	if (ImGui::Button("Add fake friend"))
	{
		CSteamID stm_id;
		uint64 stm_id_uint;

		sscanf(stm_input_id_friend, "%" PRIu64, &stm_id_uint);

		if (stm_id_uint != 0)
		{
			stm_id.SetFromUint64(stm_id_uint);
			friends.push_back(stm_id);
		}
	}
	if (ImGui::Button("Clear fake friends list"))
		friends.clear();

	//
}

const char* weaponNamesForCombobox[] = {
	"Desert Eagle",
	"Dual Berettas",
	"Five-SeveN",
	"Glock-18",
	"AK-47",
	"AUG",
	"AWP",
	"FAMAS",
	"G3SG1",
	"Galil AR",
	"M249",
	"M4A4",
	"MAC-10",
	"P90",
	"UMP-45",
	"XM1014",
	"PP-Bizon",
	"MAG-7",
	"Negev",
	"Sawed.Off",
	"Tec-9",
	"P2000",
	"MP7",
	"MP9",
	"Nova",
	"P250",
	"SCAR-20",
	"SG 553",
	"SSG 08",
	"M4A1-S",
	"USP-S",
	"CZ75-Auto" ,
	"R8 Revolver",
	"Knife"
};

const char* typeoflag[] = {
	" Factor",
	" Adaptive",
	" Break"
};

const char* ChamBTtyp[] = {
	" All Ticks",
	" Last Tick",
	" 3 Ticks"
};
const char* knifeNames[] =
{
	" Default",
	" Bayonet",
	" Flip",
	" Gut",
	" Karambit",
	" M9 Bayonet",
	" Huntsman",
	" Falchion",
	" Bowie",
	" Butterfly",
	"But Plugs"
};

const char* TrigType[] =
{
	" Rifle",
	" Pistol",
	" Sniper"
};
const char* Hitsounds[] =
{
	" Off",
	" Skeet",
	" Normal",
	" Bubble"
};
const char* Hitboxx[] =
{
	"",
	"   Head",
	"   Neck",
	"   Chest",
	"   Stomach",
	"   Closest"
};
static char* SkyColor[] = {
	" Default" ,
	" Purple" ,
	" Red" ,
	" Blue" ,
	" Green"
};
const char* Ltype[] = {

	" Aim",
	" Backtrack",
	" Trigger"

};
const char* skyboxchanger[] =
{
	"Off",
	"Ymaja",
	"Himalaya",
	"Mijtm",
	"Jungle",
	"New Jok",
	"Light",
	"Night",
	"Storm Light"
};
const char* TypeOfVis[] =
{
	" ESP" ,
	" Glow",
	" Chams"
};
const char* GlowTypeARR[] = {
	" Normal"," Skinny"," Pulsating"
};

const char* BoxTypeARR[] = {
	" Normal"," Corner"
};

const char* ChamsTypeARR[] = {
	"tvisible",
	"ctvisible",
	"colort",
	"colorct"
};

static char* sidebar_tabs[] = {
	"ESP",
	"AIM",
	"MISC",
	"CONFIG"
};
const char* esp_types[] = {
	"Normal", "Outlined"
};
const char* hitsound_types[] =
{
	"None", "Skeet", "Type1", "Bubble"
};

inline bool AWColorEdit4(const char* label, Color* v, bool show_alpha = true)
{
	auto clr = ImVec4{
		v->r() / 255.0f,
		v->g() / 255.0f,
		v->b() / 255.0f,
		v->a() / 255.0f
	};

	if (ImGui::ColorEdit4(label, &clr.x, show_alpha | ImGuiColorEditFlags_NoInputs)) {
		v->SetColor(clr.x, clr.y, clr.z, clr.w);
		return true;
	}
	return false;
}

void ButtonHackSpacing()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(7, 0));

	ImGui::Spacing();
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();
	ImGui::PopStyleVar();
	ImGui::SameLine();
}
ImU32 color_test = 0x720000d8;
ImU32 color_test2 = 0xFFFFFFFF;
float color_test_f[4];
float button_hov[4];

int active_tab = 0;


void Redtheme()
{
	auto sttt = ImGui::GetStyle();
	sttt.ScrollbarRounding = 0.0f;
	sttt.ScrollbarSize = 2.0f;
	auto colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.29f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.98f, 0.11f, 0.11f, 0.60f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.90f, 0.00f, 0.00f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.00f, 0.60f);
	colors[ImGuiCol_Button] = ImVec4(0.90f, 0.00f, 0.00f, 0.90f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.13f, 0.00f, 0.85f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.31f, 0.27f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.00f, 0.64f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	//colors[ImGuiCol_Tab] = ImVec4(0.76f, 0.80f, 0.84f, 0.93f);
	//colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	//colors[ImGuiCol_TabActive] = ImVec4(0.60f, 0.73f, 0.88f, 1.00f);
	//colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
	//colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	//colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	//colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


}


/*void AWFoxMenu::Run()
{
	if (!_visible && g_Options.spec_list)
	{
		ImGui_ImplDX9_NewFrame();
		//DrawingSpectatorList();
		ImGui::GetIO().MouseDrawCursor = _visible;

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		return;
	}
	else if (!_visible && !g_Options.spec_list)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui::GetIO().MouseDrawCursor = _visible;

	//const auto sidebar_size = get_sidebar_size();
	static int active_sidebar_tab = 0;

	//ImGui::PushStyle(_style);
	//if (g_Options.spec_list)
		//DrawingSpectatorList();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 0.1));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.85, 0, 0, 1.0));
	ImGui::SetNextWindowSize(ImVec2(800, 600));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

	ImGui::Begin("AW###Menu22", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 window;
	window = ImGui::GetWindowPos();
	//background 
	drawlist->AddRectFilled(ImVec2(window.x, window.y + 55), ImVec2(window.x + 800, window.y + 582), 0xf0ffffff);


	//down rect ("V4 for Counter-Strike: Global Offensive")
	drawlist->AddRectFilled(ImVec2(window.x, window.y + 582), ImVec2(window.x + 800, window.y + 600), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.412f)));
	drawlist->AddText(ImVec2(window.x + 5, window.y + 584), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "V4 for Counter-Strike: Global Offensive");
	drawlist->AddText(ImVec2(window.x + 650, window.y + 584), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "https://WestFoxy.net");

	//CHilds backgrounds
	drawlist->AddImage(groupBox_tex, ImVec2(window.x + 10, window.y + 90), ImVec2(window.x + 247, window.y + 570));
	drawlist->AddImage(groupBox_tex, ImVec2(window.x + 267, window.y + 90), ImVec2(window.x + 267 + 247, window.y + 570));
	drawlist->AddImage(groupBox_tex, ImVec2(window.x + 267 + 247 + 10 + 18, window.y + 90), ImVec2(window.x + 267 + 247 + 10 + 247 + 18, window.y + 570));

	//RED BACKGROUND in the top
	drawlist->AddRectFilled(ImVec2(window.x, window.y), ImVec2(window.x + 800, window.y + 55), color_test);

	//Red line under tabs
	drawlist->AddRectFilled(ImVec2(window.x, window.y + 55), ImVec2(window.x + 800, window.y + 55 + 5), ImGui::GetColorU32(ImVec4(0.75f, 0.0f, 0.0f, 1.0f)));

	//LOGO AIMWARE.NET
	drawlist->AddImage(logo_tex, ImVec2(window.x + 8, window.y + 5), ImVec2(window.x + 225, window.y + 52));


	ImGui::Dummy(ImVec2(242.0f, 20.0f));
	ImGui::SameLine();
	//__________________________________TABS
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0, 0, 0.3f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.983, 0.114, 0.114, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));


	if (active_tab == 0)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));

		ImGui::ButtonGradient("Legit", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Legit", ImVec2(80, 55)))
			active_tab = 0;
	}
	ImGui::SameLine();
	if (active_tab == 1)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Rage", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Rage", ImVec2(80, 55)))
			active_tab = 1;
	}
	ImGui::SameLine();
	if (active_tab == 2)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Vis", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Vis", ImVec2(80, 55)))
			active_tab = 2;
	}
	ImGui::SameLine();
	if (active_tab == 3)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Misc", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Misc", ImVec2(80, 55)))
			active_tab = 3;
	}
	ImGui::SameLine();
	if (active_tab == 4)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Col", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Col", ImVec2(80, 55)))
			active_tab = 4;
	}
	ImGui::SameLine();
	if (active_tab == 5)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Conf", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Conf", ImVec2(80, 55)))
			active_tab = 5;
	}
	ImGui::SameLine();
	if (active_tab == 6)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));

		ImGui::ButtonGradient("CMD", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("CMD", ImVec2(80, 55)))
			active_tab = 6;
	}

	ImGui::PopStyleColor(4);

	//__________________________________


	static bool enabled1 = true;
	ImGui::Spacing();
	ButtonHackSpacing();
	ImGui::Spacing();
	ImGui::SameLine();
	ImGui::Checkbox("Enabled##en1", &enabled1);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(21, 13));



	//Child 1
	ImGui::BeginChild("Child1", ImVec2(255, 520), false);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

	ButtonHackSpacing();
	static bool o1, o2, o3;
	ImGui::Checkbox("XQZ Fox", &o1);
	ButtonHackSpacing();
	ImGui::Checkbox("Autowall", &o2);
	ButtonHackSpacing();
	ImGui::Checkbox("NoRecoil", &o3);
	static int o4 = 0;
	ButtonHackSpacing();
	ImGui::SliderInt("Fov", &o4, 0, 180);

	ImGui::PopStyleVar();
	ImGui::EndChild();
	//________________

	ImGui::SameLine();

	//Child 1
	ImGui::BeginChild("Child2", ImVec2(255, 520), false);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 10));
	static bool tmp1 = false;
	ImGui::Checkbox("Enabled", &tmp1);

	ImGui::PopStyleVar();
	ImGui::EndChild();
	//_________________

	ImGui::SameLine();

	//Child 3
	ImGui::BeginChild("Child3", ImVec2(255, 520), false);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 10));

	ImGui::Button("fox7");
	ImGui::Button("fox8");
	ImGui::Button("fox9");

	ImGui::PopStyleVar();
	ImGui::EndChild();
	//_____________________


	ImGui::PopStyleVar(2);
	ImGui::End();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}*/
void anti_aim()
{
	const char* pitch[] = { " none", " emotion", " minimal", " fake down" };
	const char* yaw[] = { " none", " backwards", " backwards jitter", " 180z", " keybased", " freestanding desync" };

	

	ImGui::Checkbox(("enable anti-aim"), &g_Options.aa_bool);
	ImGui::Combo(("pitch"), &g_Options.aa_pitch_type, pitch, ARRAYSIZE(pitch));
	ImGui::Combo("yaw", &g_Options.aa_real_type, yaw, ARRAYSIZE(yaw));

	

	ImGui::Checkbox("desync anti-aim", &g_Options.desync_aa);
	ImGui::SliderFloat("", &g_Options.desync_range, 0, 58);

	

	//ImGui::Checkbox(("disable thirdperson on nade"), &g_Options.disable_tp_on_nade);

	

	if (g_Options.aa_real_type == 4)
	{
		ImGui::Hotkey(("left"), &g_Options.aa_left);
		ImGui::Hotkey(("right"), &g_Options.aa_right);
		ImGui::Hotkey(("back"), &g_Options.aa_back);
	}
}
void RenderRageTabAW()
{
	ImVec2 window;
	window = ImGui::GetWindowPos();
	auto drawlist = ImGui::GetWindowDrawList();

	static bool enabled1 = true;
	static bool subtab = false; //MAIN - WEAPONS
	static bool o1, o2, o3;
	static int o4 = 0;


	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 5, window.y + 90), ImVec2(window.x + 255, window.y + 571));
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265, window.y + 90), ImVec2(window.x + 265 + 250, window.y + 571));
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265 + 250 + 10, window.y + 90), ImVec2(window.x + 265 + 250 + 10 + 250, window.y + 571));

	/*if (!subtab)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button("MAIN", ImVec2(400, 30));
		ImGui::PopStyleColor(2);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		if (ImGui::Button("MAIN", ImVec2(400, 30)))
			subtab = false;
		ImGui::PopStyleColor();
	}
	ImGui::SameLine();
	if (subtab)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button("WEAPON", ImVec2(400, 30));
		ImGui::PopStyleColor(2);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		if (ImGui::Button("WEAPON", ImVec2(400, 30)))
			subtab = true;
		ImGui::PopStyleColor();
	}
	*/

	static int Weapon_selection = 0;

	ImGui::SetNextWindowPos(ImVec2(window.x + 5, window.y + 65));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 13));

	ImGui::BeginChild("rg_weapons");

	/*extern unsigned char scar20_png[1471]; //110
extern unsigned char awp_png[1033];	//130
extern unsigned char ak47_png[1132]; //99
extern unsigned char ssg08_png[1132]; //114
extern unsigned char ump_png[1394]; //77
extern unsigned char glock_png[639]; //35*/
	static int pad = 0;

	if (ImGui::ImageButton(AFoxMenu.scar20_tex, ImVec2(110 - 29, 15)))
	{
		Weapon_selection = 0;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(AFoxMenu.ssg08_tex, ImVec2(114 - 29, 15)))
	{
		Weapon_selection = 1;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(AFoxMenu.awp_tex, ImVec2(130 - 29, 15)))
	{
		Weapon_selection = 2;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(AFoxMenu.deagle_tex, ImVec2(34,15)))
	{
		Weapon_selection = 3;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(AFoxMenu.glock_tex, ImVec2(37, 15)))
	{
		Weapon_selection = 4;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(AFoxMenu.ak47_tex, ImVec2(99 - 29, 15)))
	{
		Weapon_selection = 5;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(AFoxMenu.ump_tex, ImVec2(77 - 29, 15)))
	{
		Weapon_selection = 6;
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 60), ImVec2(window.x + 800, window.y + 60 + 45));
	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 90), ImVec2(window.x + 800, window.y + 90 + 45));

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 13));


	ImGui::SetNextWindowPos(ImVec2(window.x + 23, window.y + 115));
	//Child 1


	ImGui::BeginChild("rage_tab1", ImVec2(212, 437));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

	
	ImGui::Checkbox("Enable", &g_Options.rage_enabled);
	//ImGui::Checkbox("desync", &g_Options.Rage_AA_enable);
	//ImGui::Checkbox("Auto Fire", &g_Config.Get<bool>("AimBot.AutoFire"));
	
	if (Weapon_selection == 0)
	{
		ImGui::Text("[Auto sniper]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[0].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[0].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[0].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[0].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[0].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[0].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[0].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[0].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[0].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[0].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[0].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[0].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[0].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[0].HitScan7);
	}
	else if (Weapon_selection == 1)
	{
		ImGui::Text("[SSG08]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[1].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[1].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[1].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[1].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[1].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[1].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[1].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[1].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[1].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[1].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[1].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[1].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[1].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[1].HitScan7);
	}
	else if (Weapon_selection == 2)
	{
		ImGui::Text("[AWP]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[2].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[2].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[2].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[2].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[2].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[2].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[2].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[2].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[2].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[2].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[2].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[2].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[2].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[2].HitScan7);
	}
	else if (Weapon_selection == 3)
	{
		ImGui::Text("[Deagle && Revolver]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[3].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[3].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[3].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[3].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[3].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[3].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[3].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[3].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[3].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[3].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[3].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[3].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[3].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[3].HitScan7);
	}
	else if (Weapon_selection == 4)
	{
		ImGui::Text("[Pistols other]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[4].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[4].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[4].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[4].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[4].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[4].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[4].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[4].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[4].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[4].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[4].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[4].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[4].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[4].HitScan7);
	}
	else if (Weapon_selection == 5)
	{
		ImGui::Text("[Rifles]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[5].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[5].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[5].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[5].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[5].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[5].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[5].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[5].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[5].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[5].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[5].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[5].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[5].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[5].HitScan7);
	}
	else if (Weapon_selection == 6)
	{
		ImGui::Text("[SMGS]");
		ImGui::Checkbox("No Recoil", &g_Options.RageW[6].no_recoil);
		ImGui::Checkbox("Aimstep", &g_Options.RageW[6].rage_aimstep_enabled);
		ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.RageW[6].rage_aimstep, 0.f, 6.f, "%.1f");
		ImGui::Checkbox("Extrapolation", &g_Options.RageW[6].extrapolation);
		ImGui::SliderFloat("Point Scale##scl", &g_Options.RageW[6].pointscale, 0.f, 1.f, "%.1f");
		ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.RageW[6].minimal_damage, 0.f, 120.f, "%.1f");
		ImGui::SliderFloat("Hit Chance##htsch", &g_Options.RageW[6].hit_chance, 0.f, 100.f, "%.1f");

		ImGui::Checkbox("Head", &g_Options.RageW[6].HitScan1);
		ImGui::Checkbox("Neck", &g_Options.RageW[6].HitScan2);
		ImGui::Checkbox("Chest", &g_Options.RageW[6].HitScan3);
		ImGui::Checkbox("Stomach", &g_Options.RageW[6].HitScan4);
		ImGui::Checkbox("Pelvis", &g_Options.RageW[6].HitScan5);
		ImGui::Checkbox("Arms", &g_Options.RageW[6].HitScan6);
		ImGui::Checkbox("Legs", &g_Options.RageW[6].HitScan7);
	}
	ImGui::Checkbox(("Slow Walk"), &g_Options.rage_SlowWalk);
	ImGui::SliderFloat("Speed", &g_Options.rage_SlowWalk_speed, 0.01f, 50.0f);
	if (g_Options.rage_SlowWalk)
		ImGui::Hotkey(("##slow_walkk"), &g_Options.rage_SlowWalk_key);

	
	ImGui::PopStyleVar();
	ImGui::EndChild();


	//________________


	ImGui::SetNextWindowPos(ImVec2(window.x + 283, window.y + 115));
	//Child 2


	ImGui::BeginChild("rage_tab2", ImVec2(212, 437));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 10));
	static bool tmp1 = false;
	anti_aim();

	ImGui::PopStyleVar();
	ImGui::EndChild();


	//_________________


	ImGui::SetNextWindowPos(ImVec2(window.x + 544, window.y + 115));
	//Child 3


	ImGui::BeginChild("rage_tab3", ImVec2(212, 437));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 10));

	ImGui::Button("fox7");
	ImGui::Button("fox8");
	ImGui::Button("fox9");

	ImGui::PopStyleVar();
	ImGui::EndChild();


	//_____________________


	ImGui::PopStyleVar();
}

const char* fov_types[] =
{
	"Static",
	"Dynamic"
};

const char* hitbox_types[] =
{
	"Head",
	"Neck",
	"Pelvis",
	"Legs"
};
const char* hitbox_selection[] =
{
	"Dynamic",
	"Nearest",
	"Priority only"
};
const char* weap_selection[] =
{
	"Awp",
	"M4A1-S",
	"M4A4",
	"Deagle",
	"AK-47",
	"p250",
	"Famas",
	"Ump-45",
	"USP",
	"Glock",
	"SCAR-20",
	"CZ",
	"Revolver",
	"AUG",
	"SSG08",
	"SG",
	"Knife"
};



static char inp_awp[128] = "";
static char inp_m4a1[128] = "";
static char inp_m4a4[128] = "";
static char inp_deag[128] = "";
static char inp_ak[128] = "";
static char inp_p250[128] = "";
static char inp_usp[128] = "";
static char inp_glock[128] = "";
static char inp_scar20[128] = "";
static char inp_cz[128] = "";
static char inp_revolver[128] = "";
static char inp_aug[128] = "";
static char inp_ssg08[128] = "";
static char inp_sg[128] = "";
static char inp_knife[128] = "";
static char inp_famas[128] = "";
static char inp_ump[128] = "";
static ComboFilterState fil_awp = { 0, true };
static ComboFilterState fil_knife = { 0, true };
static ComboFilterState fil_m4a1 = { 0, true };
static ComboFilterState fil_m4a4 = { 0, true };
static ComboFilterState fil_deag = { 0, true };
static ComboFilterState fil_ak = { 0, true };
static ComboFilterState fil_p250 = { 0, true };
static ComboFilterState fil_usp = { 0, true };
static ComboFilterState fil_glock = { 0, true };
static ComboFilterState fil_scar20 = { 0, true };
static ComboFilterState fil_famas = { 0, true };
static ComboFilterState fil_cz = { 0, true };
static ComboFilterState fil_revolver = { 0, true };
static ComboFilterState fil_aug = { 0, true };
static ComboFilterState fil_ssg08 = { 0, true };
static ComboFilterState fil_sg = { 0, true };
static ComboFilterState fil_ump = { 0, true };
bool aim_enabled = true;
inline void set_name_s(const char* name)
{
	ConVar* nameConvar = g_CVar->FindVar(("name"));
	*(int*)((DWORD)& nameConvar->fnChangeCallback + 0xC) = NULL;
	nameConvar->SetValue(name);
}

void RenderSkinsTab()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.9f));

	ImGui::Begin("Skins w");
	

	ImGui::Combo("Knife model", &g_Options.knifemodel, knifeNames, ARRAYSIZE(knifeNames));

	//find(k_skins.begin(), k_skins.end(), )

	static int weapon_selected = 0;
	ImGui::Combo("Weapon", &weapon_selected, weap_selection, ARRAYSIZE(weap_selection));

	switch (weapon_selected)
	{
	case 0:
		ImGui::Text("[Fox] Weapon AWP");

		if (ComboFilter("Skin id", inp_awp, IM_ARRAYSIZE(inp_awp), k_skins, k_skins.size() - 1, fil_awp))
			puts(inp_awp);

		if (ImGui::Button("Set"))
			g_Options.skin_awp = k_skins[fil_awp.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_awp, fil_awp.activeIdx);
		break;

	case 1:
		ImGui::Text("[Fox] Weapon M4A1-S");

		if (ComboFilter("Skin id", inp_m4a1, IM_ARRAYSIZE(inp_m4a1), k_skins, k_skins.size(), fil_m4a1))
			puts(inp_m4a1);

		if (ImGui::Button("Set"))
			g_Options.skin_m4a1_s = k_skins[fil_m4a1.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_m4a1_s, fil_m4a1.activeIdx);
		break;
	case 2:
		ImGui::Text("[Fox] Weapon M4A4");

		if (ComboFilter("Skin id", inp_m4a4, IM_ARRAYSIZE(inp_m4a4), k_skins, k_skins.size(), fil_m4a4))
			puts(inp_m4a4);

		if (ImGui::Button("Set"))
			g_Options.skin_m4a4 = k_skins[fil_m4a4.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_m4a4, fil_m4a4.activeIdx);
		break;
	case 3:
		ImGui::Text("[Fox] Weapon Deagle");

		if (ComboFilter("Skin id", inp_deag, IM_ARRAYSIZE(inp_deag), k_skins, k_skins.size(), fil_deag))
			puts(inp_deag);

		if (ImGui::Button("Set"))
			g_Options.skin_deagle = k_skins[fil_deag.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_deagle, fil_deag.activeIdx);
		break;
	case 4:
		ImGui::Text("[Fox] Weapon AK-47");

		if (ComboFilter("Skin id", inp_ak, IM_ARRAYSIZE(inp_ak), k_skins, k_skins.size(), fil_deag))
			puts(inp_ak);

		if (ImGui::Button("Set"))
			g_Options.skin_ak = k_skins[fil_deag.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_ak, fil_deag.activeIdx);
		break;
	case 5:
		ImGui::Text("[Fox] Weapon p250");

		if (ComboFilter("Skin id", inp_p250, IM_ARRAYSIZE(inp_p250), k_skins, k_skins.size(), fil_p250))
			puts(inp_p250);

		if (ImGui::Button("Set"))
			g_Options.skin_p250 = k_skins[fil_p250.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_p250, fil_p250.activeIdx);
		break;
	case 6:
		ImGui::Text("[Fox] Weapon famas");

		if (ComboFilter("Skin id", inp_famas, IM_ARRAYSIZE(inp_famas), k_skins, k_skins.size(), fil_famas))
			puts(inp_famas);

		if (ImGui::Button("Set"))
			g_Options.skin_famas = k_skins[fil_famas.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_famas, fil_famas.activeIdx);
		break;
	case 7:
		ImGui::Text("[Fox] Weapon ump");

		if (ComboFilter("Skin id", inp_ump, IM_ARRAYSIZE(inp_ump), k_skins, k_skins.size(), fil_ump))
			puts(inp_ump);

		if (ImGui::Button("Set"))
			g_Options.skin_ump = k_skins[fil_ump.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_ump, fil_ump.activeIdx);
		break;
	case 8:
		ImGui::Text("[Fox] Weapon usp");

		if (ComboFilter("Skin id", inp_usp, IM_ARRAYSIZE(inp_usp), k_skins, k_skins.size(), fil_usp))
			puts(inp_usp);

		if (ImGui::Button("Set"))
			g_Options.skin_usp = k_skins[fil_usp.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_usp, fil_usp.activeIdx);
		break;
	case 9:
		ImGui::Text("[Fox] Weapon glock");

		if (ComboFilter("Skin id", inp_glock, IM_ARRAYSIZE(inp_glock), k_skins, k_skins.size(), fil_glock))
			puts(inp_glock);

		if (ImGui::Button("Set"))
			g_Options.skin_glock = k_skins[fil_glock.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_glock, fil_glock.activeIdx);
		break;

	case 10:
		ImGui::Text("[Fox] Weapon scar20");

		if (ComboFilter("Skin id", inp_scar20, IM_ARRAYSIZE(inp_scar20), k_skins, k_skins.size(), fil_scar20))
			puts(inp_scar20);

		if (ImGui::Button("Set"))
			g_Options.skin_scar20 = k_skins[fil_scar20.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_scar20, fil_scar20.activeIdx);
		break;
	case 11:
		ImGui::Text("[Fox] Weapon cz");

		if (ComboFilter("Skin id", inp_cz, IM_ARRAYSIZE(inp_cz), k_skins, k_skins.size(), fil_cz))
			puts(inp_cz);

		if (ImGui::Button("Set"))
			g_Options.skin_cz = k_skins[fil_cz.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_cz, fil_cz.activeIdx);
		break;
	case 12:
		ImGui::Text("[Fox] Weapon revolver");

		if (ComboFilter("Skin id", inp_revolver, IM_ARRAYSIZE(inp_revolver), k_skins, k_skins.size(), fil_revolver))
			puts(inp_revolver);

		if (ImGui::Button("Set"))
			g_Options.skin_revolver = k_skins[fil_revolver.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_revolver, fil_revolver.activeIdx);
		break;
	case 13:
		ImGui::Text("[Fox] Weapon aug");

		if (ComboFilter("Skin id", inp_aug, IM_ARRAYSIZE(inp_aug), k_skins, k_skins.size(), fil_aug))
			puts(inp_aug);

		if (ImGui::Button("Set"))
			g_Options.skin_aug = k_skins[fil_aug.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_aug, fil_aug.activeIdx);
		break;
	case 14:
		ImGui::Text("[Fox] Weapon ssg08");

		if (ComboFilter("Skin id", inp_ssg08, IM_ARRAYSIZE(inp_ssg08), k_skins, k_skins.size(), fil_ssg08))
			puts(inp_ssg08);

		if (ImGui::Button("Set"))
			g_Options.skin_ssg08 = k_skins[fil_ssg08.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_ssg08, fil_ssg08.activeIdx);
		break;
	case 15:
		ImGui::Text("[Fox] Weapon sg");

		if (ComboFilter("Skin id", inp_sg, IM_ARRAYSIZE(inp_sg), k_skins, k_skins.size(), fil_sg))
			puts(inp_sg);

		if (ImGui::Button("Set"))
			g_Options.skin_sg = k_skins[fil_sg.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_sg, fil_sg.activeIdx);
		break;
	case 16:
		ImGui::Text("[Fox] Knife");

		if (ComboFilter("Skin id", inp_knife, IM_ARRAYSIZE(inp_knife), k_skins, k_skins.size(), fil_knife))
			puts(inp_knife);

		if (ImGui::Button("Set"))
			g_Options.knife_paint_kit = k_skins[fil_knife.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.knife_paint_kit, fil_knife.activeIdx);
		break;
	}




	ImGui::End();
	ImGui::PopStyleColor();

}

const char* clantags_type[] =
{
	"Normal", "L!SA", "AW", "?", "L!SA", "Namesc", "fox fuck", "L!SA glitch"
};

void DebugWindow()
{
	

	static int			m_nType;

	// Entities
	static C_BaseEntity* m_pStartEnt;
	static int			m_nStartAttachment;
	static C_BaseEntity* m_pEndEnt;
	static int			m_nEndAttachment;

	// Points
	static Vector		m_vecStart;
	static Vector		m_vecEnd;

	static int			m_nModelIndex;
	static const char* m_pszModelName;

	static int			m_nHaloIndex;
	static const char* m_pszHaloName;
	static float		m_flHaloScale;

	static float		m_flLife;
	static float		m_flWidth;
	static float		m_flEndWidth;
	static float		m_flFadeLength;
	static float		m_flAmplitude;

	static float		m_flBrightness;
	static float		m_flSpeed;

	static int			m_nStartFrame;
	static float		m_flFrameRate;

	static float		m_flRed;
	static float		m_flGreen;
	static float		m_flBlue;

	static bool		m_bRenderable;

	static int			m_nSegments;

	static int			m_nFlags;

	// Rings
	Vector		m_vecCenter;
	float		m_flStartRadius;
	float		m_flEndRadius;

	static float die = 0, radius = 75.0f, decay = radius / 5.0f;

	

	/*dlight_t* lpELight = g_pEffects->CL_AllocDlight(0);
	//g_pEffects->
	lpELight->color.b = 255;
	lpELight->color.g = 0;
	lpELight->color.r = 0;
	lpELight->color.exponent = 10.0f;
	lpELight->radius = 75.f;
	lpELight->decay = lpELight->radius / 5.0f;
	lpELight->key = 0;
	lpELight->m_Direction = m_vecAbsOrigin;
	lpELight->origin = m_vecHead + Vector(0, 0, 35);
	lpELight->die = g_GlobalVars->curtime + 0.05f;*/

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::Begin("Debug Engine");

	ImVec2 window;
	window = ImGui::GetWindowPos();

	ImGui::SetNextWindowPos(ImVec2(window.x + 8, window.y + 8));

	ImGui::BeginChild("debch1");

	static float d1 = 63, d2 = 15, d3 = 0, d4 = 0, d5 = 1, d6 = 1;
	static int fr_pad = -1;
	ImGui::InputFloat("width", &d1);
	ImGui::InputFloat("height", &d2);

	ImGui::InputFloat("uv1x", &d3);
	ImGui::InputFloat("uv1y", &d4);

	ImGui::InputFloat("uv2x", &d5);
	ImGui::InputFloat("uv2y", &d6);

	ImGui::InputInt("fr_pad", &fr_pad);

	if (ImGui::ImageButton(AFoxMenu.deagle_tex, ImVec2(d1, d2), ImVec2(d3,d4), ImVec2(d5,d6), fr_pad))
	{
		
	}

	ImGui::Text("m_vecStart");
	ImGui::InputFloat("x##start0", &m_vecStart.x);
	ImGui::InputFloat("y##start1", &m_vecStart.y); 
	ImGui::InputFloat("z##start2", &m_vecStart.z);
	ImGui::Text("m_vecEnd");
	ImGui::InputFloat("x##end0", &m_vecEnd.x); 
	ImGui::InputFloat("y##end1", &m_vecEnd.y);
	ImGui::InputFloat("z##end2", &m_vecEnd.z);
	ImGui::InputFloat("radius", &radius);
	ImGui::InputFloat("decay", &decay);
	ImGui::InputFloat("die", &die);

	if (ImGui::Button("Draw"))
	{
		dlight_t* lpELight = g_pEffects->CL_AllocDlight(0);
		lpELight->color.b = 255;
		lpELight->color.g = 0;
		lpELight->color.r = 0;
		lpELight->color.exponent = 10.0f;
		lpELight->radius = radius;
		lpELight->decay = decay;
		lpELight->key = 0;
		lpELight->m_Direction = m_vecEnd;
		lpELight->origin = m_vecStart;
		lpELight->die = die;
	}
	static float pI = 3.14, theta = 1;

	ImGui::InputFloat("PI", &pI);
	ImGui::InputFloat("tHeta", &theta);
	//ImGui::InputFloat("radius", &radius);

	
	ImGui::Separator();
	static bool in = false;

	

	
	/*static IMaterial* pMaterial;
	if (ImGui::Button("Next material"))
	{
		//i = g_MatSystem->NextMaterial(i);
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);
	}

	if (i != g_MatSystem->InvalidMaterial() && pMaterial)
	{
		ImGui::Text("Name: %s\nTexture group name: %s\n", pMaterial->GetName(), pMaterial->GetTextureGroupName());
	}

	static float r, g, b, a;
	ImGui::SliderFloat("Red", &r, 0, 255);
	ImGui::SliderFloat("Green", &g, 0, 255);
	ImGui::SliderFloat("Blue", &b, 0, 255);
	ImGui::SliderFloat("Alpha", &a, 0, 255);
	if (ImGui::Button("Set to material"))
	{
		pMaterial->ColorModulate(r, g, b);
		pMaterial->AlphaModulate(a);
	}*/
	static int ind = 0;
	ImGui::Combo("Materials", &ind, mat_names);
	/*for (auto& a : mat_names)
	{
		ImGui::Text(a.c_str());
	}*/
	static float r, g, b, a;
	ImGui::SliderFloat("Red", &r, 0, 255);
	ImGui::SliderFloat("Green", &g, 0, 255);
	ImGui::SliderFloat("Blue", &b, 0, 255);
	ImGui::SliderFloat("Alpha", &a, 0, 255);
	if (ImGui::Button("Set to material"))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(mat_handles[ind]);

		pMaterial->ColorModulate(r, g, b);
		pMaterial->AlphaModulate(a);
		
	}
	if (ImGui::Button("Reload"))
	{
		mat_names.clear();
		mat_handles.clear();
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* mat = g_MatSystem->GetMaterial(i);

			if (!mat)
				continue;

			std::string tmp;
			tmp += mat->GetName();

			mat_names.push_back(tmp);
			mat_handles.push_back(i);
		}
	}

	/*for (auto& a : mat_names)
	{
		ImGui::Text(a.c_str());
	}*/


	ImGui::EndChild();

	//DrawBeam();

	ImGui::End();
	ImGui::PopStyleColor();
}

bool debWin = false;

void RenderMiscTab()
{
	ImVec2 window;
	window = ImGui::GetWindowPos();
	auto drawlist = ImGui::GetWindowDrawList();

	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 5, window.y + 65), ImVec2(window.x + 255, window.y + 571));
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265, window.y + 65), ImVec2(window.x + 265 + 250, window.y + 571));
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265 + 250 + 10, window.y + 65), ImVec2(window.x + 265 + 250 + 10 + 250, window.y + 571));
	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 59), ImVec2(window.x + 800, window.y + 60 + 45));

	drawlist->AddText(ImVec2(window.x + 27, window.y + 70), 0xff000000, "Movement");
	drawlist->AddText(ImVec2(window.x + 285, window.y + 70), 0xff000000, "Automation");
	drawlist->AddText(ImVec2(window.x + 285 + 260, window.y + 70), 0xff000000, "Enchancement");

	ImGui::SetNextWindowPos(ImVec2(window.x + 23, window.y + 85));
	ImGui::BeginChild("misc_tab1", ImVec2(212, 470));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));

	
	ImGui::Checkbox("Bhop", &g_Options.misc_bhop);
	ImGui::Checkbox("Auto strafe", &g_Options.misc_autostrafe);
	ImGui::Checkbox("Fast duck", &g_Options.misc_fastduck);

	ImGui::Checkbox("Circle strafe", &g_Options.misc_circle_enable);
	if(g_Options.misc_circle_enable)
		ImGui::Hotkey("Circle Key", &g_Options.misc_circle_key);

	ImGui::Checkbox("Edge jump", &g_Options.misc_edge_jump);
	ImGui::Checkbox("Edge jump strafe", &g_Options.misc_edge_jump_strafe);
	if(g_Options.misc_edge_jump)
		ImGui::Hotkey("Edge Key", &g_Options.misc_edge_jumpkey);

	ImGui::Checkbox("Legit AA", &g_Options.misc_legitdesync);
	if (g_Options.misc_legitdesync)
	{
		ImGui::Hotkey("L AA flip", &g_Options.misc_legitdesync_keyflip);
		ImGui::Text(g_Options.misc_legitdesync_flip ? "Flipped: true" : "Flipped: false");
	}

	ImGui::Checkbox("Debug Window", &debWin);
	if (debWin)
		DebugWindow();

	ImGui::PopStyleVar();
	ImGui::End();



	ImGui::SetNextWindowPos(ImVec2(window.x + 283, window.y + 85));
	ImGui::BeginChild("misc_tab2", ImVec2(212, 470));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));

	static char cln_tag_input[259] = { 0 };
	ImGui::InputText("Clan tag##cl_tag_inp", cln_tag_input, 256); ImGui::SliderFloat("Speed##clan_spedd", &g_Options.misc_clantag_speed, 0.01, 1.5f, "%.1f");

	if (ImGui::Button("Set"))
		SetClantag(cln_tag_input);
	ImGui::SameLine();
	if (ImGui::Button("Glitch"))
	{
		std::string dtmp = cln_tag_input;
		dtmp += "\n";
		dtmp += "\r";
		SetClantag(dtmp.c_str());

	}
	ImGui::SameLine();
	if (ImGui::Button("Set to animation"))
		g_Options.gladTag = cln_tag_input;

	ImGui::Checkbox("Animated", &g_Options.clan_tag_animate);
	ImGui::Combo("Types", &g_Options.misc_clantag_type, clantags_type, ARRAYSIZE(clantags_type));

	static char nm_input[256] = { 0 };
	ImGui::InputText("Name##nm_input", nm_input, 256);
	if (ImGui::Button("Set name"))
		set_name_s(nm_input);
	ImGui::SameLine();
	if (ImGui::Button("Infinite changes"))
		set_name_s("\n\xAD\xAD\xAD");
	ImGui::SameLine();
	if (ImGui::Button("uncode glitch"))
		set_name_s("\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd");
	ImGui::Checkbox("Name stealer", &g_Options.misc_namestealer); ImGui::SameLine(); ImGui::SliderFloat("Speed", &g_Options.misc_namestealer_speed, 0.f, 1.5f, "%.1f");
	ImGui::Checkbox("Name spam##spammer_nm", &g_Options.misc_namespam);

	ImGui::Checkbox("Spectators list", &g_Options.spec_list);

	/*static char reason[256] = { 0 };
	ImGui::InputText("Reason##reasn", reason, 256);
	if (ImGui::Button("Custom disconnect reason"))
	{
		INetChannelF* ch = (INetChannelF*)g_EngineClient->GetNetChannelInfo();
		if (!ch)
			return;

		ch->Shutdown(reason);
	}*/

	ImGui::PopStyleVar();
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(window.x + 543, window.y + 85));
	ImGui::BeginChild("misc_tab3", ImVec2(212, 470));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));
	if (ImGui::Button("Clear exception"))
		g_Options.CreateMove_Crash_State = -1;

	ImGui::Checkbox("Overlay info", &g_Options.misc_overlay);
	ImGui::Combo("Hitsound", &g_Options.Hitsound, hitsound_types, ARRAYSIZE(hitsound_types));

	static bool skins_window = false;

	ImGui::Checkbox("Reveal rank", &g_Options.misc_revealranks);


	ImGui::Checkbox("Skin Changer", &skins_window);
	ImGui::Checkbox("Events log", &g_Options.misc_foxlog);
	if (skins_window)
		RenderSkinsTab();

	ImGui::PopStyleVar();
	ImGui::End();
}

void RenderLegitTab()
{
	static int legit_weap_selection = 0;

	ImVec2 window;
	window = ImGui::GetWindowPos();
	auto drawlist = ImGui::GetWindowDrawList();


	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 5, window.y + 95), ImVec2(window.x + 255, window.y + 571));
	drawlist->AddText(ImVec2(window.x + 27, window.y + 98), 0xff000000, "Accuracy");
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265, window.y + 105), ImVec2(window.x + 265 + 250, window.y + 250));
	drawlist->AddText(ImVec2(window.x + 287, window.y + 109), 0xff000000, "Target");

	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 59), ImVec2(window.x + 800, window.y + 60 + 45));


	ImGui::SetNextWindowPos(ImVec2(window.x + 23, window.y + 65));
	ImGui::BeginChild("legit_tab_weapons", ImVec2(800, 600));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	if (ImGui::Button("Pistols", ImVec2(100, 25)))
		legit_weap_selection = 0;
	ImGui::SameLine();
	if (ImGui::Button("Rifles", ImVec2(100, 25)))
		legit_weap_selection = 1;
	ImGui::SameLine();
	if (ImGui::Button("Snipers", ImVec2(100, 25)))
		legit_weap_selection = 2;
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::EndChild();



	ImGui::SetNextWindowPos(ImVec2(window.x + 23, window.y + 115));
	ImGui::BeginChild("legit_tab1", ImVec2(212, 437));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));

	static float empty1 = 50;
	static bool empty2 = true;
	static int cur = 0;

	if (legit_weap_selection == 0)
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Debug] ");
		ImGui::SameLine();
		ImGui::Text("Pistols selection");
		ImGui::SliderFloat("FOV", &g_Options.legit_fov_pistols, 0, 180, "%.1f");
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_pistols, 1, 6, "%.1f");
		//ImGui::Checkbox("RCS", &empty2);
		//ImGui::Checkbox("Autowall", &empty2);
	}
	else if (legit_weap_selection == 1)
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Debug] ");
		ImGui::SameLine();
		ImGui::Text("Rifles selection");
		ImGui::SliderFloat("FOV", &g_Options.legit_fov_rifles, 0, 180, "%.1f");
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_rifles, 1, 6, "%.1f");
		//ImGui::Checkbox("RCS", &empty2);
		//ImGui::Checkbox("Autowall", &empty2);
	}
	else if (legit_weap_selection == 2)
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Debug] ");
		ImGui::SameLine();
		ImGui::Text("Snipers selection");
		ImGui::SliderFloat("FOV", &g_Options.legit_fov_Snipers, 0, 180, "%.1f");
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_Snipers, 1, 6, "%.1f");
		//ImGui::Checkbox("RCS", &empty2);
		//ImGui::Checkbox("Autowall", &empty2);
	}


	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void RenderVisuals()
{
	ImVec2 window;
	window = ImGui::GetWindowPos();
	auto drawlist = ImGui::GetWindowDrawList();

	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 5, window.y + 65), ImVec2(window.x + 255, window.y + 571));
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265, window.y + 65), ImVec2(window.x + 265 + 250, window.y + 571));
	drawlist->AddImage(AFoxMenu.groupBox_tex, ImVec2(window.x + 265 + 250 + 10, window.y + 65), ImVec2(window.x + 265 + 250 + 10 + 250, window.y + 571));
	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 59), ImVec2(window.x + 800, window.y + 60 + 45));

	drawlist->AddText(ImVec2(window.x + 27, window.y + 70), 0xff000000, "Line");
	drawlist->AddText(ImVec2(window.x + 285, window.y + 70), 0xff000000, "Chams");
	drawlist->AddText(ImVec2(window.x + 285 + 260, window.y + 70), 0xff000000, "Glow");

	ImGui::SetNextWindowPos(ImVec2(window.x + 23, window.y + 85));
	ImGui::BeginChild("visuals_tab1", ImVec2(212, 470));

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));
	ImGui::Checkbox("Enabled", &g_Options.esp_enabled); //ImGui::SameLine(); ImGuiEx::ColorEdit3("Enemies Visible", &g_Options.color_esp_enemy_visible);
	ImGui::Checkbox("Team check", &g_Options.esp_enemies_only);
	ImGui::Checkbox("Boxes", &g_Options.esp_player_boxes); ImGui::SameLine(); AWColorEdit4("##vis", &g_Options.color_esp_Box);
	ImGui::Combo("Box Type", &g_Options.BoxType, BoxTypeARR, ARRAYSIZE(BoxTypeARR));
	ImGui::Combo("Box Outline", &g_Options.esp_type, esp_types, ARRAYSIZE(esp_types));
	ImGui::Checkbox("Trace grenade", &g_Options.GrenadeTra);
	ImGui::Checkbox("Eye Pos ESP", &g_Options.EyePosESP);
	ImGui::Checkbox("Health", &g_Options.esp_player_health);
	ImGui::Checkbox("Grenades", &g_Options.esp_grenades_esp);
	ImGui::Checkbox("Skeleton", &g_Options.esp_player_Skel); ImGui::SameLine(); AWColorEdit4("##Skeleton", &g_Options.color_Skel);
	ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons); ImGui::SameLine(); AWColorEdit4("##Weapons", &g_Options.color_esp_weapons);
	ImGui::Checkbox("Weapon", &g_Options.esp_player_weapons); ImGui::SameLine(); AWColorEdit4("##Weaponsplayer", &g_Options.color_esp_playerweapons);
	ImGui::Checkbox("Names", &g_Options.esp_player_names); ImGui::SameLine(); AWColorEdit4("##playername", &g_Options.color_esp_playername);
	ImGui::SliderInt("View fov", &g_Options.viewmodel_fov, 68, 120);
	ImGui::Checkbox("Bomb info", &g_Options.BombInfo);
	ImGui::Checkbox("Radar", &g_Options.Radar);
	ImGui::Checkbox("HitMarker", &g_Options.misc_hitmarker);
	ImGui::Checkbox("Recoil crosshair", &g_Options.RecoilCrosshair);
	ImGui::Checkbox("Remove scope", &g_Options.Noscope);
	ImGui::Checkbox("C4", &g_Options.esp_planted_c4); ImGui::SameLine(); AWColorEdit4("##planted_c4d", &g_Options.color_esp_c4);
	ImGui::Checkbox("Defuse kit", &g_Options.esp_defuse_kit);
	ImGui::Checkbox("Flash info", &g_Options.esp_player_Flash);
	ImGui::Checkbox("Snap lines", &g_Options.esp_player_snaplines);
	ImGui::Checkbox("Out of view indicator", &g_Options.FovArrrows); ImGui::SameLine(); AWColorEdit4("##outview_col", &g_Options.color_arrows);
	ImGui::Checkbox("No hands", &g_Options.misc_no_hands);
	//ImGui::Checkbox("No hands", &g_Options.misc_no_hands);
	/*OPTION(bool, misc_bullettracers, false);
	OPTION(Color, misc_bullettracers_color, Color(16,0,255,255));
	//esp_bullettrace_length
	OPTION(float, esp_bullettrace_length, 5);*/
	ImGui::Checkbox("Bullet Tracers", &g_Options.misc_bullettracers); ImGui::SameLine(); AWColorEdit4("##bullettr_col", &g_Options.misc_bullettracers_color);
	ImGui::SliderFloat("Time##bullet_trace_tm", &g_Options.esp_bullettrace_length, 1.0f, 15.0f, "%.1f");
	ImGui::Checkbox("DLight Tracers", &g_Options.misc_bullettracers_dlight); ImGui::SameLine(); AWColorEdit4("##bullettr_delcol", &g_Options.misc_bullettracers_dlightcolor);
	ImGui::InputFloat("DLight radius", &g_Options.misc_bullettracersdlight_radius);
	ImGui::InputFloat("DLight delta", &g_Options.misc_bullettracersdlight_delta);

	ImGui::Checkbox("Third person", &g_Options.misc_thirdperson);
	ImGui::SliderFloat("Distance", &g_Options.misc_thirdperson_dist, 0, 350, "%.1f");
	ImGui::Hotkey("Third person key", &g_Options.misc_thirpers_key);
	if (ImGui::Button("Night Mode"))
	{
		//nightmode::apply();
		g_Nightmode.apply();
	}
	ImGui::SameLine();
	if (ImGui::Button("remove"))
	{
		//nightmode::apply();
		g_Nightmode.remove();
	}
	//AWColorEdit4("World modulate", &g_Options)
	//ImGui::ColorEdit3("World modulate", &g_Options.mat_ambient_light_r);

	ImGui::PopStyleVar();
	ImGui::End();



	ImGui::SetNextWindowPos(ImVec2(window.x + 283, window.y + 85));
	ImGui::BeginChild("visuals_tab2", ImVec2(212, 470));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));

	ImGui::Checkbox("Enabled", &g_Options.chams_player_enabled);
	ImGui::Checkbox("Enemy", &g_Options.theirchams); ImGui::SameLine(); ColorPickerBox("##E", g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, false);
	ImGui::Checkbox("Local", &g_Options.yourteamchams); ImGui::SameLine(); ColorPickerBox("##T", g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, false);
	ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Regular", &g_Options.chams_player_regular);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat", &g_Options.chams_player_flat);
	//ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Regular XQZ", &g_Options.chams_player_regular_IgnoreZ);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat XQZ", &g_Options.chams_player_flat_IgnoreZ);
	//ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire XQZ", &g_Options.chams_player_wireframe_IgnoreZ);

	/*ImGui::Checkbox("Arms", &g_Options.chams_arms_enabled); ImGui::SameLine(); AWColorEdit4("##armsch", &g_Options.color_chams_arms_visible);

	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat##arms1", &g_Options.chams_arms_flat);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wireframe##arms2", &g_Options.chams_arms_wireframe);
	//ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Glass##arms3", &g_Options.chams_arms_glass);
	//ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("XQZ##arms4", &g_Options.color_chams_arms_visible);

	ImGui::Checkbox("Weapons##chms_weap", &g_Options.chams_wep_enabled); ImGui::SameLine(); AWColorEdit4("##weapon_col", &g_Options.color_chams_wep_visible);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat##weap1", &g_Options.chams_wep_flat);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wireframe##weap2", &g_Options.chams_wep_wireframe);
	//ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Glass##weap3", &g_Options.chams_wep_glass);*/

	/*g_Options.chams_wep_flat,
									g_Options.chams_wep_wireframe,
									g_Options.chams_wep_glass,
									g_Options.color_chams_wep_visible*/

	ImGui::PopStyleVar();
	ImGui::End();



	ImGui::SetNextWindowPos(ImVec2(window.x + 543, window.y + 85));
	ImGui::BeginChild("visuals_tab3", ImVec2(212, 470));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 7));

	ImGui::Checkbox("Enabled", &g_Options.glow_enabled); ImGui::SameLine();
	ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
	ImGui::Combo("Glow Type", &g_Options.GlowType, GlowTypeARR, ARRAYSIZE(GlowTypeARR));
	ImGui::Checkbox("Players", &g_Options.glow_players);
	AWColorEdit4("Enemy", &g_Options.color_glow_enemy);
	AWColorEdit4("Ally", &g_Options.color_glow_ally);
	ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier); ImGui::SameLine(); AWColorEdit4("##C4 Carrier", &g_Options.color_glow_c4_carrier);
	ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4); ImGui::SameLine(); AWColorEdit4("##C4", &g_Options.color_glow_planted_c4);

	ImGui::PopStyleVar();
	ImGui::End();


}

void gen_random(char* s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

char bufrand[255];
void RenderCmd()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
	ImGui::SetCursorPos(ImVec2(5, 65));
	ImGui::BeginChild("cmd_ch");


	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "0YLQtdC/0LvQsNGPINC70LjRgdCw");
	/*gen_random(bufrand, 42);
	ImGui::Text(bufrand);
	gen_random(bufrand, 42);
	ImGui::Text(bufrand);
	gen_random(bufrand, 42);
	ImGui::Text(bufrand);
	gen_random(bufrand, 42);
	ImGui::Text(bufrand);*/
	ImGui::EndChild();

	ImGui::SetCursorPos(ImVec2(6, 75));
	ImGui::BeginChild("cmd_info");

	/*ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[ENGINE INFO]");
	
	ImGui::Text("Client version %d", g_EngineClient->GetClientVersion());
	ImGui::Text("Level name %s", g_EngineClient->GetLevelName());
	ImGui::Text("Max clients %d", g_EngineClient->GetMaxClients());
	ImGui::Text("Cur time %f", g_GlobalVars->curtime);
	ImGui::Text("Frame count %d", g_GlobalVars->framecount);
	ImGui::Text("Tick count %d", g_GlobalVars->tickcount);
	ImGui::Text("Interpolation %f", g_GlobalVars->interpolation_amount);
	ImGui::Text("Interval per tick %f", g_GlobalVars->interval_per_tick);
	ImGui::Text("Real time %f", g_GlobalVars->realtime);*/

	
		/*ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Lobby info]");
		ImGui::Separator();


		ImGui::Text("Local player xuid: %" PRIu64 "\n", local_player_xiud);
		ImGui::Text("Players in lobby: ");
		if (!players.empty())
		{
			local_player_xiud = players[0].xuid;
			for (const auto& player : players)
			{
				ImGui::Text("%" PRIu64 "\n", player.xuid);
			}
		}*/

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "ISteamLobby[]");
		ImGui::Separator();

		static char stm_input_id[256] = { 0 };
		ImGui::InputText("SteamId##stm_id", stm_input_id, 256);

		static uint64 stm_last_invite = 666;

		if (ImGui::Button("Invite custom steam id"))
		{
			CSteamID stm_id;
			uint64 stm_id_uint;

			sscanf(stm_input_id, "%" PRIu64, &stm_id_uint);

			stm_id.SetFromUint64(stm_id_uint);
			SDK::Interfaces::SteamMatchmaking()->InviteUserToLobby(steam_id_global, stm_id);

			stm_last_invite = stm_id_uint;
		}

		ImGui::Text("Last invite to %" PRIu64 "\n", stm_last_invite);


		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "ISteamFriends[]");
		ImGui::Separator();

		static char stm_input_id_friend[256] = { 0 };
		ImGui::InputText("SteamId##stm_id_friend", stm_input_id_friend, 256);

		if (ImGui::Button("Add fake friend"))
		{
			CSteamID stm_id;
			uint64 stm_id_uint;

			sscanf(stm_input_id_friend, "%" PRIu64, &stm_id_uint);

			if (stm_id_uint != 0)
			{
				stm_id.SetFromUint64(stm_id_uint);
				friends.push_back(stm_id);
			}
		}
		if (ImGui::Button("Clear fake friends list"))
			friends.clear();

		ImGui::Separator();
		RenderLobby();

		//
	

	ImGui::EndChild();

	ImGui::PopStyleColor();
}
void OverlayInfo(bool* p_open)
{
	const float DISTANCE = 10.0f;
	static int corner = 3;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.2f); // Transparent background
	ImGui::SetNextWindowSize(ImVec2(210, 43));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(52, 3));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	if (ImGui::Begin("InfoFox", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar))
	{

		ImGui::SetCursorPos(ImVec2(5, 5));
		if (g_Options.enable_legitbot)
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "LEGIT");
		else
			ImGui::Text("LEGIT");


		ImGui::SameLine();
		
		if (g_Options.rage_enabled)
			ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "RAGE");
		else
			ImGui::Text("RAGE");
		
		ImGui::SameLine();
		
		if (g_Options.ForceBody)
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "BODY");
		else
			ImGui::Text("BODY");

		time_t current_time;
		struct tm* time_info;
		static char timeString[32];

		time(&current_time);
		time_info = localtime(&current_time);

		strftime(timeString, 32, "[%H:%M:%S]", time_info);

		ImGui::Separator();
		ImGui::SetCursorPosX(5);
		ImGui::Text("%s %.1f fps %.2f ms", timeString,ImGui::GetIO().Framerate / 2, 1000.0f / ImGui::GetIO().Framerate);

		if (ImGui::BeginPopupContextWindow())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) * p_open = false;
			ImGui::PopStyleColor();
			ImGui::EndPopup();
		}
	}
	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
	ImGui::End();
}


void ColorsMenu()
{

	ImVec2 window;
	window = ImGui::GetWindowPos();
	auto drawlist = ImGui::GetWindowDrawList();
	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 59), ImVec2(window.x + 800, window.y + 60 + 45));

	ImGui::SetCursorPos(ImVec2(5, 65));
	ImGui::BeginChild("col_men");

	ImGuiStyle& style = ImGui::GetStyle();
	static ImGuiStyle ref_saved_style;

	// Default to using internal storage as reference


	ref_saved_style = style;


	// Simplified Settings
	if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
		style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
	{ bool window_border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
	ImGui::SameLine();
	{ bool frame_border = (style.FrameBorderSize > 0.0f); if (ImGui::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
	ImGui::SameLine();
	{ bool popup_border = (style.PopupBorderSize > 0.0f); if (ImGui::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }


	ImGui::SameLine();


	static int output_dest = 0;
	static bool output_only_modified = true;
	if (ImGui::Button("Export Unsaved"))
	{
		if (output_dest == 0)
			ImGui::LogToClipboard();
		else
			ImGui::LogToTTY();
		ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const ImVec4& col = style.Colors[i];
			const char* name = ImGui::GetStyleColorName(i);

			ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
		}
		ImGui::LogFinish();
	}
	ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui::PopItemWidth();
	ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

	static ImGuiTextFilter filter;
	filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

	static ImGuiColorEditFlags alpha_flags = 0;
	ImGui::RadioButton("Opaque", &alpha_flags, 0); ImGui::SameLine();
	ImGui::RadioButton("Alpha", &alpha_flags, ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine();
	ImGui::RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf);
	//HelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

	ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
	ImGui::PushItemWidth(-160);
	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		const char* name = ImGui::GetStyleColorName(i);
		if (!filter.PassFilter(name))
			continue;
		ImGui::PushID(i);
		ImGui::ColorEdit4("##color", (float*)& style.Colors[i], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | alpha_flags);

		ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
		ImGui::TextUnformatted(name);
		ImGui::PopID();
	}
	ImGui::PopItemWidth();
	ImGui::EndChild();

	ImGui::EndChild();

}

void ConfigMenu()
{
	ImVec2 window;
	window = ImGui::GetWindowPos();
	auto drawlist = ImGui::GetWindowDrawList();
	drawlist->AddImage(AFoxMenu.downshadow_tex, ImVec2(window.x, window.y + 59), ImVec2(window.x + 800, window.y + 60 + 45));

	ImGui::SetCursorPos(ImVec2(5, 65));
	ImGui::BeginChild("conf_fox");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

	static int ccind = 0; 
	ImGui::Text("Location: %s", g_Options.config_path.c_str());

	if (g_Options.cached_configs_names.size() != 0)
	{
		ImGui::ListBox("Configs", &ccind, g_Options.cached_configs_names);

		ImGui::Text("Selected: %s", g_Options.cached_configs_names[ccind].c_str());
	}
	else
		ImGui::Text("Empty configs. Create new");
	static char inp_cfg[256];
	ImGui::InputText("Name", inp_cfg, 255);

	if (ImGui::Button("Refresh"))
	{
		g_Options.refresh();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		g_Options.loadcfg(g_Options.cached_configs_names[ccind].c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		g_Options.removecfg(g_Options.cached_configs_names[ccind].c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		g_Options.savecfg(inp_cfg);
		g_Options.refresh();
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void AWFoxMenu::Run()
{
	//if (!_visible)
	//	return;

	
	
	if (g_Options.spec_list)
		DrawingSpectatorList();
	
	/*if (!_visible)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		return;
	}*/

	if(g_Options.misc_overlay)
		OverlayInfo(&g_Options.misc_overlay);

	ImGui::GetIO().MouseDrawCursor = _visible;
	
	if (!_visible)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		return;
	}
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 0.1));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.85, 0, 0, 1.0));
	ImGui::GetStyle().WindowRounding = 0;
	ImGui::SetNextWindowSize(ImVec2(800, 600));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

	ImGui::SetNextWindowPos(ImVec2(wx / 2 - 800 / 2, wh / 2 - 600 / 2 ), ImGuiCond_Once);

	static bool inn = false;
	if (!inn)
	{
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* mat = g_MatSystem->GetMaterial(i);

			if (!mat)
				continue;

			std::string tmp;
			tmp += mat->GetName();

			mat_names.push_back(tmp);
			mat_handles.push_back(i);
		}

		/*for (int i = 0; i < mat_names.size(); i++)
		{
			if (strstr(mat_names[i].c_str(), "vgui_white"))
			{
				IMaterial* pMaterial = g_MatSystem->GetMaterial(mat_handles[i]);
				pMaterial->ColorModulate(1.2, 0, 0);
				pMaterial->AlphaModulate(0.4);
			}
		}*/
		inn = true;
	}

	ImGui::Begin("AW###Menu22", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 window;
	window = ImGui::GetWindowPos();
	//background
	if (!(active_tab == 6))
		drawlist->AddRectFilled(ImVec2(window.x, window.y + 55), ImVec2(window.x + 800, window.y + 582), ImGui::GetColorU32(ImVec4(1,1,1,0.7)));
	else
		drawlist->AddRectFilled(ImVec2(window.x, window.y + 55), ImVec2(window.x + 800, window.y + 582), 0xff000000);

	//down rect ("V4 for Counter-Strike: Global Offensive")
	drawlist->AddRectFilled(ImVec2(window.x, window.y + 582), ImVec2(window.x + 800, window.y + 600), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.412f)));
	gen_random(bufrand, 8);

	drawlist->AddText(ImVec2(window.x + 5, window.y + 584), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), bufrand);
	drawlist->AddText(ImVec2(window.x + 5, window.y + 584), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "         LISA Stable V6 for Counter-Strike: Global Offensive");
	drawlist->AddText(ImVec2(window.x + 650, window.y + 584), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "https://WestFoxy.net");

	//CHilds backgrounds

	//RED BACKGROUND in the top
	drawlist->AddRectFilled(ImVec2(window.x, window.y), ImVec2(window.x + 800, window.y + 55), color_test);

	//Red line under tabs
	drawlist->AddRectFilled(ImVec2(window.x, window.y + 55), ImVec2(window.x + 800, window.y + 55 + 5), ImGui::GetColorU32(ImVec4(0.75f, 0.0f, 0.0f, 1.0f)));

	//LOGO AIMWARE.NET
	drawlist->AddImage(logo_tex, ImVec2(window.x + 8, window.y + 5), ImVec2(window.x + 225, window.y + 52));


	ImGui::Dummy(ImVec2(242.0f, 20.0f));
	ImGui::SameLine();
	//__________________________________TABS
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0, 0, 0.3f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.983, 0.114, 0.114, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));


	if (active_tab == 0)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));

		ImGui::ButtonGradient("Legit", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Legit", ImVec2(80, 55)))
			active_tab = 0;
	}
	ImGui::SameLine();
	if (active_tab == 1)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Rage", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Rage", ImVec2(80, 55)))
			active_tab = 1;
	}
	ImGui::SameLine();
	if (active_tab == 2)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Vis", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Vis", ImVec2(80, 55)))
			active_tab = 2;
	}
	ImGui::SameLine();
	if (active_tab == 3)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Misc", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Misc", ImVec2(80, 55)))
			active_tab = 3;
	}
	ImGui::SameLine();
	if (active_tab == 4)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Color", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Color", ImVec2(80, 55)))
			active_tab = 4;
	}
	ImGui::SameLine();
	if (active_tab == 5)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::ButtonGradient("Conf", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Conf", ImVec2(80, 55)))
			active_tab = 5;
	}
	ImGui::SameLine();
	if (active_tab == 6)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.983, 0.114, 0.114, 0.800));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.983, 0.114, 0.114, 0.800));

		ImGui::ButtonGradient("CMD", ImVec2(80, 55));
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("CMD", ImVec2(80, 55)))
			active_tab = 6;
	}

	ImGui::PopStyleColor(4);

	//__________________________________
	if (active_tab == 0)
		RenderLegitTab();
	else if (active_tab == 1)
		RenderRageTabAW();
	else if (active_tab == 2)
		RenderVisuals();
	else if (active_tab == 3)
		RenderMiscTab();
	else if (active_tab == 4)
		ColorsMenu();
	else if (active_tab == 5)
		ConfigMenu();
	else if (active_tab == 6)
		RenderCmd();

	ImGui::End();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);

	
}



void AWFoxMenu::Init(IDirect3DDevice9* dev)
{
	_visible = false;

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
	ImGui::CreateContext();
	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), dev);


	D3DDEVICE_CREATION_PARAMETERS cparams;
	RECT rect;

	dev->GetCreationParameters(&cparams);
	GetWindowRect(cparams.hFocusWindow, &rect);

	wx = rect.right;
	wh = rect.bottom;


	//g_EngineClient->GetScreenSize(wx, wh);
	

	//Redtheme();
	Redtheme();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = 9.0f;
	style.ScrollbarRounding = 0.0f;
	style.WindowRounding = 0.0f;

	D3DXCreateTextureFromFileInMemory(dev, groupBox, sizeof(groupBox), &groupBox_tex);

	/*D3DXCreateTextureFromFileInMemoryEx(dev
		, groupBox, sizeof(groupBox),
		283, 516, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &groupBox_tex);*/
	D3DXCreateTextureFromFileInMemory(dev, logo_data, sizeof(logo_data), &logo_tex);
	D3DXCreateTextureFromFileInMemory(dev, down_shadow, sizeof(down_shadow), &downshadow_tex);
	D3DXCreateTextureFromFileInMemory(dev, deagle_png, sizeof(deagle_png), &deagle_tex);

	D3DXCreateTextureFromFileInMemory(dev, scar20_png, sizeof(scar20_png), &scar20_tex);
	D3DXCreateTextureFromFileInMemory(dev, awp_png, sizeof(awp_png), &awp_tex);
	D3DXCreateTextureFromFileInMemory(dev, ak47_png, sizeof(ak47_png), &ak47_tex);
	D3DXCreateTextureFromFileInMemory(dev, ssg08_png, sizeof(ssg08_png), &ssg08_tex);
	D3DXCreateTextureFromFileInMemory(dev, ump_png, sizeof(ump_png), &ump_tex);
	D3DXCreateTextureFromFileInMemory(dev, glock_png, sizeof(glock_png), &glock_tex);

	

	/*D3DXCreateTextureFromFileInMemoryEx(dev
		, logo_data, sizeof(logo_data),
		400, 90, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logo_tex);
	D3DXCreateTextureFromFileInMemoryEx(dev
		, down_shadow, sizeof(down_shadow),
		800, 50, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &downshadow_tex);*/

	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.9f, 0, 0, 0.9);
	ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(0.80, 0, 0, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.840, 0.840, 0.840, 0.243f);
	ImGui::GetStyle().FrameRounding = 2;
}




void AWFoxMenu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void AWFoxMenu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void AWFoxMenu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void AWFoxMenu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void AWFoxMenu::Toggle()
{
	cl_mouseenable->SetValue(_visible);
	_visible = !_visible;
}


