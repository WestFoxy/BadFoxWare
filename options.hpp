#pragma once
#include <string>
#include "valve_sdk/Misc/Color.hpp"
#include "MainInclude.hpp"
#include<list>
#include <d3dx9.h>
#include <d3d9.h>
#include <shlobj.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <unordered_map>
#include "SkinChanger.h"

#define OPTION(type, var, val) type var = val
bool Save(std::string file_name);
bool Load(std::string file_name);
void CreateConfig(std::string name);
void Delete(std::string name);
std::vector<std::string> GetConfigs();
namespace fs = std::filesystem;



static const char* KeyStrings[] = {
	"",
	"Mouse 1",
	"Mouse 2",
	"Cancel",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

};

static const char* SelectionMode[] =
{
	"Fov",
	"Distance",
	"Cycle",
	"Velocity",
	"Health"
};

static const char* ResolverType[] =
{
	"Default",
	"Experimental"
};

static const char* pzj[] =
{
	"Off",
	"Low",
	"High"

};
static const char* HitboxMode[] =
{
	"Off",
	"Head",
	"Stomach"
};

static const char* Multimem2[] =
{
	"Disabled",
	"Full"
};

static const char* Multimem[] =
{
	"Disabled",
	"Full",

};
static const char* HitscanMode[] =
{
	"Disabled",
	"Low",
	"Medium",
	"High",
	"Ultra"
};

static const char* MultipointMode[] =
{
	"Disabled",
	"Normal",
	"Full"
};

static const char* FakelagMovement[] =
{
	"On ground",
	"On jump"
};

static const char* FakelagMode[] =
{
	"Off",
	"Factor",
	"Unit Break"
};

static const char* HeightCompensation[] =
{
	"Spread",
	"No-spread"
};

static const char* PreferBodyaim[] =
{
	"Disabled",
	"Health based",
	"If lethal ",
	"Smart"
};

static const char* AntiaimbotPitch[] =
{
	"Disabled",
	"Down",
	"Zero",
	"Up",
	"Switch"

};

static const char* Skyboxmode[] =
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


static const char* bbb[] =
{
	"Disabled",
	"Manual",
	"Freestanding"
};

static const char* AntiaimbotYawa[] =
{
	"Disabled",
	"Back",
	"Fake lby",
	"Fatality.win",
	"180Z",
	"Random",
	"Jitter"
};
static const char* AntiaimbotYaw[] =
{
	"Disabled",
	"Back",
	"Fake LBY",
	"Jitter LBY",
	"Yaw Z",
	"Random",
	"Jitter"
};

static const char* AntiaimbotYawRun[] =
{
	"Disabled", "Backwards", "jitters", "Swap", "yawz", "Rotate", "Random"
};

static const char* AntiaimbotYawFake[] =
{
	"Disabled", "Backwards", "jitters", "Swap Rotate", "yawz", "Rotate", "Random"
};

static const char* Freestanding[] =
{
	"Disabled",
	"type 1",
	"type 2"
};

static const char* AtTarget[] =
{
	"Disabled",
	"Closest",
	"Average"
};

static const char* ModelsMode[] =
{
	"Flat",
	"Covered",
	"Reflective"
};

static const char* manualtype[] =
{
	"Arrows",
	"Text"
};

static const char* rifles[] =
{
	"Disabled",
	"Auto",
	"AWP",
	"SSG08",
	"Negev"
};

static const char* pistols[] =
{
	"Disabled",
	"Elites",
	"Deagle/R8",
	"P250"
};

static const char* other[] =
{
	"Disabled",
	"He/Molo/Armor"
};


static const char* Fakeghost[] =
{
	"Disabled",
	"Fake Angle",
	"Lowerbody"
};

static const char* ThirdpersonAngles[] =
{
	"Fake",
	"Real",
	"Lowerbody"
};

static const char* DroppedWeapons[] =
{
	"Disabled",
	"Text"
};

static const char* Configs[] =
{
	"Scar",
	"Scout",
	"Awp",
	"Duals",
	"Revolver",
	"Other"
};


enum Knives_t
{
	KNIFE_DEFAULT,
	KNIFE_BAYONET,
	KNIFE_FLIP,
	KNIFE_GUT,
	KNIFE_KARAMBIT,
	KNIFE_M9BAYONET,
	KNIFE_HUNTSMAN,
	KNIFE_FALCHION,
	KNIFE_BOWIE,
	KNIFE_BUTTERFLY,
	KNIFE_PUSHDAGGER
};
class Config
{
public:
	std::string gladTag = "mov eax, byte ptr [\"love fox\"] ";

	bool Manual_Side;
	Vector AntiAimRealAngle;
	bool antiaim_freestanding, antiaim_enabled;
	bool DesyncEnabledSpecialChecks;

	int desync;
	int yaw_standing, yaw_moving, yaw_air, antiaim_freestanding_mode;
	int pitch_standing, pitch_moving, pitch_air;
	int aa_jitter;

	bool clan_tag_animate = false;
	bool desync_aa, should_send_packet;
	int aa_mode; 

