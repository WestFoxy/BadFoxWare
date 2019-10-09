#include "FoxEvents.h"
#include "imgui/imgui_internal.h"

ScreenEvents* g_FoxEvents = new ScreenEvents;
void ScreenEvents::TickElements()
{
	{
		static clock_t start_t = clock();
		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
		if (timeSoFar < 0.01)
			goto next;
		for (int i = 0; i < data1.size(); i++)
		{
			data1[i].timelife -= 0.01f;
			if (data1[i].pad1 < data1[i].width + 10)
				data1[i].pad1 += (data1[i].width + 10) / 20 + 0.01 - data1[i].pad1 * 0.05;


			if ((data1[i].pad2 < data1[i].width) && (data1[i].timelife < 4.8))
				data1[i].pad2 += data1[i].width / 20 - data1[i].pad2 * 0.05;



			if (data1[i].timelife <= 0.0f)
			{
				data1[i].timelife2 -= 0.01f;
				//data1[i].width -= 1;

				data1[i].pad2 -= data1[i].width / 20 + data1[i].pad2 * 0.1;
				if (data1[i].timelife2 < 4.8)
					data1[i].pad1 -= data1[i].width / 20 + data1[i].pad1 * 0.15;

				if (data1[i].pad1 <= 0 || data1[i].timelife2 < 0)
					data1.erase(data1.begin() + i);
			}
		}
		start_t = clock();
	}
next:
	{
		static clock_t start_t = clock();
		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
		if (timeSoFar < 0.1)
			return;


		for (int i = 0; i < data.size(); i++)
		{
			data[i].alpha -= 0.02f;
			if (data[i].alpha <= 0.2f)
			{
				data.erase(data.begin() + i);
			}
		}

		start_t = clock();
	}
}
void ScreenEvents::Render(ImDrawList* drawlist)
{
	for (int i = 0; i < data.size(); i++)
	{
		drawlist->AddRectFilledMultiColor(ImVec2(0, 17 * i), ImVec2(400, (i + 1) * 17), ImGui::GetColorU32(ImVec4(0, 0, 0, data[i].alpha)), 0x00000000, 0x00000000, ImGui::GetColorU32(ImVec4(0, 0, 0, data[i].alpha)));
		drawlist->AddText(ImVec2(0, 17 * i), ImGui::GetColorU32(ImVec4(data[i].col_red, data[i].col_green, data[i].col_blue, data[i].alpha + 0.8f)), data[i].what.c_str());
	}
	for (int i = 0; i < data1.size(); i++)
	{
		drawlist->AddRectFilled(ImVec2(0, NotificationPadding1.y + i * 40 + i * 10), ImVec2(data1[i].pad1, NotificationPadding1.y + 40 + i * 40 + i * 10), ImGui::GetColorU32(ImVec4(data1[i].col_red, data1[i].col_green, data1[i].col_blue, 1.0f)));
		drawlist->AddRectFilled(ImVec2(0, NotificationPadding1.y + i * 40 + i * 10), ImVec2(data1[i].pad2, NotificationPadding1.y + 40 + i * 40 + i * 10), ImGui::GetColorU32(ImVec4(0.0, 0.0, 0.0, 1.0f)));

		drawlist->AddText(ImVec2(data1[i].pad2 - data1[i].width + 10, NotificationPadding1.y + i * 40 + i * 10 + 5), ImGui::GetColorU32(ImVec4(data1[i].col_red, data1[i].col_green, data1[i].col_blue, data1[i].alpha)), data1[i].what.c_str());


	}
}

void ScreenEvents::AddEvent(std::string EvData, bool slide)
{
	//data.emplace_back(EvData);
	if (slide)
		data1.emplace_back(EvData);
	else
		data.emplace_back(EvData);
}

void ScreenEvents::AddEventRed(std::string EvData, bool slide)
{
	if (slide)
		data1.emplace_back(EvData, 1.0f, 0, 0);
	else
		data.emplace_back(EvData, 1.0f, 0, 0);
}

void ScreenEvents::AddEventGreen(std::string EvData, bool slide)
{
	if (slide)
		data1.emplace_back(EvData, 0, 1.0f, 0);
	else
		data.emplace_back(EvData, 0, 1.0f, 0);
}

void ScreenEvents::AddEventOrange(std::string EvData, bool slide)
{
	if (slide)
		data1.emplace_back(EvData, 0, 1.0f, 0);
	else
		data.emplace_back(EvData, 0, 1.0f, 0);
}

void ScreenEvents::AddEventCustom(std::string EvData, float r, float g, float b, bool slide)
{
	//data.emplace_back(EvData, r, g, b);
	if (slide)
		data1.emplace_back(EvData, r, g, b);
	else
		data.emplace_back(EvData, r, g, b);
}

