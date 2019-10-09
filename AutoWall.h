#pragma once

#include "MainInclude.hpp"
#include "valve_sdk/interfaces/IEngineTrace.hpp"
#include "valve_sdk/interfaces/IPhysics.hpp"

#define DAMAGE_NO		         0
#define DAMAGE_EVENTS_ONLY	     1	
#define DAMAGE_YES		         2
#define DAMAGE_AIM		         3
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< weird-looking jello effect for advisor shield.

struct FireBulletData
{
	FireBulletData(const Vector &eye_pos) : src(eye_pos) {}

	Vector          src;
	trace_t         enter_trace;
	Vector          direction;
	CTraceFilter    filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};

inline void VectorAngles(Vector forward, Vector &angles)
{
	float tmp, yaw, pitch;

	if (forward[2] == 0 && forward[0] == 0)
	{
		yaw = 0;

		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / 3.141);

		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / 3.141);

		if (pitch < 0)
			pitch += 360;
	}

	if (pitch > 180)
		pitch -= 360;
	else if (pitch < -180)
		pitch += 360;

	if (yaw > 180)
		yaw -= 360;
	else if (yaw < -180)
		yaw += 360;

	if (pitch > 89)
		pitch = 89;
	else if (pitch < -89)
		pitch = -89;

	if (yaw > 180)
		yaw = 180;
	else if (yaw < -180)
		yaw = -180;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
#define Assert( _exp ) ((void)0)

inline void AngleVectorsAA(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}


class CAutoWall
{
private:
	void TraceLine(Vector & absStart, Vector & absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr);
	void ClipTraceToPlayers(const Vector& absStart, const Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr);
	void GetBulletTypeParameters(float & maxRange, float & maxDistance, char * bulletType, bool sv_penetration_type);
	bool BreakableEntity(IClientEntity * entity);
	void ScaleDamage(CGameTrace & enterTrace, CCSWeaponInfo* weaponData, float & currentDamage);
	bool TraceToExit(CGameTrace & enterTrace, CGameTrace & exitTrace, Vector startPosition, Vector direction);
	bool HandleBulletPenetration(CCSWeaponInfo * weaponData, CGameTrace & enterTrace, Vector & eyePosition, Vector direction, int & possibleHitsRemaining, float & currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration);
	bool FireBullet(C_BaseCombatWeapon * pWeapon, Vector & direction, float & currentDamage);
	bool VectortoVectorVisibleAW(Vector src, Vector point);
	bool TraceToExitalt(Vector& end, CGameTrace& tr, Vector start, Vector vEnd, CGameTrace* trace);
	bool HandleBulletPenetration(CCSWeaponInfo* wpn_data, FireBulletData& data, bool extracheck);
public:
	bool CanHitFloatingPoint(const Vector& point, const Vector& source);
	float CanHit(Vector & point);
};

extern CAutoWall g_AutoWall;