	bool aa_bool;
	int aa_left, aa_right, aa_back;
	int aa_real_type;
	int aa_real1_type;
	int aa_real2_type;
	Vector RealAngle = Vector(0,0,0);
	Vector FakeAngle = Vector(0,0,0);
	Vector LastNetworkedLocalOrigin = Vector(0, 0, 0);
	bool FakeAng1type = false;
	bool FakeAng2type = false;

	bool FakeDuck = true;
	bool IsFakeDucking = false;
	int FakeDuckKey = 'F';
	int FakeDuckTestAmount = 7;

	float fakeLBYValue = 0;

	bool* bSendPacketLink;

	bool mutex_validrecords_lock = false;

	float LBYLocalUpdate;
	bool IsFakeWalking;
	bool rage_SlowWalk = false;
	int rage_SlowWalk_key = VK_LSHIFT;
	float rage_SlowWalk_speed = 45.0f;

	int aa_fake_type;
	int aa_fake1_type;
	int aa_fake2_type;

	int aa_pitch_type;
	int aa_pitch1_type;
	int aa_pitch2_type;

	float aa_realadditive_val;
	float aa_fakeadditive_val;

	float aa_realadditive1_val;
	float aa_fakeadditive1_val;
	float delta1_val;

	float aa_realadditive2_val;
	float aa_fakeadditive2_val;
	float delta2_val;

	float spinangle;
	float spinspeed;

	float spinangle1;
	float spinspeed1;

	float spinangle2;
	float spinspeed2;

	float spinanglefake;
	float spinspeedfake;

	float spinanglefake1;
	float spinspeedfake1;

	float spinanglefake2;
	float spinspeedfake2;

