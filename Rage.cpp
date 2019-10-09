#include "Rage.h"
#include "Resolver.h"

CAimBot g_AimBot;
std::vector<BackTrackT> rageFoxBacktrack[65];
#define M_PI		3.14159265358979323846

void CalcAngle(Vector src, Vector dst, Vector &angles)
{
	Vector delta = src - dst;
	double hyp = delta.Length2D();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}

void NormalizeAngles(Vector& angles)
{
	for (auto i = 0; i < 3; i++)
	{
		while (angles[i] < -180.0f) angles[i] += 360.0f;
		while (angles[i] > 180.0f) angles[i] -= 360.0f;
	}
}

Vector CrossProduct(const Vector& a, const Vector& b)
{
	return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

void VectorAngles(const Vector& forward, Vector& up, Vector& angles)
{
	Vector left = CrossProduct(up, forward);

	left.NormalizeInPlace();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI;
		angles.y = atan2f(forward.y, forward.x) * 180 / M_PI;

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.z = atan2f(left.z, upZ) * 180 / M_PI;
	}
	else
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI;
		angles.y = atan2f(-left.x, left.y) * 180 / M_PI;
		angles.z = 0;
	}
}

float DotProduct(const float *a, const float *b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
void VectorTransform(const float *in1, const matrix3x4_t& in2, float *out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}
void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	VectorTransform(&in1.x, in2, &out.x);
}
float RandomFloat(float min, float max)
{
	static auto random_float = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));

	return random_float(min, max);
}

void SinCosRG(float a, float* s, float* c)
{
	*s = sin(a);
	*c = cos(a);
}
void AngleVectorsRG(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sp, sy, sr, cp, cy, cr;

	SinCosRG(DEG2RAD(angles.x), &sp, &cp);
	SinCosRG(DEG2RAD(angles.y), &sy, &cy);
	SinCosRG(DEG2RAD(angles.z), &sr, &cr);

	if (forward != nullptr)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right != nullptr)
	{
		right->x = -1 * sr * sp * cy + -1 * cr * -sy;
		right->y = -1 * sr * sp * sy + -1 * cr * cy;
		right->z = -1 * sr * cp;
	}

	if (up != nullptr)
	{
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

template <typename T>
static T Clamp(const T& n, const T& lower, const T& upper)
{
	return std::max(lower, std::min(n, upper));
}

float CAimBot::GetInterp()
{
	int updaterate = g_CVar->FindVar("cl_updaterate")->GetInt();
	ConVar* minupdate = g_CVar->FindVar("sv_minupdaterate");
	ConVar* maxupdate = g_CVar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->GetInt();

	float ratio = g_CVar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = g_CVar->FindVar("cl_interp")->GetFloat();
	ConVar* cmin = g_CVar->FindVar("sv_client_min_interp_ratio");
	ConVar* cmax = g_CVar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratio = Clamp(ratio, cmin->GetFloat(), cmax->GetFloat());

	return std::max(lerp, (ratio / updaterate));
}

std::vector<int> CAimBot::GetHitboxes()
{
	std::vector<int> hitboxes;

	if (g_Options.RageW[curweap].HitScan1) hitboxes.push_back((int)Hitboxes::HITBOX_HEAD);
	if (g_Options.RageW[curweap].HitScan2) hitboxes.push_back((int)Hitboxes::HITBOX_NECK);
	if (g_Options.RageW[curweap].HitScan3)
	{
		hitboxes.push_back((int)Hitboxes::HITBOX_UPPER_CHEST);
		hitboxes.push_back((int)Hitboxes::HITBOX_CHEST);
		hitboxes.push_back((int)Hitboxes::HITBOX_LOWER_CHEST);
	}
	if (g_Options.RageW[curweap].HitScan4) hitboxes.push_back((int)Hitboxes::HITBOX_STOMACH);
	if (g_Options.RageW[curweap].HitScan5) hitboxes.push_back((int)Hitboxes::HITBOX_PELVIS);
	if (g_Options.RageW[curweap].HitScan6)
	{
		hitboxes.push_back(18);
		hitboxes.push_back(16);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_UPPER_ARM);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_UPPER_ARM);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_HAND);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_HAND);
	}
	if (g_Options.RageW[curweap].HitScan7)
	{
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_CALF);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_CALF);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_THIGH);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_THIGH);
		hitboxes.push_back((int)Hitboxes::HITBOX_LEFT_FOOT);
		hitboxes.push_back((int)Hitboxes::HITBOX_RIGHT_FOOT);
	}

	return hitboxes;
}

