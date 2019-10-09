#define NOMINMAX


#include "FoxyMenu.h"
FoxMenu g_LisaMenu;
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
#include "SkinChanger.h"
#include <fstream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"
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
			ImGui::TextColored(ImVec4(g_Options.rage_forcebaim_STATE ? 0.0f : 1.0f, 0, g_Options.rage_forcebaim_STATE ? 1.0f : 0.0f, 1.0f), "RAGE");
		else
			ImGui::Text("RAGE");

		ImGui::SameLine();

		if (g_Options.ForceBody)
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "BODY");
		else
			ImGui::Text("BODY");

		/*time_t current_time;
		struct tm* time_info;
		static char timeString[32];

		time(&current_time);
		time_info = localtime(&current_time);

		strftime(timeString, 32, "[%H:%M:%S]", time_info);

		ImGui::Separator();
		ImGui::SetCursorPosX(5);
		//ImGui::Text("%s %.1f fps %.2f ms", timeString, ImGui::GetIO().Framerate / 2, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%s", timeString);*/
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
inline void set_name_s(const char* name)
{
	ConVar* nameConvar = g_CVar->FindVar(("name"));
	*(int*)((DWORD)& nameConvar->fnChangeCallback + 0xC) = NULL;
	nameConvar->SetValue(name);
}
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

