#include "Misc.h"
#include "Utils_fox.h"
#include <time.h>
#include <vector>
FoxMisc g_chMisc;

void FoxMisc::Run()
{
	
	if (g_Options.clan_tag_animate)
		this->clantag();

	if (g_Options.misc_namestealer)
		this->stealname();

	if (g_Options.misc_fastduck)
		this->fastduck();

	if (g_Options.misc_bhop)
		this->bhop();

	if (g_Options.misc_revealranks)
		this->revealrank();

	if (g_Options.misc_autostrafe)
		this->autostrafe();

	if (g_Options.misc_namespam)
		this->NameSpam();

	if (GetAsyncKeyState(g_Options.misc_circle_key) && g_Options.misc_circle_enable)
		this->CircleStrafe();

	if (GetAsyncKeyState(g_Options.misc_edge_jumpkey) && g_Options.misc_edge_jump)
		this->edgejump();
}
FORCEINLINE void VectorCopy(const Vector& src, Vector& dst)
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
void FoxMisc::edgejump()
{
	
		if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
			return;

		Vector Start, End;
		Start = g_LocalPlayer->m_vecOrigin();

		VectorCopy(g_LocalPlayer->m_vecOrigin(), Start);
		VectorCopy(Start, End);

		End.z -= 32;

		Ray_t ray;
		ray.Init(Start, End);

		trace_t trace;

		CTraceFilter filter;
		filter.pSkip = g_LocalPlayer;

		g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trace);

		int EdgeJumpKey = g_Options.misc_edge_jumpkey;
		if (trace.fraction == 1.0f && EdgeJumpKey > 0 && GetAsyncKeyState(EdgeJumpKey))
		{
			cmd->buttons |= IN_JUMP;
		}

		if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && GetAsyncKeyState(EdgeJumpKey) && g_Options.misc_edge_jump_strafe)
		{
			cmd->forwardmove = (1550.f * 5) / g_LocalPlayer->GetVelocity().Length2D();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -450.f : 450.f;
			if (cmd->forwardmove > 450.f)
				cmd->forwardmove = 450.f;
		}
	
}

void FoxMisc::bhop()
{
	/*auto local_player = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!local_player) return;

	if (cmd->buttons & IN_JUMP)
	{
		int local_flags = local_player->m_fFlags();
		if (!(local_flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;
	}*/

	static bool bLastJumped = false;
	static bool bShouldFake = false;
	auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		cmd->buttons |= IN_JUMP;
	}
	else if (cmd->buttons & IN_JUMP)
	{
		if (pLocal->m_fFlags() & FL_ONGROUND)
			bShouldFake = bLastJumped = true;
		else
		{
			cmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
		bShouldFake = bLastJumped = false;
}

typedef void(*RevealAllFn)(int);
RevealAllFn fnReveal = 0;
void FoxMisc::revealrank()
{
	/*if ((cmd->buttons & IN_SCORE) != 0) {
		g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);
	}*/
	if (cmd->buttons & IN_SCORE)
		g_CHLClient->DispatchMsg(50, 0, 0, nullptr);
	
}
void set_nameM(const char* name)
{
	ConVar* nameConvar = g_CVar->FindVar(("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = NULL;
	nameConvar->SetValue(name);
}
void FoxMisc::stealname()
{
	
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < g_Options.misc_namestealer_speed)
		return;
	const char* result;
	std::vector <std::string> names;

	for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BaseEntity *entity = (C_BaseEntity*)g_EntityList->GetClientEntity(i);

		player_info_t pInfo;

		if (entity && g_LocalPlayer->m_iTeamNum() == entity->m_iTeamNum())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			if (cClass->m_ClassID == (int)ClassID::CCSPlayer)
			{
				if (g_EngineClient->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.szName, "GOTV"))
						names.push_back(pInfo.szName);
				}
			}
		}
	}
	if (g_Options.IsNewMap)
	{
		g_Options.IsNewMap = false;
		set_nameM("\n\xAD\xAD\xAD");
		start_t = clock();
		return;
	}
	int randomIndex = rand() % names.size();

	char buffer[128];
	sprintf_s(buffer, "%s ", names[randomIndex].c_str());
	result = buffer;

	set_nameM(result);
	start_t = clock();
}