PointInfo CAimBot::HitScan(C_BaseEntity* pBaseEntity)
{
	auto hitboxes = this->GetHitboxes();

	matrix3x4_t matrix[128];

	if (!pBaseEntity->BoneSetup(matrix, 128, BONE_USED_BY_HITBOX, g_GlobalVars->curtime))
		printf("Failed to bone setup!\n");

	PointInfo bestPoint;
	bestPoint.clear();

	if (g_Options.rage_forcebaim_STATE)
	{
		//(int)Hitboxes::HITBOX_PELVIS
		auto point = this->GetBestPointByHitbox(pBaseEntity, (int)Hitboxes::HITBOX_PELVIS, matrix);
		return point;
	}
	else
	{
		for (auto hitbox : hitboxes)
		{
			auto point = this->GetBestPointByHitbox(pBaseEntity, hitbox, matrix);

			if (point.isPriority)
				return point;
			else if (point.damage > bestPoint.damage)
				bestPoint = point;
		}
	}

	return bestPoint;
}

class ent_r
{
public:
	model_t* GetModel()
	{
		typedef model_t* (__thiscall* fnGetModel)(PVOID);
		return CallVFunction<fnGetModel>(this, 8)(this);
	}
};

PointInfo CAimBot::GetBestPointByHitbox(C_BaseEntity* pBaseEntity, int iHitbox, matrix3x4_t boneMatrix[128])
{
	PointInfo pn;

	ent_r *pnt = (ent_r*)(pBaseEntity);
	auto pl = (C_BasePlayer*)pBaseEntity;
	auto modelf = pl->GetModel();

	if (!modelf)
	{
		printf("modelf null for %s health %d\n", pl->GetName().c_str(), pl->m_iHealth());
		return 0;
	}
	studiohdr_t* pStudioModel = g_MdlInfo->GetStudioModel(modelf);
	if (!pStudioModel)
	{
		printf("pStudioModel is null for %s modelf.type = %d\n", pl->GetName().c_str(), modelf->type);
		return 0;
	}
	
	mstudiohitboxset_t* set = pStudioModel->GetHitboxSet(0);
	if (pl->m_iHealth() < g_Options.rage_baimautodamage || ResolveData::MissedShots[pBaseEntity->EntIndex()] >= 3)
		iHitbox == (int)Hitboxes::HITBOX_PELVIS;

	mstudiobbox_t* hitbox = set->GetHitbox(iHitbox); //mstudiobbox_t* hitbox = set->GetHitbox(iHitbox);
	//C_BasePlayer* pl = (C_BasePlayer*)pBaseEntity;

	//baim lethal vfox 1
	if ((g_Options.rage_baimlethal_mode == 1) && (g_Options.rage_baimlethal) && (iHitbox == (int)Hitboxes::HITBOX_PELVIS))
	{
			Vector centerpelvis = (hitbox->bbmax + hitbox->bbmin) * 0.5f;
			float pelvisdamage = g_AutoWall.CanHit(centerpelvis);
			
			PointInfo pelvisinfo;
			pelvisinfo.clear();

			if (pelvisdamage > pl->m_iHealth())
			{
				pelvisinfo.damage = pelvisdamage;
				pelvisinfo.point = centerpelvis;
				pelvisinfo.isPriority = true;
				
				return pelvisinfo;
			}
			
	}
	//--------------

	std::vector<Vector> points;
	points.clear();
	points.push_back((hitbox->bbmax + hitbox->bbmin) * 0.5f); //center

	float radius = hitbox->m_flRadius;

	Vector bbmin = hitbox->bbmin, bbmax = hitbox->bbmax;

	if (hitbox->m_flRadius != -1.f)
	{
		bbmin -= radius;
		bbmax += radius;
	}
	
	bbmin *= g_Options.RageW[curweap].pointscale;
	bbmax *= g_Options.RageW[curweap].pointscale;

	if ((iHitbox == (int)Hitboxes::HITBOX_HEAD) && g_Options.rage_backward_head_scan)
	{
		
		points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmax.x, bbmin.y, bbmax.z)) * 0.5f); //top/back (for down/backwards AA)
	}
	points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmax.x, bbmax.y, bbmax.z)) * 0.5f); //top		
	points.push_back((Vector(bbmin.x, bbmin.y, bbmax.z) + Vector(bbmax.x, bbmax.y, bbmax.z)) * 0.5f); //right		
	points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmin.x, bbmax.y, bbmin.z)) * 0.5f); //left

	PointInfo bestpoint;
	bestpoint.clear();

	for (auto& point : points)
	{
		Vector hitbox_point;
		VectorTransform(point, boneMatrix[hitbox->bone], hitbox_point);

		float tempdamage = g_AutoWall.CanHit(hitbox_point);

		if (tempdamage < 0.01f)
			continue;
		if ((iHitbox == (int)Hitboxes::HITBOX_PELVIS) && g_Options.rage_forcebaim_STATE && (tempdamage > 25))
		{
			bestpoint.damage = tempdamage;
			bestpoint.point = hitbox_point;
			bestpoint.isPriority = true;

			//g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "FBAIM\n");
			return bestpoint;
		}
		if ((iHitbox == (int)Hitboxes::HITBOX_PELVIS) && (tempdamage > pl->m_iHealth()) && g_Options.rage_baimlethal && (g_Options.rage_baimlethal_mode == 0))
		{
			
			bestpoint.damage = tempdamage;
			bestpoint.point = hitbox_point;
			bestpoint.isPriority = true;

			g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "[type 0] PELVISDAMAGE = %f\n", tempdamage);
			return bestpoint;
		}
		if (tempdamage < g_Options.RageW[curweap].minimal_damage)
		{
			
			continue;
		}
		if (tempdamage > bestpoint.damage)
		{
			bestpoint.damage = tempdamage;
			bestpoint.point = hitbox_point;
			
		}
		
		

		//HACK:
		//if (bestpoint.damage > pl->m_iHealth())
		//	return bestpoint;
		
	}

	if (bestpoint.damage < 0.01f)
	{
		
		return PointInfo();

	}

	
	return bestpoint;
}



