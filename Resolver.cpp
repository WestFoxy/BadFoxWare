#include "Resolver.h"
#include "LagComp.h"
#include "ValveMath.h"
#include "AutoWall.h"

Resolver g_Resolver;
namespace ResolveData
{
	bool Hit[65];
	bool Shot[65];
	int MissedShots[65];
}
/*
my attempt at fixing desync and i was pretty successful
it can resolve static desync pretty perfectly
and can resolve some jitter desync but
it still gets rekt by other things
*/

void Resolver::AnimationFix(C_BasePlayer* pEnt)
{
	if (pEnt == g_LocalPlayer) {
		pEnt->ClientAnimations(true);
		auto player_animation_state = pEnt->AnimState();
		player_animation_state->m_flLeanAmount = 20;
		player_animation_state->m_flCurrentTorsoYaw += 15;
		pEnt->UpdateClientAnimation();
		pEnt->SetAbsAngles(Vector(0, player_animation_state->m_flGoalFeetYaw, 0));
		pEnt->ClientAnimations(false);
	}
	else {
		auto player_index = pEnt->EntIndex() - 1;

		pEnt->ClientAnimations(true);

		auto old_curtime = g_GlobalVars->curtime;
		auto old_frametime = g_GlobalVars->frametime;

		g_GlobalVars->curtime = pEnt->m_flSimulationTime();
		g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

		auto player_animation_state = pEnt->AnimState();
		auto player_model_time = reinterpret_cast<int*>(player_animation_state + 112);
		if (player_animation_state != nullptr && player_model_time != nullptr)
			if (*player_model_time == g_GlobalVars->framecount)
				* player_model_time = g_GlobalVars->framecount - 1;


		pEnt->UpdateClientAnimation();

		g_GlobalVars->curtime = old_curtime;
		g_GlobalVars->frametime = old_frametime;

		//pEnt->SetAbsAngles(Vector(0, player_animation_state->m_flGoalFeetYaw, 0));

		pEnt->ClientAnimations(false);
	}

}
float flAngleMod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}
float ApproachAngle(float target, float value, float speed)
{
	target = flAngleMod(target);
	value = flAngleMod(value);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}


