#pragma once
#include "valve_sdk\sdk.hpp"
#include ".//valve_sdk/interfaces/CInput.hpp"
#include "helpers/utils.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "helpers\math.hpp"
#include "valve_sdk\sdk.hpp"
#include ".//valve_sdk/interfaces/CInput.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "options.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "valve_sdk/sdk.hpp"
#include "valve_sdk/interfaces/IClientEntity.hpp"
#include "helpers/math.hpp"
#include "helpers/utils.hpp"
#include "ILocalize.hpp"
#include "options.hpp"
#include "singleton.hpp"
#include "valve_sdk/interfaces/CInput.hpp"
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "valve_sdk\interfaces\IMDLCache.hpp"
#include <array>
#include <deque>
extern inline float distance_point_to_line(Vector Point, Vector LineOrigin, Vector Dir);
extern inline Vector angle_vector(Vector meme);
struct StoredData
{
	float simtime = -1;
	Vector hitboxPos = Vector{ 0.f, 0.f, 0.f };
	matrix3x4_t matrix[MAXSTUDIOBONES];
	bool matrixbuilt = false;
	float m_flPoseParameter[24];

	StoredData(C_BasePlayer* ent) {
		simtime = ent->m_flSimulationTime();
		hitboxPos = ent->GetBonePos(8);
		if (ent->SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, g_GlobalVars->curtime))
			matrixbuilt = true;
	}
};


class TimeWarp : public Singleton<TimeWarp>
{
public:
	struct SequenceData {
		SequenceData::SequenceData(int instate, int outstate, int seqnr, float realtime) {
			inreliablestate = instate;
			outreliablestate = outstate;
			sequencenr = seqnr;
			Realtime = realtime;
		}
		int inreliablestate;
		int outreliablestate;
		int sequencenr;
		float Realtime;
	};
	void StoreRecords(CUserCmd* cmd, C_BasePlayer* local);
	void DoBackTrack(CUserCmd* cmd, C_BasePlayer* pLocal);
	void ClearIncomingSequences();
	void AddLatencyToNetchan(INetChannel * netchan, float Latency);
	bool IsTickValid(int tick);
	void UpdateBacktrackRecords(C_BasePlayer * pPlayer);
	void UpdateIncomingSequences();
};
extern TimeWarp* backtracking;
extern std::array<std::deque<StoredData>, 65> validRecords;
extern std::array<std::deque<StoredData>, 65> viableRecords;