void CAimBot::FindTarget()
{
	float bestDistance = 8192.f;
	Target_t bestTarget;
	PointInfo bestInfo;
	bestInfo.clear();

	this->backtrackRecord = 0;
	this->ShouldBacktrack = false;

	/*for (int i = 0; i < 64; i++)
	{
		for (int f = 0; f < rageFoxBacktrack[i].size(); f++)
		{
			if (!TimeWarp::Get().IsTickValid(TIME_TO_TICKS(rageFoxBacktrack[i][f].simtime)))
				rageFoxBacktrack[i].erase(rageFoxBacktrack[i].begin() + f);
		}
	}*/

	for (int it = 0; it < g_EngineClient->GetMaxClients(); ++it)
	{
		C_BasePlayer* pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(it);
		auto pent = g_EntityList->GetClientEntity(it);
		if
			(
				!pEntity
				|| pEntity == g_LocalPlayer
				|| pEntity->IsDormant()
				|| !pEntity->IsAlive()
				|| pEntity->m_bGunGameImmunity()
				|| pEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()
				)
		{
			continue;
		}


		//rageFoxBacktrack[it].emplace_back(pEntity->m_flSimulationTime(),pEntity->GetBonePos(8), pEntity);



		//if (rageFoxBacktrack[it].size() > 12)
		//	rageFoxBacktrack[it].erase(rageFoxBacktrack[it].begin());


		float distance = pEntity->GetEyePos().DistTo(g_LocalPlayer->GetEyePos());

		if (distance < bestDistance)
		{
			
			auto pointinfo = this->HitScan(pEntity);
			if (pointinfo.isPriority)
			{
				bestTarget = Target_t(pEntity, pointinfo.point);
				this->tTarget = bestTarget;
				return;
			}
			else if (pointinfo.damage > bestInfo.damage)
			{
				bestDistance = distance;
				bestInfo = pointinfo;
				bestTarget = Target_t(pEntity, pointinfo.point);
			}

			
				/*float bkdamage = g_AutoWall.CanHit(validRecords[it].front().hitboxPos);
				if (bkdamage > g_Options.RageW[curweap].minimal_damage)
				{
					bestTarget = Target_t(pEntity, validRecords[it].front().hitboxPos);
					this->ShouldBacktrack = true;
					this->bestTime = validRecords[it].front().simtime;
				}*/
			
		}
		
	}

	if (bestTarget.pEnt != nullptr && bestTarget.vPos != Vector(0.f, 0.f, 0.f))
	{
		this->tTarget = bestTarget;
		
	}
	else
	{
		//trying to fox backtrack
		
	}
	
}
float get_aimbot_biggest_delta()
{
	// calculate delta between viewangles and aimbot angles
	// do it yourself -_-

	float pitch_delta = 0.f;
	float yaw_delta = 0.f;

	return std::max(fabs(pitch_delta), fabs(yaw_delta));
}

