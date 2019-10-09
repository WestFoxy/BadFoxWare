#include "AntiAim.h"
#include "CSX.h"
#include "AutoWall.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#define M_PI 3.14159265358979323846
#define max1(a,b)            (((a) > (b)) ? (a) : (b))
#define min1(a,b)            (((a) < (b)) ? (a) : (b))

AntiAim g_AntiAim;

bool Swtich = false;
void MovementFix(Vector& oldang);
enum MovingAAType {
	Standing = 0,
	Moving,
	Air
};
float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}
float NormalizeYawAA(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}
/*void AngleVectorsAA(const Vector& angles, Vector* forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}*/
int DistanceAA(Vector2D point1, Vector2D point2)
{
	int diffY = point1.y - point2.y;
	int diffX = point1.x - point2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}
int ReturnMovementType() {
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		if (g_LocalPlayer->GetVelocity().Length2D() < 32)
		{
			return MovingAAType::Standing;
		}
		else if (g_LocalPlayer->GetVelocity().Length2D() > 32)
		{
			return MovingAAType::Moving;
		}
	}
	else
	{
		return MovingAAType::Air;
	}
}

void FreeStanding()
{
	
	static float FinalAngle;
	bool bside1 = false;
	bool bside2 = false;
	bool autowalld = false;
	for (int i = 0; i <= g_EngineClient->GetMaxClients(); ++i)
	{
		C_BasePlayer* pPlayerEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g_LocalPlayer
			|| pPlayerEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		float angToLocal = Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), pPlayerEntity->m_vecOrigin()).yaw;
		Vector ViewPoint = pPlayerEntity->m_vecOrigin() + Vector(0, 0, 90);

		Vector2D Side1 = { (45 * sin(GRD_TO_BOG(angToLocal))),(45 * cos(GRD_TO_BOG(angToLocal))) };
		Vector2D Side2 = { (45 * sin(GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(GRD_TO_BOG(angToLocal + 180))) };

		Vector2D Side3 = { (50 * sin(GRD_TO_BOG(angToLocal))),(50 * cos(GRD_TO_BOG(angToLocal))) };
		Vector2D Side4 = { (50 * sin(GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(GRD_TO_BOG(angToLocal + 180))) };

		Vector Origin = g_LocalPlayer->m_vecOrigin();

		Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

		Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

		for (int side = 0; side < 2; side++)
		{
			Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 80 };
			Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

			if (g_AutoWall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
			{
				if (side == 0)
				{
					bside1 = true;
					FinalAngle = angToLocal + 90;
				}
				else if (side == 1)
				{
					bside2 = true;
					FinalAngle = angToLocal - 90;
				}
				autowalld = true;
			}
			else
			{
				for (int side222 = 0; side222 < 2; side222++)
				{
					Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 80 };

					if (g_AutoWall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
					{
						if (side222 == 0)
						{
							bside1 = true;
							FinalAngle = angToLocal + 90;
						}
						else if (side222 == 1)
						{
							bside2 = true;
							FinalAngle = angToLocal - 90;
						}
						autowalld = true;
					}
				}
			}
		}
	}

	if (!autowalld || (bside1 && bside2)) {
		switch (ReturnMovementType()) {
		case MovingAAType::Standing: {
			switch (g_Options.yaw_standing) {
			case 1: g_Options.cmd->viewangles.yaw += 180; break;
			case 2: g_Options.cmd->viewangles.yaw = 0; break;
			case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
			case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
			}
		}break;
		case MovingAAType::Moving: {
			switch (g_Options.yaw_moving) {
			case 1: g_Options.cmd->viewangles.yaw += 180; break;
			case 2: g_Options.cmd->viewangles.yaw = 0; break;
			case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
			case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
			}
		}break;
		case MovingAAType::Air: {
			switch (g_Options.yaw_air) {
			case 1: g_Options.cmd->viewangles.yaw += 180; break;
			case 2: g_Options.cmd->viewangles.yaw = 0; break;
			case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
			case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
			}
		}break;
		}
	}
	else
		g_Options.cmd->viewangles.yaw = FinalAngle;

	if (g_Options.aa_jitter != 0)
	{
		float Offset = g_Options.aa_jitter;


		Swtich ? g_Options.cmd->viewangles.yaw -= Offset : g_Options.cmd->viewangles.yaw += Offset;
	}
}