void update_state(C_AnimStateA* state, Vector angles) {
	using Fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
	static auto fn = reinterpret_cast<Fn>(Utils::PatternScan("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	fn(state, nullptr, 0.0f, angles[1], angles[0], nullptr);
}

void HandleBackUpResolve(C_BasePlayer* pEnt) {

	if (!g_Options.rage_resolver)
		return;

	if (pEnt->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		return;

	const auto player_animation_state = pEnt->AnimState();

	if (!player_animation_state)
		return;

	if (ResolveData::MissedShots[pEnt->EntIndex()] > 2) {
		switch (ResolveData::MissedShots[pEnt->EntIndex()] % 4) {
		case 0: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 45; break;
		case 1: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 45; break;
		case 2: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw - 30; break;
		case 3: player_animation_state->m_flGoalFeetYaw = player_animation_state->m_flGoalFeetYaw + 30; break;
		}
	}
	else {

		float m_flLastClientSideAnimationUpdateTimeDelta = fabs(player_animation_state->m_iLastClientSideAnimationUpdateFramecount - player_animation_state->m_flLastClientSideAnimationUpdateTime);

		auto v48 = 0.f;

		if (player_animation_state->m_flFeetSpeedForwardsOrSideWays >= 0.0f)
		{
			v48 = fminf(player_animation_state->m_flFeetSpeedForwardsOrSideWays, 1.0f);
		}
		else
		{
			v48 = 0.0f;
		}

		float v49 = ((player_animation_state->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999) * v48;

		float flYawModifier = v49 + 1.0;

		if (player_animation_state->m_fDuckAmount > 0.0)
		{
			float v53 = 0.0f;

			if (player_animation_state->m_flFeetSpeedUnknownForwardOrSideways >= 0.0)
			{
				v53 = fminf(player_animation_state->m_flFeetSpeedUnknownForwardOrSideways, 1.0);
			}
			else
			{
				v53 = 0.0f;
			}
		}

		float flMaxYawModifier = player_animation_state->pad10[516] * flYawModifier;
		float flMinYawModifier = player_animation_state->pad10[512] * flYawModifier;

		float newFeetYaw = 0.f;

		auto eyeYaw = player_animation_state->m_flEyeYaw;

		auto lbyYaw = player_animation_state->m_flGoalFeetYaw;

		float eye_feet_delta = fabs(eyeYaw - lbyYaw);

		if (eye_feet_delta <= flMaxYawModifier)
		{
			if (flMinYawModifier > eye_feet_delta)
			{
				newFeetYaw = fabs(flMinYawModifier) + eyeYaw;
			}
		}
		else
		{
			newFeetYaw = eyeYaw - fabs(flMaxYawModifier);
		}

		float v136 = fmod(newFeetYaw, 360.0);

		if (v136 > 180.0)
		{
			v136 = v136 - 360.0;
		}

		if (v136 < 180.0)
		{
			v136 = v136 + 360.0;
		}

		player_animation_state->m_flGoalFeetYaw = v136;
	}
	
	//if (pEnt->GetVelocity().Length2D() > 0.1f)
	//{
	//	player_animation_state->m_flGoalFeetYaw = ApproachAngle(pEnt->GetLowerBodyYaw(), player_animation_state->m_flGoalFeetYaw, (player_animation_state->m_flStopToFullRunningFraction * 20.0f) + 30.0f *player_animation_state->m_flLastClientSideAnimationUpdateTime);
	//}
	//else
	//{
	//	player_animation_state->m_flGoalFeetYaw = ApproachAngle(pEnt->GetLowerBodyYaw(), player_animation_state->m_flGoalFeetYaw, (m_flLastClientSideAnimationUpdateTimeDelta * 100.0f));
	//}
	//if (Globals::MissedShots[pEnt->EntIndex()] > 3) {
	//	switch (Globals::MissedShots[pEnt->EntIndex()] % 4) {
	//	case 0: pEnt->GetEyeAnglesPointer()->y = pEnt->GetEyeAnglesPointer()->y + 45; break;
	//	case 1: pEnt->GetEyeAnglesPointer()->y = pEnt->GetEyeAnglesPointer()->y - 45; break;
	//	case 2: pEnt->GetEyeAnglesPointer()->y = pEnt->GetEyeAnglesPointer()->y - 30; break;
	//	case 3: pEnt->GetEyeAnglesPointer()->y = pEnt->GetEyeAnglesPointer()->y + 30; break;
	//	}
	//}
}
#define TICKS_TO_TIME( t )		( g_GlobalVars->interval_per_tick *( t ) )
void HandleHits(C_BasePlayer* pEnt)
{
	auto NetChannel = g_EngineClient->GetNetChannelInfo();

	if (!NetChannel)
		return;
	
	static float predTime[65];
	static bool init[65];


	if (ResolveData::Shot[pEnt->EntIndex()])
	{
		if (init[pEnt->EntIndex()])
		{
			g_Resolver.pitchHit[pEnt->EntIndex()] = pEnt->m_angEyeAngles().pitch;
			predTime[pEnt->EntIndex()] = g_GlobalVars->curtime + NetChannel->GetAvgLatency(1) + NetChannel->GetAvgLatency(0) + TICKS_TO_TIME(1) + TICKS_TO_TIME(g_EngineClient->GetNetChannel()->m_nChokedPackets);
			init[pEnt->EntIndex()] = false;
		}

		if (g_GlobalVars->curtime > predTime[pEnt->EntIndex()] && !ResolveData::Hit[pEnt->EntIndex()])
		{
			ResolveData::MissedShots[pEnt->EntIndex()] += 1;
			ResolveData::Shot[pEnt->EntIndex()] = false;
		}
		else if (g_GlobalVars->curtime <= predTime[pEnt->EntIndex()] && ResolveData::Hit[pEnt->EntIndex()])
			ResolveData::Shot[pEnt->EntIndex()] = false;

	}
	else
		init[pEnt->EntIndex()] = true;

	ResolveData::Hit[pEnt->EntIndex()] = false;
}
Vector CalcAngleR(const Vector& vecSource, const Vector& vecDestination)
{
	Vector qAngles;
	Vector delta = Vector((vecSource[0] - vecDestination[0]), (vecSource[1] - vecDestination[1]), (vecSource[2] - vecDestination[2]));
	double hyp = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);
	qAngles[0] = (float)(atan(delta[2] / hyp) * (180.0 / 3.14159265358979323846));
	qAngles[1] = (float)(atan(delta[1] / delta[0]) * (180.0 / 3.14159265358979323846));
	qAngles[2] = 0.f;
	if (delta[0] >= 0.f)
		qAngles[1] += 180.f;

	return qAngles;
}
float GRD_TO_BOGR(float GRD) {
	return (3.14159265358979323846 / 180) * GRD;
}

void Resolver::OnCreateMove() // cancer v2
{
	if (!g_Options.rage_resolver)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_LocalPlayer->m_hActiveWeapon() /*|| g_LocalPlayer->IsKnifeorNade()*/)
		return;


	for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i)
	{
		C_BasePlayer* pPlayerEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g_LocalPlayer
			|| pPlayerEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		{
			UseFreestandAngle[i] = false;
			continue;
		}

		if (abs(pPlayerEntity->GetVelocity().Length2D()) > 29.f)
			UseFreestandAngle[pPlayerEntity->EntIndex()] = false;

		if (abs(pPlayerEntity->GetVelocity().Length2D()) <= 29.f && !UseFreestandAngle[pPlayerEntity->EntIndex()])
		{
			bool Autowalled = false, HitSide1 = false, HitSide2 = false;

			float angToLocal = CalcAngleR(g_LocalPlayer->m_vecOrigin(), pPlayerEntity->m_vecOrigin()).y;
			Vector ViewPoint = g_LocalPlayer->m_vecOrigin() + Vector(0, 0, 90);

			Vector2D Side1 = { (45 * sin(GRD_TO_BOGR(angToLocal))),(45 * cos(GRD_TO_BOGR(angToLocal))) };
			Vector2D Side2 = { (45 * sin(GRD_TO_BOGR(angToLocal + 180))) ,(45 * cos(GRD_TO_BOGR(angToLocal + 180))) };

			Vector2D Side3 = { (50 * sin(GRD_TO_BOGR(angToLocal))),(50 * cos(GRD_TO_BOGR(angToLocal))) };
			Vector2D Side4 = { (50 * sin(GRD_TO_BOGR(angToLocal + 180))) ,(50 * cos(GRD_TO_BOGR(angToLocal + 180))) };

			Vector Origin = pPlayerEntity->m_vecOrigin();

			Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

			Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
				Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (g_AutoWall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
				{
					if (side == 0)
					{
						HitSide1 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = 90;
					}
					else if (side == 1)
					{
						HitSide2 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = -90;
					}

					Autowalled = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 90 };

						if (g_AutoWall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
						{
							if (side222 == 0)
							{
								HitSide1 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = 90;
							}
							else if (side222 == 1)
							{
								HitSide2 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = -90;
							}

							Autowalled = true;
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)
					UseFreestandAngle[pPlayerEntity->EntIndex()] = false;
				else
					UseFreestandAngle[pPlayerEntity->EntIndex()] = true;
			}
		}
	}
}

void Resolver::FrameStage(ClientFrameStage_t stage)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_Options.rage_resolver)
		return;

	static bool  wasDormant[65];

	for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i)
	{
		C_BasePlayer* pPlayerEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive())
			continue;
		if (pPlayerEntity->IsDormant())
		{
			wasDormant[i] = true;
			continue;
		}

		if (stage == FRAME_RENDER_START)
		{
			HandleHits(pPlayerEntity);
			//AnimationFix(pPlayerEntity);


		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			HandleBackUpResolve(pPlayerEntity);
		}

		if (stage == FRAME_NET_UPDATE_END && pPlayerEntity != g_LocalPlayer)
		{
			auto VarMap = reinterpret_cast<uintptr_t>(pPlayerEntity) + 36;
			auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

			for (auto index = 0; index < VarMapSize; index++)
				* reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = 0;
		}

		wasDormant[i] = false;
	}
}