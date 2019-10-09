#pragma once
#include <array>
#include "../IRefCounted.h"
#include "../helpers/utils.hpp"
#include "../singleton.hpp"
#include "../helpers/utils.hpp"
#include "../valve_sdk/netvars.hpp"
#include "Utils_fox.h"
#include <xmmintrin.h>

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define NETPROP(name, table, netvar) static RecvProp* name() \
{ \
	static auto prop_ptr = NetvarSys::Get().GetNetvarProp(table,netvar); \
	return prop_ptr; \
}
class CLocalPlayerExclusive
{
public:

	Vector GetViewPunchAngle()
	{
		return *(Vector*)((uintptr_t)this + 0x64);
	}
	Vector GetAimPunchAngle()
	{
		return *(Vector*)((uintptr_t)this + 0x70);
	}
	Vector GetAimPunchAngleVel()
	{
		return *(Vector*)((uintptr_t)this + 0x7C);
	}
};
struct datamap_t;
class AnimationLayer;
class CBasePlayerAnimState;
class CCSPlayerAnimState;
class CCSGOPlayerAnimState;
class C_BaseEntity;

struct c_animstate {

	char pad_0x0000[0x18]; //0x0000
	float anim_update_timer; //0x0018 
	char pad_0x001C[0xC]; //0x001C
	float started_moving_time; //0x0028 
	float last_move_time; //0x002C 
	char pad_0x0030[0x10]; //0x0030
	float last_lby_time; //0x0040 
	char pad_0x0044[0x8]; //0x0044
	float run_amount; //0x004C 
	char pad_0x0050[0x10]; //0x0050
	void* entity; //0x0060 
	__int32 active_weapon; //0x0064 
	__int32 last_active_weapon; //0x0068 
	float last_client_side_animation_update_time; //0x006C 
	__int32 last_client_side_animation_update_framecount; //0x0070 
	float eye_timer; //0x0074 
	float eye_angles_y; //0x0078 
	float eye_angles_x; //0x007C 
	float goal_feet_yaw; //0x0080 
	float current_feet_yaw; //0x0084 
	float torso_yaw; //0x0088 
	float last_move_yaw; //0x008C 
	float lean_amount; //0x0090 
	char pad_0x0094[0x4]; //0x0094
	float feet_cycle; //0x0098 
	float feet_yaw_rate; //0x009C 
	char pad_0x00A0[0x4]; //0x00A0
	float duck_amount; //0x00A4 
	float landing_duck_amount; //0x00A8 
	char pad_0x00AC[0x4]; //0x00AC
	Vector current_origin;
	Vector last_origin;
	float velocity_x; //0x00C8 
	float velocity_y; //0x00CC 
	char pad_0x00D0[0x10]; //0x00D0
	float move_direction_1; //0x00E0 
	float move_direction_2; //0x00E4 
	char pad_0x00E8[0x4]; //0x00E8
	float m_velocity; //0x00EC 
	float jump_fall_velocity; //0x00F0 
	float clamped_velocity; //0x00F4 
	float feet_speed_forwards_or_sideways; //0x00F8 
	float feet_speed_unknown_forwards_or_sideways; //0x00FC 
	float last_time_started_moving; //0x0100 
	float last_time_stopped_moving; //0x0104 
	bool on_ground; //0x0108 
	bool hit_in_ground_animation; //0x010C 
	char pad_0x0110[0x4]; //0x0110
	float last_origin_z; //0x0114 
	float head_from_ground_distance_standing; //0x0118 
	float stop_to_full_running_fraction; //0x011C 
	char pad_0x0120[0x14]; //0x0120
	__int32 is_not_moving; //0x0134 
	char pad_0x0138[0x20]; //0x0138
	float last_anim_update_time; //0x0158 
	float moving_direction_x; //0x015C 
	float moving_direction_y; //0x0160 
	float moving_direction_z; //0x0164 
	char pad_0x0168[0x44]; //0x0168
	__int32 started_moving; //0x01AC 
	char pad_0x01B0[0x8]; //0x01B0
	float lean_yaw; //0x01B8 
	char pad_0x01BC[0x8]; //0x01BC
	float poses_speed; //0x01C4 
	char pad_0x01C8[0x8]; //0x01C8
	float ladder_speed; //0x01D0 
	char pad_0x01D4[0x8]; //0x01D4
	float ladder_yaw; //0x01DC 
	char pad_0x01E0[0x8]; //0x01E0
	float some_pose; //0x01E8 
	char pad_0x01EC[0x14]; //0x01EC
	float body_yaw; //0x0200 
	char pad_0x0204[0x8]; //0x0204
	float body_pitch; //0x020C 
	char pad_0x0210[0x8]; //0x0210
	float death_yaw; //0x0218 
	char pad_0x021C[0x8]; //0x021C
	float stand; //0x0224 
	char pad_0x0228[0x8]; //0x0228
	float jump_fall; //0x0230 
	char pad_0x0234[0x8]; //0x0234
	float aim_blend_stand_idle; //0x023C 
	char pad_0x0240[0x8]; //0x0240
	float aim_blend_crouch_idle; //0x0248 
	char pad_0x024C[0x8]; //0x024C
	float strafe_yaw; //0x0254 
	char pad_0x0258[0x8]; //0x0258
	float aim_blend_stand_walk; //0x0260 
	char pad_0x0264[0x8]; //0x0264
	float aim_blend_stand_run; //0x026C 
	char pad_0x0270[0x8]; //0x0270
	float aim_blend_crouch_walk; //0x0278 
	char pad_0x027C[0x8]; //0x027C
	float move_blend_walk; //0x0284 
	char pad_0x0288[0x8]; //0x0288
	float move_blend_run; //0x0290 
	char pad_0x0294[0x8]; //0x0294
	float move_blend_crouch; //0x029C 
	char pad_0x02A0[0x4]; //0x02A0
	float speed; //0x02A4 
	__int32 moving_in_any_direction; //0x02A8 
	float acceleration; //0x02AC 
	char pad_0x02B0[0x74]; //0x02B0
	float crouch_height; //0x0324 
	__int32 is_full_crouched; //0x0328 
	char pad_0x032C[0x4]; //0x032C
	float velocity_subtract_x; //0x0330 
	float velocity_subtract_y; //0x0334 
	float velocity_subtract_z; //0x0338 
	float standing_head_height; //0x033C 
	char pad_0x0340[0x4]; //0x0340
}; //Size=0x0344

