#pragma once
//#include "MainInclude.hpp"
#include <string>
#include <vector>
#include <time.h>
#include "imgui/imgui.h"
//#include "imgui/imgui_internal.h"
#include "imgui/imconfig.h"

class ScreenEvents
{
private:

	struct event_fox
	{
		std::string what;

		float alpha;
		float col_red;
		float col_green;
		float col_blue;
		float pad1;
		float pad2;
		float timelife, timelife2;
		float width;
		event_fox(std::string te_what)
		{
			what = te_what;
			alpha = 1.0f;
			col_red = 1.0f;
			col_green = 1.0f;
			col_blue = 1.0f;
			timelife = timelife2 = 5;
			pad1 = 0;
			pad2 = 0;



			if (what.size() > 50)
			{
				std::string relay_str;
				what = what.substr(0, 50);
				what += "...";
			}

			width = ImGui::CalcTextSize(what.c_str()).x + 20;
		}
		event_fox(std::string te_what, float r, float g, float b)
		{
			what = te_what;

			alpha = 1.0f;
			col_red = r;
			col_green = g;
			col_blue = b;
			timelife = timelife2 = 5;
			pad1 = 0;
			pad2 = 0;


			if (what.size() > 50)
			{
				std::string relay_str;
				what = what.substr(0, 50);
				what += "...";
			}

			width = ImGui::CalcTextSize(what.c_str()).x + 20;
		}
	};
	std::vector <event_fox>data;
	std::vector <event_fox>data1;

public:
	ImVec2 NotificationPadding1 = ImVec2(5, 5);

	void TickElements();
	void Render(ImDrawList* drawlist);
	void AddEvent(std::string EvData, bool slide);
	void AddEventRed(std::string EvData, bool slide);
	void AddEventGreen(std::string EvData, bool slide);
	void AddEventOrange(std::string EvData, bool slide);
	void AddEventCustom(std::string EvData, float r, float g, float b, bool slide);

};
extern ScreenEvents* g_FoxEvents;
