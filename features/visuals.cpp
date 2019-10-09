#include "visuals.hpp"
#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../valve_sdk/sdk.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include "GrenadeTra.hpp"
using namespace std;
#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))
player_info_t info;
vgui::HFont esp_font;
vgui::HFont defuse_font;
vgui::HFont hit_font;
vgui::HFont icon_font;
vgui::HFont dropped_weapons_font;
float alpha_ent[150] = { 255 };
bool Visuals::CreateFonts()
{
	esp_font = g_VGuiSurface->CreateFont_();
	defuse_font = g_VGuiSurface->CreateFont_();
	dropped_weapons_font = g_VGuiSurface->CreateFont_();
	hit_font = g_VGuiSurface->CreateFont_();
	icon_font = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(esp_font, "Tahoma", 11, FW_NORMAL, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(defuse_font, "Tahoma", 15, FW_NORMAL, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(dropped_weapons_font, "Tahoma", 10, 700, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(hit_font, "Tahoma", 11, FW_NORMAL, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(icon_font, "undefeated", 13, FW_NORMAL, 0, 0, FONTFLAG_DROPSHADOW);
	return true;
}
// ESP Context
// This is used so that we dont have to calculate player color and position
// on each individual function over and over
struct
{
	C_BasePlayer* pl;
	bool          is_enemy;
	bool          is_visible;
	Color         clr;
	Vector        head_pos;
	Vector        feet_pos;
	RECT          bbox;
} esp_ctx;


RECT GetBBox(C_BaseEntity* ent, Vector pointstransf[])
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t &trans = ent->m_rgflCoordinateFrame();

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
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void Visuals::DestroyFonts()
{
	// Is there a way to destroy vgui fonts?
	// TODO: Find out
}
//--------------------------------------------------------------------------------
void Visuals::Misc::Radar()
{
	{
		if (!g_Options.Radar)
			return;

		for (int i = 1; i <= g_GlobalVars->maxClients; i++)
		{
			if (i == g_EngineClient->GetLocalPlayer())
				continue;

			auto pPlayer = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
			if (!pPlayer)
				continue;

			pPlayer->m_bSpotted() = true;
		}
	}
}
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	esp_ctx.pl = pl;
	esp_ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	esp_ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	if (!esp_ctx.is_enemy && g_Options.esp_enemies_only)
		return false;

	esp_ctx.clr = esp_ctx.is_enemy ? (esp_ctx.is_visible ? g_Options.color_esp_enemy_visible : g_Options.color_esp_enemy_occluded) : (esp_ctx.is_visible ? g_Options.color_esp_ally_visible : g_Options.color_esp_ally_occluded);

	auto head = pl->GetHitboxPos(HITBOX_HEAD);
	auto origin = pl->m_vecOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, esp_ctx.head_pos) ||
		!Math::WorldToScreen(origin, esp_ctx.feet_pos))
		return false;

	auto h = fabs(esp_ctx.head_pos.y - esp_ctx.feet_pos.y);
	auto w = h / 2.0f;

	esp_ctx.bbox.left = static_cast<long>(esp_ctx.feet_pos.x - w * 0.5f);
	esp_ctx.bbox.right = static_cast<long>(esp_ctx.bbox.left + w);
	esp_ctx.bbox.bottom = static_cast<long>(esp_ctx.feet_pos.y);
	esp_ctx.bbox.top = static_cast<long>(esp_ctx.head_pos.y);

	return true;
}
//--------------------------------------------------------------------------------



void Visuals::Player::RenderBox()
{
	float hor = (esp_ctx.bbox.right - esp_ctx.bbox.left) * 0.2f;
	float ver = (esp_ctx.bbox.bottom - esp_ctx.bbox.top) * 0.2f;

	if (esp_ctx.pl->IsDormant() && alpha_ent[esp_ctx.pl->EntIndex()] > 0)
		alpha_ent[esp_ctx.pl->EntIndex()] -= (g_Options.esp_fade_speed / 2);

	else if(!esp_ctx.pl->IsDormant() && alpha_ent[esp_ctx.pl->EntIndex()] < 255)
		alpha_ent[esp_ctx.pl->EntIndex()] += g_Options.esp_fade_speed;

	switch (g_Options.BoxType)
	{
	case 0:

		/*g_VGuiSurface->DrawSetColor(g_Options.color_esp_Box);*/ g_VGuiSurface->DrawSetColor(Color(g_Options.color_esp_Box.r(), g_Options.color_esp_Box.g(), g_Options.color_esp_Box.b(), (int)alpha_ent[esp_ctx.pl->EntIndex()]));
		g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.right, esp_ctx.bbox.bottom);
		if (g_Options.esp_type == 1)
		{
			g_VGuiSurface->DrawSetColor(Color::Black);
			g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);
			g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left + 1, esp_ctx.bbox.top + 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
		}
		break;

	case 1:
		if (g_Options.esp_type == 1)
		{
			g_VGuiSurface->DrawSetColor(Color::Black);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.left + 1 + hor, esp_ctx.bbox.top + 2);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 1 - hor, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.top + 2);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom - 2, esp_ctx.bbox.left + 1 + hor, esp_ctx.bbox.bottom + 1);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 1 - hor, esp_ctx.bbox.bottom - 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);

			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top + 2, esp_ctx.bbox.left + 2, esp_ctx.bbox.top + 1 + ver);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 2, esp_ctx.bbox.top + 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.top + 1 + ver);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom - 1 - ver, esp_ctx.bbox.left + 2, esp_ctx.bbox.bottom - 2);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 2, esp_ctx.bbox.bottom - 1 - ver, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom - 2);
		}
		/*g_VGuiSurface->DrawSetColor(g_Options.color_esp_Box);*/
		g_VGuiSurface->DrawSetColor(Color(g_Options.color_esp_Box.r(), g_Options.color_esp_Box.g(), g_Options.color_esp_Box.b(), (int)alpha_ent[esp_ctx.pl->EntIndex()]));
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.left + hor - 1, esp_ctx.bbox.top);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - hor, esp_ctx.bbox.top, esp_ctx.bbox.right - 1, esp_ctx.bbox.top);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.bottom - 1, esp_ctx.bbox.left + hor - 1, esp_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - hor, esp_ctx.bbox.bottom - 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.left, esp_ctx.bbox.top + ver - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - 1, esp_ctx.bbox.top, esp_ctx.bbox.right - 1, esp_ctx.bbox.top + ver - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.bottom - ver, esp_ctx.bbox.left, esp_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - ver, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
		break;
	}
}
//--------------------------------------------------------------------------------
/*
std::string WeaponName = "defuser";
std::wstring ws = std::wstring(WeaponName.begin(), WeaponName.end());
const wchar_t* buf = ws.c_str();
g_VGuiSurface->DrawSetTextColor(Color::Green);
g_VGuiSurface->DrawSetTextFont(esp_font);
g_VGuiSurface->DrawSetTextPos((int)vEntScreen.x, (int)vEntScreen.y);
g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
*/
void Visuals::Player::RenderName()
{
	wchar_t buf[128];

	player_info_t info = esp_ctx.pl->GetPlayerInfo();

	if (MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 128) > 0) {
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_playername);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.head_pos.y - th);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth()
{
	auto  hp = esp_ctx.pl->m_iHealth();
	float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 8;

	auto height = (((box_h * hp) / 100));

	int green = int(hp * 2.55f);
	int red = 255 - green;

	int x = esp_ctx.bbox.left - off;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color(red, green, 0, 255));
	g_VGuiSurface->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 2);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
	auto  armour = esp_ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 4;

	auto height = (((box_h * armour) / 100));

	int x = esp_ctx.bbox.right + off;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color(0, 50, 255, 255));
	g_VGuiSurface->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 2);
}
//--------------------------------------------------------------------------------

