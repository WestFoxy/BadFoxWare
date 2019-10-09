#pragma once
#include "MainInclude.hpp"
void correct_movement1(CUserCmd* cmd, const QAngle& old_angles, const float& old_forward, const float& old_sidemove)
{
	// side/forward move correction
	float f1, f2, deltaView;
	if (old_angles.yaw < 0.f)
		f1 = 360.0f + old_angles.yaw;
	else
		f1 = old_angles.yaw;

	if (cmd->viewangles.yaw < 0.0f)
		f2 = 360.0f + cmd->viewangles.yaw;
	else
		f2 = cmd->viewangles.yaw;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	cmd->forwardmove = cos(DEG2RAD(deltaView)) * old_forward + cos(DEG2RAD(deltaView + 90.f)) * old_sidemove;
	cmd->sidemove = sin(DEG2RAD(deltaView)) * old_forward + sin(DEG2RAD(deltaView + 90.f)) * old_sidemove;
}
namespace desync
{
	void correct_movement(CUserCmd* cmd, const QAngle& old_angles)
	{
		correct_movement1(cmd, old_angles, cmd->forwardmove, cmd->sidemove);
	}

	


	float yaw_offset;

	bool flip_yaw = false;
	bool flip_packet = false;

	
	CCSPlayerAnimState* anim_state;

	bool is_enabled(CUserCmd* cmd)
	{
		if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
			return false;

		if (!g_Options.misc_legitdesync || (cmd->buttons & IN_USE))
			return false;

		auto * channel_info = g_EngineClient->GetNetChannelInfo();
		if (channel_info && (channel_info->GetAvgLoss(1) > 0.f || channel_info->GetAvgLoss(0) > 0.f))
			return false;

		if (g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
			return false;

		if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
			return false;

		return true;
	}

	bool is_firing(CUserCmd * cmd)
	{
		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (!weapon)
			return false;

		const auto weapon_type = weapon->GetCSWeaponData()->m_WeaponType;
		if (weapon_type == WEAPONTYPE_GRENADE)
		{
			return true;

			//if (!weapon->m_bPinPulled() && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
				//return false;
		}
		else if (weapon_type == WEAPONTYPE_KNIFE)
		{
			if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2)
				return true;
		}
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		{
			if (cmd->buttons & IN_ATTACK && weapon->CanFire())
				return true;

			if (cmd->buttons & IN_ATTACK2)
				return true;
		}
		else if (cmd->buttons & IN_ATTACK && weapon->CanFire() && weapon_type != WEAPONTYPE_C4)
			return true;

		return false;
	}

	float get_max_desync_delta()
	{
		const auto speed_factor = std::max<float>(0.f, std::min<float>(1, anim_state->m_flSpeedFactor));
		const auto speed_fraction = std::max<float>(0.f, std::min<float>(anim_state->m_flSpeedFraction, 1.f));

		const auto unk1 = (anim_state->m_flLandingRatio * -0.30000001 - 0.19999999) * speed_fraction;
		float unk2 = unk1 + 1.f;

		if (anim_state->m_flDuckAmount > 0)
			unk2 += anim_state->m_flDuckAmount * speed_factor * (0.5f - unk2);

		return anim_state->m_flMaxBodyYawDegrees * unk2;
	}

	void Normalize(QAngle& angles)
	{
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}
	void Clamp(QAngle& angles)
	{
		if (angles.pitch > 89.0f) angles.pitch = 89.0f;
		else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

		if (angles.yaw > 180.0f) angles.yaw = 180.0f;
		else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

		angles.roll = 0;
	}
	void NormalizeClamp(QAngle& angles)
	{
		Normalize(angles);
		Clamp(angles);
	}

	void handle(CUserCmd * cmd, bool& send_packet)
	{
		if (!send_packet)
			return;

		if (!is_enabled(cmd))
		{
			yaw_offset = 0.f;

			return;
		}

		anim_state = g_LocalPlayer->GetPlayerAnimState();

		assert(anim_state);

		yaw_offset = get_max_desync_delta();

		const auto old_angles = cmd->viewangles;

		flip_packet = !flip_packet;
		send_packet = flip_packet;
		if (!flip_packet)
		{
			if (g_Options.misc_legitdesync_flip)
				flip_yaw = !flip_yaw;
		}

		static float last_lby = 0.f;
		static float last_update = 0.f;

		const auto current_lby = g_LocalPlayer->m_flLowerBodyYawTarget();
		const float current_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;

		const float delta = ceilf((current_time - last_update) * 100) / 100;
		const auto next_delta = ceilf((delta + g_GlobalVars->interval_per_tick) * 100) / 100;

		if (g_LocalPlayer->m_vecVelocity().Length2D() <= 0.f)
		{
			if (current_lby != 180.f && last_lby != current_lby)
			{
				g_CVar->ConsolePrintf("lby updated after %.4f\n", delta);

				last_lby = current_lby;
				last_update = current_time - g_GlobalVars->interval_per_tick;
			}
			else if (next_delta >= 1.1f)
			{
				g_CVar->ConsolePrintf("curr: %.4f; next: %.4f\n", delta, next_delta);

				send_packet = flip_packet = true;

				last_update = current_time;
			}
		}
		else
		{
			last_lby = current_lby;
			last_update = current_time;
		}

		const auto low_fps = g_GlobalVars->interval_per_tick * 0.9f < g_GlobalVars->absoluteframetime;
		if (low_fps || is_firing(cmd))
			send_packet = flip_packet = true;

		if (send_packet)
		{
			anim_state->m_flGoalFeetYaw += flip_yaw ? yaw_offset : -yaw_offset;
			g_Options.RealAngle = Vector(cmd->viewangles.pitch, cmd->viewangles.yaw, cmd->viewangles.roll);
		}
		else
		{
			cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);

			cmd->viewangles.yaw += 180.f;
			//cmd->viewangles.Normalize();
			NormalizeClamp(cmd->viewangles);

			correct_movement(cmd, old_angles);
		}
	}
}