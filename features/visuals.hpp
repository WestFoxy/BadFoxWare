#pragma once

class C_BasePlayer;
class C_BaseEntity;
class C_BaseCombatWeapon;
class C_PlantedC4;
class Color;
class ClientClass;
class CUserCmd;
extern unsigned long esp_font;
extern unsigned long hit_font;
extern float alpha_ent[150];

namespace Visuals
{
    namespace Player
    {
        bool Begin(C_BasePlayer* pl);

        void RenderBox();

        void RenderName();

        void RenderHealth();

        void RenderArmour();

        void RenderWeapon();

        void RenderSnapline();

		//void BoneESP(C_BasePlayer * entity);

		void HeadEsp(C_BasePlayer * entity);

		void KevESP(C_BasePlayer * entity);

		void EyePos(C_BasePlayer * ent);
		void DrawFovArrows(C_BasePlayer* entity, Color color);

    }

    namespace Misc
    {;

	void Radar();



	void RenderC4Info(C_PlantedC4 * ent);

	void Flash(C_BasePlayer * entity);

	void GrenESP(C_BaseCombatWeapon * m_nade);


	void EyePos(C_BasePlayer * ent);

	void Skel(C_BasePlayer * ent);

	void SniperX(C_BasePlayer * entity);

        void RenderWeapon(C_BaseCombatWeapon* ent)
			;
		void RenderNet(C_BasePlayer * Player);

		void RenderDefuseKit(C_BasePlayer * ent);

		//void RenderDefuseKit(C_BaseEntity* ent);
        void RenderPlantedC4(C_BaseEntity* ent);

		void NightMode();

		void KnifeLeft();

		void RecoilCrosshair();

		void SkyChanger();

		void ColorModulateSkybox();

		void ThirdPerson();

		void Noscope();

		void SetLocalPlayerReady();

		//void DamageInd;
		//void Recoilx(C_BaseEntity * loc, C_BasePlayer * ply);
    }
	/* if (g_Options.ColorSkybox)
	{
		Visuals::Misc::ColorModulateSkybox();
	}/**/
    bool CreateFonts();

    void DestroyFonts();

	bool IsVisibleScan(C_BasePlayer * player);

	void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char * msg, ...);

	void Hitbox(int index);
	
}