void Visuals::Misc::RenderC4Info(C_PlantedC4* ent)
{

	float flBlow = ent->m_flC4Blow();

	flBlow -= g_GlobalVars->interval_per_tick * g_LocalPlayer->m_nTickBase();
	auto bombT = ent->m_flTimerLength();
	int screenx, screeny;
	g_VGuiSurface->GetScreenSize(screenx, screeny);

	screenx = screenx / 2;
	screeny = screeny - 100;

	if (flBlow <= 0.0)
		flBlow = 0.0;

	if (flBlow < bombT)
	{
		float ptc = flBlow / bombT;

		g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 110));
		g_VGuiSurface->DrawFilledRect(screenx - 150, screeny - 4, screenx + 150, screeny + 4);

		g_VGuiSurface->DrawSetColor(194, 244, 66, 110);
		g_VGuiSurface->DrawFilledRect(screenx - 150, screeny - 4, screenx - 150 + (300 * ptc), screeny + 4);

		if (flBlow > 5.f)
		{
			g_VGuiSurface->DrawSetColor(27, 120, 214, 110);
			g_VGuiSurface->DrawFilledRect(screenx - 150, screeny - 4, screenx - 150 + (300 * 0.125), screeny + 4);
		}
		else
		{
			g_VGuiSurface->DrawSetColor(27, 120, 214, 110);
			g_VGuiSurface->DrawFilledRect(screenx - 150, screeny - 4, screenx - 150 + (300 * ptc), screeny + 4);
		}

		g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 110));
		g_VGuiSurface->DrawOutlinedRect(screenx - 150, screeny - 4, screenx + 150, screeny + 4);

		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << flBlow;

		std::string s = stream.str();
		std::wstring ws = std::wstring(s.begin(), s.end());

		const wchar_t* buf = ws.c_str();

		int w = 300;
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);
		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color(255, 0, 0, 255));
		g_VGuiSurface->DrawSetTextPos(screenx - tw / 2, screeny - th / 2 - 15);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		if (ent->m_hBombDefuser() > 0)
		{
			float flCountdown = ent->m_flDefuseCountDown() - (g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick);

			float ptc2 = flCountdown / 10;

			std::stringstream sstream;
			sstream << std::fixed << std::setprecision(1) << flCountdown;

			std::string ss = sstream.str();
			std::wstring wss = std::wstring(ss.begin(), ss.end());

			const wchar_t* buf2 = wss.c_str();

			g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 110));
			g_VGuiSurface->DrawFilledRect(screenx - 150, screeny - 4 + 30, screenx + 150, screeny + 4 + 30);

			g_VGuiSurface->DrawSetColor(84, 169, 255, 110);
			g_VGuiSurface->DrawFilledRect(screenx - 150, screeny - 4 + 30, screenx - 150 + (300 * ptc2), screeny + 4 + 30);

			g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 110));
			g_VGuiSurface->DrawOutlinedRect(screenx - 150, screeny - 4 + 30, screenx + 150, screeny + 4 + 30);

			g_VGuiSurface->GetTextSize(esp_font, buf2, tw, th);
			g_VGuiSurface->DrawSetTextFont(esp_font);
			g_VGuiSurface->DrawSetTextColor(Color(84, 169, 255, 110));
			g_VGuiSurface->DrawSetTextPos(screenx - tw / 2, screeny - th / 2 - 15 + 30);
			g_VGuiSurface->DrawPrintText(buf2, wcslen(buf2));

			if (flCountdown > flBlow)
			{
				g_VGuiSurface->GetTextSize(esp_font, L"can't be defused", tw, th);
				g_VGuiSurface->DrawSetTextFont(esp_font);
				g_VGuiSurface->DrawSetTextColor(Color(255, 48, 79, 110));
				g_VGuiSurface->DrawSetTextPos(screenx - tw / 2, screeny - th / 2 - 15 + 60);
				g_VGuiSurface->DrawPrintText(L"can't be defused", wcslen(L"can't be defused"));
			}
		}
	}
}