void AnimFixReversed(C_BasePlayer* entity, float simtime)
{
	auto animation_state = entity->AnimStatev2();
	auto old_curtime = g_GlobalVars->curtime;
	auto old_frametime = g_GlobalVars->frametime;
	//auto old_curtime = g_GlobalVars->curtime;
	auto realtime = g_GlobalVars->realtime;

	entity->ClientAnimations(true);
	auto curtime = g_GlobalVars->curtime;
	auto frametime = g_GlobalVars->frametime;
	auto frametimne = g_GlobalVars->frametime;
	auto absoluteframtime = g_GlobalVars->absoluteframetime;
	auto interp_amount = g_GlobalVars->interpolation_amount;
	auto framecount = g_GlobalVars->framecount;
	auto tickcount = g_GlobalVars->tickcount;
	g_GlobalVars->realtime = simtime;
	g_GlobalVars->curtime = simtime;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->framecount = TIME_TO_TICKS(simtime);
	g_GlobalVars->tickcount = TIME_TO_TICKS(simtime);
	g_GlobalVars->interpolation_amount = 0.0f;
	g_GlobalVars->curtime = entity->m_flSimulationTime();
	//g_GlobalVars->frametime = G::interfaces.globaldata->interval_per_tick;
	
	if (animation_state->last_client_side_animation_update_framecount >= TIME_TO_TICKS(simtime))
		animation_state->last_client_side_animation_update_framecount = TIME_TO_TICKS(simtime) - 1;

	g_LocalPlayer->UpdateClientSideAnimation();
	g_LocalPlayer->ClientAnimations(false);

	g_GlobalVars->curtime = old_curtime;
	g_GlobalVars->frametime = old_frametime;
	g_GlobalVars->realtime = realtime;
	g_GlobalVars->curtime = curtime;
	g_GlobalVars->frametime = frametimne;
	g_GlobalVars->absoluteframetime = absoluteframtime;
	g_GlobalVars->interpolation_amount = interp_amount;
	g_GlobalVars->framecount = framecount;
	g_GlobalVars->tickcount = tickcount;
}