class C_BaseAnimState
{
public:
	char pad[4];
	char bUnknown; //0x4
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];
	float* feetyaw() //rofl
	{
		return reinterpret_cast<float*>((DWORD)this + 0x70);
	}
};

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class C_BaseEntity;

// Created with ReClass.NET by KN4CK3R
class CHudTexture
{
public:
	char szShortName[64];    //0x0000
	char szTextureFile[64];  //0x0040
	bool bRenderUsingFont;   //0x0080
	bool bPrecached;         //0x0081
	int8_t cCharacterInFont; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t hFont;          //0x0084
	int32_t iTextureId;      //0x0088
	float afTexCoords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};
class C_EconItemViewBT
{
public:
	char _pad_0x0000[0x14];
	CUtlVector<IRefCounted*>	m_CustomMaterials; //0x0014
	char _pad_0x0028[0x1F8];
	CUtlVector<IRefCounted*>	m_VisualsDataProcessors; //0x0220
};

class C_EconItemView
{
private:
	using str_32 = char[32];
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(int32_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");

	CUtlVector<IRefCounted*>& m_CustomMaterials();
	CUtlVector<IRefCounted*>& m_VisualsDataProcessors();


};

class C_AttributeManager
{
public:
	char	_pad_0x0000[0x18];
	__int32						m_iReapplyProvisionParity; //0x0018 
	uintptr_t						m_hOuter; //0x001C 
	char	_pad_0x0020[0x4];
	__int32						m_ProviderType; //0x0024 
	char	_pad_0x0028[0x18];
	C_EconItemViewBT				m_Item; //0x0040 
};

class C_WeaponCSBase : public IClientEntity
{
public:
	char	_pad_0x0000[0x09CC];
	CUtlVector<IRefCounted*>	m_CustomMaterials; //0x09DC
	char	_pad_0x09F0[0x2380];
	C_AttributeManager			m_AttributeManager; //0x2D70
	char	_pad_0x2D84[0x2F9];
	bool						m_bCustomMaterialInitialized; //0x32DD
};

class C_BaseEntity : public IClientEntity
{
public:
	int GetSequenceActivity(int sequence);
	bool BoneSetup(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		//int old1 = *(int*)((uintptr_t)this + 0xA30);
		//int old2 = *(int*)((uintptr_t)this + 0xA28);

		int backup = *(int*)((uintptr_t)this + 0x274);

		//*(int*)((uintptr_t)this + 0xA30) = g_pGlobalVars->framecount;
		//*(int*)((uintptr_t)this + 0xA28) = 0;

		//*(int*)(this + 2600) |= 0xA;
		*(int*)((uintptr_t)this + 0x274) = 0;

		auto result = this->SetupBones(pBoneToWorldOut, nMaxBones, boneMask, currentTime);

		*(int*)((uintptr_t)this + 0x274) = backup;

		//*(int*)((uintptr_t)this + 0xA30) = old1;
		//*(int*)((uintptr_t)this + 0xA28) = old2;

		return result;
	}