void Visuals::Player::RenderWeapon()
{
	wchar_t buf[80];

	auto weapon = esp_ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (MultiByteToWideChar(CP_UTF8, 0, weapon->GetCSWeaponData()->m_WeaponName + 7, -1, buf, 80) > 0) 
	{
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_playerweapons);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.feet_pos.y);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{
	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	g_VGuiSurface->DrawSetColor(esp_ctx.clr);

	g_VGuiSurface->DrawLine(
		screen_w / 2,
		screen_h,
		esp_ctx.feet_pos.x,
		esp_ctx.feet_pos.y);
}
//--------------------------------------------------------------------------------
	void Visuals::Misc::Flash(C_BasePlayer* entity)//sex with fox
	{
		Vector pointsTransformed[8];
		std::string text;
		if (!g_LocalPlayer->IsAlive())return;

		if (g_EngineClient->GetPlayerInfo(entity->EntIndex(), &info))
		{ 
			if (g_Options.esp_player_Flash && entity->IsFlashed())
			{
				int FlashTime = entity->GetFlashBangTime();
				std::string indicator(std::to_string(FlashTime).append(" /s"));
				std::wstring textt = std::wstring(indicator.begin(), indicator.end());
				const wchar_t* converted_text = textt.c_str();
				auto bbox = GetBBox(entity, pointsTransformed);
				int w = bbox. right - bbox.left;
				int tw, th;
				g_VGuiSurface->DrawSetTextColor(g_Options.FlashCol);
				g_VGuiSurface->GetTextSize(hit_font, converted_text, tw, th);
				g_VGuiSurface->DrawSetTextFont(hit_font);
				g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.head_pos.y - th);
				g_VGuiSurface->DrawPrintText(converted_text, wcslen(converted_text));

			}
		}
	}