//SPEC_LIST______________________________
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
//_______________________________________

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
const char* RageBaimMode[] = {
	"Multipoint pelvis",
	"Center pelvis"
};
const char* rage_autostopmodes[] = {
	"Minimal speed [buggy]",
	"Full"
};
const char* rage_baimmodes[] = {
	"Hold",
	"Switch"
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
static bool emptyF = false;
void render_esp()
{
	
	ImGui::Columns(3);
	ImGui::Text("Box"); ImGui::NextColumn();
	ImGui::Text("Chams"); ImGui::NextColumn();
	ImGui::Text("Glow"); ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Checkbox("Enabled", &g_Options.esp_enabled);
	/////////////////////////////////////////
	 //ImGui::SameLine(); ImGuiEx::ColorEdit3("Enemies Visible", &g_Options.color_esp_enemy_visible);
	ImGui::SliderFloat("ESP Fade speed", &g_Options.esp_fade_speed, 0.01f, 1, "%.3f");
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
	ImGui::Checkbox("Sleeves", &g_Options.chams_arms_enabled);
	ImGui::Checkbox("No smoke", &g_Options.esp_no_smoke);

	ImGui::SliderFloat("Fake chams R", &g_Options.fake_chams_r, 0, 5);
	ImGui::SliderFloat("Fake chams G", &g_Options.fake_chams_g, 0, 5);
	ImGui::SliderFloat("Fake chams B", &g_Options.fake_chams_b, 0, 5);
	ImGui::SliderFloat("Fake chams A", &g_Options.fake_chams_a, 0, 1);
	//g_Options.esp_no_smoke
	//ImGui::Checkbox("No hands", &g_Options.misc_no_hands);
	/*OPTION(bool, misc_bullettracers, false);
	OPTION(Color, misc_bullettracers_color, Color(16,0,255,255));
	//esp_bullettrace_length
	OPTION(float, esp_bullettrace_length, 5);*/
	

	
	////////////////////////////////////////

	ImGui::NextColumn();
	ImGui::Checkbox("Noflash", &g_Options.flash_remove);

	ImGui::Checkbox("Enabled##Chams", &g_Options.chams_player_enabled);
	ImGui::Checkbox("Enemy", &g_Options.theirchams); ImGui::SameLine(); ColorPickerBox("##E", g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, false);
	ImGui::Checkbox("Local", &g_Options.yourteamchams); ImGui::SameLine(); ColorPickerBox("##T", g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, false);
	ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Regular", &g_Options.chams_player_regular);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat", &g_Options.chams_player_flat);
	//ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Regular XQZ", &g_Options.chams_player_regular_IgnoreZ);
	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat XQZ", &g_Options.chams_player_flat_IgnoreZ);

	ImGui::NextColumn();
	ImGui::Checkbox("Enabled##Glow", &g_Options.glow_enabled); ImGui::SameLine();
	ImGui::Checkbox("Team check##Glow", &g_Options.glow_enemies_only);
	ImGui::Combo("Glow Type##Glow", &g_Options.GlowType, GlowTypeARR, ARRAYSIZE(GlowTypeARR));
	ImGui::Checkbox("Players##Glow", &g_Options.glow_players);
	AWColorEdit4("Enemy##Glow", &g_Options.color_glow_enemy);
	AWColorEdit4("Ally##Glow", &g_Options.color_glow_ally);
	ImGui::Checkbox("C4 Carrier##Glow", &g_Options.glow_c4_carrier); ImGui::SameLine(); AWColorEdit4("##C4 Carrier", &g_Options.color_glow_c4_carrier);
	ImGui::Checkbox("Planted C4##Glow", &g_Options.glow_planted_c4); ImGui::SameLine(); AWColorEdit4("##C4", &g_Options.color_glow_planted_c4);

	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Checkbox("Bullet Tracers", &g_Options.misc_bullettracers); ImGui::SameLine(); AWColorEdit4("##bullettr_col", &g_Options.misc_bullettracers_color);
	ImGui::SliderFloat("Time##bullet_trace_tm", &g_Options.esp_bullettrace_length, 1.0f, 15.0f, "%.1f");
	ImGui::Checkbox("DLight Tracers", &g_Options.misc_bullettracers_dlight); ImGui::SameLine(); AWColorEdit4("##bullettr_delcol", &g_Options.misc_bullettracers_dlightcolor);
	ImGui::InputFloat("DLight radius", &g_Options.misc_bullettracersdlight_radius);
	ImGui::InputFloat("DLight delta", &g_Options.misc_bullettracersdlight_delta);
}
const char* clantags_type[] =
{
	"Normal", "L!SA", "AW", "?", "L!SA", "Namesc", "fox fuck", "L!SA glitch"
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
static char inp_glove[128] = "";

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
static ComboFilterState fil_glove = { 0, true };

void render_paste_skins()
{
	
	if (ImGui::Checkbox("Enabled##Skinchanger", &g_Options.skinchanger_enabled))
		Skinchanger::Get().bForceFullUpdate = true;
	std::vector<EconomyItem_t>& entries = Skinchanger::Get().GetItems();
	// If the user deleted the only config let's add one
	if (entries.size() == 0)
		entries.push_back(EconomyItem_t());
	static int selected_id = 0;
	ImGui::Columns(2, nullptr, false);
	// Config selection
	{
		ImGui::PushItemWidth(-1);
		char element_name[64];
		ImGui::ListBox("##skinchangerconfigs", &selected_id, [&element_name, &entries](int idx)
			{
				sprintf_s(element_name, "%s (%s)", entries.at(idx).name, k_weapon_names.at(entries.at(idx).definition_vector_index).name);
				return element_name;
			}, entries.size(), 15);
		ImVec2 button_size = ImVec2(ImGui::GetColumnWidth() / 2 - 12.8f, 25);
		if (ImGui::Button("Add Item", button_size))
		{
			entries.push_back(EconomyItem_t());
			selected_id = entries.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove Item", button_size))
			entries.erase(entries.begin() + selected_id);
		ImGui::PopItemWidth();
	}
	ImGui::NextColumn();
	selected_id = selected_id < int(entries.size()) ? selected_id : entries.size() - 1;
	EconomyItem_t& selected_entry = entries[selected_id];
	{
		// Name
		ImGui::InputText("Name", selected_entry.name, 32);
		ImGui::Dummy(ImVec2(1, 4));
		// Item to change skins for
		ImGui::Combo("Item", &selected_entry.definition_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_weapon_names[idx].name;
				return true;
			}, nullptr, k_weapon_names.size(), 5);
		ImGui::Dummy(ImVec2(1, 3));
		// Enabled
		ImGui::Checkbox("Enabled", &selected_entry.enabled);
		ImGui::Dummy(ImVec2(1, 3));
		// Pattern Seed
		ImGui::InputInt("Seed", &selected_entry.seed);
		ImGui::Dummy(ImVec2(1, 4));
		// Custom StatTrak number
		ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
		ImGui::Dummy(ImVec2(1, 4));
		// Wear Float
		ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);
		ImGui::Dummy(ImVec2(1, 4));
		// Paint kit
		
		if (selected_entry.definition_index != GLOVE_T_SIDE)
		{
			static char paintkit_input[255] = { 0 };
			static ComboFilterState fState = { 0,true };
			/*ImGui::Combo("PaintKit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_skins[idx].name.c_str();
					return true;
				}, nullptr, k_skins.size(), 10);*/

			bool res = ComboFilter("Paintkit", paintkit_input, 255, k_skins, k_skins.size(), fState);
			if (selected_entry.paint_kit_vector_index != fState.activeIdx && res)
			{
				selected_entry.paint_kit_vector_index = fState.activeIdx;
				Skinchanger::Get().bForceFullUpdate = true;
			}
			ImGui::Text("Paintkit id: %d", selected_entry.paint_kit_vector_index);

		}
		else
		{
			/*ImGui::Combo("PaintKit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_gloves[idx].name.c_str();
					return true;
				}, nullptr, k_gloves.size(), 10);*/

			static char paintkit_input[255] = { 0 };
			static ComboFilterState fState = { 0,true };
			

			bool res = ComboFilter("Paintkit", paintkit_input, 255, k_gloves, k_gloves.size(), fState);
				if (selected_entry.paint_kit_vector_index != fState.activeIdx && res)
				{
					selected_entry.paint_kit_vector_index = fState.activeIdx;
					Skinchanger::Get().bForceFullUpdate = true;
				}
			ImGui::Text("Paintkit id: %d", selected_entry.paint_kit_vector_index);
		}
		ImGui::Dummy(ImVec2(1, 4));
		// Quality
		ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_quality_names[idx].name;
				return true;
			}, nullptr, k_quality_names.size(), 5);
		ImGui::Dummy(ImVec2(1, 4));
		// Yes we do it twice to decide knifes
		selected_entry.UpdateValues();
		// Item defindex override
		if (selected_entry.definition_index == WEAPON_KNIFE)
		{
			ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_knife_names.at(idx).name;
					return true;
				}, nullptr, k_knife_names.size(), 5);
		}
		else if (selected_entry.definition_index == GLOVE_T_SIDE)
		{
			ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = k_glove_names.at(idx).name;
					return true;
				}, nullptr, k_glove_names.size(), 5);
		}
		else
		{
			// We don't want to override weapons other than knives or gloves
			static auto unused_value = 0;
			selected_entry.definition_override_vector_index = 0;
			ImGui::Combo("Unavailable", &unused_value, "Only available for knives or gloves!\0");
		}
		ImGui::Dummy(ImVec2(1, 4));
		selected_entry.UpdateValues();
		// Custom Name tag
		ImGui::InputText("Nametag", selected_entry.custom_name, 32);
		ImGui::Dummy(ImVec2(1, 4));
	}
	ImGui::NextColumn();
	ImGui::Columns(1, nullptr, false);
	ImGui::Separator();
	ImGui::Dummy(ImVec2(1, 10));
	ImGui::Columns(3, nullptr, false);
	ImGui::PushItemWidth(-1);
	// Lower buttons for modifying items and saving
	{
		ImVec2 button_size = ImVec2(ImGui::GetColumnWidth() - 17.f, 25);
		if (ImGui::Button("Force Update##Skinchanger", button_size))
			Skinchanger::Get().bForceFullUpdate = true;
		ImGui::NextColumn();
		if (ImGui::Button("Save##Skinchanger", button_size))
			Skinchanger::Get().SaveSkins();
		ImGui::NextColumn();
		if (ImGui::Button("Load##Skinchanger", button_size))
			Skinchanger::Get().LoadSkins();
		ImGui::NextColumn();
	}
	ImGui::PopItemWidth();


}

