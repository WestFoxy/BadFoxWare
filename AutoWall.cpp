#include "AutoWall.h"
//#include "ISurface.h"

#define BONE_CALCULATE_MASK             0x1F
#define BONE_PHYSICALLY_SIMULATED       0x01    // bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL         0x02    // procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL          0x04    // bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE        0x08    // bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER      0x10    // bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK                  0x0007FF00
#define BONE_USED_BY_ANYTHING           0x0007FF00
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT         0x00000200    // bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK        0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0        0x00000400    // bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1        0x00000800    
#define BONE_USED_BY_VERTEX_LOD2        0x00001000  
#define BONE_USED_BY_VERTEX_LOD3        0x00002000
#define BONE_USED_BY_VERTEX_LOD4        0x00004000
#define BONE_USED_BY_VERTEX_LOD5        0x00008000
#define BONE_USED_BY_VERTEX_LOD6        0x00010000
#define BONE_USED_BY_VERTEX_LOD7        0x00020000
#define BONE_USED_BY_BONE_MERGE         0x00040000    // bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8
#define MAXSTUDIOBONES		128		// total bones actually used

#define BONE_TYPE_MASK                  0x00F00000
#define BONE_FIXED_ALIGNMENT            0x00100000    // bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS          0x00200000    // Vector48
#define BONE_HAS_SAVEFRAME_ROT64        0x00400000    // Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32        0x00800000    // Quaternion32

#define Assert( _exp ) ((void)0)

#define HITGROUP_GENERIC 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4    
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10
inline bool CGameTrace::DidHitWorld() const
{
	return hit_entity->EntIndex() == 0;
}

inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return hit_entity != NULL && !DidHitWorld();
}

CAutoWall g_AutoWall;

void CAutoWall::TraceLine(Vector & absStart, Vector & absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	g_EngineTrace->TraceRay(ray, mask, &filter, ptr);
}

void CAutoWall::ClipTraceToPlayers(const Vector& absStart, const Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{
	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)g_LocalPlayer->m_hActiveWeapon();

	static auto dwAddress = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10");

	if (!dwAddress)
		return;

	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, absEnd
		LEA		ECX, absStart
		CALL	dwAddress
		ADD		ESP, 0xC
	}
}

void CAutoWall::GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, ("BULLET_PLAYER_338MAG")))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_762MM")))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_556MM")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_BOX")))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_57MM")))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_50AE")))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_357SIG")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_P250")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_MIN")))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_9MM")))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_45ACP")))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_BUCKSHOT")))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}

bool CAutoWall::BreakableEntity(IClientEntity* entity)
{
	ClientClass* pClass = (ClientClass*)entity->GetClientClass();

	if (!pClass)
		return false;

	if (pClass == nullptr)
		return false;

	return pClass->m_ClassID == ClassID::CBreakableProp || pClass->m_ClassID == ClassID::CBreakableSurface;
}

void CAutoWall::ScaleDamage(CGameTrace &enterTrace, CCSWeaponInfo *weaponData, float& currentDamage)
{
	if (!g_LocalPlayer)
		return;

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)g_LocalPlayer->m_hActiveWeapon();

	C_BasePlayer *pl = (C_BasePlayer*)(enterTrace.hit_entity);

	bool hasHeavyArmor = pl->m_bHasHeavyArmor();
	int armorValue = pl->m_ArmorValue();
	
	//screw making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&enterTrace]() -> bool
	{
		C_BasePlayer *pl1 = (C_BasePlayer*)(enterTrace.hit_entity);
		switch (enterTrace.hitgroup)
		{
		case HITGROUP_HEAD:
			return pl1->m_bHasHelmet(); //screw compiler errors - force-convert it to a bool via (!!)
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (enterTrace.hitgroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->m_ArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}

bool CAutoWall::TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction)
{
	Vector start, end;
	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		currentDistance += rayExtension;

		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = g_EngineTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		int pointContents = g_EngineTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents)
		{
			end = start - (direction * rayExtension);

			TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				TraceLine(start, startPosition, MASK_SHOT_HULL, (IClientEntity*)exitTrace.hit_entity, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}
				continue;
			}

			if (exitTrace.DidHit() && !exitTrace.startsolid)
			{

				if (BreakableEntity((IClientEntity*)enterTrace.hit_entity) && BreakableEntity((IClientEntity*)exitTrace.hit_entity))
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.startsolid)
			{
				if (enterTrace.DidHitNonWorldEntity() && BreakableEntity((IClientEntity*)enterTrace.hit_entity))
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}

				continue;
			}
		}
	}

	return false;
}