void Visuals::Player::HeadEsp(C_BasePlayer* entity)
{
	if (!entity->IsAlive())return;
		static int Scale = 2;
		if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		return;
		Vector cross = entity->GetBonePos(8),screen;
		QAngle eyepos = g_LocalPlayer->m_angEyeAngles();
		if (Math::WorldToScreen(cross, screen))
		{
			g_VGuiSurface->DrawSetColor(g_Options.color_esp_HeadESP);
			CCSGrenadeHint::Get().Draw3dCube(5.f, eyepos, cross, g_Options.color_esp_HeadESP);
		}

		

}
// m_bHasHeavyArmor
//m_bHasHelmet
void Visuals::Player::KevESP(C_BasePlayer* entity)
{
	Vector pointsTransformed[8];
	if (!entity->IsAlive())
		return;
	float off = 4;
	float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
	int x = esp_ctx.bbox.right + off;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;
	int tw, th;
	if (entity->m_bHasHelmet() && entity->IsAlive())
	{
		const wchar_t* xd = L"hk";
		g_VGuiSurface->GetTextSize(hit_font, xd, tw, th);
		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_enemy_visible);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw * w, esp_ctx.head_pos.y - th / h );
		g_VGuiSurface->DrawPrintText(xd, wcslen(xd));
	}


}
bool WorldToScreenV(Vector world, Vector& screen)
{
	return (g_DebugOverlay->ScreenPosition(world, screen) != 1);
}
bool IsOnScreen(Vector origin, Vector& screen)
{
	if (!Math::WorldToScreen(origin, screen)) return false;
	int iScreenWidth, iScreenHeight;
	g_EngineClient->GetScreenSize(iScreenWidth, iScreenHeight);
	bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
	return xOk && yOk;
}
#define M_PI_VV 3.14159265358979323846
void VectorAnglesV(const Vector& forward, Vector& angles)
{
	float tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI_VV);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI_VV);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