void render_skins()
{
	static const char* weap_selection[] =
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
		"Knife",
		"Glove"
	};
	static const char* glove_id_selection[] =
	{
		"Bloodhound",
		"T",
		"CT",
		"Sporty",
		"Slick",
		"Handwrap",
		"Motorcycle",
		"Specialist"
	};
	


	ImGui::Combo("Knife model", &g_Options.knifemodel, knifeNames, ARRAYSIZE(knifeNames));
	ImGui::Separator();
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

	case 17:
		ImGui::Text("Gloves beta##glove");
		ComboFilter("Skin##glove", inp_glove, IM_ARRAYSIZE(inp_glove), k_gloves, k_gloves.size(), fil_glove);
		if (ImGui::Button("Set##glove"))
			g_Options.glove_paint_kit = k_gloves[fil_glove.activeIdx].id;

		ImGui::Combo("Model##glove", &g_Options.glove_index, glove_id_selection, ARRAYSIZE(glove_id_selection));
		ImGui::Text("Paintkit = %d ModelIndex = %d", g_Options.glove_paint_kit, g_Options.glove_index);
		
		/*case 0:
				glove_name = "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl";
				break;
			case 1:
				glove_name = "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl";
				break;
			case 2:
				glove_name = "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl";
				break;
			case 3:
				glove_name = "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl";
				break;
			case 4:
				glove_name = "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl";
				break;
			case 5:
				glove_name = "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl";
				break;
			case 6:
				glove_name = "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl";
				break;
			case 7:
				glove_name = "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl";
				break;*/



		break;
	default:
		break;
	}


	if (ImGui::Button("Force full update", ImVec2(120, 35)))
		g_ClientState->ForceFullUpdate();
}

