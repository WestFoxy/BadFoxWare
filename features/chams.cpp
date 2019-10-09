
#include "chams.hpp"
#include <fstream>
#include <thread>
#include <mutex>
#include <iostream>

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"
#include "../LagComp.h"
#define PI 3.14159265358979323846f
#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TICKS_TO_TIME(t) ( g_GlobalVars->interval_per_tick * (t) )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define FLOW_OUTGOING    0
#define FLOW_INCOMING    1
Hooks::DrawModelExecute fnDME;
Hooks::SceneEnd_t fnSE;

Chams::Chams()
{
    std::ofstream("csgo\\materials\\simple_regular.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    std::ofstream("csgo\\materials\\simple_ignorez.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    std::ofstream("csgo\\materials\\simple_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    std::ofstream("csgo\\materials\\simple_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

    materialRegular = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
    materialRegularIgnoreZ = g_MatSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
    materialFlatIgnoreZ = g_MatSystem->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);
    materialFlat = g_MatSystem->FindMaterial("simple_flat", TEXTURE_GROUP_MODEL);
}

Chams::~Chams()
{
    std::remove("csgo\\materials\\simple_regular.vmt");
    std::remove("csgo\\materials\\simple_ignorez.vmt");
    std::remove("csgo\\materials\\simple_flat.vmt");
    std::remove("csgo\\materials\\simple_flat_ignorez.vmt");
}
void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass,const Color& rgba)
{
    IMaterial* material = nullptr;

    if(flat) {
        if(ignoreZ)
            material = materialFlatIgnoreZ;
        else
            material = materialFlat;
    } else {
        if(ignoreZ)
            material = materialRegularIgnoreZ;
        else
            material = materialRegular;
    }


    if(glass) {
        material = materialFlat;
        material->AlphaModulate(0.45f);
    } else {
        material->AlphaModulate(
            rgba.a() / 255.0f);
    }

		material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
		g_RenderView->SetColorModulation(
			rgba.r() / 255.0f,
			rgba.g() / 255.0f,
			rgba.b() / 255.0f);
	
	
	

    g_MdlRender->ForcedMaterialOverride(material);
}
void Chams::OverrideMaterialse(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba)
{
	IMaterial* material = nullptr;

	if (flat) {
		if (ignoreZ)
			material = materialFlatIgnoreZ;
		else
			material = materialFlat;
	}
	else {
		if (ignoreZ)
			material = materialRegularIgnoreZ;
		else
			material = materialRegular;
	}


	if (glass) {
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else {
		material->AlphaModulate(
			rgba.a() / 255.0f);
	}
	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	g_RenderView->SetColorModulation(rgba.r() / 255.0f ,rgba.g() / 255.0f ,rgba.b() / 255.0f);
}
bool Chams::Do_BTChams(IVModelRender* thisptr, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo)
{
	constexpr float temp[4] = { 166, 167, 169, 255 };
	if  (!g_Options.backtrackchams || !fnDME)
		return false;

	auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(pInfo.entity_index));
	IMaterial* mat = g_MatSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
	C_BasePlayer* Model_Entity = (C_BasePlayer*)g_EntityList->GetClientEntity(pInfo.entity_index);
	C_BasePlayer* pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	const char * Model_Name = g_MdlInfo->GetModelName(pInfo.pModel);
	if (!pLocal || !mat || !Model_Entity || !Model_Name || Model_Entity->m_iTeamNum() == pLocal->m_iTeamNum() || !strstr(Model_Name, "models/player") || Model_Entity == pLocal || !Model_Entity->m_iHealth())
		return false;
		bool used = false;
		if (validRecords.at(pInfo.entity_index).empty())
			return false;
		int validRecordsCount = 0;
		switch (g_Options.BTType)
		{
		case 0:
			for (auto& record : validRecords.at(pInfo.entity_index))
			{
				if (!record.matrix || !record.matrixbuilt)
					continue;
				//validRecordsCount++; is the original.
				validRecordsCount++;
			auto NormiData =   (float)validRecordsCount / validRecords.at(pInfo.entity_index).size();
				Color color = Color::FromHSB(NormiData, 1.f, 1.f);
				float	r =	color.r() / 255.0f;
				float	g =	color.g() / 255.0f;
				float	b =	color.b() / 255.0f;

				g_RenderView->SetBlend(g_Options.ChamsAlphaBT);
				g_MdlRender->ForcedMaterialOverride(materialRegular);
				g_RenderView->SetColorModulation(r, g, b);
				fnDME(thisptr, ctx, state, pInfo, record.matrix);
				if (!used)
					used = true;
			}
			return used;
			break;
		case 1:
			auto& record = validRecords.at(pInfo.entity_index).back();
			if (record.matrix && record.matrixbuilt)
			{
				g_MdlRender->ForcedMaterialOverride(mat);
				g_RenderView->SetBlend(g_Options.ChamsAlphaBT);
				g_RenderView->SetColorModulation(ent->m_iTeamNum() == 2 ? g_Options.esp_player_chams_color_tBT : g_Options.esp_player_chams_color_ctBT);
				fnDME(thisptr, ctx, state, pInfo, record.matrix);
				if (!used)
					used = true;
			}
			return used;
			break;
			/*
			case 2:
				for (auto& record : validRecords.at(pInfo.entity_index))
				{

					if (!record.matrix || !record.matrixbuilt) continue;
					g_RenderView->SetBlend(g_Options.ChamsAlphaBT);
					g_MdlRender->ForcedMaterialOverride(mat);
					OverrideMaterial(false, false, false, false, Color::FromHSB(50, 1.f, 1.f));
					fnDME(thisptr, ctx, state, pInfo, record.matrix);
					if (!used)
						used = true;

				}
				break;
				*/
		}
	}