bool CAutoWall::HandleBulletPenetration(CCSWeaponInfo* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	//Because there's been issues regarding this- putting this here.
	if (&currentDamage == nullptr)
	{
		return false;
	}

	auto data = FireBulletData(g_LocalPlayer->GetEyePos());
	data.filter = CTraceFilter();
	data.filter.pSkip = g_LocalPlayer;
	CGameTrace exitTrace;
	C_BaseEntity* pEnemy = (C_BaseEntity*)enterTrace.hit_entity;

	surfacedata_t *enterSurfaceData = g_PhysSurface->GetSurfaceData(enterTrace.surface.surfaceProps);
	int enterMaterial = enterSurfaceData->game.material;

	float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
	float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

	if (possibleHitsRemaining <= 0
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
		|| weaponData->m_Penetration <= 0.f
		|| !TraceToExit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(g_EngineTrace->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
	{
		return false;
	}

	surfacedata_t *exitSurfaceData = g_PhysSurface->GetSurfaceData(exitTrace.surface.surfaceProps);
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

	if (sv_penetration_type)
	{
		if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
		{
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf)
		{
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == CHAR_TEX_FLESH && (g_LocalPlayer->m_iTeamNum() == pEnemy->m_iTeamNum() && ff_damage_reduction_bullets == 0.f))
		{
			if (ff_damage_bullet_penetration == 0.f)
			{
				return false;
			}
			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else
		{
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		if (enterMaterial == exitMaterial)
		{
			if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == CHAR_TEX_PLASTIC)
				combinedPenetrationModifier = 2.f;
		}

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
		modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * finalDamageModifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		if (lostDamage > currentDamage)
		{
			return false;
		}

		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		if (currentDamage < 1.f)
		{
			return false;
		}

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f;
		else
		{
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
		{
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;
			return true;
		}
		return false;
	}
}

template<unsigned int IIdx, typename TRet, typename ... TArgs>
static auto CallVFuncPsu(void* thisptr, TArgs ... argList) -> TRet
{
	using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
	return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
}

class PseudoCvar
{
public:
	float GetFloat()
	{
		return CallVFuncPsu<12, float>(this);
	}
	int GetInt()
	{
		return CallVFuncPsu<13, int>(this);
	}
	bool GetBool()
	{
		return !!this->GetInt();
	}
};

bool CAutoWall::FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage)
{
	if (!pWeapon)
	{
		
		return false;
	}
	auto data = FireBulletData(g_LocalPlayer->GetEyePos());

	data.filter = CTraceFilter();
	data.filter.pSkip = g_LocalPlayer;

	bool sv_penetration_type;

	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration, rayExtension = 40.f;
	Vector eyePosition = g_LocalPlayer->GetEyePos();

	static ConVar* penetrationSystem = g_CVar->FindVar(("sv_penetration_type"));
	static ConVar* damageReductionBullets = g_CVar->FindVar(("ff_damage_reduction_bullets"));
	static ConVar* damageBulletPenetration = g_CVar->FindVar(("ff_damage_bullet_penetration"));

	sv_penetration_type = penetrationSystem->GetBool();
	ff_damage_reduction_bullets = damageReductionBullets->GetFloat();
	ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	CCSWeaponInfo* weaponData = pWeapon->GetCSWeaponData();
	CGameTrace enterTrace;
	CTraceFilter filter;

	filter.pSkip = g_LocalPlayer;

	if (!weaponData)
		return false;

	maxRange = weaponData->m_WeaponRange;
	GetBulletTypeParameters(penetrationPower, penetrationDistance, weaponData->m_BulletType, sv_penetration_type);

	if (sv_penetration_type)
		penetrationPower = weaponData->m_Penetration;

	int possibleHitsRemaining = 4;

	currentDamage = weaponData->m_WeaponDamage;

	

	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		maxRange -= currentDistance;

		Vector end = eyePosition + direction * maxRange;

		TraceLine(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, (IClientEntity*)g_LocalPlayer, &enterTrace);
		ClipTraceToPlayers(eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace); //  | CONTENTS_HITBOX

		surfacedata_t *enterSurfaceData = g_PhysSurface->GetSurfaceData(enterTrace.surface.surfaceProps);

		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;

		int enterMaterial = enterSurfaceData->game.material;

		if (enterTrace.fraction == 1.f)
			break;

		currentDistance += enterTrace.fraction * maxRange;

		currentDamage *= pow(weaponData->m_RangeModifier, (currentDistance / 500.f));

		if ((currentDistance > penetrationDistance && weaponData->m_Penetration > 0.f) || enterSurfPenetrationModifier < 0.1f)
			break;

		bool canDoDamage = (enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC);


		C_BaseEntity *pl = (C_BasePlayer*)(enterTrace.hit_entity);
		bool isEnemy = (g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum());

		if ((canDoDamage && isEnemy))
		{
			ScaleDamage(enterTrace, weaponData, currentDamage);
			return true;
		}

		if (!HandleBulletPenetration(weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}

	return false;
}

float CAutoWall::CanHit(Vector &point)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		return -1;
		
	}
	auto data = FireBulletData(g_LocalPlayer->GetEyePos());
	data.filter = CTraceFilter();
	data.filter.pSkip = g_LocalPlayer;

	

	Vector angles;
	Vector direction;
	Vector tmp = point - g_LocalPlayer->GetEyePos();

	float currentDamage = 0;

	VectorAngles(tmp, angles);
	AngleVectorsAA(angles, &direction);

	direction.NormalizeInPlace();



	if (FireBullet(g_LocalPlayer->m_hActiveWeapon(), direction, currentDamage))
	{
		return currentDamage;
	}
	

	return -1;
}

Vector aim;

Vector CalcAngleAW(const Vector& vecSource, const Vector& vecDestination)
{
	Vector qAngles;
	Vector delta = Vector((vecSource[0] - vecDestination[0]), (vecSource[1] - vecDestination[1]), (vecSource[2] - vecDestination[2]));
	double hyp = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);
	qAngles[0] = (float)(atan(delta[2] / hyp) * (180.0 / 3.1415926535897932384));
	qAngles[1] = (float)(atan(delta[1] / delta[0]) * (180.0 / 3.1415926535897932384));
	qAngles[2] = 0.f;
	if (delta[0] >= 0.f)
		qAngles[1] += 180.f;

	return qAngles;
}

inline float VectorNormalizeAW(Vector& v)
{
	Assert(v.IsValid());
	float l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		// FIXME:
		// Just copying the existing implemenation; shouldn't res.z == 0?
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}
bool CAutoWall::VectortoVectorVisibleAW(Vector src, Vector point)
{
	CGameTrace Trace;
	TraceLine(src, point, MASK_SOLID, g_LocalPlayer, &Trace);

	if (Trace.fraction == 1.0f)
	{
		return true;
	}

	return false;
}


bool CAutoWall::TraceToExitalt(Vector& end, CGameTrace& tr, Vector start, Vector vEnd, CGameTrace* trace)
{
	/*typedef bool(__fastcall * TraceToExitFn)(Vector&, CGameTrace&, float, float, float, float, float, float, CGameTrace*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)Utils::PatternScan("client_panorama.dll", "55 8B EC 83 EC 30 F3 0F 10 75");

	if (!TraceToExit)
		return false;

	return TraceToExit(end, tr, start.x, start.y, start.z, vEnd.x, vEnd.y, vEnd.z, trace);*/
	return false;
}
#define FastSqrt(x)	(sqrt)(x)

inline float VectorLength(const Vector& v)
{
	
	return (float)FastSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

bool CAutoWall::HandleBulletPenetration(CCSWeaponInfo* wpn_data, FireBulletData& data, bool extracheck)
{

	surfacedata_t* enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->m_RangeModifier, (data.trace_length * 0.002));
	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;
	if (data.penetrate_count <= 0)
		return false;
	static Vector dummy;
	static CGameTrace trace_exit;
	if (!TraceToExitalt(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;
	surfacedata_t* exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;
	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)combined_penetration_modifier = 2.0f;
	}
	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->m_Penetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);
	if (extracheck)
		if (!VectortoVectorVisibleAW(trace_exit.endpos, aim))
			return false;
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage)
		return false;
	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f)
		return false;
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool CAutoWall::CanHitFloatingPoint(const Vector& point, const Vector& source)
{
	

	aim = point;

	if (!g_LocalPlayer)
		return false;
	
	FireBulletData data = FireBulletData(source);

	Vector angles = CalcAngleAW(data.src, point);
	AngleVectorsAA(angles, &data.direction);
	VectorNormalizeAW(data.direction);

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)g_LocalPlayer->m_hActiveWeapon();

	if (!pWeapon)
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	CCSWeaponInfo* weaponData = pWeapon->GetCSWeaponData();

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->m_WeaponDamage;

	data.trace_length_remaining = weaponData->m_WeaponRange - data.trace_length;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	TraceLine(data.src, end, MASK_SHOT | CONTENTS_HITBOX, g_LocalPlayer, &data.enter_trace);

	if (VectortoVectorVisibleAW(data.src, point))
	{
		return true;
	}
	static bool extra_check = true;
	if (HandleBulletPenetration(weaponData, data, extra_check))
	{
		return true;
	}

	return false;
}