Vector CalcAngleV(Vector src, Vector dst)
{
	Vector ret;
	VectorAnglesV(dst - src, ret);
	return ret;
}
void TexturedPolygon(int n, std::vector<Vertex_t> vertice, Color color)
{
	static int texture_id = g_VGuiSurface->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	g_VGuiSurface->DrawSetColor(color); //
	g_VGuiSurface->DrawSetTexture(texture_id); //
	g_VGuiSurface->DrawTexturedPolygon(n, vertice.data()); //
}


void Visuals::Player::DrawFovArrows(C_BasePlayer* entity, Color color)
{
	
	auto local_player = g_LocalPlayer;
	if (!local_player) return;
	if (entity->GetIsDormant()) return;

	Vector screenPos, client_viewangles;
	QAngle temp_client_viewangles;
	int screen_width = 0, screen_height = 0;
	float radius = 300.f;

	if (IsOnScreen(entity->GetHitboxPos(HITBOX_HEAD), screenPos)) return;

	g_EngineClient->GetViewAngles(temp_client_viewangles);
	client_viewangles.x = temp_client_viewangles.pitch;
	client_viewangles.y = temp_client_viewangles.yaw;
	client_viewangles.z = temp_client_viewangles.roll;
	g_EngineClient->GetScreenSize(screen_width, screen_height);

	const auto screen_center = Vector(screen_width / 2.f, screen_height / 2.f, 0);

	const auto rot = DEG2RAD(client_viewangles.y - CalcAngleV(local_player->GetEyePos(), entity->GetHitboxPos(HITBOX_HEAD)).y - 90);

	std::vector<Vertex_t> vertices;

	vertices.push_back(Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * radius)));
	vertices.push_back(Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(2)) * (radius - 16), screen_center.y + sinf(rot + DEG2RAD(2)) * (radius - 16))));
	vertices.push_back(Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(2)) * (radius - 16), screen_center.y + sinf(rot - DEG2RAD(2)) * (radius - 16))));
	//vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * SETTINGS::settings.arrowradius)));
	//vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(2)) * (radius - SETTINGS::settings.arrowsizesh), screen_center.y + sinf(rot + DEG2RAD(2)) * (SETTINGS::settings.arrowradius - SETTINGS::settings.arrowsizesh))));
	//vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(2)) * (radius - SETTINGS::settings.arrowsizesh), screen_center.y + sinf(rot - DEG2RAD(2)) * (SETTINGS::settings.arrowradius - SETTINGS::settings.arrowsizesh))));
	TexturedPolygon(3, vertices, color);

}