std::vector<const char*> vistasmoke_mats =
{
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
};
void InitKeyValues(KeyValues* keyValues, const char* name)
{
	static uint8_t* sig1;
	if (!sig1)
	{
		sig1 = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "68 ? ? ? ? 8B C8 E8 ? ? ? ? 89 45 FC EB 07 C7 45 ? ? ? ? ? 8B 03 56");
		sig1 += 7;
		sig1 = sig1 + *reinterpret_cast<PDWORD_PTR>(sig1 + 1) + 5;
	}

	static auto function = (void(__thiscall*)(KeyValues*, const char*))sig1;
	function(keyValues, name);

}

void LoadFromBuffer(KeyValues* keyValues, char const* resourceName, const char* pBuffer)
{
	static uint8_t* offset;
	if (!offset) offset = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
	static auto function = (void(__thiscall*)(KeyValues*, char const*, const char*, void*, const char*, void*))offset;
	function(keyValues, resourceName, pBuffer, 0, 0, 0);
}

void Chams::OnDrawModelExecute(IMatRenderContext* ctx,const DrawModelState_t &state,const ModelRenderInfo_t &info,matrix3x4_t *matrix)
{
	static bool inited = false;
	if (!inited) {
		fnDME = Hooks::mdlrender_hook.get_original<Hooks::DrawModelExecute>(index::DrawModelExecute);
		inited = true;
	}
	const auto mdl = info.pModel;

	bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	bool is_wep = strstr(mdl->szName, "weapons/v_") != nullptr;
	bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;

	auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);
	if (g_Options.esp_no_smoke)
	{
		/*for (auto mat_s : vistasmoke_mats)
		{
			IMaterial* mat = g_MatSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}*/

		const char* pszModelName = g_MdlInfo->GetModelName(info.pModel); //Get the model name
		if (strstr(pszModelName, "flash") && g_Options.flash_remove) // Credits to dude719
		{
			//Get flashbang's materials
			IMaterial* Flash = g_MatSystem->FindMaterial("effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);
			IMaterial* FlashWhite = g_MatSystem->FindMaterial("effects\\flashbang_white", TEXTURE_GROUP_CLIENT_EFFECTS);

			//Set flag to not draw the passed materials
			Flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			FlashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			//Force to use our modified materials
			g_MdlRender->ForcedMaterialOverride(Flash);
			g_MdlRender->ForcedMaterialOverride(FlashWhite);
			g_MdlRender->ForcedMaterialOverride(NULL);
		}

		if (strstr(pszModelName, "smoke")) // Credits to Simon-Benyo
		{

			//Get smokegrenade's materials & Set flag to not draw the passed materials

			IMaterial* dust_motes = g_MatSystem->FindMaterial("particle\\smoke1\\dust_motes", TEXTURE_GROUP_PARTICLE);
			dust_motes->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* dust_motes_alpha = g_MatSystem->FindMaterial("particle\\smoke1\\dust_motes_alpha", TEXTURE_GROUP_PARTICLE);
			dust_motes_alpha->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* smoke1 = g_MatSystem->FindMaterial("particle\\smoke1\\smoke1", TEXTURE_GROUP_PARTICLE);
			smoke1->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* smoke1_additive = g_MatSystem->FindMaterial("particle\\smoke1\\smoke1_additive", TEXTURE_GROUP_PARTICLE);
			smoke1_additive->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* smoke1_nearcull = g_MatSystem->FindMaterial("particle\\smoke1\\smoke1_nearcull", TEXTURE_GROUP_PARTICLE);
			smoke1_nearcull->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* smoke1_nearcull2 = g_MatSystem->FindMaterial("particle\\smoke1\\smoke1_nearcull2", TEXTURE_GROUP_PARTICLE);
			smoke1_nearcull2->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* particle_smoke_dust = g_MatSystem->FindMaterial("particle\\particle_smoke_dust", TEXTURE_GROUP_PARTICLE);
			particle_smoke_dust->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* particle_smokegrenade = g_MatSystem->FindMaterial("particle\\particle_smokegrenade", TEXTURE_GROUP_PARTICLE);
			particle_smokegrenade->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			IMaterial* particle_smokegrenade1 = g_MatSystem->FindMaterial("particle\\particle_smokegrenade1", TEXTURE_GROUP_PARTICLE);
			particle_smokegrenade1->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);


			//Force to use our modified materials    
			g_MdlRender->ForcedMaterialOverride(dust_motes);
			g_MdlRender->ForcedMaterialOverride(dust_motes_alpha);
			g_MdlRender->ForcedMaterialOverride(smoke1);
			g_MdlRender->ForcedMaterialOverride(smoke1_additive);
			g_MdlRender->ForcedMaterialOverride(smoke1_nearcull);
			g_MdlRender->ForcedMaterialOverride(smoke1_nearcull2);
			g_MdlRender->ForcedMaterialOverride(particle_smoke_dust);
			g_MdlRender->ForcedMaterialOverride(particle_smokegrenade);
			g_MdlRender->ForcedMaterialOverride(particle_smokegrenade1);

			g_MdlRender->ForcedMaterialOverride(NULL);
		}
	}
	if (ent && g_LocalPlayer && ent->IsAlive()) {
		const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
		if (!enemy && g_Options.chams_player_enemies_only)
			return;
		if (ent && ent->IsAlive())
		{
			if (is_sleeve && g_Options.chams_arms_enabled) {
				auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
				if (!material)
					return;
				// 
				// Remove sleeves when drawing Chams.
				// 
				material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				g_MdlRender->ForcedMaterialOverride(material);
			}
			else if (is_arm)
			{
				auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
				if (!material)
					return;
				if (g_Options.misc_no_hands)
				{
					// 
					// No hands.
					// 
					material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);//https://i.imgur.com/tvPcI09.png
					g_MdlRender->ForcedMaterialOverride(material);
				}
				else if (g_Options.chams_arms_enabled) {
					if (g_Options.chams_arms_ignorez) {
						OverrideMaterial(
							true,
							g_Options.chams_arms_flat,
							g_Options.chams_arms_wireframe,
							false,
							g_Options.color_chams_arms_occluded);
						fnDME(g_MdlRender, ctx, state, info, matrix);
						OverrideMaterial(
							false,
							g_Options.chams_arms_flat,
							g_Options.chams_arms_wireframe,
							false,
							g_Options.color_chams_arms_visible);
					}
					else {
						OverrideMaterial(
							false,
							g_Options.chams_arms_flat,
							g_Options.chams_arms_wireframe,
							g_Options.chams_arms_glass,
							g_Options.color_chams_arms_visible);
					}
				}
				//https://i.imgur.com/tvPcI09.png //materialRegular
			}
			//https://i.imgur.com/tvPcI09.png
			if (g_Options.chams_arms_enabled) {
				if (is_wep && !is_arm) {
					auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);
					const char * ModelName = g_MdlInfo->GetModelName((model_t*)info.pModel);
					auto material = g_MatSystem->FindMaterial(ModelName, TEXTURE_GROUP_MODEL);//g_MatSystem->FindMaterial(ModelName, TEXTURE_GROUP_MODEL);

					if (!material)
						return;
					if (!g_LocalPlayer && !is_wep)
						return;
					if (g_LocalPlayer && is_wep)
						material->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, true);
					g_MdlRender->ForcedMaterialOverride(materialRegular);
					//OverrideMaterial(true, g_Options.chams_arms_flat,g_Options.chams_arms_wireframe,g_Options.chams_arms_glass,g_Options.color_chams_arms_visible);
					if (!g_LocalPlayer && !is_wep)
						return;
				}

				if (g_Options.chams_wep_enabled)
					if (is_wep && !is_arm) {
						auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);
						const char * ModelName = g_MdlInfo->GetModelName((model_t*)info.pModel);
						auto material = g_MatSystem->FindMaterial(ModelName, TEXTURE_GROUP_MODEL);
						if (!material)
							return;
						if (!g_LocalPlayer && !is_wep)
							return;                //https://i.imgur.com/tvPcI09.png

						if (g_Options.chams_wep_enabled && is_wep) {
							material->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, true);
							if (g_Options.chams_wep_ignorez) {
								OverrideMaterial(true, g_Options.chams_wep_flat, g_Options.chams_wep_wireframe, false, g_Options.color_chams_wep_visible);
								fnDME(g_MdlRender, ctx, state, info, matrix);
								OverrideMaterial(
									false,
									g_Options.chams_wep_flat,
									g_Options.chams_wep_wireframe,
									false,
									g_Options.color_chams_wep_visible);
							}
							else {
								OverrideMaterial(
									false,
									g_Options.chams_wep_flat,
									g_Options.chams_wep_wireframe,
									g_Options.chams_wep_glass,
									g_Options.color_chams_wep_visible);
							}
						}
					}
			}
		}
	}
}
/*
void Chams::ChamsSE(void * thisptr, void * edx)
{
	static bool inited = false;
	if (!inited) {
		fnSE = Hooks::mdlrender_hook.get_original<Hooks::SceneEnd_t>(index::SceneEnd);
		inited = true;
	}
	if (!g_LocalPlayer)
		return;
	const float colordefault[4] = { 0,0,0,255 };
	IMaterial* Sim_Reg = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
	IMaterial* Sim_Ignore = g_MatSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
	IMaterial* Flat_Reg = g_MatSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
	IMaterial* Flat_Ignore = g_MatSystem->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);
	for (int i = 1; i < g_GlobalVars->maxClients; i++)
	{
		auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
		const auto front = enemy ? g_Options.color_chams_player_e_visible : g_Options.color_chams_player_a_visible;
		const auto back = enemy ? g_Options.color_chams_player_e_occluded : g_Options.color_chams_player_a_occluded;
		if (g_Options.chams_player_enabled)
		{
			if (ent && ent->IsAlive())
			{
				if (g_Options.chams_player_ignorez)
				{
					g_RenderView->SetBlend(g_Options.ChamsAlpha);
					OverrideMaterialse(true, g_Options.chams_player_flat, g_Options.chams_player_wireframe, false, Color(back));
					ent->DrawModel(0x1, 255);
					g_MdlRender->ForcedMaterialOverride(nullptr);
					g_RenderView->SetBlend(g_Options.ChamsAlpha);
					OverrideMaterial(false, g_Options.chams_player_flat, g_Options.chams_player_wireframe, false, Color(front));
					ent->DrawModel(0x1, 255);
					g_MdlRender->ForcedMaterialOverride(nullptr);

				}











			}
		}






	}
}
*/