mstudiobbox_t* get_hitboxv2(C_BasePlayer* entity, int hitbox_index)
{
	if (entity->IsDormant() || entity->m_iHealth() <= 0)
		return NULL;

	const auto pModel = entity->GetModel();
	if (!pModel)
		return NULL;

	auto pStudioHdr = g_MdlInfo->GetStudioModel(pModel);
	if (!pStudioHdr)
		return NULL;

	auto pSet = pStudioHdr->GetHitboxSet(0);
	if (!pSet)
		return NULL;

	if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
		return NULL;

	return pSet->GetHitbox(hitbox_index);
}
Vector GetHitboxPositionv2(C_BasePlayer* entity, int hitbox_id)
{
	auto hitbox = get_hitboxv2(entity, hitbox_id);
	if (!hitbox)
		return Vector(0, 0, 0);
	
	
	auto bone_matrix = entity->GetBoneMatrixA(hitbox->bone);

	Vector bbmin, bbmax;
	Math::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
	Math::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}
float m_bestthreat = 0.f;
void NormalizeNum(Vector& vIn, Vector& vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}
void VectorSubtractv2(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}


float fov_player(Vector ViewOffSet, Vector View, C_BasePlayer* entity, int hitbox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectorsAA(Angles, &Forward);

	Vector AimPos = GetHitboxPositionv2(entity, hitbox); //pvs fix disabled

	
	VectorSubtractv2(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	NormalizeNum(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / M_PI));
}


int closest_to_crosshair()
{
	int index = -1;
	float lowest_fov = INT_MAX;

	auto local_player = g_LocalPlayer;

	if (!local_player)
		return -1;

	Vector local_position = local_player->m_vecOrigin() + local_player->m_vecViewOffset();

	Vector angles;
	QAngle patchang;
	g_EngineClient->GetViewAngles(patchang);
	angles.x = patchang.pitch;
	angles.y = patchang.yaw;
	angles.z = patchang.roll;

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		auto entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity || !entity->IsAlive() || entity->m_iTeamNum() == local_player->m_iTeamNum() || entity->IsDormant() || entity == local_player)
			continue;

		float fov = fov_player(local_position, angles, entity, 0);

		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
		}
	}

	return index;
}

int closest_to_local_player()
{
	int index = -1;
	float lowest_distance = INT_MAX;

	auto local_player = g_LocalPlayer;

	if (!local_player)
		return -1;

	Vector local_position = local_player->m_vecOrigin() + local_player->m_vecViewOffset();

	Vector angles;
	QAngle angles_patch;
	g_EngineClient->GetViewAngles(angles_patch);

	angles.x = angles_patch.pitch;
	angles.y = angles_patch.yaw;
	angles.z = angles_patch.roll;

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		auto entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity || !entity->IsAlive() || entity->m_iTeamNum() == local_player->m_iTeamNum() || entity->IsDormant() || entity == local_player)
			continue;

		float distance = DistanceAA(Vector2D(local_player->m_vecOrigin().x, local_player->m_vecOrigin().y), Vector2D(entity->m_vecOrigin().x, entity->m_vecOrigin().y));

		if (distance < lowest_distance)
		{
			lowest_distance = distance;
			index = i;
		}
	}

	return index;
}


