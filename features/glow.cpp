#include "glow.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"

Glow::Glow()
{
}

Glow::~Glow()
{

}

void Glow::Shutdown()
{
    // Remove glow from all entities
    for(auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) {
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

        if(glowObject.IsUnused())
            continue;

        if(!entity || entity->IsDormant())
            continue;

        glowObject.m_flAlpha = 0.0f;
    }
}

void Glow::Run()
{
	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) {
		auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);
		glowObject.m_flAlpha = 50.0f;
		if (glowObject.IsUnused())
			continue;

		if (!entity || entity->IsDormant())
			continue;

		auto class_id = entity->GetClientClass()->m_ClassID;
		auto color = Color{};
		switch (class_id) {
		case CCSPlayer:
		{
			auto is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();

			if (entity->HasC4() && is_enemy && g_Options.glow_c4_carrier) {
				color = g_Options.color_glow_c4_carrier;
				break;
			}

			if (!g_Options.glow_players || !entity->IsAlive())
				continue;

			if (!is_enemy && g_Options.glow_enemies_only)
				continue;

			color = is_enemy ? g_Options.color_glow_enemy : g_Options.color_glow_ally;

			break;
		}
		case CChicken:
			if (!g_Options.glow_chickens)
				continue;
			entity->m_bShouldGlow() = true;
			color = g_Options.color_glow_chickens;
			break;
		case CBaseAnimating:
			if (!g_Options.glow_defuse_kits)
				continue;
			color = g_Options.color_glow_defuse;
			break;
		case CPlantedC4:
			if (!g_Options.glow_planted_c4)
				continue;
			color = g_Options.color_glow_planted_c4;
			break;
		case CHostage:
			if (!g_Options.glow_planted_c4)
				continue;
			color = g_Options.color_glow_planted_c4;
		default:
		{
			if (entity->IsWeapon()) {
				if (!g_Options.glow_weapons)
					continue;
				color = g_Options.color_glow_weapons;
			}
		}
		}
		switch (g_Options.GlowType)
		{
		case 0:
			glowObject.m_nGlowStyle = 0;
			break;
		case 1:
			glowObject.m_nGlowStyle = 2;
			break;
		case 2:
			glowObject.m_nGlowStyle = 1;
			break;
		}
			glowObject.m_flRed = color.r() / 255.0f;
			glowObject.m_flGreen = color.g() / 255.0f;
			glowObject.m_flBlue = color.b() / 255.0f;
			glowObject.m_flAlpha = color.a() / 255.0f;
			glowObject.m_bRenderWhenOccluded = true;
			glowObject.m_bRenderWhenUnoccluded = false;
		
	}
}