	bool lbyflickup;
	bool lbyflickup1;
	bool lbyflickup2;
	bool ForceBody = false;
	bool yourteamchams = false;
	bool theirchams = false;
	bool chams_player_regular = false;
	bool EndRound = false;
	bool EyePosESP = false;
	bool FovArrrows = false;
	int knifemodel;
	float esp_player_chams_color_t_not_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_chams_color_ct_not_visible[4] = { 0.15f, 0.7f, 1.f, 1.0f };
	float esp_player_chams_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_mine[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_you[4] = { 0.f, 0.f, 0.9f, 1.f };
	float esp_player_chams_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_chams_color_t_visibleBT[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_chams_color_ct_visibleBT[4] = { 0.15f, 0.7f, 1.f, 1.0f };
	float esp_player_chams_color_tBT[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_ctBT[4] = { 0.f, 0.2f, 1.f, 1.f };
	float color_chams_player_a_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float color_chams_player_e_visible[4] = { 1.f, 0.f, 0.0f, 1.f };
	float color_chams_player_a_occluded[4] = { 0.f, 1.f, 0.0f, 1.f };
	float color_chams_player_e_occluded[4] = { 0.f, 0.f, 1.0f, 1.f };

	float Fakelat = 0.0;
	float fakelatency_amt;

	int iTab;

	int GlowType;
	int BoxType = 1;



	int BTType;
	int GrenadeBox;
	
	
	int FakelagMode = 0;
	int FakelagAmount = 3;
	bool FakeLagEnable = false;
	bool FakelagShoot = false;
	//
	//AIM
	//
	OPTION(int, esp_player_chams_type, 0);
	OPTION(bool, misc_fake_angles, false);
	OPTION(bool, misc_foot_trails, true);
	OPTION(float, misc_foot_trails_lenght, 10);
	OPTION(int, misc_foot_trails_segments, 2);
	OPTION(float, misc_foot_trails_delay_between, 0.00001);
	OPTION(float, misc_foot_trails_startsize, 2);
	OPTION(float, misc_foot_trails_endsize, 2);
	OPTION(float, misc_foot_trails_rainbowdelta, 0.001);
	OPTION(float, misc_foot_trails_xshift, 0);
	OPTION(float, misc_foot_trails_yshift, 0);
	OPTION(float, misc_foot_trails_zshift, 14);

	//0.001f
	OPTION(bool, misc_backtrack, true);
	OPTION(bool, misc_foxlog, false);
 	OPTION(float, visuals_lagcomp_duration, 0.5, 5);
	OPTION(bool, lagcomp_hitboxes, false);
	OPTION(float, ChamsAlphaBT, 0.1,1);
	float ChamsAlpha = 0.9;

	OPTION(bool, esp_hitbox, false);
	float RecoilCrosshairSize = 2;
	
	float HitboxDuration;
	int backtrackticks;
	int TrigTypeCombo;
	int HitboxAlphaDuration;
	struct
	{
		bool Enable;
		int	 Key;
		bool Head;
		bool Arms;
		bool Chest;
		bool Stomach;
		bool Legs;

		//rifles
		bool Rifles_Enable;
		int Rifles_Key;
		bool Rifles_Head;
		bool Rifles_Arms;
		bool Rifles_Chest;
		bool Rifles_Stomach;
		bool Rifles_Legs;
		//pistols
		bool Pistols_Enable;
		int Pistols_Key;
		bool Pistols_Head;
		bool Pistols_Arms;
		bool Pistols_Chest;
		bool Pistols_Stomach;
		bool Pistols_Legs;
		//snipers
		bool Snipers_Enable;
		int Snipers_Key;
		bool Snipers_Head;
		bool Snipers_Arms;
		bool Snipers_Chest;
		bool Snipers_Stomach;
		bool Snipers_Legs;

	}Trigger;
	OPTION(bool, esp_no_smoke, false);
	OPTION(bool, legit_enabled, false);
	OPTION(int, aimkey, VK_LBUTTON);

	OPTION(bool, legit_rcs, false);
	OPTION(int, LegitType,NULL);
	OPTION(bool, enable_legitbot, false);
	OPTION(int, enable_legitbot_key, VK_MENU);
	OPTION(int, forcebody_legitbot_key, VK_DELETE);
	//pistols
	OPTION(int, hitbox_pistols, 0);
	OPTION(bool, aim_LegitBotPistols, true);
	OPTION(float, legit_fov_pistols, 2.f);
	OPTION(float, legit_smooth_pistols, 1.7);
	OPTION(float, legit_rcsmin_pistols, 0.f);
	OPTION(float, legit_rcsmax_pistols, 0.f);
	OPTION(bool, rage_baimlethal, true);
	OPTION(bool, rage_resolver, false);
	OPTION(bool, rage_backward_head_scan, true);
	OPTION(int, rage_debug_currentweapontype, 0);


	OPTION(int, rage_baimlethal_mode, 0);
	OPTION(int, rage_baimautodamage, 40);

	bool Rage_AA_enable = false;

	// rifles
	OPTION(int, hitbox_rifles, 0);
	OPTION(bool, aim_LegitBotRifles, true);
	OPTION(float, legit_smooth_rifles, 1.7);
	OPTION(float, legit_fov_rifles, 2.f);
	OPTION(float, legit_rcsmin_rifles, 0.f);
	OPTION(float, legit_rcsmax_rifles, 0.f);
	//snipers
	OPTION(int, hitbox_snipers, 0);
	OPTION(bool, aim_LegitBotSnipers, true);
	OPTION(float, legit_smooth_Snipers, 1.5);
	OPTION(float, legit_fov_Snipers, 15.f);
	OPTION(float, legit_rcsmin_Snipers, 0.f);
	OPTION(float, legit_rcsmax_Snipers, 0.f);
	OPTION(float, hitchance_amount, 0.f);
    // 
    // ESP
    // 
    OPTION(bool, esp_enabled, true);
    OPTION(bool, esp_enemies_only, true);
    OPTION(bool, esp_player_boxes, true);
    OPTION(bool, esp_player_names, false);
	//OPTION(bool, boneESP         , false);
    OPTION(bool, esp_player_health, false);
    OPTION(bool, esp_player_armour, false);
	OPTION(bool, esp_player_Skel, false);
    OPTION(bool, esp_player_weapons, false);
	OPTION(bool, KevESP, false);
	OPTION(bool, BombInfo, true);
	OPTION(bool, RecoilCrosshair, false);
	OPTION(bool, esp_player_Flash,false);
	OPTION(bool, GrenadeESP, false);
	OPTION(bool, HeadEsp, false); 
    OPTION(bool, esp_player_snaplines, false);
    OPTION(bool, esp_crosshair, false);
    OPTION(bool, esp_dropped_weapons, false);
    OPTION(bool, esp_defuse_kit, false);
    OPTION(bool, esp_planted_c4, true);
	OPTION(int, esp_type, 0);

    // 
    // GLOW
    // 
    OPTION(bool, glow_enabled, false);
    OPTION(bool, glow_enemies_only, false);
    OPTION(bool, glow_players, false);
    OPTION(bool, glow_chickens, false);
    OPTION(bool, glow_c4_carrier, false);
    OPTION(bool, glow_planted_c4, false);
    OPTION(bool, glow_defuse_kits, false);
    OPTION(bool, glow_weapons, false);

    //
    // CHAMS
    //
	OPTION(bool, chams_wep_enabled, false);
	OPTION(bool, backtrackchams1tick, false); 
	OPTION(bool, chams_wep_wireframe, false);
	OPTION(bool, chams_wep_flat, false);
	OPTION(bool, chams_wep_ignorez, false);
	OPTION(bool, chams_wep_glass, false);
    OPTION(bool, chams_Metal_enabled, false);
	OPTION(bool, chams_player_enabled, false);
    OPTION(bool, chams_player_enemies_only, false);
    OPTION(bool, chams_player_wireframe, false);
    OPTION(bool, chams_player_flat, false);
    OPTION(bool, chams_player_ignorez, false);
    OPTION(bool, chams_player_glass, false);
    OPTION(bool, chams_arms_enabled, false);
    OPTION(bool, chams_arms_wireframe, false);
    OPTION(bool, chams_arms_flat, false);
    OPTION(bool, chams_arms_ignorez, false);
    OPTION(bool, chams_arms_glass, false);
	bool chams_player_regular_IgnoreZ = false;
	bool chams_player_flat_IgnoreZ = false;
	bool chams_player_wireframe_IgnoreZ = false;

	OPTION(float, fake_chams_r, 1);
	OPTION(float, fake_chams_g, 1);
	OPTION(float, fake_chams_b, 1);
	OPTION(float, fake_chams_a, 1);

    //
    // MISC
    //
	OPTION(bool, spec_list, false);
	OPTION(int, menu_background_image, 1);
    OPTION(bool, misc_bhop, true);
	OPTION(bool, misc_fastduck, true);
		//OPTION(int, misc_circle_key, (int)'C');
	int misc_circle_key = (int)'C';
	OPTION(bool, misc_revealranks, false);
	OPTION(bool, misc_autostrafe, false);
	OPTION(bool, misc_namespam, false);
    OPTION(bool, misc_no_hands, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(bool, misc_legitdesync, false);
	OPTION(bool, misc_legitdesync_flip, false);
	OPTION(int, misc_legitdesync_keyflip, VK_LEFT);
	OPTION(bool, misc_KnifeLeft, false);
	OPTION(bool, misc_circle_enable, false);
	OPTION(bool, SniperX, false);
	OPTION(bool, Radar, false);
	OPTION(bool, Noscope, false);
	OPTION(bool, flash_remove, true);
	OPTION(bool, GrenadeTra, true);
	OPTION(bool, backtrackchams, false); 
	OPTION(bool, misc_hitmarker, true);
	int Hitsound = 0;
	//OPTION(bool, misc_DamageInd, false);
	//OPTION(bool, misc_bulletshow, false);
	//OPTION(int, misc_BeamType, NULL); 
	//OPTION(bool, misc_BuyBot, false);
	//OPTION(bool, misc_EventLogs, false);
	OPTION(float, misc_thirdperson_dist, 150.f);
    OPTION(int, viewmodel_fov, 68);
    OPTION(float, mat_ambient_light_r, 0.0f);
    OPTION(float, mat_ambient_light_g, 0.0f);
    OPTION(float, mat_ambient_light_b, 0.0f);
	OPTION(bool, NightMode, false);
    // 
    // COLORS
    // 
	OPTION(int,SkyColor1,NULL);
	OPTION(int, SkyBoxChanger,0);
	OPTION(int, TypeOfESP, NULL);
	OPTION(bool, ColorSkybox, false);
	float g_fMColor[4] = { 0.21f, 0.21f, 0.21f, 1.0f }; //RGBA color
	float g_fBColor[4] = { 0.64f, 0.f, 0.35f, 1.0f };
	float g_fTColor[4] = { 1.f, 1.f, 1.f, 1.0f };
    OPTION(Color, color_esp_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_esp_ally_occluded, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_occluded, Color(255, 0, 0));
	OPTION(Color, color_esp_Box, Color(255, 0, 0));
    OPTION(Color, color_esp_crosshair, Color(255, 255, 255));
	OPTION(Color, color_esp_DamgeIndicator,Color(255, 0, 0));
    OPTION(Color, color_esp_weapons, Color(128, 0, 128));
    OPTION(Color, color_esp_defuse, Color(0, 128, 255));
    OPTION(Color, color_esp_c4, Color(255, 255, 255));
	OPTION(Color, color_esp_playerweapons, Color(255, 255, 0));
	OPTION(Color, color_esp_HeadESP, Color(255, 0, 0));
	OPTION(Color, color_esp_playername, Color(255, 255, 255));

    OPTION(Color, color_glow_ally, Color(0, 128, 255));
	OPTION(unsigned int, color_background_image, 0xFFFFFF90);
    OPTION(Color, color_glow_enemy, Color(255, 0, 0));
    OPTION(Color, color_glow_chickens, Color(0, 128, 0));
    OPTION(Color, color_glow_c4_carrier, Color(255, 255, 0));
    OPTION(Color, color_glow_planted_c4, Color(128, 0, 128));
    OPTION(Color, color_glow_defuse, Color(255, 255, 255));
    OPTION(Color, color_glow_weapons, Color(255, 128, 0));

    OPTION(Color, color_chams_player_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_chams_player_ally_occluded, Color(0, 255, 128));
    OPTION(Color, color_chams_player_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_chams_player_enemy_occluded, Color(255, 128, 0));
    OPTION(Color, color_chams_arms_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_T, Color(255, 22, 22));
	OPTION(Color, color_chams_CT, Color(22, 22, 255));

    OPTION(Color, color_chams_arms_occluded, Color(0, 128, 255));
	OPTION(Color, color_chams_wep_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_wep_occluded, Color(0, 128, 255));
	OPTION(Color, color_Scoped_wep,         Color(0, 0, 0));
	OPTION(Color, color_hitboxCol,          Color(1.f, 0.f, 0.f));
	OPTION(Color, color_arrows, Color(213,0,255,255));
	OPTION(Color, color_Skel,				Color(255,255,255));
	OPTION(Color, FlashCol,                 Color(255, 255, 255));
	OPTION(Color, GrenESPCol,               Color(255, 0, 0));
	OPTION(bool, IsNewMap, true);
	OPTION(bool, misc_namestealer, false);
	OPTION(float, misc_namestealer_speed, 0.3f);
	OPTION(float, misc_clantag_speed, 0.5f);
	OPTION(int, misc_clantag_type, 0);
	OPTION(bool, misc_antikick, true);
	OPTION(bool, misc_overlay, true);
	OPTION(bool, misc_edge_jump, true);
	OPTION(bool, misc_edge_jump_strafe, false);
	OPTION(int, misc_edge_jumpkey, VK_MENU);
	OPTION(float, misc_antikick_scramble, 75.0f);
	OPTION(int, knife_paint_kit, 413);
	OPTION(int, glove_paint_kit, 5027);
	OPTION(int, glove_index, 0);
	//misc_bullettracers
	OPTION(bool, misc_bullettracers, false);
	OPTION(bool, misc_bullettracers_dlight, false);
	OPTION(Color, misc_bullettracers_color, Color(255,255,255));
	OPTION(float, misc_bullettracersdlight_radius, 55.0f);
	OPTION(float, misc_bullettracersdlight_delta, 6.0f);
	OPTION(Color, misc_bullettracers_dlightcolor, Color(255, 255, 255));
	//esp_bullettrace_length
	OPTION(float, esp_bullettrace_length, 5.0f);
	OPTION(bool, esp_grenades_esp, true);
	OPTION(float, esp_fade_speed, 0.5f);
	//misc_bullettracers_color
	//SKIN
	OPTION(int, skin_awp , 838);
	OPTION(int, skin_m4a1_s, 383);
	OPTION(int, skin_m4a4, 844);
	OPTION(int, skin_deagle, 841);
	OPTION(int, skin_ak, 639);
	OPTION(int, skin_p250, 551);
	OPTION(int, skin_beretas, 747);
	OPTION(int, skin_ump, 851);
	OPTION(int, skin_famas, 604);
	OPTION(int, skin_glock, 694);
	OPTION(int, skin_usp, 637);
	OPTION(int, skin_revolver, 522);
	OPTION(int, skin_cz, 315);
	OPTION(int, skin_aug, 845);
	OPTION(int, skin_ssg08, 319);
	OPTION(int, skin_sg, 750);
	OPTION(int, skin_scar20, 597);
	//g_Options.misc_no_hands
	OPTION(bool, skinchanger_enabled, true);
	char my_documents_folder[MAX_PATH];
	OPTION(int, misc_thirpers_key, 'V');
	//OPTION(int, skin_scar_t, );

	//RAGE
	/*OPTION(bool, HitScan1, true);
	OPTION(bool, HitScan2, false);
	OPTION(bool, HitScan3, false);
	OPTION(bool, HitScan4, false);
	OPTION(bool, HitScan5, false);
	OPTION(bool, HitScan6, false);
	OPTION(bool, HitScan7, false);

	OPTION(bool, auto_fire, true);
	OPTION(bool, rage_enabled, false);
	OPTION(bool, no_recoil, true);
	OPTION(bool, extrapolation, true);

	OPTION(float, pointscale, 1.0f);
	OPTION(float, minimal_damage, 99.f);
	OPTION(float, hit_chance, 91.f);
	
	OPTION(float, rage_aimstep, 3.50f);
	OPTION(bool, rage_aimstep_enabled, false);*/
	OPTION(bool, auto_fire, true);
	OPTION(bool, rage_enabled, false);
	OPTION(bool, rage_enabled_bind, false);
	OPTION(int, rage_enabled_bindkey, VK_CAPITAL);

	struct RagedWeapon
	{
		OPTION(bool, HitScan1, true);
		OPTION(bool, HitScan2, false);
		OPTION(bool, HitScan3, false);
		OPTION(bool, HitScan4, false);
		OPTION(bool, HitScan5, false);
		OPTION(bool, HitScan6, false);
		OPTION(bool, HitScan7, false);

		
		
		OPTION(bool, no_recoil, true);
		OPTION(bool, extrapolation, true);

		OPTION(float, pointscale, 1.0f);
		OPTION(float, minimal_damage, 99.f);
		OPTION(float, hit_chance, 91.f);
		

		OPTION(float, rage_aimstep, 3.50f);
		OPTION(bool, rage_aimstep_enabled, false);
	}RageW[7];
	OPTION(bool, rage_autostop, true);
	OPTION(int, rage_autostop_mode, 0);

	OPTION(bool, rage_autocrouch, false);
	OPTION(bool, rage_autoscope, true);

	OPTION(bool, rage_forcebaim_bool, true);
	OPTION(int, rage_forcebaim_key, (int)'Z');
	OPTION(int, rage_forcebaim_mode, 0);

	OPTION(bool, rage_forcebaim_STATE, false);

	bool mmoving = false;
	bool limbsifwalking;
	float pointlimit;
	int hitscan = 0;
	bool autopoint;
	int hit_sound;
	int aim_type;
	
	int aa_pitch;
	float desync_range;
	OPTION(int, CreateMove_Crash_State, -1);
	OPTION(int, EAX_S, 0);
	OPTION(int, EBX_S, 0);
	OPTION(int, ECX_S, 0);
	OPTION(int, EDX_S, 0);
	OPTION(int, CS_S, 0);

	LPDIRECT3DDEVICE9 pDev = nullptr;
	IDirect3DTexture9 *tImage = nullptr;
	IDirect3DTexture9 *tImage_blackfox = nullptr;
	CUserCmd *cmd;

	bool GlobalSendPacket = true;
	
	Config()
	{
		CHAR my_documents[MAX_PATH];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

		this->config_path = my_documents;
		this->config_path += "\\L1SA\\";

		CreateDirectoryA(config_path.c_str(), NULL);

		//printf("Path: %s\nConfigs: \n", config_path.c_str());

		this->refresh();

	}

	void loadcfg(const char* name)
	{
		std::string tmp_dir = config_path;
		tmp_dir += name;

		std::ifstream foxcfg(tmp_dir);
		//foxcfg >> ;
		if (foxcfg.is_open())
		{
			foxcfg >> misc_revealranks;
			foxcfg >> misc_autostrafe;
			foxcfg >> misc_bhop;
			foxcfg >> misc_bullettracers;
			foxcfg >> misc_bullettracersdlight_delta;
			foxcfg >> misc_bullettracersdlight_radius;

			

			foxcfg >> misc_bullettracers_dlight;
			

			foxcfg >> misc_edge_jump;
			foxcfg >> misc_edge_jumpkey;
			foxcfg >> misc_edge_jump_strafe;

			foxcfg >> misc_fastduck;
			foxcfg >> misc_hitmarker;

			foxcfg >> misc_legitdesync;
			foxcfg >> misc_legitdesync_keyflip;

			foxcfg >> misc_namestealer_speed;
			foxcfg >> misc_overlay;

			foxcfg >> misc_thirdperson;
			foxcfg >> misc_thirdperson_dist;
			foxcfg >> misc_thirpers_key;

			foxcfg >> esp_bullettrace_length;
			foxcfg >> esp_crosshair;
			foxcfg >> esp_defuse_kit;
			foxcfg >> esp_dropped_weapons;
			foxcfg >> esp_enabled;
			foxcfg >> esp_enemies_only;
			foxcfg >> esp_grenades_esp;
			foxcfg >> esp_hitbox;
			foxcfg >> esp_planted_c4;
			foxcfg >> esp_type;
			foxcfg >> Radar;
			foxcfg >> BoxType;
			foxcfg >> EyePosESP;
			foxcfg >> GrenadeTra;
			foxcfg >> Noscope;


			foxcfg >> esp_player_armour;
			foxcfg >> esp_player_boxes;
			foxcfg >> esp_player_chams_type;
			foxcfg >> esp_player_Skel;
			foxcfg >> esp_player_names;
			foxcfg >> esp_player_weapons;
			foxcfg >> esp_player_health;
			foxcfg >> esp_player_snaplines;

			foxcfg >> FovArrrows;

			foxcfg >> chams_arms_enabled;
			foxcfg >> chams_arms_flat;
			foxcfg >> chams_arms_glass;
			foxcfg >> chams_arms_ignorez;
			foxcfg >> chams_arms_wireframe;
			foxcfg >> chams_Metal_enabled;
			foxcfg >> chams_player_enabled;
			foxcfg >> chams_player_enemies_only;
			foxcfg >> chams_player_flat;
			foxcfg >> chams_player_flat_IgnoreZ;
			foxcfg >> chams_player_glass;
			foxcfg >> chams_player_ignorez;
			foxcfg >> chams_player_regular;
			foxcfg >> chams_player_regular_IgnoreZ;
			foxcfg >> chams_player_wireframe;
			foxcfg >> chams_player_wireframe_IgnoreZ;
			
			foxcfg >> GlowType;
			foxcfg >> glow_c4_carrier;
			foxcfg >> glow_chickens;
			foxcfg >> glow_defuse_kits;
			foxcfg >> glow_enabled;
			foxcfg >> glow_enemies_only;
			foxcfg >> glow_planted_c4;
			foxcfg >> glow_players;
			foxcfg >> glow_weapons;

			foxcfg >> rage_enabled;
			foxcfg >> rage_SlowWalk;
			foxcfg >> rage_SlowWalk_key;
			foxcfg >> rage_SlowWalk_speed;
			foxcfg >> aa_bool;
			foxcfg >> aa_pitch_type;
			foxcfg >> aa_real_type;
			foxcfg >> desync_aa;
			foxcfg >> desync_range;
			foxcfg >> aa_left;
			foxcfg >> aa_back;
			foxcfg >> aa_right;

			for (int i = 0; i < 7; i++)
			{
				foxcfg >> RageW[i].extrapolation;
				foxcfg >> RageW[i].HitScan1;
				foxcfg >> RageW[i].HitScan2;
				foxcfg >> RageW[i].HitScan3;
				foxcfg >> RageW[i].HitScan4;
				foxcfg >> RageW[i].HitScan5;
				foxcfg >> RageW[i].HitScan6;
				foxcfg >> RageW[i].HitScan7;
				foxcfg >> RageW[i].hit_chance;
				foxcfg >> RageW[i].minimal_damage;
				foxcfg >> RageW[i].no_recoil;
				foxcfg >> RageW[i].pointscale;
				foxcfg >> RageW[i].rage_aimstep;
				foxcfg >> RageW[i].rage_aimstep_enabled;
			}

			foxcfg >> knife_paint_kit;
			foxcfg >> knifemodel;
			foxcfg >> skin_ak;
			foxcfg >> skin_aug;
			foxcfg >> skin_awp;
			foxcfg >> skin_awp;
			foxcfg >> skin_beretas;
			foxcfg >> skin_cz;
			foxcfg >> skin_deagle;
			foxcfg >> skin_famas;
			foxcfg >> skin_glock;
			foxcfg >> skin_m4a1_s;
			foxcfg >> skin_m4a4;
			foxcfg >> skin_p250;
			foxcfg >> skin_revolver;
			foxcfg >> skin_scar20;
			foxcfg >> skin_sg;
			foxcfg >> skin_usp;
			foxcfg >> skin_ump;
			foxcfg >> skin_ssg08;


			//LEGIT
			foxcfg >> legit_enabled;
			foxcfg >> legit_fov_pistols;
			foxcfg >> legit_fov_rifles;
			foxcfg >> legit_fov_Snipers;

			foxcfg >> legit_rcs;

			foxcfg >> legit_rcsmax_pistols;
			foxcfg >> legit_rcsmax_rifles;
			foxcfg >> legit_rcsmax_Snipers;

			foxcfg >> legit_rcsmin_pistols;
			foxcfg >> legit_rcsmin_rifles;
			foxcfg >> legit_rcsmin_Snipers;

			foxcfg >> legit_smooth_pistols;
			foxcfg >> legit_smooth_rifles;
			foxcfg >> legit_smooth_Snipers;

			foxcfg.close();
		}
	}

	void removecfg(const char* name)
	{
		std::string tmp_dir = config_path;
		tmp_dir += name;
		remove(tmp_dir.c_str());

		this->refresh();
	}

	void savecfg(const char* name)
	{
		std::string tmp_dir = config_path;
		tmp_dir += name;

		std::ofstream foxcfg(tmp_dir);

		if (foxcfg.is_open())
		{
			/*foxcfg << misc_revealranks << std::endl;
			foxcfg << misc_autostrafe << std::endl;
			foxcfg << misc_bhop << std::endl;
			
			foxcfg.close();*/
			foxcfg << misc_revealranks << std::endl;
			foxcfg << misc_autostrafe << std::endl;
			foxcfg << misc_bhop << std::endl;
			foxcfg << misc_bullettracers << std::endl;
			foxcfg << misc_bullettracersdlight_delta << std::endl;
			foxcfg << misc_bullettracersdlight_radius << std::endl;

			

			foxcfg << misc_bullettracers_dlight << std::endl;
			

			foxcfg << misc_edge_jump << std::endl;
			foxcfg << misc_edge_jumpkey << std::endl;
			foxcfg << misc_edge_jump_strafe << std::endl;

			foxcfg << misc_fastduck << std::endl;
			foxcfg << misc_hitmarker << std::endl;

			foxcfg << misc_legitdesync << std::endl;
			foxcfg << misc_legitdesync_keyflip << std::endl;

			foxcfg << misc_namestealer_speed << std::endl;
			foxcfg << misc_overlay << std::endl;

			foxcfg << misc_thirdperson << std::endl;
			foxcfg << misc_thirdperson_dist << std::endl;
			foxcfg << misc_thirpers_key << std::endl;

			foxcfg << esp_bullettrace_length << std::endl;
			foxcfg << esp_crosshair << std::endl;
			foxcfg << esp_defuse_kit << std::endl;
			foxcfg << esp_dropped_weapons << std::endl;
			foxcfg << esp_enabled << std::endl;
			foxcfg << esp_enemies_only << std::endl;
			foxcfg << esp_grenades_esp << std::endl;
			foxcfg << esp_hitbox << std::endl;
			foxcfg << esp_planted_c4 << std::endl;
			foxcfg << esp_type << std::endl;
			foxcfg << Radar << std::endl;
			foxcfg << BoxType << std::endl;
			foxcfg << EyePosESP << std::endl;
			foxcfg << GrenadeTra << std::endl;
			foxcfg << Noscope << std::endl;


			foxcfg << esp_player_armour << std::endl;
			foxcfg << esp_player_boxes << std::endl;
			foxcfg << esp_player_chams_type << std::endl;
			foxcfg << esp_player_Skel << std::endl;
			foxcfg << esp_player_names << std::endl;
			foxcfg << esp_player_weapons << std::endl;
			foxcfg << esp_player_health << std::endl;
			foxcfg << esp_player_snaplines << std::endl;

			foxcfg << FovArrrows << std::endl;

			foxcfg << chams_arms_enabled << std::endl;
			foxcfg << chams_arms_flat << std::endl;
			foxcfg << chams_arms_glass << std::endl;
			foxcfg << chams_arms_ignorez << std::endl;
			foxcfg << chams_arms_wireframe << std::endl;
			foxcfg << chams_Metal_enabled << std::endl;
			foxcfg << chams_player_enabled << std::endl;
			foxcfg << chams_player_enemies_only << std::endl;
			foxcfg << chams_player_flat << std::endl;
			foxcfg << chams_player_flat_IgnoreZ << std::endl;
			foxcfg << chams_player_glass << std::endl;
			foxcfg << chams_player_ignorez << std::endl;
			foxcfg << chams_player_regular << std::endl;
			foxcfg << chams_player_regular_IgnoreZ << std::endl;
			foxcfg << chams_player_wireframe << std::endl;
			foxcfg << chams_player_wireframe_IgnoreZ << std::endl;

			foxcfg << GlowType << std::endl;
			foxcfg << glow_c4_carrier << std::endl;
			foxcfg << glow_chickens << std::endl;
			foxcfg << glow_defuse_kits << std::endl;
			foxcfg << glow_enabled << std::endl;
			foxcfg << glow_enemies_only << std::endl;
			foxcfg << glow_planted_c4 << std::endl;
			foxcfg << glow_players << std::endl;
			foxcfg << glow_weapons << std::endl;

			foxcfg << rage_enabled << std::endl;
			foxcfg << rage_SlowWalk << std::endl;
			foxcfg << rage_SlowWalk_key << std::endl;
			foxcfg << rage_SlowWalk_speed << std::endl;
			foxcfg << aa_bool << std::endl;
			foxcfg << aa_pitch_type << std::endl;
			foxcfg << aa_real_type << std::endl;
			foxcfg << desync_aa << std::endl;
			foxcfg << desync_range << std::endl;
			foxcfg << aa_left << std::endl;
			foxcfg << aa_back << std::endl;
			foxcfg << aa_right << std::endl;

			for (int i = 0 ; i < 7 ; i++)
			{
				foxcfg << RageW[i].extrapolation << std::endl;
				foxcfg << RageW[i].HitScan1 << std::endl;
				foxcfg << RageW[i].HitScan2 << std::endl;
				foxcfg << RageW[i].HitScan3 << std::endl;
				foxcfg << RageW[i].HitScan4 << std::endl;
				foxcfg << RageW[i].HitScan5 << std::endl;
				foxcfg << RageW[i].HitScan6 << std::endl;
				foxcfg << RageW[i].HitScan7 << std::endl;
				foxcfg << RageW[i].hit_chance << std::endl;
				foxcfg << RageW[i].minimal_damage << std::endl;
				foxcfg << RageW[i].no_recoil << std::endl;
				foxcfg << RageW[i].pointscale << std::endl;
				foxcfg << RageW[i].rage_aimstep << std::endl;
				foxcfg << RageW[i].rage_aimstep_enabled << std::endl;
			}

			foxcfg << knife_paint_kit << std::endl;
			foxcfg << knifemodel << std::endl;
			foxcfg << skin_ak << std::endl;
			foxcfg << skin_aug << std::endl;
			foxcfg << skin_awp << std::endl;
			foxcfg << skin_awp << std::endl;
			foxcfg << skin_beretas << std::endl;
			foxcfg << skin_cz << std::endl;
			foxcfg << skin_deagle << std::endl;
			foxcfg << skin_famas << std::endl;
			foxcfg << skin_glock << std::endl;
			foxcfg << skin_m4a1_s << std::endl;
			foxcfg << skin_m4a4 << std::endl;
			foxcfg << skin_p250 << std::endl;
			foxcfg << skin_revolver << std::endl;
			foxcfg << skin_scar20 << std::endl;
			foxcfg << skin_sg << std::endl;
			foxcfg << skin_usp << std::endl;
			foxcfg << skin_ump << std::endl;
			foxcfg << skin_ssg08 << std::endl;


			//LEGIT
			foxcfg << legit_enabled << std::endl;
			foxcfg << legit_fov_pistols << std::endl;
			foxcfg << legit_fov_rifles << std::endl;
			foxcfg << legit_fov_Snipers << std::endl;

			foxcfg << legit_rcs << std::endl;

			foxcfg << legit_rcsmax_pistols << std::endl;
			foxcfg << legit_rcsmax_rifles << std::endl;
			foxcfg << legit_rcsmax_Snipers << std::endl;

			foxcfg << legit_rcsmin_pistols << std::endl;
			foxcfg << legit_rcsmin_rifles << std::endl;
			foxcfg << legit_rcsmin_Snipers << std::endl;

			foxcfg << legit_smooth_pistols << std::endl;
			foxcfg << legit_smooth_rifles << std::endl;
			foxcfg << legit_smooth_Snipers;

			foxcfg.close();

		}

	}

	

	void refresh()
	{
		cached_configs.clear();
		cached_configs_names.clear();
		for (const auto& entry : fs::directory_iterator(config_path))
		{
			const char* entry_data = entry.path().generic_string().c_str();

			//std::cout << entry_data << std::endl;

			cached_configs.push_back(entry_data);
		}


		std::string last_name;
		std::string delimiter = "//";

		for (auto& a : cached_configs)
		{
			std::string tmp(a.c_str());
			auto pos = tmp.find_last_of("/");
			last_name = tmp.substr(pos + 1);

			cached_configs_names.push_back(last_name);
		}


	}

	std::string config_path;
	std::vector <std::string>cached_configs;
	std::vector <std::string>cached_configs_names;

};


extern Config g_Options;
extern bool   g_Unload;
extern bool   g_Save;
extern bool   g_Load;
extern bool   g_Clear;
extern bool   g_View;
