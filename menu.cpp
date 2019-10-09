#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>
#include <d3dx9tex.h>
#include "imgdata.h"
#include "KitParser.h"

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "droid.hpp"
#include "Lobby.h"
#include "FoxSystem.h"
#include "SDK_lobby.h"
#include <fstream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"

/*struct ComboFilterState
{
	int  activeIdx;         // Index of currently 'active' item by use of up/down keys
	bool selectionChanged;  // Flag to help focus the correct item when selecting active item
};

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

static bool ComboFilter(const char *id, char *buffer, int bufferlen, vector <Kit_t>hints, int num_hints, ComboFilterState &s) {
	struct fuzzy {
		static int score(const char *str1, const char *str2) {
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
		static int search(const char *str, int num, std::vector <Kit_t>words) {
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

void StyleColorsDarkBlack()
{
	
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.78f, 0.78f, 0.93f, 0.88f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.90f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 0.90f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.00f, 0.00f, 0.31f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.00f, 0.99f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.04f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.00f, 0.00f, 0.65f);
	colors[ImGuiCol_Header] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);


	ImGuiStyle& style1 = ImGui::GetStyle();
	style1.Alpha = 1.0f;
	style1.WindowPadding = ImVec2(8, 8);
	style1.WindowMinSize = ImVec2(32, 32);
	style1.WindowRounding = 0.0f;
	//style.ChildWindowRounding = 0.0f;
	//style1.FramePadding = ImVec2(4, 3);
	style1.FrameRounding = 0;
	//style1.ItemSpacing = ImVec2(8, 4);
	//style1.ItemInnerSpacing = ImVec2(4, 4);
	style1.TouchExtraPadding = ImVec2(0, 0);
	style1.IndentSpacing = 21.0f;
	style1.ColumnsMinSpacing = 3.0f;
	style1.ScrollbarSize = 2.0f;
	style1.ScrollbarRounding = 0.0f;
	style1.GrabMinSize = 2.0f;
	style1.GrabRounding = 0;
	style1.DisplayWindowPadding = ImVec2(22, 22);
	style1.DisplaySafeAreaPadding = ImVec2(4, 4);
	style1.AntiAliasedLines = true;
	style1.WindowBorderSize = 0.0f;
	//style.AntiAliasedShapes = true;
	//style1.WindowTitleAlign = ImVec2(0.5, 0.5);

}

void orange()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.0f;
	//style.ChildWindowRounding = 0.0f;
	style.FramePadding = ImVec2(4, 3);
	style.FrameRounding = 0;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 2.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 2.0f;
	style.GrabRounding = 0;
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	//style.AntiAliasedShapes = true;
	style.WindowTitleAlign = ImVec2(0.5, 0.5);

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImColor(53, 59, 81);
	style.Colors[ImGuiCol_ChildWindowBg] = ImColor(0, 0, 0,46);
	style.Colors[ImGuiCol_Border] = ImColor(66, 72, 99);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(77, 86, 115);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(70, 79, 104);
	style.Colors[ImGuiCol_TitleBg] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.00f, 0.0f, 0.0f, 0.96f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImColor(53, 59, 81);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(101, 112, 142);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(97, 107, 135);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(90, 100, 127);
	style.Colors[ImGuiCol_ComboBg] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_CheckMark] = ImColor(25, 176, 215);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(25, 176, 215);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(27, 178, 217);
	style.Colors[ImGuiCol_Button] = ImColor(5, 5, 5, 249);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(31, 143, 207);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(25, 176, 215);
	style.Colors[ImGuiCol_Header] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(31, 143, 207);
	style.Colors[ImGuiCol_HeaderActive] = ImColor(25, 176, 215);
	style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_ResizeGripActive] = ImColor(64, 72, 95);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.0f, 0.0f, 0.86f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.87f, 0.87f, 0.87f, 0.87f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.87f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.09f, 0.43f, 0.0f);

}



static ConVar* cl_mouseenable = nullptr;

constexpr static float get_sidebar_item_width() { return 129.0f; }
constexpr static float get_sidebar_item_height() { return  40.0f; }

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

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
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
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, false);
	}
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0; i < N; ++i) {
		if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
			activetab = i;
		}
		if (sameline && i < N - 1)
			ImGui::SameLine();
	}
}

ImVec2 get_sidebar_size()
{

	constexpr float padding = 10.0f;

	constexpr auto size_w = 0.0f;
	constexpr auto size_h = 56.0f;


	return ImVec2{ size_w, size_h };
}

void RenderAimTab() 
{
	//ImGui::ShowDemoWindow();

	ImGui::Checkbox("Enabled", &g_Options.enable_legitbot);
	ImGui::Hotkey("Bind enable", &g_Options.enable_legitbot_key);
	ImGui::Hotkey("Bind force body", &g_Options.forcebody_legitbot_key);

	static int page = 0;

	const char* tabs[] = {
				"Pistols",
				"Rifles",
				"Snipers"
	};

	for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
	{
		int distance = i == page ? 0 : i > page ? i - page : page - i;

		if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
			page = i;

		if (i < IM_ARRAYSIZE(tabs) - 1)
			ImGui::SameLine();
	}

	ImGui::Separator();

	switch (page)
	{
	case 0:
		ImGui::Text("Pistols");
		ImGui::SliderFloat("Fov", &g_Options.legit_fov_pistols, 1.0f, 20.0f, "%.1f", 0.5f);
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_pistols, 1.0f, 5.0f, "%.1f", 0.5f);
		ImGui::SliderInt("Hitbox", &g_Options.hitbox_pistols, 1, 9);
		break;
	case 1:
		ImGui::Text("Rifles");
		ImGui::SliderFloat("Fov", &g_Options.legit_fov_rifles, 1.0f, 20.0f, "%.1f", 0.5f);
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_rifles, 1.0f, 5.0f, "%.1f", 0.5f);
		ImGui::SliderInt("Hitbox", &g_Options.hitbox_rifles, 1, 9);
		break;
	case 2:
		ImGui::Text("Snipers");
		ImGui::SliderFloat("Fov", &g_Options.legit_fov_Snipers, 1.0f, 20.0f, "%.1f", 0.5f);
		ImGui::SliderFloat("Smooth", &g_Options.legit_smooth_Snipers, 1.0f, 5.0f, "%.1f", 0.5f);
		ImGui::SliderInt("Hitbox", &g_Options.hitbox_snipers, 1, 9);
		break;
	}

	
}
const char *esp_types[] = {
	"Normal", "Outlined"
};

void RenderEspTab()
{
	static int   active_esp_tab = 0;

	const char* tabs[] = {
			"Box",
			"Chams",
			"Glow",
			"Menu customization"
	};

	for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
	{
		int distance = i == active_esp_tab ? 0 : i > active_esp_tab ? i - active_esp_tab : active_esp_tab - i;

		if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
			active_esp_tab = i;

		if (i < IM_ARRAYSIZE(tabs) - 1)
			ImGui::SameLine();
	}

	if (active_esp_tab == 0) {
		ImGui::Checkbox("Enabled", &g_Options.esp_enabled); //ImGui::SameLine(); ImGuiEx::ColorEdit3("Enemies Visible", &g_Options.color_esp_enemy_visible);
		ImGui::Checkbox("Team check", &g_Options.esp_enemies_only);
		ImGui::Checkbox("Boxes", &g_Options.esp_player_boxes); ImGui::SameLine(); ImGuiEx::ColorEdit3("##vis", &g_Options.color_esp_Box);
		ImGui::Combo   ("Box Type", &g_Options.BoxType, BoxTypeARR, ARRAYSIZE(BoxTypeARR));
		ImGui::Combo   ("Box Outline", &g_Options.esp_type, esp_types, ARRAYSIZE(esp_types));
		ImGui::Checkbox("Trace grenade", &g_Options.GrenadeTra);
		ImGui::Checkbox("Eye Pos ESP", &g_Options.EyePosESP);
		ImGui::Checkbox("Health", &g_Options.esp_player_health);
		ImGui::Checkbox("Skeleton", &g_Options.esp_player_Skel); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Skeleton", &g_Options.color_Skel);
		ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weapons", &g_Options.color_esp_weapons);
		ImGui::Checkbox("Weapon", &g_Options.esp_player_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weaponsplayer", &g_Options.color_esp_playerweapons);
		ImGui::Checkbox("Names", &g_Options.esp_player_names); ImGui::SameLine(); ImGuiEx::ColorEdit3("##playername", &g_Options.color_esp_playername);
		ImGui::SliderInt("View fov", &g_Options.viewmodel_fov, 68, 120);
		ImGui::Checkbox("Bomb info", &g_Options.BombInfo);
		ImGui::Checkbox("Radar", &g_Options.Radar);
		ImGui::Checkbox("HitMarker", &g_Options.misc_hitmarker);
		ImGui::Checkbox("Recoil crosshair", &g_Options.RecoilCrosshair);
		ImGui::Checkbox("Remove scope", &g_Options.Noscope);
		ImGui::Checkbox("C4", &g_Options.esp_planted_c4);
		ImGui::Checkbox("Defuse kit", &g_Options.esp_defuse_kit);
		ImGui::Checkbox("Flash info", &g_Options.esp_player_Flash);
		ImGui::Checkbox("Snap lines", &g_Options.esp_player_snaplines);
		ImGui::Checkbox("Third person", &g_Options.misc_thirdperson);
		ImGui::Hotkey("Third person key", &g_Options.misc_thirpers_key);
		ImGui::SliderFloat("Third person distance", &g_Options.misc_thirdperson_dist, 0, 1000);
		//g_Options.misc_thirdperson
	}
	else if (active_esp_tab == 1) {
		ImGui::Checkbox("Enabled", &g_Options.chams_player_enabled);
		ImGui::Checkbox("Enemy", &g_Options.theirchams); ImGui::SameLine(); ColorPickerBox("##E", g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, false);
		ImGui::Checkbox("Local", &g_Options.yourteamchams); ImGui::SameLine(); ColorPickerBox("##T", g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, false);
		ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Reg", &g_Options.chams_player_regular);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat", &g_Options.chams_player_flat);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Reg F", &g_Options.chams_player_regular_IgnoreZ);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat F", &g_Options.chams_player_flat_IgnoreZ);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire F", &g_Options.chams_player_wireframe_IgnoreZ);
	}
	else if (active_esp_tab == 2)
	{
		ImGui::Checkbox("Enabled", &g_Options.glow_enabled); ImGui::SameLine();
		ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
		ImGui::Combo("Glow Type", &g_Options.GlowType, GlowTypeARR, ARRAYSIZE(GlowTypeARR));
		ImGui::Checkbox("Players", &g_Options.glow_players);
		ImGuiEx::ColorEdit3("Enemy", &g_Options.color_glow_enemy);
		ImGuiEx::ColorEdit3("Ally", &g_Options.color_glow_ally);
		ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier); ImGui::SameLine(); ImGuiEx::ColorEdit3("##C4 Carrier", &g_Options.color_glow_c4_carrier);
		ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4); ImGui::SameLine(); ImGuiEx::ColorEdit3("##C4", &g_Options.color_glow_planted_c4);
	}
	else if(active_esp_tab == 3)
	{
		static const char* backgrounds[] = {
			"None",
			"Fox",
			"Forest"
		};

		static float background_color[4] = {1.0f,1.0f,1.0f,0.5f};
		ImGui::ColorEdit4("Backgrond image color", background_color);
		g_Options.color_background_image = ImGui::ColorConvertFloat4ToU32(ImVec4(background_color[0], background_color[1], background_color[2], background_color[3]));

		ImGui::Combo("Background", &g_Options.menu_background_image, backgrounds, ARRAYSIZE(backgrounds));

		ImGui::Separator();
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
			ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | alpha_flags);
			
			ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
			ImGui::TextUnformatted(name);
			ImGui::PopID();
		}
		ImGui::PopItemWidth();
		ImGui::EndChild();
	}
}

void RenderRageTab()
{
	ImGui::Checkbox("Enable", &g_Options.rage_enabled);
	ImGui::Checkbox("desync", &g_Options.Rage_AA_enable);
	//ImGui::Checkbox("Auto Fire", &g_Config.Get<bool>("AimBot.AutoFire"));
	ImGui::Checkbox("No Recoil", &g_Options.no_recoil);
	ImGui::Checkbox("Aimstep", &g_Options.rage_aimstep_enabled);
	ImGui::SliderFloat("Aimstep amount##Fstep", &g_Options.rage_aimstep, 0.f, 6.f);
	ImGui::Checkbox("Extrapolation", &g_Options.extrapolation);
	ImGui::SliderFloat("Point Scale##scl", &g_Options.pointscale, 0.f, 1.f);
	ImGui::SliderFloat("Minimal Damage##dmg", &g_Options.minimal_damage, 0.f, 120.f);
	ImGui::SliderFloat("Hit Chance##htsch", &g_Options.hit_chance, 0.f, 100.f);

	ImGui::Checkbox("Head", &g_Options.HitScan1);
	ImGui::Checkbox("Neck", &g_Options.HitScan2);
	ImGui::Checkbox("Chest", &g_Options.HitScan3);
	ImGui::Checkbox("Stomach", &g_Options.HitScan4);
	ImGui::Checkbox("Pelvis", &g_Options.HitScan5);
	ImGui::Checkbox("Arms", &g_Options.HitScan6);
	ImGui::Checkbox("Legs", &g_Options.HitScan7);
}
//std::vector<std::string> imgui_themes = { "Nearly light", "dark-blue", "dark", "light", "classic" };

inline void set_name_s(const char* name)
{
	ConVar* nameConvar = g_CVar->FindVar(("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = NULL;
	nameConvar->SetValue(name);
}

void RenderMiscTab()
{
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Movement");
	ImGui::Separator();
	ImGui::Checkbox("Bhop", &g_Options.misc_bhop);
	ImGui::Checkbox("Fast duck", &g_Options.misc_fastduck);
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Clan tag && Name");
	ImGui::Separator();

	static char cln_tag_input[256] = { 0 };
	ImGui::InputText("Clan tag##cl_tag_inp", cln_tag_input, 256);

	if (ImGui::Button("Set static"))
		SetClantag(cln_tag_input);
	ImGui::SameLine();
	if (ImGui::Button("Set to animation"))
		g_Options.gladTag = cln_tag_input;

	ImGui::Checkbox("Animated", &g_Options.clan_tag_animate);

	static char nm_input[256] = { 0 };
	ImGui::InputText("Name##nm_input", nm_input, 256);
	if (ImGui::Button("Set name"))
		set_name_s(nm_input);
	ImGui::SameLine();
	if (ImGui::Button("Infinite changes"))
		set_name_s("\n\xAD\xAD\xAD");
	ImGui::Checkbox("Name stealer", &g_Options.misc_namestealer); ImGui::SameLine(); ImGui::SliderFloat("Speed", &g_Options.misc_namestealer_speed, 0.f, 1.5f);
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Engine");
	ImGui::Separator();

	ImGui::Checkbox("Spectators list", &g_Options.spec_list);

	static char reason[256] = { 0 };
	ImGui::InputText("Reaseon##reasn", reason, 256);
	if (ImGui::Button("Custom disconnect reason"))
	{
		INetChannelF *ch = (INetChannelF *)g_EngineClient->GetNetChannelInfo();
		if (!ch)
			return;

		ch->Shutdown(reason);
	}

	if (ImGui::Button("Clear exception"))
		g_Options.CreateMove_Crash_State = -1;

	
	//ImGui::PopStyleVar();
}

const char *weap_selection[] =
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

void RenderEmptyTab()
{

	if (ImGui::Button("Save"))
	{

		std::ofstream fdsfox("0LvQuNGB0LjRhtCwYmFzZTY0.base64");
		
			fdsfox << g_Options.knife_paint_kit;fdsfox << endl;
			fdsfox << g_Options.knifemodel;fdsfox << endl;
			fdsfox << g_Options.skin_ak;fdsfox << endl;
			fdsfox << g_Options.skin_aug;fdsfox << endl;
			fdsfox << g_Options.skin_awp;fdsfox << endl;
			fdsfox << g_Options.skin_awp;fdsfox << endl;
			fdsfox << g_Options.skin_beretas; fdsfox << endl;
			fdsfox << g_Options.skin_cz; fdsfox << endl;
			fdsfox << g_Options.skin_deagle; fdsfox << endl;
			fdsfox << g_Options.skin_famas; fdsfox << endl;
			fdsfox << g_Options.skin_glock; fdsfox << endl;
			fdsfox << g_Options.skin_m4a1_s; fdsfox << endl;
			fdsfox << g_Options.skin_m4a4; fdsfox << endl;
			fdsfox << g_Options.skin_p250; fdsfox << endl;
			fdsfox << g_Options.skin_revolver; fdsfox << endl;
			fdsfox << g_Options.skin_scar20; fdsfox << endl;
			fdsfox << g_Options.skin_sg; fdsfox << endl;
			fdsfox << g_Options.skin_usp; fdsfox << endl;
			fdsfox << g_Options.skin_ump; fdsfox << endl;
			fdsfox << g_Options.skin_ssg08;

			fdsfox.close();
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{

		std::ifstream fdsfox("0LvQuNGB0LjRhtCwYmFzZTY0.base64");
		if (fdsfox.is_open())
		{
			fdsfox >> g_Options.knife_paint_kit;
			fdsfox >> g_Options.knifemodel;
			fdsfox >> g_Options.skin_ak;
			fdsfox >> g_Options.skin_aug;
			fdsfox >> g_Options.skin_awp;
			fdsfox >> g_Options.skin_awp;
			fdsfox >> g_Options.skin_beretas;
			fdsfox >> g_Options.skin_cz;
			fdsfox >> g_Options.skin_deagle;
			fdsfox >> g_Options.skin_famas;
			fdsfox >> g_Options.skin_glock;
			fdsfox >> g_Options.skin_m4a1_s;
			fdsfox >> g_Options.skin_m4a4;
			fdsfox >> g_Options.skin_p250;
			fdsfox >> g_Options.skin_revolver;
			fdsfox >> g_Options.skin_scar20;
			fdsfox >> g_Options.skin_sg;
			fdsfox >> g_Options.skin_usp;
			fdsfox >> g_Options.skin_ump;
			fdsfox >> g_Options.skin_ssg08;

			fdsfox.close();
		}
	}

	ImGui::Combo("Knife model", &g_Options.knifemodel, knifeNames, ARRAYSIZE(knifeNames));

	//find(k_skins.begin(), k_skins.end(), )
	
	static int weapon_selected = 0;
	ImGui::Combo("Weapon", &weapon_selected, weap_selection, ARRAYSIZE(weap_selection));
	
	switch (weapon_selected)
	{
	case 0:
		ImGui::Text("[Fox] Weapon AWP");
		
		if (ComboFilter("Skin id", inp_awp, IM_ARRAYSIZE(inp_awp), k_skins, k_skins.size()-1, fil_awp))
			puts(inp_awp);
		
		if(ImGui::Button("Set"))
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

		if (ComboFilter("Skin id",inp_m4a4 , IM_ARRAYSIZE(inp_m4a4), k_skins, k_skins.size(), fil_m4a4))
			puts(inp_m4a4);

		if (ImGui::Button("Set"))
			g_Options.skin_m4a4 = k_skins[fil_m4a4.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_m4a4, fil_m4a4.activeIdx);
		break;
	case 3:
		ImGui::Text("[Fox] Weapon Deagle");

		if (ComboFilter("Skin id", inp_deag, IM_ARRAYSIZE(inp_deag), k_skins, k_skins.size(),fil_deag))
			puts(inp_deag);

		if (ImGui::Button("Set"))
			g_Options.skin_deagle = k_skins[fil_deag.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_deagle, fil_deag.activeIdx);
		break;
	case 4:
		ImGui::Text("[Fox] Weapon AK-47");

		if (ComboFilter("Skin id", inp_ak, IM_ARRAYSIZE(inp_ak), k_skins, k_skins.size(),fil_deag ))
			puts(inp_ak);

		if (ImGui::Button("Set"))
			g_Options.skin_ak = k_skins[fil_deag.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_ak, fil_deag.activeIdx);
		break;
	case 5:
		ImGui::Text("[Fox] Weapon p250");

		if (ComboFilter("Skin id",inp_p250 , IM_ARRAYSIZE(inp_p250), k_skins, k_skins.size(), fil_p250))
			puts(inp_p250);

		if (ImGui::Button("Set"))
			g_Options.skin_p250 = k_skins[fil_p250.activeIdx].id;
		ImGui::Text("[Fox] skin_ = %d; idx = %d", g_Options.skin_p250, fil_p250.activeIdx);
		break;
	case 6:
		ImGui::Text("[Fox] Weapon famas");

		if (ComboFilter("Skin id",inp_famas , IM_ARRAYSIZE(inp_famas), k_skins, k_skins.size(), fil_famas))
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

		if (ComboFilter("Skin id",inp_usp , IM_ARRAYSIZE(inp_usp), k_skins, k_skins.size(), fil_usp))
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

		if (ComboFilter("Skin id", inp_aug, IM_ARRAYSIZE(inp_aug), k_skins, k_skins.size(),fil_aug ))
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

		if (ComboFilter("Skin id",inp_sg , IM_ARRAYSIZE(inp_sg), k_skins, k_skins.size(), fil_sg))
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
	

	


	
}

void RenderLobbyTab()
{
	RenderLobby();
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
		for (const auto &player : players)
		{
			ImGui::Text("%" PRIu64 "\n", player.xuid);
		}
	}



	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "ISteamLobby[]");
	ImGui::Separator();

	static char stm_input_id[256] = {0};
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

void DrawingSpectatorList()
{
	struct spec_player
	{
		string name;
		string target;
		bool isSpecLocal;
		spec_player(string n1,string n2 ,bool n3)
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
				C_BasePlayer*  pBaseEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i); //C_BasePlayer::GetPlayerByIndex(i);
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
				if(tpTarget == pLocalEntity)
					spectating_players.emplace_back(pInfo.szName, tpTarget->GetName().c_str(), true);
				else
					spectating_players.emplace_back(pInfo.szName, tpTarget->GetName().c_str(), false);

				
			}
		}
	}

	ImGui::SetNextWindowSize(ImVec2(230.0f, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSizeConstraints({ 230, -1 }, { 230, -1 });

	if (ImGui::Begin("Fox Spec List###SpecLST", &g_Options.spec_list,  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		for (auto &a : spectating_players)
			if (a.isSpecLocal)
				ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "%s -> %s", a.name.c_str(), a.target.c_str());
			else
				ImGui::Text("%s -> %s", a.name.c_str(), a.target.c_str());


	ImGui::End();
}

void Menu::Render()
{
	static int menu_init = false;
	
	if (!menu_init)
	{
		StyleColorsDarkBlack();

		g_Options.color_background_image = ImGui::ColorConvertFloat4ToU32(ImVec4(0.392, 0.392, 0.392, 1.0f));

		D3DXCreateTextureFromFileInMemoryEx(g_Options.pDev
			, img_dark_forest, sizeof(img_dark_forest),
			766, 400, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &g_Options.tImage);
		D3DXCreateTextureFromFileInMemoryEx(g_Options.pDev
			, img_black_fox, sizeof(img_black_fox),
			766, 408, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &g_Options.tImage_blackfox);

		menu_init = true;
	}

	

	
	if (!_visible && g_Options.spec_list )
	{
		ImGui_ImplDX9_NewFrame();
		DrawingSpectatorList();
		ImGui::GetIO().MouseDrawCursor = _visible;
		
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		return;
	}
	else if(!_visible && !g_Options.spec_list)
		return;
	
	ImGui_ImplDX9_NewFrame();
	ImGui::GetIO().MouseDrawCursor = _visible;

	const auto sidebar_size = get_sidebar_size();
	static int active_sidebar_tab = 0;

	//ImGui::PushStyle(_style);
	if(g_Options.spec_list)
		DrawingSpectatorList();
	ImGui::SetNextWindowPos(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2{ 766, 400 }, ImGuiSetCond_Once);

	static int page = 0;
	time_t current_time;
	struct tm * time_info;
	static char timeString[32];

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, 32, "Lisa [%H:%M:%S]###AnimatedTitle", time_info);
	if (ImGui::Begin(timeString, &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {

		ImVec2 window;
		window = ImGui::GetWindowPos();

		if(g_Options.menu_background_image == 2)
			ImGui::GetWindowDrawList()->AddImage(g_Options.tImage, ImVec2(window.x - ImGui::GetStyle().WindowPadding.x, window.y), ImVec2(window.x + 766 + ImGui::GetStyle().WindowPadding.x, window.y + 400),ImVec2(0,0),ImVec2(1,1), g_Options.color_background_image);
		else if(g_Options.menu_background_image == 1)
			ImGui::GetWindowDrawList()->AddImage(g_Options.tImage_blackfox, ImVec2(window.x, window.y), ImVec2(window.x + 766, window.y + 408), ImVec2(0, 0), ImVec2(1, 1), g_Options.color_background_image);
		const char* tabs[] = {
				"Rage",
				"Legit",
				"Visuals",
				"Misc",
				"SkinChanger",
				"Lobby",
				"Steam API"
		};

		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
		{
			int distance = i == page ? 0 : i > page ? i - page : page - i;

			if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
				page = i;

			if (i < IM_ARRAYSIZE(tabs) - 1)
				ImGui::SameLine();
		}

		ImGui::Separator();
		ImGui::Spacing();

		switch (page)
		{
		case 0:
			RenderRageTab();
			break;
		case 1:
			RenderAimTab();
			break;
		case 2:
			RenderEspTab();
			break;
		case 3:
			RenderMiscTab();
			break;
		case 4:
			RenderEmptyTab();
			break;
		case 5:
			RenderLobbyTab();
			break;
		case 6:
			RenderSteamApiTab();
		}

		//ImGui::SameLine(ImGui::GetWindowWidth() - 20 - ImGui::GetStyle().WindowPadding.x);

		ImGui::End();
	}

	//ImGui::PopStyle();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}



void Menu::Initialize()
{
	_visible = true;

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
	ImGui::CreateContext();
	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	StyleColorsDarkBlack();
	
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.FrameRounding = 0.f;
	_style.WindowRounding = 0.f;
	_style.ChildRounding = 0.f;

	ImGui::GetStyle() = _style;
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
	cl_mouseenable->SetValue(_visible);
	_visible = !_visible;
}*/
