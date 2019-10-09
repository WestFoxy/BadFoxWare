#pragma once

class CEnginePrediction
{
public:
	void Start();
	void End();

private:
	float flOldCurtime;
	float flOldFrametime;
	int* m_pPredictionRandomSeed;
};

extern CEnginePrediction g_EnginePrediction;