Vector2D clamp_screen_pos(Vector2D screen_point, Vector2D screen_reference)
{
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	if (screen_point.IsValid())
		return screen_point; //Greater than or Equal to 0 and Less than or Equal to the Screen Height/Width

	bool
		negativeX = screen_reference.x < screen_point.x,
		negativeY = screen_reference.y < screen_point.y;

	Vector2D slope = Vector2D(negativeX ? -(screen_point.x / screen_reference.x) : (screen_reference.x / screen_point.x), negativeY ? -(screen_point.y / screen_reference.y) : (screen_reference.y / screen_point.y)); //Basic Algebra (Account for an Inverse Slope)

	while (!screen_point.IsValid()) //This will Fail if the End Point Failed the Same Way (Also < 0/Also > Height/Width)
		screen_point += slope; //Keep Moving the Starting Point along the Line Until it is Valid

	return screen_point;
}
void Visuals::Misc::EyePos(C_BasePlayer * ent)
{                                           //                    
	static float rainbow;                   //
	rainbow += 0.0002f;                     //
	if (rainbow > 1.f)                      //
		rainbow = 0.f;                      //
											//
//////////////////////////////////////////////
	if (!ent->IsAlive())
		return;
	QAngle angs = ent->m_angEyeAngles();
	Vector vecForward;

	Math::AngleVectors(angs, vecForward);

	Vector startvis = ent->GetBonePos(8);
	Vector start = ent->GetBonePos(8); //i use bone pos 8 as eyepos is shit as the line is out the back of the head 
	Vector endvis = start + (vecForward * 30);
	Vector end = start + (vecForward * 8000);


	CTraceFilter filtervis;
	CTraceFilter filter;
	Ray_t rayvis;
	Ray_t ray;
	trace_t trvis;
	trace_t tr;

	filtervis.pSkip = ent;
	filter.pSkip = ent;
	rayvis.Init(startvis, endvis);
	ray.Init(start, end);
	g_EngineTrace->TraceRay(rayvis, MASK_SHOT, &filtervis, &trvis);
	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	Vector begoftracevis, endoftracevis;

	bool didHitvis = false;
	if (trvis.hitgroup <= 7 && trvis.hitgroup > 1)
	{
		didHitvis = true;
	}
	bool didHit = false;
	if (tr.hitgroup <= 7 && tr.hitgroup > 1)
	{
		didHit = true;
	}



	if (Math::WorldToScreen(trvis.startpos, begoftracevis) && Math::WorldToScreen(trvis.endpos, endoftracevis))
	{
		if (didHit)
		{
			g_VGuiSurface->DrawSetColor(Color::Red); // if the enemys traceray is on another entity 
		}
		else {
			g_VGuiSurface->DrawSetColor(Color::White);
		
		}
		g_VGuiSurface->DrawLine(begoftracevis.x, begoftracevis.y, endoftracevis.x, endoftracevis.y);
	}	
			
}



void Visuals::Misc::Skel(C_BasePlayer* ent)
{
	studiohdr_t * pStudioModel = g_MdlInfo->GetStudioModel(ent->GetModel());
	if (pStudioModel)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (ent->SetupBones(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_GlobalVars->curtime))
		{
			for (int xd = 0; xd < pStudioModel->numbones; xd++)
			{
				mstudiobone_t* pBone = pStudioModel->GetBone(xd);
				if (!pBone || !(pBone->flags & BONE_USED_BY_HITBOX) || pBone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[xd][0][3], boneToWorldOut[xd][1][3], boneToWorldOut[xd][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[pBone->parent][0][3], boneToWorldOut[pBone->parent][1][3], boneToWorldOut[pBone->parent][2][3]), bonePos2))
					continue;

				g_VGuiSurface->DrawSetColor(g_Options.color_Skel);
				g_VGuiSurface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
}
void Visuals::Misc::SniperX(C_BasePlayer* entity)
{
	
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderWeapon(C_BaseCombatWeapon* ent)
{
	Vector pointsTransformed[8];
	wchar_t buf[80];
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	// We don't want to Render weapons that are being held
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent, pointsTransformed);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_weapons);
	g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
	g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
	g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);

	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0) {
		int w = bbox.right - bbox.left;
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_weapons);
		g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderNet(C_BasePlayer* Player)
{

}
void Visuals::Misc::RenderDefuseKit(C_BasePlayer* ent)
{
	Vector pointsTransformed[8];
	auto bbox = GetBBox(ent, pointsTransformed);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_defuse);
	g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.left, bbox.top);
	g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
	g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawLine(bbox.right, bbox.bottom, bbox.left, bbox.bottom);

	const wchar_t* buf = L"Defuse Kit";

	int w = bbox.right - bbox.left;
	int tw, th;
	g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

	g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
	g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
	g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderPlantedC4(C_BaseEntity* ent)
{
	Vector  pointsTransformed[8];
	auto bbox = GetBBox(ent, pointsTransformed);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_c4);
	g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.left, bbox.top);
	g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
	g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawLine(bbox.right, bbox.bottom, bbox.left, bbox.bottom);

	const wchar_t* buf = L"C4";

	int w = bbox.right - bbox.left;
	int tw, th;
	g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

	g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
	g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
	g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}