void CAimBot::Fire()
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	
	

	if (/*!(g_Options.cmd->buttons & IN_ATTACK2) && */weapon->CanFire() && this->tTarget.pEnt != nullptr && this->tTarget.vPos != Vector(0.f, 0.f, 0.f))
	{
		
		Vector result;

		CalcAngle(g_LocalPlayer->GetEyePos(), this->tTarget.vPos, result);

		if (g_Options.RageW[curweap].no_recoil)
		{
			auto qang = g_LocalPlayer->m_aimPunchAngle();

			auto aimpunchangle = Vector(qang.pitch, qang.yaw, qang.roll);

			result -= (aimpunchangle * g_CVar->FindVar("weapon_recoil_scale")->GetFloat());
		}

		bool hitchance = this->HitChance(this->tTarget.pEnt, weapon, result, g_Options.RageW[curweap].hit_chance);
	

		if (hitchance )
		{
			
			if (this->ShouldBacktrack && (bestTime != 0))
			{
				g_Options.cmd->tick_count = TIME_TO_TICKS(bestTime + GetInterp());
				g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "BACKTRACKED RAGE\n");
			}
			else
				g_Options.cmd->tick_count = TIME_TO_TICKS(this->tTarget.pEnt->m_flSimulationTime() + GetInterp());

			QAngle tmp_qngl = { result.x, result.y, result.z };

			//bool res = get_aimbot_fire();

			//if(res)
			//Math::ClampAngles(tmp_qngl);
			//AnimFixReversed(this->tTarget.pEnt, this->tTarget.pEnt->m_flSimulationTime());
			g_Options.cmd->viewangles = tmp_qngl;
			g_Options.cmd->buttons |= IN_ATTACK;



			if(!g_Options.IsFakeDucking)
				*g_Options.bSendPacketLink = true;

			ResolveData::Shot[this->tTarget.pEnt->EntIndex()] = true;

			//G::RealAngles = g_Options.cmd->viewangles;
		}
		else 
		{
			if (g_Options.rage_autostop)
			{
				if (g_Options.rage_autostop_mode == 0)
				{
					g_Options.cmd->forwardmove /= 20;
					g_Options.cmd->sidemove /= 20;
				}
				else if (g_Options.rage_autostop_mode == 1)
				{
					g_Options.cmd->forwardmove = 0;
					g_Options.cmd->sidemove = 0;
				}
			}
			if (g_Options.rage_autocrouch)
				g_Options.cmd->buttons |= IN_DUCK;

			if (g_Options.rage_autoscope && !g_LocalPlayer->m_bIsScoped() && weapon->IsSniper())
				g_Options.cmd->buttons |= IN_ATTACK2;

		}

	}

	//drop target
	this->tTarget = Target_t(nullptr, Vector(0.f, 0.f, 0.f));
}
/*if (ImGui::Button("AUTO"))
	{
		Weapon_selection = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("SSG08"))
	{
		Weapon_selection = 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("AWP"))
	{
		Weapon_selection = 2;
	}
	ImGui::SameLine();
	if (ImGui::Button("Deagle && Revolver"))
	{
		Weapon_selection = 3;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pistols"))
	{
		Weapon_selection = 4;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rifles"))
	{
		Weapon_selection = 5;
	}
	ImGui::SameLine();
	if (ImGui::Button("SMGS"))
	{
		Weapon_selection = 6;
	}*/

void CAimBot::Do()
{
	if ( !g_LocalPlayer->IsAlive()) return;
	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon)
		return;
	auto wpnData = weapon->GetCSWeaponData();
	if (!wpnData)
		return;

	int type = wpnData->m_WeaponType; //->iWeaponType
	int defind = weapon->m_iItemDefinitionIndex();

	g_Options.rage_debug_currentweapontype = defind;

	if (defind == WEAPON_CZ75 || defind == WEAPON_GLOCK || defind == WEAPON_USPS || defind == WEAPON_TEC9) //PISTOL
		curweap = 4;
	else if (type == 4) //RIFLE
		curweap = 5;
	else if (type == 3) //UMP-48 and others
		curweap = 6;
	
	if (defind == WEAPON_SSG08)
		curweap = 1;
	else if (defind == WEAPON_AWP)
		curweap = 2;
	else if (defind == WEAPON_DEAGLE || defind == WEAPON_REVOLVER)
		curweap = 3;
	if (defind == WEAPON_SCAR20 || defind == WEAPON_G3SG1 || defind == 262155/*WTF fox VALVE*/)
		curweap = 0;
	//g_CVar->ConsolePrintf("%d\n", curweap);
	//weapon->IsWeapon();

	this->FindTarget();
	this->Fire();

}