	datamap_t * GetDataDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 17)(this);
	}
	static __forceinline C_BaseEntity* GetEntityByIndex(int index) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntityFromHandle(h));
	}

	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(Vector, m_vecABSorigin, "DT_BaseEntity", "m_vecABSorigin");
	//m_vecABSorigin
	NETVAR(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	NETVAR(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");


	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	bool IsPlayer();
	bool IsWeapon();
	bool IsPlantedC4();
	bool IsDefuseKit();
	//bool isSpotted();
};

class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};
class C_BaseWeaponWorldModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseWeaponWorldModel", "m_nModelIndex");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");
	NETVAR(int32_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(char*, m_SzCustomName, "DT_BaseAttributableItem", "m_szCustomName");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(C_EconItemView, m_Item2, "DT_BaseAttributableItem", "m_Item");
	NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModelATITEM, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");

	//m_iAccountID

	NETVAR(int, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");

	C_EconItemView& m_Item()
	{
		// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *(C_EconItemView*)this;
	}
	void SetGloveModelIndex(int modelIndex);

	void SetModelIndex(const int index)
	{
		return CallVFunction<void(__thiscall*)(C_BaseEntity*, int)>(this, 75)(this, index);
	}

};
struct Weapon_Info
{
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* hud_name;				//0x0088 
	char* weapon_name;			//0x008C 
	char _0x0090[60];
	__int32 m_WeaponType;				//0x00CC 
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	BYTE full_auto;				//0x00EC 
	char _0x00ED[3];
	__int32 m_Damage;				//0x00F0 
	float m_ArmorRatio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float m_Penetration;			//0x00FC 
	char _0x0100[8];
	float m_Range;				//0x0108 
	float m_RangeModifier;		//0x010C 
	char _0x0110[16];
	BYTE m_HasSilencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
	NETVAR(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime");
	NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");
	float GetCone()
	{
		if (!this)
			return -1.f;
		typedef float(__thiscall* OriginalFn)(void*);
		return CallVFunction<OriginalFn>(this, 468)(this);

	}

	CCSWeaponInfo* GetCSWeaponData();
	bool HasBullets();
	bool CanFire();
	bool IsGrenade();
	bool IsKnife();
	bool IsReloading();
	bool IsRifle();
	bool IsPistol();
	bool IsSniper();
	bool IsShotBeingFired();

	Weapon_Info* GetWeaponInfo()
	{
		if (!this) return nullptr;
		typedef Weapon_Info* (__thiscall * Fn)(void*);
		return CallVFunction<Fn>(this, 448)(this);
	}

	bool IsGun();
	float GetInaccuracy();
	float GetSpread();
	void UpdateAccuracyPenalty();
	CUtlVector<IRefCounted*>& m_CustomMaterials();
	bool* m_bCustomMaterialInitialized();

	

};
enum class ObserverMode_t : int
{
	OBS_MODE_NONE = 0,
	OBS_MODE_DEATHCAM = 1,
	OBS_MODE_FREEZECAM = 2,
	OBS_MODE_FIXED = 3,
	OBS_MODE_IN_EYE = 4,
	OBS_MODE_CHASE = 5,
	OBS_MODE_ROAMING = 6
};
template< typename t = float >
t maximum(const t & a, const t & b) {
	// check type.
	static_assert(std::is_arithmetic< t >::value, "Math::max only supports integral types.");
	return (t)_mm_cvtss_f32(
		_mm_max_ss(_mm_set_ss((float)a),
			_mm_set_ss((float)b))
	);
}
template< typename t = float >
t minimum(const t & a, const t & b) {
	// check type.
	static_assert(std::is_arithmetic< t >::value, "Math::min only supports integral types.");
	return (t)_mm_cvtss_f32(
		_mm_min_ss(_mm_set_ss((float)a),
			_mm_set_ss((float)b))
	);
}
class C_AnimStateA
{
public:
	char pad[4];
	char bUnknown; //0x4
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];
};
class C_BasePlayer : public C_BaseEntity
{
public:

	matrix3x4_t GetBoneMatrixA(int BoneID)
	{
		matrix3x4_t matrix;

		auto offset = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + 0x26A8);
		if (offset)
			matrix = *reinterpret_cast<matrix3x4_t*>(offset + 0x30 * BoneID);

		return matrix;
	}
	float DesyncValue() {

		if (!this)
			return 0.f;

		auto anim_state = (c_animstate*)this->AnimationState();
		if (!anim_state)
			return 0.f;

		float duck_amount = anim_state->duck_amount;
		float speed_fraction = maximum< float >(0, minimum< float >(anim_state->feet_speed_forwards_or_sideways, 1));
		float speed_factor = maximum< float >(0, minimum< float >(1, anim_state->feet_speed_unknown_forwards_or_sideways));

		float yaw_modifier = (((anim_state->stop_to_full_running_fraction * -0.3f) - 0.2f) * speed_fraction) + 1.0f;

		if (duck_amount > 0.f) {
			yaw_modifier += ((duck_amount * speed_factor) * (0.5f - yaw_modifier));
		}

		return anim_state->velocity_subtract_y * yaw_modifier;

	}
	bool GetIsDormant()
	{
		return *reinterpret_cast<bool*>(uintptr_t(this) + 0xED);
	}
	bool* GetIsDormantPt()
	{
		return reinterpret_cast<bool*>(uintptr_t(this) + 0xED);
	}
	static __forceinline C_BasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(g_EngineClient->GetPlayerForUserID(id)));
	}
	static __forceinline C_BasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}

	C_BaseAnimState* AnimationState()
	{
		return *(C_BaseAnimState**)((DWORD)this + 0x3900);
	}

	c_animstate* AnimStatev2()
	{
		return *reinterpret_cast<c_animstate * *>(uintptr_t(this) + 0x3900);
	}
	void ClientAnimations(bool value)
	{
		static int m_bClientSideAnimation = NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		*reinterpret_cast<bool*>(uintptr_t(this) + m_bClientSideAnimation) = value;
	}
	C_AnimStateA* AnimState()
	{
		return *reinterpret_cast<C_AnimStateA * *>(uintptr_t(this) + 0x3900);
	}
	void UpdateClientAnimation()
	{
		GetVFuncF<void(__thiscall*)(void*)>(this, 221)(this);
	}
	void SetAbsAngles(Vector angles)
	{
		using Fn = void(__thiscall*)(C_BasePlayer*, const Vector & angles);
		static Fn AbsAngles = (Fn)(Utils_fox.FindPatternMask("client_panorama.dll", (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxxxx"));

		AbsAngles(this, angles);
	}
	bool& GetClientSideAnimation()
	{
		static int m_bClientSideAnimation = NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		return *reinterpret_cast<bool*>((DWORD)this + m_bClientSideAnimation);
	}
	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(Vector, GetVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(ObserverMode_t, m_iObserverMode, "DT_BasePlayer", "m_iObserverMode");
	//m_iObserverMode
	//m_hObserverTarget
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVAR(float, m_flCycle, "DT_ServerAnimationData", "m_flCycle");
	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	PNETVAR(char, m_szLastPlaceName, "DT_BasePlayer", "m_szLastPlaceName");
	NETPROP(m_flLowerBodyYawTargetProp, "DT_CSPlayer", "m_flLowerBodyYawTarget");

	//NETVAR(int, m_iAccount, "DT_CSPlayer", "m_iAccount");


	NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	NETVAR(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
	NETVAR(float, m_flDuckSpeed, "DT_BaseEntity", "m_flDuckSpeed");
	NETVAR(float, m_flDuckAmount, "DT_BaseEntity", "m_flDuckAmount");
	//NETVAR(std::array<float, 24>,			 m_flPoseParameter,		 "DT_BaseAnimating", "m_flPoseParameter");
	std::array<float, 24> m_flPoseParameter() const {
		static int _m_flPoseParameter = NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
	}


	PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");

	CUserCmd*& m_pCurrentCommand();

	/*gladiator v2*/
	void InvalidateBoneCache();
	int GetNumAnimOverlays();
	AnimationLayer *GetAnimOverlays();
	AnimationLayer *GetAnimOverlay(int i);


	CBasePlayerAnimState *GetBasePlayerAnimState();
	CCSPlayerAnimState *GetPlayerAnimState();

	CLocalPlayerExclusive * localPlayerExclusive();

	static void UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle);
	static void ResetAnimationState(CCSGOPlayerAnimState *state);
	void CreateAnimationState(CCSGOPlayerAnimState *state);

	float_t &m_surfaceFriction()
	{
		static unsigned int _m_surfaceFriction = Utils::FindInDataMap(GetPredDescMap(), "m_surfaceFriction");
		return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
	}
	Vector &m_vecBaseVelocity()
	{
		static unsigned int _m_vecBaseVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
	}

	float GetOldSimulationTime()
	{
		static uintptr_t offset = Utils::FindInDataMap(GetPredDescMap(), "m_flSimulationTimen");
		//static uintptr_t offset = g_CVar->GetOffset("DT_CSPlayer", "m_flSimulationTimen") + 0x4;
		return *(float*)((DWORD)this + offset);
	}

	float_t &m_flMaxspeed()
	{
		static unsigned int _m_flMaxspeed = Utils::FindInDataMap(GetPredDescMap(), "m_flMaxspeed");
		return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
	}
	void SetAbsOrigin(const Vector &origin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &origin);
		static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)Utils::PatternScan(GetModuleHandle("client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
		SetAbsOrigin(this, origin);
	}
	/*QAngle &m_angAbsRotation()
	{
	return *(QAngle*)((DWORD)&m_angRotation() - 12);
	}*/



	Vector        GetEyePos();
	player_info_t GetPlayerInfo();
	bool          IsAlive();
	bool		  IsFlashed();
	bool          HasC4();
	Vector GetHitboxPos1(C_BasePlayer * pEntity, int Hitbox);
	Vector        GetHitboxPos(int hitbox_id);
	mstudiobbox_t * GetHitbox(int hitbox_id);
	bool          GetHitboxPos(int hitbox, Vector &output);
	std::string GetName();
	Vector        GetBonePos(int bone);
	bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
	bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);
	void UpdateClientSideAnimation();
	void SetAngle2(QAngle wantedang);

	int m_nMoveType();


	QAngle * GetVAngles();

	float GetFlashBangTime();

	float_t m_flSpawnTime();

};

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(CHandle<C_BasePlayer>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
	
	NETPROP(m_nSequence, "DT_BaseViewModel", "m_nSequence");
	void SendViewModelMatchingSequence(int sequence);
};
class CAnimationLayer
{
public:
	float m_flLayerAnimtime; //0
	float m_flLayerFadeOuttime;
	float m_flBlendIn;
	float m_flBlendOut;
	int Unknown1;
	int m_nOrder; //20
	int m_nSequence; //24
	float m_flPrevCycle;
	float m_flWeight; //32
	float m_flWeightDeltaRate;
	float m_flPlaybackRate;
	float m_flCycle; //44
	C_BaseEntity *m_pOwner; //48
	int	m_nInvalidatePhysicsBits;
};
class AnimationLayer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class CBasePlayerAnimState
{
public:
	// Class has no vtable.
	char pad_0x0000[0x28]; //0x0000
	float_t m_flTimeMoving; //0x0028 // unsure, seemed like it, unreliable
	char pad_0x002C[0x30]; //0x002C
	C_BaseCombatWeapon *Weapon0; //0x005C All the weapons are the ptr to the same weapon, like, what?
	C_BasePlayer *player; //0x0060 // didnt check indepth but probably the this ptr
	C_BaseCombatWeapon *Weapon1; //0x0064 
	C_BaseCombatWeapon *Weapon2; //0x0068 
	float_t m_flUnknownTimer1; //0x006C 
	float m_flLastAnimationStateClearTime; // not sure if this is it exactly but dumps say shit
	char pad_0x0074[0x4]; //0x0074
	float_t m_flEyeYaw; //0x0078 
	float_t m_flEyePitch; //0x007C 
	float_t m_flGoalFeetYaw; //0x0080 // both feet yaws go from 0 to 360 so -180 when using
	float_t m_flCurrentFeetYaw; //0x0084 
	bool  m_bCurrentFeetYawInitialized; //0x0088 
	float_t m_flLastMoveYaw; //0x008C // unsure.
	char pad_0x0090[0x5C]; //0x0090
	float m_flVelocity;
};

/*class CCSPlayerAnimState
{
public:

	virtual ~CCSPlayerAnimState() = 0;
	virtual void Update(float_t eyeYaw, float_t eyePitch) = 0;
};*/
class CCSPlayerAnimState
{
public:
	char pad_0000[120]; //0x0000
	float m_flEyeYaw; //0x0078
	float m_flPitch; //0x007C
	float m_flGoalFeetYaw; //0x0080
	float m_flCurrentFeetYaw; //0x0084
	char pad_0088[28]; //0x0088
	float m_flDuckAmount; //0x00A4
	char pad_00A8[8]; //0x00A8
	Vector m_vecOrigin; //0x00B0
	Vector m_vecLastOrigin; //0x00BC
	char pad_00C8[0x30]; //0x00C8
	float m_flSpeedFraction; //0x00F8
	float m_flSpeedFactor; //0x00FC
	char pad_0100[28]; //0x0100
	float m_flLandingRatio; //0x011C
	char pad_0120[528]; //0x0120
	float m_flMinBodyYawDegrees; //0x0330
	float m_flMaxBodyYawDegrees; //0x0334
};

class CCSGOPlayerAnimState
{
public:

	Vector GetVecVelocity()
	{
		// Only on ground velocity
		return *(Vector*)((uintptr_t)this + 0xC8);
	}

	float GetVelocity()
	{
		return *(float*)((uintptr_t)this + 0xEC);
	}

	char pad_0x0000[0x344]; //0x0000
}; //Size=0x344

class DT_CSPlayerResource
{
public:
	PNETVAR(int32_t, m_nActiveCoinRank, "DT_CSPlayerResource", "m_nActiveCoinRank");
	PNETVAR(int32_t, m_nMusicID, "DT_CSPlayerResource", "m_nMusicID");
	PNETVAR(int32_t, m_nPersonaDataPublicLevel, "DT_CSPlayerResource", "m_nPersonaDataPublicLevel");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsLeader, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsLeader");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsTeacher, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsTeacher");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsFriendly, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsFriendly");
	PNETVAR(int32_t, m_iCompetitiveRanking, "DT_CSPlayerResource", "m_iCompetitiveRanking");
	PNETVAR(int32_t, m_iCompetitiveWins, "DT_CSPlayerResource", "m_iCompetitiveWins");
	PNETVAR(int32_t, m_iPlayerVIP, "DT_CSPlayerResource", "m_iPlayerVIP");
	PNETVAR(int32_t, m_iMVPs, "DT_CSPlayerResource", "m_iMVPs");
	PNETVAR(int32_t, m_iScore, "DT_CSPlayerResource", "m_iScore");
};