const char* predefined_names[] =
{
	"Fuck a fox",
	"Fuck Fox pussy \xe2\x99\xa5",
	"lisik",
	"FoxyFox",
	"WestFoxy",
	"Fuck Ksy\xe2\x99\xa5sha",
	"Fuck Ksyusha",
	"Ksy\xe2\x99\xa5sha"
};
struct pplayer
{
	std::string name;
	int index, team;

	pplayer(std::string nm1, int ind1, int tm1)
	{
		name = nm1;
		index = ind1;
		team = tm1;
	}
};
std::vector <pplayer>players_map;


void render_players()
{
	players_map.clear();

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
		{
			C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
			if (!player)
				continue;
			if (!player->IsPlayer() || (player == g_LocalPlayer))
				continue;
			players_map.emplace_back(player->GetName(), i, player->m_iTeamNum());
		}
	}

	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Fox players list"))
	{
		// left
		static int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		/*for (int i = 0; i < 100; i++)
		{
			char label[128];
			sprintf(label, "MyObject %d", i);
			if (ImGui::Selectable(label, selected == i))
				selected = i;
		}*/
		if (players_map.size() != 0)
		{
			for (int i = 0; i < players_map.size(); i++)
			{
				if (ImGui::Selectable(players_map[i].name.c_str(), selected == i))
					selected = i;
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();

		// right
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && players_map.size() > 0 && selected < players_map.size())
		{
			ImGui::Text("Name: %s", players_map[selected].name.c_str());
			ImGui::Text("Entity index: %d", players_map[selected].index);
			ImGui::Text("Team index: %d", players_map[selected].team);
		}
		else
		{
			ImGui::Text("Name: ");
			ImGui::Text("Entity index: ");
			ImGui::Text("Team index: ");
		}
		ImGui::Separator();

		if (ImGui::Button("Steal name"))
		{
			players_map[selected].name += " ";
			set_name_s(players_map[selected].name.c_str());

		}
		if (ImGui::Button("Steal to fake name [clan tag clitch]"))
		{
			std::string temp1 = players_map[selected].name;
			temp1 += "\n";
			SetClantag(temp1.c_str());
		}
		ImGui::EndChild();
		if (ImGui::Button("Revert")) {}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {}
		ImGui::EndGroup();
		ImGui::End();
	}
	
}