void CAimBot::Extrapolation()
{
	/*

	for (int i = 0; i < g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* pEntity = g_pEntityList->GetClientEntity(i);

		if
		(
			!pEntity
			|| pEntity == G::pLocalPlayer
			|| pEntity->IsDormant()
			|| !pEntity->IsAlive()
			|| pEntity->IsImmune()
		)
		{
			continue;
		}

		float simtime_delta = pEntity->GetSimTime() - pEntity->GetOldSimTime();
		int choked_ticks = Utils::Clamp(TIME_TO_TICKS(simtime_delta), 1, 15);

		static Vector lastOrig;

		if (lastOrig.Length() != pEntity->GetVecOrigin().Length())
			lastOrig = pEntity->GetVecOrigin();

		float delta_distance = (pEntity->GetVecOrigin() - lastOrig).LengthSqr();

		if (delta_distance > 4096.f)
		{
			Vector velocity_per_tick = pEntity->GetVelocity() * g_pGlobalVars->intervalPerTick;
			auto new_origin = pEntity->GetVecOrigin() + (velocity_per_tick * choked_ticks);

			pEntity->SetAbsOrigin(new_origin);
		}
	}

	*/
}

void CAimBot::StartMoveFix()
{
	if (g_Options.cmd != nullptr && g_LocalPlayer->IsAlive())
	{
		Vector tmp_vc;
		if (IsBadReadPtr(&g_Options.cmd->viewangles.pitch, sizeof(float)))
			tmp_vc = { g_Options.cmd->viewangles.pitch, g_Options.cmd->viewangles.yaw, g_Options.cmd->viewangles.roll };
		else
			return;
		m_oldangle = tmp_vc;
		m_oldforward = g_Options.cmd->forwardmove;
		m_oldsidemove = g_Options.cmd->sidemove;
	}
}