int BestTargetByThreat() {

	for (int i = 0; i <= g_GlobalVars->maxClients; i++)
	{
		auto Entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		auto LocalPlayer = g_LocalPlayer;

		if (!Entity || !LocalPlayer || !Entity->IsAlive() || Entity->IsDormant())
			continue;

		m_bestthreat = 0.f;

		float dist = DistanceAA(Vector2D(LocalPlayer->m_vecOrigin().x, LocalPlayer->m_vecOrigin().y), Vector2D(Entity->m_vecOrigin().x, Entity->m_vecOrigin().y));

		float health = (float)Entity->m_iHealth();

		float threat = health / dist;

		if (threat > m_bestthreat)
		{
			return Entity->EntIndex();
		}

		return 0;
	}
}
void freestand(CUserCmd* cmd)
{
	auto local_player = g_LocalPlayer;

	if (!local_player)
		return;

	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	Vector besthead;

	m_bestthreat = 0.f;

	auto leyepos = local_player->m_vecOrigin() + local_player->m_vecViewOffset();
	auto headpos = GetHitboxPositionv2(local_player, 0);
	auto origin = local_player->m_vecABSorigin();


	auto checkWallThickness = [&](C_BasePlayer* pPlayer, Vector newhead) -> float
	{
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->m_vecOrigin() + pPlayer->m_vecViewOffset();


		CTraceFilterSkipTwoEntities filter(pPlayer, local_player);

		trace_t trace1, trace2;
		Ray_t dd;
		dd.Init(newhead, eyepos);

		g_EngineTrace->TraceRay(dd, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.endpos;
		else
			return 0.f;

		dd.Init(eyepos, newhead);
		g_EngineTrace->TraceRay(dd, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.endpos;

		float add = newhead.DistTo(eyepos) - leyepos.DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	int index = -1;

	switch (g_Options.antiaim_freestanding_mode) {
	case 1: { index = closest_to_crosshair();  } break;
	case 2: { index = closest_to_local_player(); } break;
	}

	static C_BasePlayer* entity;

	if (index != -1)
		entity = (C_BasePlayer*)g_EntityList->GetClientEntity(index);

	float step = (2 * M_PI) / 18.f; // One PI = half a circle ( for stacker cause low iq :sunglasses: ), 28

	float radius = fabs(Vector(headpos - origin).Length2D());

	if (index == -1)
	{
		no_active = true;
	}
	else
	{
		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

			float totalthickness = 0.f;

			no_active = false;

			totalthickness += checkWallThickness(entity, newhead);

			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				bestrotation = rotation;
				besthead = newhead;
			}
		}
	}


	if (no_active)
		switch (ReturnMovementType()) {
		case MovingAAType::Standing: {
			switch (g_Options.yaw_standing) {
			case 1: g_Options.cmd->viewangles.yaw += 180; break;
			case 2: g_Options.cmd->viewangles.yaw = 0; break;
			case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
			case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
			}
		}break;
		case MovingAAType::Moving: {
			switch (g_Options.yaw_moving) {
			case 1: g_Options.cmd->viewangles.yaw += 180; break;
			case 2: g_Options.cmd->viewangles.yaw = 0; break;
			case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
			case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
			}
		}break;
		case MovingAAType::Air: {
			switch (g_Options.yaw_air) {
			case 1: g_Options.cmd->viewangles.yaw += 180; break;
			case 2: g_Options.cmd->viewangles.yaw = 0; break;
			case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
			case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
			}
		}break;
		}
	else
		cmd->viewangles.yaw = RAD2DEG(bestrotation);

	last_real = cmd->viewangles.yaw;
}

bool Left_AA = false;
bool Right_AA = false;
bool Back_AA = false;

static float next_lby_update_time = 0;
float get_curtime(CUserCmd* ucmd) {
	auto local_player = g_LocalPlayer;

	if (!local_player)
		return 0;

	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = (float)local_player->m_nTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * g_GlobalVars->interval_per_tick;
	return curtime;
}
float next_lby_update(const float yaw_to_break, CUserCmd* cmd)
{
	auto local_player = g_LocalPlayer;

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime(cmd);

	auto animstate = local_player->AnimationState();
	if (!animstate)
		return false;

	if (!(local_player->m_fFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		//return true;
		return next_lby_update_time;
	}

	return false;
}
bool m_bAutomaticDir = false;
int m_iAutoDirection = 0;

int m_iJitter = 0;
int m_iJitterIteration = 0;

float m_flCurrentFeetYaw = 0.0f;
float m_flPreviousFeetYaw = 0.0f;
bool next_lby_update(CUserCmd* cmd)
{
	static float local_update;
	if (!g_LocalPlayer)
		return false;

	static float next_lby_update_time = 0;
	float curtime = g_GlobalVars->curtime;
	local_update = next_lby_update_time;

	auto animstate = g_LocalPlayer->AnimStatev2();

	if (!animstate)
		return false;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed > 0.1f)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}
void Real()
{
	//if (!g::bSendPacket && g::pLocalEntity->AnimState() && g_Menu.Config.DesyncAngle)
		//g::pcmd->viewangles.yaw = g_Math.NormalizeYaw(g::pLocalEntity->AnimState()->m_flGoalFeetYaw + 180.f);
	//else {
		// freestanding soon
	//}
	if (g_Options.desync == 5)
	{
		if (next_lby_update(g_Options.cmd))
		{
			if (g_Options.Manual_Side)
				g_Options.cmd->viewangles.yaw += 180;
			else
				g_Options.cmd->viewangles.yaw -= 180;
			*g_Options.bSendPacketLink = true;
		}
		else
		{
			if (*g_Options.bSendPacketLink)
			{
				if (g_Options.Manual_Side)
					g_Options.cmd->viewangles.yaw += 180.0f + g_LocalPlayer->DesyncValue();
				else
					g_Options.cmd->viewangles.yaw += 180.0f - g_LocalPlayer->DesyncValue();

				//g_Options.FakeAngle = Vector()
			}
			else
				g_Options.cmd->viewangles.yaw += 180.0f;
		}
	}


	static bool Swtich2 = false;
	Swtich2 = !Swtich2;

	static float test = 0.f;
	if (Swtich2)
		test += 90.f;

	test = NormalizeYawAA(test);

	if (g_Options.desync > 0) {
		g_Options.DesyncEnabledSpecialChecks = true;
	}
	else {
		g_Options.DesyncEnabledSpecialChecks = false;
	}
	if (g_Options.desync == 3 && g_LocalPlayer->AnimationState()) {
		float desync = g_LocalPlayer->DesyncValue();
		float balance = 1.0f;

		int type = 2;
		if (type == 2)
			balance = -1.0f;

		if (g_GlobalVars->curtime <= next_lby_update(g_Options.cmd->viewangles.yaw, g_Options.cmd)) {

			auto net_channel = g_EngineClient->GetNetChannel();

			if (!net_channel)
				return;

			if (net_channel->m_nChokedPackets >= 2) {
				g_Options.cmd->viewangles.yaw = NormalizeYawAA(g_Options.cmd->viewangles.yaw);
				return;
			}

			if (type == 1)
				g_Options.cmd->viewangles.yaw -= 100.0f;
			else
				g_Options.cmd->viewangles.yaw += (balance * 120.0f);
		}
		else if (type != 1) {
			g_Options.cmd->viewangles.yaw -= (desync + 30.0f) * balance;
		}
	}

	if (g_Options.desync == 4) {
		int jitter_side = 1;

		g_Options.cmd->viewangles.yaw += 180.0f;

		float desync = g_LocalPlayer->DesyncValue();
		float lby_delta = 180.0f - desync + 10.0f;
		float desync_length = 180.0f - lby_delta * 0.5f;
		float jitter = 90.0f * jitter_side;


		if (jitter_side == 1)
			g_Options.cmd->viewangles.yaw += desync_length;
		else if (jitter_side == -1)
			g_Options.cmd->viewangles.yaw -= desync_length;


		int v19 = 0;
		if (g_GlobalVars->curtime <= next_lby_update(g_Options.cmd->viewangles.yaw, g_Options.cmd)) {
			v19 = m_iJitter;
		}
		else {
			m_iJitter = 0;
		}

		int v20 = v19 - 1;
		if (v20) {
			if (v20 == 1) {
				if (jitter_side == 1)
					g_Options.cmd->viewangles.yaw += lby_delta;
				else
					g_Options.cmd->viewangles.yaw += desync - 190.0f;
			}
		}
		else {
			if (jitter_side == 1)
				g_Options.cmd->viewangles.yaw += desync - 190.0;
			else
				g_Options.cmd->viewangles.yaw += lby_delta;
			g_Options.GlobalSendPacket = true;
		}

		if (++m_iJitter >= 3)
			m_iJitter = 0;

		g_Options.cmd->viewangles.yaw = NormalizeYawAA(g_Options.cmd->viewangles.yaw);
	}

	if (!g_Options.GlobalSendPacket && g_LocalPlayer->AnimationState() && g_Options.desync > 0) {
		if (g_Options.desync == 1) {
			g_Options.cmd->viewangles.yaw = NormalizeYawAA(g_Options.RealAngle.y + 90 + test);
		}
		else if (g_Options.desync == 2) {
			g_Options.cmd->viewangles.yaw += g_LocalPlayer->DesyncValue();
		}
	}
	else {
		if (!g_Options.antiaim_freestanding) {

			switch (ReturnMovementType()) {
			case MovingAAType::Standing: {
				switch (g_Options.yaw_standing) {
				case 1: g_Options.cmd->viewangles.yaw += 180; break;
				case 2: g_Options.cmd->viewangles.yaw = 0; break;
				case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
				case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
				}
			}break;
			case MovingAAType::Moving: {
				switch (g_Options.yaw_moving) {
				case 1: g_Options.cmd->viewangles.yaw += 180; break;
				case 2: g_Options.cmd->viewangles.yaw = 0; break;
				case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
				case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
				}
			}break;
			case MovingAAType::Air: {
				switch (g_Options.yaw_air) {
				case 1: g_Options.cmd->viewangles.yaw += 180; break;
				case 2: g_Options.cmd->viewangles.yaw = 0; break;
				case 3: g_Options.cmd->viewangles.yaw += g_GlobalVars->curtime * 3000; break;
				case 4: g_Options.cmd->viewangles.yaw += g_Options.Manual_Side ? 90 : -90; break;
				}
			}break;
			}

		}
		else {
			if (g_Options.antiaim_freestanding_mode == 0) {
				FreeStanding();
			}
			else {
				freestand(g_Options.cmd);
			}
		}
	}
}

void AntiAim::OnCreateMove()
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_LocalPlayer->m_hActiveWeapon() || !g_Options.antiaim_enabled) {
		g_Options.DesyncEnabledSpecialChecks = false;
		return;
	}

	Vector originalview = Vector( g_Options.cmd->viewangles.pitch, g_Options.cmd->viewangles.yaw, g_Options.cmd->viewangles.roll);

	auto pWeap = g_LocalPlayer->m_hActiveWeapon();
	

	if (!g_LocalPlayer->IsAlive() || pWeap->IsGrenade()) {
		g_Options.DesyncEnabledSpecialChecks = false;
		return;
	}


	if (g_LocalPlayer->m_nMoveType() == MoveType_t::MOVETYPE_LADDER) {
		g_Options.DesyncEnabledSpecialChecks = false;
		return;
	}

	if (g_Options.cmd->buttons & IN_USE)
	{
		g_Options.DesyncEnabledSpecialChecks = false;
		return;
	}
	/*if (next_lby_update(g_Options.cmd))
	{
		g_Options.cmd->viewangles.yaw += 1;
		*g_Options.bSendPacketLink = true;
		return;
	}*/
	
	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	bool canShoot = (g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() <= flServerTime);

	if ((canShoot && (g_Options.cmd->buttons & IN_ATTACK)) && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER) {
		g_Options.DesyncEnabledSpecialChecks = false;

		return;
	}

	
	//g_Options.GlobalSendPacket = !g_Options.GlobalSendPacket;

	switch (ReturnMovementType()) {
	case MovingAAType::Standing: {
		switch (g_Options.pitch_standing) {
		case 1: g_Options.cmd->viewangles.pitch = 89.9f; break;
		case 2: g_Options.cmd->viewangles.pitch = -89.9f; break;
		case 3: g_Options.cmd->viewangles.pitch = 0; break;
		case 4: g_Options.cmd->viewangles.pitch = 45.9f; break;
		}
	}break;
	case MovingAAType::Moving: {
		switch (g_Options.pitch_moving) {
		case 1: g_Options.cmd->viewangles.pitch = 89.9f; break;
		case 2: g_Options.cmd->viewangles.pitch = -89.9f; break;
		case 3: g_Options.cmd->viewangles.pitch = 0; break;
		case 4: g_Options.cmd->viewangles.pitch = 45.9f; break;
		}
	}break;
	case MovingAAType::Air: {
		switch (g_Options.pitch_air) {
		case 1: g_Options.cmd->viewangles.pitch = 89.9f; break;
		case 2: g_Options.cmd->viewangles.pitch = -89.9f; break;
		case 3: g_Options.cmd->viewangles.pitch = 0; break;
		case 4: g_Options.cmd->viewangles.pitch = 45.9f; break;
		}
	}break;
	}


	Real();

	

	MovementFix(originalview);
}
inline float VectorNormalizeAAA(Vector& v)
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
void sin_cos(float radian, float* sin, float* cos)
{
	*sin = std::sin(radian);
	*cos = std::cos(radian);
}