void FoxMisc::fastduck()
{
	
	cmd->buttons |= IN_BULLRUSH;

}

void FoxMisc::autostrafe()
{
	if (!g_LocalPlayer->IsAlive())
		return;
	
	
	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && GetAsyncKeyState(VK_SPACE))
	{
		cmd->forwardmove = (10000.f / g_LocalPlayer->GetVelocity().Length2D() > 450.f) ? 450.f : 10000.f / g_LocalPlayer->GetVelocity().Length2D();
		cmd->sidemove = (cmd->mousedx != 0) ? (cmd->mousedx < 0.0f) ? -450.f : 450.f : (cmd->command_number % 2) == 0 ? -450.f : 450.f;
	}
}

void FoxMisc::fakeduck()
{
}

void FoxMisc::antiafk()
{
}
inline void SetClantagM(const char *tag)
{

	static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))Utils_fox.FindPattern("engine.dll", "53 56 57 8B DA 8B F9 FF 15");
	//if (!fnClantagChanged)
	//	return;

	fnClantagChanged(tag, tag);
}
void FoxMisc::clantag()
{
	if (g_Options.misc_clantag_type == 0)
	{
		static clock_t start_t = clock();
		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
		if (timeSoFar < g_Options.misc_clantag_speed)
			return;

		g_Options.gladTag += g_Options.gladTag.at(0);
		g_Options.gladTag.erase(0, 1);
		SetClantagM(g_Options.gladTag.c_str());

		start_t = clock();

		/*static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % (g_Options.gladTag.size() + 1);

		if (FrameNum != LastFrame)
		{
			g_Options.gladTag += g_Options.gladTag.at(0);
			g_Options.gladTag.erase(0, 1);
			SetClantagM(g_Options.gladTag.c_str());

			LastFrame = FrameNum;
		}*/

	}
	else if (g_Options.misc_clantag_type == 1)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 11;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("L!SA beta"); break;
			case 1: SetClantagM("!SA beta L"); break;
			case 2: SetClantagM("SA beta L!"); break;
			case 3: SetClantagM("A beta L!S"); break;
			case 4: SetClantagM(" beta L!SA"); break;
			case 5: SetClantagM("beta L!SA "); break;
			case 6: SetClantagM("eta L!SA b"); break;
			case 7: SetClantagM("ta L!SA be"); break;
			case 8: SetClantagM("a L!SA bet"); break;
			case 9: SetClantagM(" L!SA beta"); break;
			case 10: SetClantagM("L!SA beta"); break;
			}
			LastFrame = FrameNum;
		}

		/*static int iLastTime = 0;
		if (int(g_GlobalVars->curtime) != iLastTime)
		{
			switch (int(g_GlobalVars->curtime) % 11)
			{
			case 0: SetClantagM("L!SA beta"); break;
			case 1: SetClantagM("!SA beta L"); break;
			case 2: SetClantagM("SA beta L!"); break;
			case 3: SetClantagM("A beta L!S"); break;
			case 4: SetClantagM(" beta L!SA"); break;
			case 5: SetClantagM("beta L!SA "); break;
			case 6: SetClantagM("eta L!SA b"); break;
			case 7: SetClantagM("ta L!SA be"); break;
			case 8: SetClantagM("a L!SA bet"); break;
			case 9: SetClantagM(" L!SA beta"); break;
		   case 10: SetClantagM("L!SA beta "); break;
			}
		}

		iLastTime = int(g_GlobalVars->curtime);*/



	}
	else if (g_Options.misc_clantag_type == 2)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 12;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("AIMWARE.net "); break;
			case 1: SetClantagM("IMWARE.net A"); break;
			case 2: SetClantagM("MWARE.net AI"); break;
			case 3: SetClantagM("WARE.net AIM"); break;
			case 4: SetClantagM("ARE.net AIMW"); break;
			case 5: SetClantagM("RE.net AIMWA"); break;
			case 6: SetClantagM("E.net AIMWAR"); break;
			case 7: SetClantagM(".net AIMWARE"); break;
			case 8: SetClantagM("net AIMWARE."); break;
			case 9: SetClantagM("et AIMWARE.n"); break;
			case 10: SetClantagM("t AIMWARE.ne"); break;
			case 11: SetClantagM("AIMWARE.net"); break;
			}
			LastFrame = FrameNum;
		}


	}
	else if (g_Options.misc_clantag_type == 3)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 12;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("[]{{}}{{}}[]"); break;
			case 1: SetClantagM("_]{{}}{{}}[]"); break;
			case 2: SetClantagM("[_{{}}{{}}[]"); break;
			case 3: SetClantagM("[]_{}}{{}}[]"); break;
			case 4: SetClantagM("[]{_}}{{}}[]"); break;
			case 5: SetClantagM("[]{{_}{{}}[]"); break;
			case 6: SetClantagM("[]{{}_{{}}[]"); break;
			case 7: SetClantagM("[]{{}__{}}[]"); break;
			case 8: SetClantagM("[*]{}__{}[*]"); break;
			case 9: SetClantagM("[*]{}__{*}[*]"); break;
			case 10: SetClantagM("[*]{*}_{*}[*]"); break;
			case 11: SetClantagM("[*]{*0*0*}[*]"); break;
			}
			LastFrame = FrameNum;
		}


	}
	else if (g_Options.misc_clantag_type == 4)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 21;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("L!SA beta"); break;
			case 1: SetClantagM("+!SA beta"); break;
			case 2: SetClantagM("L+SA beta"); break;
			case 3: SetClantagM("L!+A beta"); break;
			case 4: SetClantagM("L!S+ beta"); break;
			case 5: SetClantagM("L!SA+beta"); break;
			case 6: SetClantagM("L!SA +eta"); break;
			case 7: SetClantagM("L!SA b+ta"); break;
			case 8: SetClantagM("L!SA be+a"); break;
			case 9: SetClantagM("L!SA bet+"); break;
			case 10: SetClantagM("L!SA beta"); break;
			case 11: SetClantagM("L!SA bet+"); break;
			case 12: SetClantagM("L!SA be+a"); break;
			case 13: SetClantagM("L!SA b+ta"); break;
			case 14: SetClantagM("L!SA +eta"); break;
			case 15: SetClantagM("L!SA+beta"); break;
			case 16: SetClantagM("L!S+ beta"); break;
			case 17: SetClantagM("L!+A beta"); break;
			case 18: SetClantagM("L+SA beta"); break;
			case 19: SetClantagM("+!SA beta"); break;
			case 20: SetClantagM("L!SA beta"); break;
			}
			LastFrame = FrameNum;
		}


	}
	else if (g_Options.misc_clantag_type == 5)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 8;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("gamesense?"); break;
			case 1: SetClantagM("pphud beta?"); break;
			case 2: SetClantagM("AIMWARE.net?"); break;
			case 3: SetClantagM("ETERNITY.CC?"); break;
			case 4: SetClantagM("wanheda.red?"); break;
			case 5: SetClantagM("onetap.su?"); break;
			case 6: SetClantagM("FERRUMCHEATS?"); break;
			case 7: SetClantagM("L!SA?"); break;
			}
			LastFrame = FrameNum;
		}

	}
	else if (g_Options.misc_clantag_type == 6)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 21;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("fuck a fox"); break;
			case 1: SetClantagM("=uck a fox"); break;
			case 2: SetClantagM("==ck a fox"); break;
			case 3: SetClantagM("===k a fox"); break;
			case 4: SetClantagM("==== a fox"); break;
			case 5: SetClantagM("=====a fox"); break;
			case 6: SetClantagM("====== fox"); break;
			case 7: SetClantagM("=======fox"); break;
			case 8: SetClantagM("========ox"); break;
			case 9: SetClantagM("=========x"); break;
			case 10: SetClantagM("=========="); break;
			case 11: SetClantagM("=========x"); break;
			case 12: SetClantagM("========ox"); break;
			case 13: SetClantagM("=======fox"); break;
			case 14: SetClantagM("====== fox"); break;
			case 15: SetClantagM("=====a fox"); break;
			case 16: SetClantagM("==== a fox"); break;
			case 17: SetClantagM("===k a fox"); break;
			case 18: SetClantagM("==ck a fox"); break;
			case 19: SetClantagM("=uck a fox"); break;
			case 20: SetClantagM("fuck a fox"); break;
			}
			LastFrame = FrameNum;
		}

	}
	else if (g_Options.misc_clantag_type == 7)
	{
		static int FrameNum = 0, LastFrame = 0;
		FrameNum = (int)(g_GlobalVars->curtime / g_Options.misc_clantag_speed) % 11;

		if (FrameNum != LastFrame)
		{
			switch (FrameNum)
			{
			case 0: SetClantagM("L!SA beta\n\r"); break;
			case 1: SetClantagM("!SA beta L"); break;
			case 2: SetClantagM("SA beta L!\n\r"); break;
			case 3: SetClantagM("A beta L!S"); break;
			case 4: SetClantagM(" beta L!SA\n\r"); break;
			case 5: SetClantagM("beta L!SA "); break;
			case 6: SetClantagM("eta L!SA b\n\r"); break;
			case 7: SetClantagM("ta L!SA be"); break;
			case 8: SetClantagM("a L!SA bet\n\r"); break;
			case 9: SetClantagM(" L!SA beta"); break;
			case 10: SetClantagM("L!SA beta \n\r"); break;
			}
			LastFrame = FrameNum;
		}
	}
}
Vector CalculateAngle(Vector vecOrigin, Vector vecOther)
{
	auto ret = Vector();
	Vector delta = vecOrigin - vecOther;
	double hyp = delta.Length2D();
	ret.y = (atan(delta.y / delta.x) * 57.295779513082f);
	ret.x = (atan(delta.z / hyp) * 57.295779513082f);
	ret.z = 0.f;

	if (delta.x >= 0.f)
		ret.y += 180.f;
	return ret;
}
template <class T>
constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
{
	return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
}