void CAimBot::EndMoveFix()
{
	if (g_Options.cmd != nullptr && g_LocalPlayer->IsAlive() && !IsBadReadPtr(&g_Options.cmd->viewangles.pitch, sizeof(float)))
	{
		float f1, f2, yaw_delta = g_Options.cmd->viewangles.yaw - m_oldangle.y;
		if (m_oldangle.y < 0.f)
			f1 = 360.0f + m_oldangle.y;
		else
			f1 = m_oldangle.y;

		if (g_Options.cmd->viewangles.yaw < 0.0f)
			f2 = 360.0f + g_Options.cmd->viewangles.yaw;
		else
			f2 = g_Options.cmd->viewangles.yaw;

		if (f2 < f1)
			yaw_delta = abs(f2 - f1);
		else
			yaw_delta = 360.0f - abs(f1 - f2);

		yaw_delta = 360.0f - yaw_delta;

		g_Options.cmd->forwardmove = cos(DEG2RAD(yaw_delta)) * m_oldforward + cos(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
		g_Options.cmd->sidemove = sin(DEG2RAD(yaw_delta)) * m_oldforward + sin(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
	}
}
#define M_PI_F		((float)(M_PI))
bool CAimBot::HitChance(C_BaseEntity* pEnt, C_BaseCombatWeapon* pWeapon, Vector Angle, float chance)
{
	

	/*if (!chance)
		return true;

	int iHit = 0;
	int iHitsNeed = (int)(256.f * (chance / 100.f));
	bool bHitchance = false;

	Vector forward, right, up;
	AngleVectors(Angle, &forward, &right, &up);

	pWeapon->UpdateAccuracyPenalty();

	for (auto i = 0; i < 256; ++i) {

		float RandomA = RandomFloat(0.0f, 1.0f);
		float RandomB = 1.0f - RandomA * RandomA;

		RandomA = RandomFloat(0.0f, M_PI_F * 2.0f);
		RandomB *= pWeapon->GetInaccuracy();

		float SpreadX1 = (cos(RandomA) * RandomB);
		float SpreadY1 = (sin(RandomA) * RandomB);

		float RandomC = RandomFloat(0.0f, 1.0f);
		float RandomF = RandomF = 1.0f - RandomC * RandomC;

		RandomC = RandomFloat(0.0f, M_PI_F * 2.0f);
		RandomF *= pWeapon->GetSpread();

		float SpreadX2 = (cos(RandomC) * RandomF);
		float SpreadY2 = (sin(RandomC) * RandomF);

		float fSpreadX = SpreadX1 + SpreadX2;
		float fSpreadY = SpreadY1 + SpreadY2;

		Vector vSpreadForward;
		vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
		vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
		vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
		vSpreadForward.NormalizeInPlace();

		Vector qaNewAngle;
		VectorAngles(vSpreadForward, qaNewAngle);
		NormalizeAngles(qaNewAngle);

		Vector vEnd;
		AngleVectors(qaNewAngle, &vEnd);

		vEnd = g_LocalPlayer->GetEyePos() + (vEnd * 8192.f);

		trace_t tr;
		Ray_t ray;

		ray.Init(g_LocalPlayer->GetEyePos(), vEnd);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, pEnt, &tr);

		if (tr.hit_entity == pEnt)
			iHit++;

		if ((((float)iHit / 256.f) * 100.f) >= chance)
		{
			bHitchance = true;
			break;
		}

		if ((256 - 1 - i + iHit) < iHitsNeed)
			break;
	}

	return bHitchance;
	*/
	

	float Seeds = (chance <= 1.f) ? 356.f : 256.f;

	Vector forward, right, up;

	AngleVectorsRG(Angle, &forward, &right, &up);

	int Hits = 0, neededHits = (Seeds * (chance / 100.f));

	float weapSpread = pWeapon->GetSpread(), weapInaccuracy = pWeapon->GetInaccuracy();

	for (int i = 0; i < Seeds; i++)
	{
		float Inaccuracy = RandomFloat(0.f, 1.f) * weapInaccuracy;
		float Spread = RandomFloat(0.f, 1.f) * weapSpread;

		

		Vector spreadView((cos(RandomFloat(0.f, 2.f * M_PI)) * Inaccuracy) + (cos(RandomFloat(0.f, 2.f * M_PI)) * Spread), (sin(RandomFloat(0.f, 2.f * M_PI)) * Inaccuracy) + (sin(RandomFloat(0.f, 2.f * M_PI)) * Spread), 0), direction;
		direction = Vector(forward.x + (spreadView.x * right.x) + (spreadView.y * up.x), forward.y + (spreadView.x * right.y) + (spreadView.y * up.y), forward.z + (spreadView.x * right.z) + (spreadView.y * up.z)).Normalized();

		Vector viewanglesSpread, viewForward;

		VectorAngles(direction, up, viewanglesSpread);
		NormalizeAngles(viewanglesSpread);

		AngleVectorsAA(viewanglesSpread, &viewForward);
		viewForward.NormalizeInPlace();

		viewForward = g_LocalPlayer->GetEyePos() + (viewForward * pWeapon->GetCSWeaponData()->m_WeaponRange); //->flRange

		

		trace_t tr;
		Ray_t ray;

		ray.Init(g_LocalPlayer->GetEyePos(), viewForward);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, pEnt, &tr);

		if (tr.hit_entity == pEnt)
			Hits++;

		if (((Hits / Seeds) * 100.f) >= chance)
			return true;

		if ((Seeds - i + Hits) < neededHits)
			return false;
	}

	return false;
}