//--------------------------------------------------------------------------------
		
	
		

void Visuals::Misc::KnifeLeft()
{

	static auto left_knife = g_CVar->FindVar("cl_righthand");

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		left_knife->SetValue(1);
		return;
	}

	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon) return;

	left_knife->SetValue(!weapon->IsKnife());
}
//----------------------------------------------------------
void Visuals::Misc::SkyChanger()
{
	static auto sv_skyname = g_CVar->FindVar("sv_skyname");

	switch (g_Options.SkyBoxChanger)
	{
	case 1: //Baggage
		sv_skyname->SetValue("cs_baggage_skybox_");
		break;
	case 2: //Tibet
		sv_skyname->SetValue("cs_tibet");
		break;
	case 3: //Clear Sky
		sv_skyname->SetValue("clearsky");
		break;
	case 4: //Clear Sky HD
		sv_skyname->SetValue("clearsky_hdr");
		break;
	case 5: //Embassy
		sv_skyname->SetValue("embassy");
		break;
	case 6: //Italy
		sv_skyname->SetValue("italy");
		break;
	case 7: //Daylight 1
		sv_skyname->SetValue("sky_cs15_daylight01_hdr");
		break;
	case 8: //Daylight 2
		sv_skyname->SetValue("sky_cs15_daylight02_hdr");
		break;
	case 9: //Daylight 3
		sv_skyname->SetValue("sky_cs15_daylight03_hdr");
		break;
	case 10: //Daylight 4
		sv_skyname->SetValue("sky_cs15_daylight04_hdr");
		break;
	case 11: //Cloudy
		sv_skyname->SetValue("sky_csgo_cloudy01");
		break;
	case 12: //Night 1
		sv_skyname->SetValue("sky_csgo_night02");
		break;
	case 13: //Night 2
		sv_skyname->SetValue("sky_csgo_night02b");
		break;
	case 14: //Night Flat
		sv_skyname->SetValue("sky_csgo_night_flat");
		break;
	case 15: //Day HD
		sv_skyname->SetValue("sky_day02_05_hdr");
		break;
	case 16: //Day
		sv_skyname->SetValue("sky_day02_05");
		break;
	case 17: //Black
		sv_skyname->SetValue("sky_l4d_rural02_ldr");
		break;
	case 18: //Vertigo HD
		sv_skyname->SetValue("vertigo_hdr");
		break;
	case 19: //Vertigo Blue HD
		sv_skyname->SetValue("vertigoblue_hdr");
		break;
	case 20: //Vertigo
		sv_skyname->SetValue("vertigo");
		break;
	case 21: //Vietnam
		sv_skyname->SetValue("vietnam");
		break;
	case 22: //Dusty Sky
		sv_skyname->SetValue("sky_dust");
		break;
	case 23: //Jungle
		sv_skyname->SetValue("jungle");
		break;
	case 24: //Nuke
		sv_skyname->SetValue("nukeblank");
		break;
	case 25: //Office
		sv_skyname->SetValue("office");
		break;
	}

}
void Visuals::Misc::ColorModulateSkybox()

{
	if (g_Options.ColorSkybox)
	{
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			const char* group = pMaterial->GetTextureGroupName();
			const char* name = pMaterial->GetName();
			if (strstr(pMaterial->GetTextureGroupName(), "SkyBox textures"))
			{
				switch (g_Options.SkyColor1)
				{
				case 0:
					pMaterial->ColorModulate(1, 1, 1);
					break;
				case 1:
					pMaterial->ColorModulate(0.77, 0.02, 0.77);
					break;
				case 2:
					pMaterial->ColorModulate(0.77, 0.02, 0.02);
					break;
				case 3:
					pMaterial->ColorModulate(0.02, 0.02, 0.77);
					break;
				case 4:
					pMaterial->ColorModulate(0.02, 0.77, 0.02);
					break;
				}
			}

		}
	}
}

