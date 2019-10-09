#pragma once

#include <thread>
#include <chrono>
#include "steam_sdk/isteamfriends.h"

#include "steam_sdk/isteamclient.h"
#include "steam_sdk/isteamuser.h"
#include "steam_sdk/isteammatchmaking.h"
#include "steam_sdk/isteamgamecoordinator.h"


#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/cstrike15_gcmessages.pb.h"
#include "protobuf/econ_gcmessages.pb.h"
#include "protobuf/engine_gcmessages.pb.h"
#include "protobuf/gcsystemmsgs.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include "protobuf/netmessages.pb.h"
#include "protobuf/steammessages.pb.h"

#include "FoxSystem.h"

#include "CSX.h"
//#include "Utils_fox.h"


namespace ProfileChanger {
	extern bool enabled;
	extern int rank_id ;
	extern int wins;
	extern int cmd_friendly;
	extern int cmd_leader;
	extern int cmd_teaching;
	extern int level;
	extern int xp;
	extern int ban;
	extern int time ;

}

namespace MedalChanger {
	static bool enabled;
	static std::vector<uint32_t> medals;
	static bool equipped_medal_override;
	static uint32_t equipped_medal;
	static bool filter = false;
}

extern CSX::Hook::VTable ClientModeTable;
extern CSX::Hook::VTable SteamGameCoordinatorTable;
extern CSX::Hook::VTable SteamMatchMakingTable;
extern CSX::Hook::VTable SteamFriendsHook;

//extern IClientMode* g_pClientMode = nullptr;
//extern IBaseClientDLL* g_pClient = nullptr;
//extern ISteamGameCoordinator* g_pSteamGameCoordinator;

namespace SDK
{
	class Interfaces
	{
	private:
		static ISteamGameCoordinator* g_pSteamGameCoordinator;
		static ISteamMatchmaking* g_pSteamMatchmaking;
		static ISteamUser* g_pSteamUser;
		static ISteamFriends* g_pSteamFriends;

	public:
		static ISteamMatchmaking* SteamMatchmaking();
		static ISteamUser* SteamUser();
		static ISteamGameCoordinator* SteamGameCoordinator();
		static ISteamFriends* SteamFriends();
	};



	//ISteamUser* Interfaces::g_pSteamUser = nullptr;
}
