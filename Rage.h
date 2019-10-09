#pragma once
#include "MainInclude.hpp"
#include "AutoWall.h"
#include "LagComp.h"
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / g_GlobalVars->interval_per_tick ) )
#define TICKS_TO_TIME( t )      ( g_pGlobalVars->intervalPerTick * ( t ) )

enum AimBotHitboxes
{
	Head,
	Neck,
	Chest,
	Stomach,
	Pelvis,
	Arms,
	Legs
};

struct PointInfo
{
public:
	PointInfo(float damage = 0.f, Vector point = Vector(0.f, 0.f, 0.f), bool prio = false)
	{
		this->damage = damage;
		this->point = point;
		this->isPriority = prio;
	}
	void clear()
	{
		this->damage = 0;
		this->point = Vector(0.f, 0.f, 0.f);
		this->isPriority = false;
	}
	bool isPriority;
	float damage;
	Vector point;
};

struct BackTrackT
{
	int simtime;
	Vector point;
	C_BasePlayer* player;
	BackTrackT(int s, Vector p, C_BasePlayer* p1)
	{
		simtime = s;
		point = p;
		player = p1;
	}
};

extern std::vector<BackTrackT> rageFoxBacktrack[65];

class CAimBot
{
public:
	void Do();
	void Extrapolation();
	void StartMoveFix();
	void EndMoveFix();


	struct Target_t
	{
	public:
		Target_t(C_BasePlayer* pEnt = nullptr, Vector vPos = Vector(0.f, 0.f, 0.f))
		{
			this->pEnt = pEnt;
			this->vPos = vPos;
		}

		C_BasePlayer* pEnt = nullptr;
		Vector vPos = { 0.f, 0.f, 0.f };
	};

	PointInfo GetBestPointByHitbox(C_BaseEntity* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[128]);
	bool HitChance(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, float chance);
	void FindTarget();
	void Fire();
	float GetInterp();
	std::vector<int> GetHitboxes();
	PointInfo HitScan(C_BaseEntity* pBaseEntity);

	Target_t tTarget;
	StoredData* backtrackRecord;

	bool ShouldBacktrack;
	int bestTime = 0;

	Vector m_oldangle;
	float m_oldforward, m_oldsidemove;
	int curweap;
};

extern CAimBot g_AimBot;