void Visuals::Misc::ThirdPerson() {
	if (!g_LocalPlayer)
		return;

	if (g_LocalPlayer->IsAlive() && g_Options.misc_thirdperson)
	{
		g_Input->m_fCameraInThirdPerson = true;
		
	}
	else
		g_Input->m_fCameraInThirdPerson = false;
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}


void Visuals::Misc::RecoilCrosshair()
{
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	int x = w / 2;
	int y = h / 2;
	int dy = h / 97;
	int dx = w / 97;
	QAngle punchAngle = g_LocalPlayer->m_aimPunchAngle();
	x -= (dx*(punchAngle.yaw));
	y += (dy*(punchAngle.pitch));
	if (g_Options.RecoilCrosshair)
	{
		if (!g_LocalPlayer)
			return;
		if (!g_LocalPlayer->IsAlive())
			return;
		if (g_LocalPlayer && g_LocalPlayer->IsAlive())
		{

			//if (g_LocalPlayer->m_hActiveWeapon()->IsSniper());
			//return;

			g_VGuiSurface->DrawSetColor(g_Options.color_esp_crosshair);
			g_VGuiSurface->DrawLine(x - g_Options.RecoilCrosshairSize, y, x + g_Options.RecoilCrosshairSize, y); 
			g_VGuiSurface->DrawLine(x, y - g_Options.RecoilCrosshairSize, x, y + g_Options.RecoilCrosshairSize);
		}
	}
}

void Visuals::Misc::Noscope()
{
	if (g_Options.Noscope && g_LocalPlayer && g_LocalPlayer->IsAlive() && g_LocalPlayer->m_iHealth() > 0)
	{
		auto actWeaponHandle = g_LocalPlayer->m_hActiveWeapon();
		if (!actWeaponHandle)
			return;

			if (actWeaponHandle->IsSniper())
			{   
				int width = 0;
				int height = 0;
				g_EngineClient->GetScreenSize(width, height);
				int centerX = static_cast<int>(width * 0.5f);
				int centerY = static_cast<int>(height * 0.5f);

				if (g_LocalPlayer->m_bIsScoped())
				{
					g_VGuiSurface->DrawSetColor(Color::Black);
					g_VGuiSurface->DrawLine(0, centerY, width, centerY);
					g_VGuiSurface->DrawLine(centerX, 0, centerX, height);
				}
			}
	}
}
 void Visuals::Hitbox(int index)
{
	if (!g_Options.esp_hitbox)
		return;

	C_BasePlayer* pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	float duration = g_Options.HitboxDuration;
	int durationAlpha = g_Options.HitboxAlphaDuration;

	if (index < 0)
		return;

	Color color = Color(int(g_Options.color_hitboxCol[0]), int(g_Options.color_hitboxCol[1]), int(g_Options.color_hitboxCol[2]));

	auto entity = reinterpret_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));

	if (!entity)
		return;

	studiohdr_t* pStudioModel = g_MdlInfo->GetStudioModel(entity->GetModel());

	if (!pStudioModel)
		return;

	static matrix3x4_t pBoneToWorldOut[128];

	if (!entity->SetupBones(pBoneToWorldOut, MAXSTUDIOBONES, 256,g_GlobalVars->curtime))
		return;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);
	if (!pHitboxSet)
		return;

	for (int i = 0; i < pHitboxSet->numhitboxes; i++)
	{
		mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		Math::VectorTransform(pHitbox->bbmin, pBoneToWorldOut[pHitbox->bone], vMin); //nullptr???
		Math::VectorTransform(pHitbox->bbmax, pBoneToWorldOut[pHitbox->bone], vMax);

		if (pHitbox->m_flRadius > -1)
		{
			g_DebugOverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, color.r(), color.g(), color.b(), durationAlpha, duration);
		}
	}
}


