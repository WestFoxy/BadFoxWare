#include "EnginePrediction.h"
#include "MainInclude.hpp"

CEnginePrediction g_EnginePrediction;

void CEnginePrediction::Start()
{
	static int nTickBase;
	static CUserCmd* pLastCmd;

	// fix tickbase if game didnt render previous tick
	if (pLastCmd)
	{
		if (pLastCmd->hasbeenpredicted)
			nTickBase = g_LocalPlayer->m_nTickBase();
		else
			++nTickBase;
	}

	// get random_seed as its 0 in clientmode->createmove
	const auto getRandomSeed = []()
	{
		using MD5_PseudoRandomFn = unsigned long(__cdecl*)(std::uintptr_t);
		static uint8_t *offset = 0;
		if(!offset)
			offset = Utils::PatternScan(GetModuleHandle("client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 70 6A 58");
		
		auto MD5_PseudoRandom = (MD5_PseudoRandomFn)(offset);
		if (!MD5_PseudoRandom)
			return (unsigned long)0;
		return MD5_PseudoRandom(g_Options.cmd->command_number) & 0x7FFFFFFF;
	};

	pLastCmd = g_Options.cmd;
	this->flOldCurtime = g_GlobalVars->curtime;
	this->flOldFrametime = g_GlobalVars->frametime;

	float rand_seed = getRandomSeed();
	g_GlobalVars->curtime = nTickBase * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(g_LocalPlayer);

	CMoveData data;
	memset(&data, 0, sizeof(CMoveData));

	g_MoveHelper->SetHost(g_LocalPlayer);
	g_Prediction->SetupMove(g_LocalPlayer, g_Options.cmd, g_MoveHelper, &data);
	g_GameMovement->ProcessMovement(g_LocalPlayer, &data);
	g_Prediction->FinishMove(g_LocalPlayer, g_Options.cmd, &data);
}

void CEnginePrediction::End()
{
	g_GameMovement->FinishTrackPredictionErrors(g_LocalPlayer);
	g_MoveHelper->SetHost(nullptr);

	g_GlobalVars->curtime = this->flOldCurtime;
	g_GlobalVars->frametime = this->flOldFrametime;
}