void render_misc()
{
	ImGui::Columns(3);
	ImGui::Text("Movement");
	ImGui::NextColumn();
	ImGui::Text("Automation");
	ImGui::NextColumn();
	ImGui::Text("Enchancement");
	ImGui::NextColumn();
	ImGui::Separator();

	ImGui::Checkbox("Bhop", &g_Options.misc_bhop);
	ImGui::Checkbox("Auto strafe", &g_Options.misc_autostrafe);
	ImGui::Checkbox("Fast duck", &g_Options.misc_fastduck);

	ImGui::Checkbox("Circle strafe", &g_Options.misc_circle_enable);
	if (g_Options.misc_circle_enable)
		ImGui::Hotkey("Circle Key", &g_Options.misc_circle_key);

	ImGui::Checkbox("Edge jump", &g_Options.misc_edge_jump);
	ImGui::Checkbox("Edge jump strafe", &g_Options.misc_edge_jump_strafe);
	if (g_Options.misc_edge_jump)
		ImGui::Hotkey("Edge Key", &g_Options.misc_edge_jumpkey);

	ImGui::SliderInt("Fake lag", &g_Options.FakelagAmount, 0, 50);

	ImGui::Checkbox("Legit AA", &g_Options.misc_legitdesync);
	if (g_Options.misc_legitdesync)
	{
		ImGui::Hotkey("L AA flip", &g_Options.misc_legitdesync_keyflip);
		ImGui::Text(g_Options.misc_legitdesync_flip ? "Flipped: true" : "Flipped: false");
	}

	ImGui::NextColumn();
	static char cln_tag_input[259] = { 0 };
	ImGui::InputText("Clan tag##cl_tag_inp", cln_tag_input, 256, ImGuiInputTextFlags_Multiline); ImGui::SliderFloat("Speed##clan_spedd", &g_Options.misc_clantag_speed, 0.01, 1.5f, "%.1f");

	if (ImGui::Button("Set"))
		SetClantag(cln_tag_input);
	ImGui::SameLine();
	if (ImGui::Button("Fake name"))
	{
		std::string dtmp = cln_tag_input;
		dtmp += "\n";
		SetClantag(dtmp.c_str());

	}
	ImGui::SameLine();
	if (ImGui::Button("Set to animation"))
		g_Options.gladTag = cln_tag_input;

	ImGui::Checkbox("Animated", &g_Options.clan_tag_animate);
	ImGui::Combo("Types", &g_Options.misc_clantag_type, clantags_type, ARRAYSIZE(clantags_type));

	
	static int nm_predefined_select = 0;
	static char nm_input[256] = { 0 };

	ImGui::InputText("Name input##nm_input", nm_input, 256, ImGuiInputTextFlags_Multiline);
	ImGui::Combo("Predefined", &nm_predefined_select, predefined_names, ARRAYSIZE(predefined_names));
	if (ImGui::Button("Set predefined"))
	{
		set_name_s(predefined_names[nm_predefined_select]);
	}
	ImGui::SameLine();
	if (ImGui::Button("Set##set_anme"))
		set_name_s(nm_input);
	ImGui::SameLine();
	if (ImGui::Button("Infinite change"))
		set_name_s("\n\xAD\xAD\xAD");

	ImGui::Checkbox("Name stealer", &g_Options.misc_namestealer); ImGui::SameLine(); ImGui::SliderFloat("Speed", &g_Options.misc_namestealer_speed, 0.f, 1.5f, "%.1f");
	ImGui::Checkbox("Name spam##spammer_nm", &g_Options.misc_namespam);

	ImGui::Checkbox("Spectators list", &g_Options.spec_list);


	ImGui::NextColumn();

	if (ImGui::Button("Clear exception"))
		g_Options.CreateMove_Crash_State = -1;

	static bool PlayersList_enable = false;
	ImGui::Checkbox("Players list", &PlayersList_enable);
	if (PlayersList_enable)
		render_players();

	ImGui::Checkbox("Overlay info", &g_Options.misc_overlay);
	ImGui::Combo("Hitsound", &g_Options.Hitsound, hitsound_types, ARRAYSIZE(hitsound_types));
	ImGui::Checkbox("Reveal rank", &g_Options.misc_revealranks);
	ImGui::Checkbox("Events log", &g_Options.misc_foxlog);
	ImGui::Checkbox("Third person", &g_Options.misc_thirdperson);
	ImGui::SliderFloat("Distance", &g_Options.misc_thirdperson_dist, 0, 350, "%.1f");
	ImGui::Hotkey("Third person key", &g_Options.misc_thirpers_key);
	if (ImGui::Button("Night Mode"))
		g_Nightmode.apply();

	ImGui::SameLine();
	if (ImGui::Button("remove"))
		g_Nightmode.remove();
	ImGui::SliderFloat("World ambient r", &g_Options.mat_ambient_light_r, 0.0, 5.0f);
	ImGui::SliderFloat("World ambient g", &g_Options.mat_ambient_light_g, 0.0, 5.0f);
	ImGui::SliderFloat("World ambient b", &g_Options.mat_ambient_light_b, 0.0, 5.0f);

	ImGui::Checkbox("Paws trails", &g_Options.misc_foot_trails);
	ImGui::SliderFloat("Trails life time", &g_Options.misc_foot_trails_lenght, 0.1, 25);
	ImGui::SliderFloat("Trails start width", &g_Options.misc_foot_trails_startsize, 0.1, 25);
	ImGui::SliderFloat("Trails end width", &g_Options.misc_foot_trails_endsize, 0.1, 25);
	ImGui::SliderFloat("Trails delay between", &g_Options.misc_foot_trails_delay_between, 0.000001, 1);
	ImGui::SliderInt("Trails segments", &g_Options.misc_foot_trails_segments, 1, 15);
	ImGui::SliderFloat("Trails x shift", &g_Options.misc_foot_trails_xshift, 0.1, 40);
	ImGui::SliderFloat("Trails y shift", &g_Options.misc_foot_trails_yshift, 0.1, 40);
	ImGui::SliderFloat("Trails z shift", &g_Options.misc_foot_trails_zshift, 0.1, 40);
	ImGui::SliderFloat("Trails rainbow delta", &g_Options.misc_foot_trails_rainbowdelta, 0.000001, 1);

	ImGui::Columns(1);
	ImGui::Separator();
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

}

