#pragma once
#include <vector>

#include "MainInclude.hpp"
#include <deque>

struct grenade_info
{
	float x, y, z;
	float time;
	grenade_info(float X, float Y, float Z, float TiMe)
	{
		x = X;
		y = Y;
		z = Z;
		time = TiMe;
	}
};

extern std::vector <grenade_info>Smokes;
extern std::vector <grenade_info>Molotovs;

/*struct GrenadePath
{
	GrenadePath(const std::deque<std::pair<Vector, float>>& path, const float& flThrowTime, const float& flDuration, const float& flFadeSpeed)
	{
		this->path = path;
		this->flThrowTime = flThrowTime;
		this->flDuration = flDuration;
		this->flFadeSpeed = flFadeSpeed;
		this->iInitialPathSize = path.size();
		this->bThrown = true;
		this->step = 0;
	}
	void Fade()
	{
		step += g_GlobalVars->frametime;
		if (step <= flDuration)
			return;

		for (int i = path.size() - 1; i >= 0; i--)
		{
			auto it = path.begin() + i;

			if (!it->second)
			{
				path.erase(it);
				continue;
			}

			it->second -= (iInitialPathSize * flFadeSpeed / 255) / (i + 6);
			if (it->second < 0) it->second = 0.f;
		}
	}
	std::deque<std::pair<Vector, float>> path;
	float flThrowTime;
	float flDuration;
	float flFadeSpeed;
	size_t iInitialPathSize;
	bool bThrown;
	float step;
};*/