static inline float GetIdealRotation(float speed)
{
	return Clamp<float>(RAD2DEG(std::atan2(15.f, speed)), 0.f, 45.f)* g_GlobalVars->interval_per_tick;
}

void RotateMovement(float yaw, CUserCmd *cmd)
{
	QAngle viewangles;
	g_EngineClient->GetViewAngles(viewangles);

	float rotation = DEG2RAD(viewangles.yaw - yaw);

	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);

	float new_forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
	float new_sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);

	cmd->forwardmove = new_forwardmove;
	cmd->sidemove = new_sidemove;
}

void AngleVectorsMSC(const Vector& angles, Vector* forward)
{
	//Assert(s_bMathlibInitialized);
	//Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
float VerifyRotation(float ideal_rotation)
{
	static constexpr float ray_length = 1000.f;
	static constexpr float minimum_distance = 50.f;
	static constexpr float rotation_step = 5.f;

	auto local_player = g_LocalPlayer;
	if (!local_player)
		return ideal_rotation;

	auto collideable = local_player->GetCollideable();
	
	if (!collideable)
		return ideal_rotation;

	auto TraceRayBoundingBox = [](Vector start, Vector end, Vector min, Vector max, float& fraction) -> void
	{
		Vector starts[8];
		Vector ends[8];

		starts[0] = start + Vector(min.x, min.y, min.z); /// min
		starts[1] = start + Vector(max.x, min.y, min.z);
		starts[2] = start + Vector(max.x, max.y, min.z);
		starts[3] = start + Vector(max.x, max.y, max.z); /// max
		starts[4] = start + Vector(min.x, max.y, max.z);
		starts[5] = start + Vector(min.x, min.y, max.z);
		starts[6] = start + Vector(max.x, min.y, max.z);
		starts[7] = start + Vector(min.x, max.y, min.z);

		ends[0] = end + Vector(min.x, min.y, min.z); /// min
		ends[1] = end + Vector(max.x, min.y, min.z);
		ends[2] = end + Vector(max.x, max.y, min.z);
		ends[3] = end + Vector(max.x, max.y, max.z); /// max
		ends[4] = end + Vector(min.x, max.y, max.z);
		ends[5] = end + Vector(min.x, min.y, max.z);
		ends[6] = end + Vector(max.x, min.y, max.z);
		ends[7] = end + Vector(min.x, max.y, min.z);

		float fractions[8];
		for (int i = 0; i < 8; i++)
		{
			CTraceFilterWorldOnly filter;
			trace_t trace;
			Ray_t ray;
			ray.Init(starts[i], ends[i]);

			g_EngineTrace->TraceRay(ray, MASK_ALL, &filter, &trace);
			fractions[i] = trace.fraction;
		}

		fraction = 1.f;
		for (const float& frac : fractions)
		{
			if (frac < fraction)
				fraction = frac;
		}
	};

	Vector bbmin, bbmax;
	bbmin = collideable->OBBMins();
	bbmax = collideable->OBBMaxs();

	const Vector velocity_angle = CalculateAngle(Vector(0, 0, 0), local_player->GetVelocity());
	float verified_roation = ideal_rotation;

	for (float i = ideal_rotation; ((ideal_rotation > 0) ? (i < 180.f) : (i > -180.f)); ((ideal_rotation > 0) ? (i += rotation_step) : (i -= rotation_step)))
	{
		Vector direction;
		float fraction_1a, fraction_1b;
		float fraction_2a, fraction_2b;

		AngleVectorsMSC(Vector(0.f, velocity_angle.y + i, 0), &direction);
		TraceRayBoundingBox(local_player->m_vecOrigin(), local_player->m_vecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_1a);
		AngleVectorsMSC(Vector(0.f, velocity_angle.y + i + rotation_step, 0), &direction);
		TraceRayBoundingBox(local_player->m_vecOrigin(), local_player->m_vecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_1b);

		AngleVectorsMSC(Vector(0.f, velocity_angle.y - i, 0), &direction);
		TraceRayBoundingBox(local_player->m_vecOrigin(), local_player->m_vecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_2a);
		AngleVectorsMSC(Vector(0.f, velocity_angle.y - (i + rotation_step), 0), &direction);
		TraceRayBoundingBox(local_player->m_vecOrigin(), local_player->m_vecOrigin() + (direction * ray_length), bbmin, bbmax, fraction_2b);

		if ((fraction_1a * ray_length > minimum_distance) && (fraction_1b * ray_length > minimum_distance))
		{
			verified_roation = i;
			break;
		}
		else if ((fraction_2a * ray_length > minimum_distance) && (fraction_2b * ray_length > minimum_distance))
		{
			verified_roation = -i;
			break;
		}
	}

	return verified_roation;
}

void FoxMisc::CircleStrafe()
{
		auto local_player = g_LocalPlayer;
		if (!local_player || local_player->m_iHealth() <= 0 || local_player->m_fFlags() & FL_ONGROUND)
			return;

		static bool clock_wise = false;

		const float velocity_yaw = CalculateAngle(Vector(0, 0, 0), local_player->GetVelocity()).y;
		const float ideal_rotation_amount = GetIdealRotation(local_player->GetVelocity().Length2D()) * (clock_wise ? -1.f : 1.f);

		float rotation_amount = VerifyRotation(ideal_rotation_amount);

		cmd->forwardmove = 0.f;
		cmd->sidemove = (rotation_amount > 0) ? -450.f : 450.f;
		RotateMovement(velocity_yaw + rotation_amount, cmd);
	
}

void FoxMisc::NameSpam()
{

	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.1)
		return;

	static bool flip = false;

	if (!flip)
	{
		set_nameM("LISA");
		flip = true;
	}
	else
	{
		set_nameM("LISA ");
		flip = false;
	}
	

	start_t = clock();
}