void render_stemapi()
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
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "ISteamLobby");
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
}

void render_rage()
{
	static int Weapon_selection = 0;

	if (ImGui::ImageButton(g_LisaMenu.scar20_tex, ImVec2(110 - 29, 15), ImVec2(0,0), ImVec2(1,1), -1, ImVec4(0,0,0,0), ImVec4(1,1,1, Weapon_selection == 0 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 0;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(g_LisaMenu.ssg08_tex, ImVec2(114 - 29, 15), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, Weapon_selection == 1 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 1;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(g_LisaMenu.awp_tex, ImVec2(130 - 29, 15), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, Weapon_selection == 2 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 2;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(g_LisaMenu.deagle_tex, ImVec2(34, 15), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, Weapon_selection == 3 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 3;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(g_LisaMenu.glock_tex, ImVec2(37, 15), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, Weapon_selection == 4 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 4;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(g_LisaMenu.ak47_tex, ImVec2(99 - 29, 15), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, Weapon_selection == 5 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 5;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(g_LisaMenu.ump_tex, ImVec2(77 - 29, 15), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, Weapon_selection == 6 ? 0.3f : 1.0f)))
	{
		Weapon_selection = 6;
	}

	ImGui::Separator();
	ImGui::Columns(3);

	ImGui::Text("Weapons");
	ImGui::NextColumn();
	ImGui::Text("Anti Aim");
	ImGui::NextColumn();
	ImGui::Text("Features");
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Checkbox("Enable", &g_Options.rage_enabled);
	ImGui::Checkbox("Bind##raggbind", &g_Options.rage_enabled_bind);
	/*OPTION(bool, rage_enabled, false);
	OPTION(bool, rage_enabled_bind, false);
	OPTION(int, rage_enabled_bindkey, VK_CAPITAL);*/
	if (g_Options.rage_enabled_bind)
	{
		ImGui::Hotkey("Rage switch bind", &g_Options.rage_enabled_bindkey);
	}
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
	ImGui::Checkbox("Resolver", &g_Options.rage_resolver);
	
	ImGui::NextColumn();
	

	const char* freestand_modeR[] = { "Threat", "Crosshair", "Distance" };
	const char* pitchR[5] = { "Disabled", "Down", "Up", "Origin", "Offset" };
	const char* yawR[5] = { "Disabled", "Backwards", "Origin", "Spin", "Manual" };
	const char* desyncR[6] = { "Disabled", "Unused", "Normal", "Balance experimental", "Jitter", "Static fox" };

	ImGui::Checkbox(("Anti-aim"), &g_Options.antiaim_enabled);
	ImGui::Combo("Desync type##stand", &g_Options.desync, desyncR, ARRAYSIZE(desyncR));

	ImGui::Text("[Standing]");
	ImGui::Combo("Pitch [X]##stand", &g_Options.pitch_standing, pitchR, ARRAYSIZE(pitchR));
	ImGui::Combo("Yaw [Y]##stand", &g_Options.yaw_standing, yawR, ARRAYSIZE(yawR));
	

	ImGui::Text("[Moving]");
	ImGui::Combo("Pitch [X]##move", &g_Options.pitch_moving, pitchR, ARRAYSIZE(pitchR));
	ImGui::Combo("Yaw [Y]##move", &g_Options.yaw_moving, yawR, ARRAYSIZE(yawR));
	

	ImGui::Text("[Air]");
	ImGui::Combo("Pitch [X]##air", &g_Options.pitch_air, pitchR, ARRAYSIZE(pitchR));
	ImGui::Combo("Yaw [Y]##air", &g_Options.yaw_air, yawR, ARRAYSIZE(yawR));
	


	ImGui::Checkbox("Freestanding", &g_Options.antiaim_freestanding);
	ImGui::Combo("Freestand mode", &g_Options.antiaim_freestanding_mode, freestand_modeR, ARRAYSIZE(freestand_modeR));
	ImGui::SliderInt("Jitter range", &g_Options.aa_jitter, 1, 180);
	

	if (g_Options.aa_real_type == 4)
	{
		ImGui::Hotkey(("left"), &g_Options.aa_left);
		ImGui::Hotkey(("right"), &g_Options.aa_right);
		ImGui::Hotkey(("back"), &g_Options.aa_back);
	}


	ImGui::NextColumn();


	ImGui::Checkbox("Lines fake", &g_Options.misc_fake_angles);
	ImGui::Checkbox("Fakeduck", &g_Options.FakeDuck);
	ImGui::Hotkey("Fakeduck key##key", &g_Options.FakeDuckKey);
	//ImGui::SliderInt("Fakeduck delta", &g_Options.FakeDuckTestAmount, 0, 35);
	ImGui::Checkbox("Fakelag", &g_Options.FakeLagEnable);
	ImGui::SliderInt("Fakelag amount", &g_Options.FakelagAmount, 2, 14);

	ImGui::Checkbox("Backwards AA head scan", &g_Options.rage_backward_head_scan);

	ImGui::Checkbox("BAIM if lethal", &g_Options.rage_baimlethal);
	ImGui::Combo("Baim mode", &g_Options.rage_baimlethal_mode, RageBaimMode, ARRAYSIZE(RageBaimMode));

	ImGui::SliderInt("BAIM if lower than", &g_Options.rage_baimautodamage, 1, 100);
	

	ImGui::Checkbox("Auto stop", &g_Options.rage_autostop);
	ImGui::Combo("Auto stop mode", &g_Options.rage_autostop_mode, rage_autostopmodes, ARRAYSIZE(rage_autostopmodes));

	ImGui::Checkbox("Autoscope", &g_Options.rage_autoscope);
	ImGui::Checkbox("Autocrouch", &g_Options.rage_autocrouch);

	ImGui::Checkbox("FBAIM on key", &g_Options.rage_forcebaim_bool);
	ImGui::Hotkey("FBAIM key", &g_Options.rage_forcebaim_key);
	ImGui::Combo("FBAIM mode", &g_Options.rage_forcebaim_mode, rage_baimmodes, ARRAYSIZE(rage_baimmodes));

	ImGui::Checkbox(("Slow Walk"), &g_Options.rage_SlowWalk);
	ImGui::SliderFloat("Speed", &g_Options.rage_SlowWalk_speed, 0.01f, 50.0f);
	if (g_Options.rage_SlowWalk)
		ImGui::Hotkey(("##slow_walkk"), &g_Options.rage_SlowWalk_key);

}

void render_legit()
{
	static int legit_weap_selection = 0;
	if (ImGui::Button("Pistols", ImVec2(100, 25)))
		legit_weap_selection = 0;
	ImGui::SameLine();
	if (ImGui::Button("Rifles", ImVec2(100, 25)))
		legit_weap_selection = 1;
	ImGui::SameLine();
	if (ImGui::Button("Snipers", ImVec2(100, 25)))
		legit_weap_selection = 2;

	if (legit_weap_selection == 0)
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Fox] ");
		ImGui::SameLine();
		ImGui::Text("Pistols selection");
		ImGui::SliderFloat("FOV", &g_Options.legit_fov_pistols, 0, 180, "%.1f");
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_pistols, 1, 6, "%.1f");
		//ImGui::Checkbox("RCS", &empty2);
		//ImGui::Checkbox("Autowall", &empty2);
	}
	else if (legit_weap_selection == 1)
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Fox] ");
		ImGui::SameLine();
		ImGui::Text("Rifles selection");
		ImGui::SliderFloat("FOV", &g_Options.legit_fov_rifles, 0, 180, "%.1f");
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_rifles, 1, 6, "%.1f");
		//ImGui::Checkbox("RCS", &empty2);
		//ImGui::Checkbox("Autowall", &empty2);
	}
	else if (legit_weap_selection == 2)
	{
		ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "[Fox] ");
		ImGui::SameLine();
		ImGui::Text("Snipers selection");
		ImGui::SliderFloat("FOV", &g_Options.legit_fov_Snipers, 0, 180, "%.1f");
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_Snipers, 1, 6, "%.1f");
		//ImGui::Checkbox("RCS", &empty2);
		//ImGui::Checkbox("Autowall", &empty2);
	}

}
#include "Resolver.h"
void render_cmd()
{
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Love fox");
	ImGui::Text("Website: https://WestFoxy.net");
	ImGui::Text("VK: https://vk.com/westfoxx");
	ImGui::Text("Steam: https://steamcommunity.com/id/WestFoxy00");

	//auto* drawl = ImGui::GetWindowDrawList();
	//drawl->
	ImGui::Image(g_LisaMenu.redfox_tex, ImVec2(756, 503));
	
	
	for (int i = 0; i < 32; i++)
	{
		ImGui::Text("%d) %d %d %d", i, ResolveData::Shot[i], ResolveData::Hit[i], ResolveData::MissedShots[i]);
	}
}