static constexpr float deg_2_rad(float degree)
{
	return degree * (3.14159265358979323846f / 180.f);
}

void AngleVectorsAAAA(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sp, sy, sr, cp, cy, cr;

	sin_cos(deg_2_rad(angles.x), &sp, &cp);
	sin_cos(deg_2_rad(angles.y), &sy, &cy);
	sin_cos(deg_2_rad(angles.z), &sr, &cr);

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

void MovementFix(Vector& oldang)
{
	Vector vMovements(g_Options.cmd->forwardmove, g_Options.cmd->sidemove, 0.f);

	if (vMovements.Length2D() == 0)
		return;

	Vector vRealF, vRealR;



	Vector aRealDir = Vector(g_Options.cmd->viewangles.pitch, g_Options.cmd->viewangles.yaw, g_Options.cmd->viewangles.roll );
	aRealDir.Clamp();


	AngleVectorsAAAA(aRealDir, &vRealF, &vRealR, nullptr);
	vRealF[2] = 0;
	vRealR[2] = 0;

	VectorNormalizeAAA(vRealF);
	VectorNormalizeAAA(vRealR);

	Vector aWishDir = oldang;
	aWishDir.Clamp();

	Vector vWishF, vWishR;
	AngleVectorsAAAA(aWishDir, &vWishF, &vWishR, nullptr);

	vWishF[2] = 0;
	vWishR[2] = 0;

	VectorNormalizeAAA(vWishF);
	VectorNormalizeAAA(vWishR);

	Vector vWishVel;
	vWishVel[0] = vWishF[0] * g_Options.cmd->forwardmove + vWishR[0] * g_Options.cmd->sidemove;
	vWishVel[1] = vWishF[1] * g_Options.cmd->forwardmove + vWishR[1] * g_Options.cmd->sidemove;
	vWishVel[2] = 0;

	float a = vRealF[0], b = vRealR[0], c = vRealF[1], d = vRealR[1];
	float v = vWishVel[0], w = vWishVel[1];

	float flDivide = (a * d - b * c);
	float x = (d * v - b * w) / flDivide;
	float y = (a * w - c * v) / flDivide;

	g_Options.cmd->forwardmove = x;
	g_Options.cmd->sidemove = y;
}