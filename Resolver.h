#pragma once
#include <deque>
#include "MainInclude.hpp"

namespace ResolveData
{
	extern bool Hit[65];
	extern bool Shot[65];
	extern int MissedShots[65];
}

class Resolver
{
public:
	bool UseFreestandAngle[65];
	float FreestandAngle[65];

	float pitchHit[65];

	void OnCreateMove();
	void Yaw(C_BaseEntity* ent);
	void FrameStage(ClientFrameStage_t stage);
private:
	void AnimationFix(C_BasePlayer* pEnt);
};
extern Resolver g_Resolver;