void render_config()
{
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



void FoxMenu::Run()
{
	if (g_Options.spec_list)
		DrawingSpectatorList();

	if (g_Options.misc_overlay)
		OverlayInfo(&g_Options.misc_overlay);

	ImGui::GetIO().MouseDrawCursor = _visible;

	if (!_visible)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		return;
	}


	static const char* tabs[] = {
				"Rage",
				"Legit",
				"Visuals",
				"Misc",
				"SkinChanger",
				"Steam API",
				"Console"
	};

	auto drawlist = ImGui::GetOverlayDrawList();

	

	static int page = 0;
	time_t current_time;
	struct tm* time_info;
	static char timeString[32];

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, 32, "L!SA [%H:%M:%S]###AnimatedTitle", time_info);
	static bool isColorsEnabled = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Colors menu"))
		{
			if (ImGui::MenuItem("Enable", "Color")) {
				isColorsEnabled = !isColorsEnabled;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Log"))
		{
			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	if (isColorsEnabled)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_Once);
		ImGui::Begin("Colors menu");
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
		ImGui::End();
	}

	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(1.0f, abs(sin((float)GetTickCount() / 2000) * 0.4), 0.0f, 0.7f);

	//ImGui::GetStyle().Colors[ImGuiCol_Separator] = 

	ImGui::SetNextWindowSize(ImVec2(800, 600));
	ImGui::Begin(timeString, 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

	for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
	{
		int distance = i == page ? 0 : i > page ? i - page : page - i;

		if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 34)))
			page = i;

		if (i < IM_ARRAYSIZE(tabs) - 1)
			ImGui::SameLine();
	}
	ImGui::Separator();
	ImGui::BeginChild("SubTab");

	if (page == 0)
		render_rage();
	else if (page == 1)
		render_legit();
	else if (page == 2)
		render_esp();
	else if (page == 3)
		render_misc();
	else if (page == 4)
		render_paste_skins();
	else if (page == 5)
		render_stemapi();
	else if (page == 6)
		render_cmd();


	ImGui::EndChild();

	ImGui::End();

}

