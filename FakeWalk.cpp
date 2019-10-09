#include <time.h>
#include <iostream>
#include "FakeWalk.h"

void AngleVectors4(const Vector& angles, Vector forward)
{
	float sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

void c_fake_walk::do_fake_walk(CUserCmd* cmd)
{
	auto local_player = g_LocalPlayer;

	if (!local_player)
		return;
	
	if (!g_Options.rage_SlowWalk || !GetAsyncKeyState(g_Options.rage_SlowWalk_key))
		return;

	//auto weapon_handle = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));

	//if (!weapon_handle)
	//	return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon)
		return;

	auto weapon_handle = weapon->GetWeaponInfo();
	if (!weapon_handle)
		return;

	float amount = 0.0034f * g_Options.rage_SlowWalk_speed;

	Vector velocity = local_player->GetVelocity();
	Vector direction;

	AngleVectors4(velocity, direction);

	float speed = velocity.Length2D();

	direction.y = cmd->viewangles.yaw - direction.y;

	Vector forward;

	AngleVectors4(direction, forward);

	Vector source = forward * -speed;

	if (speed >= (weapon_handle->max_speed * amount))
	{
		cmd->forwardmove = source.x;
		cmd->sidemove = source.y;
	}
}

c_fake_walk* fakewalk = new c_fake_walk();