void FoxMenu::Init(IDirect3DDevice9* dev)
{
	_visible = false;

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
	ImGui::CreateContext();

	ImGui::GetIO().IniFilename = NULL;

	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), dev);

	//Monitor size-----------------
	D3DDEVICE_CREATION_PARAMETERS cparams;
	RECT rect;

	dev->GetCreationParameters(&cparams);
	GetWindowRect(cparams.hFocusWindow, &rect);

	this->_startX = rect.right / 2;
	this->_startY = rect.bottom / 2;
	//-----------------------------

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.68f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.63f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.12f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.16f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 0.21f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.98f, 0.00f, 0.00f, 0.45f);
	colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.34f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.115f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.162f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.07f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.00f, 1.00f, 0.372f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(1.00f, 0.00f, 0.00f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.49f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(1.00f, 0.00f, 0.00f, 0.43f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);

	ImGui::GetStyle().FrameBorderSize = 1.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;
	ImGui::GetStyle().ChildRounding = 0.0f;

	//------------------------------
	D3DXCreateTextureFromFileInMemory(dev, deagle_png, sizeof(deagle_png), &deagle_tex);
	D3DXCreateTextureFromFileInMemory(dev, scar20_png, sizeof(scar20_png), &scar20_tex);
	D3DXCreateTextureFromFileInMemory(dev, awp_png, sizeof(awp_png), &awp_tex);
	D3DXCreateTextureFromFileInMemory(dev, ak47_png, sizeof(ak47_png), &ak47_tex);
	D3DXCreateTextureFromFileInMemory(dev, ssg08_png, sizeof(ssg08_png), &ssg08_tex);
	D3DXCreateTextureFromFileInMemory(dev, ump_png, sizeof(ump_png), &ump_tex);
	D3DXCreateTextureFromFileInMemory(dev, glock_png, sizeof(glock_png), &glock_tex);
	D3DXCreateTextureFromFileInMemory(dev, redfox_png, sizeof(redfox_png), &redfox_tex);
	//-------------------------------

	
	TahomaFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	//ImGui::GetIO().Fonts->AddFontDefault();
}

void FoxMenu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void FoxMenu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void FoxMenu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void FoxMenu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void FoxMenu::Toggle()
{
	cl_mouseenable->SetValue(_visible);
	_visible = !_visible;
}


