#include "SDK_lobby.h"


CSX::Hook::VTable ClientModeTable;
CSX::Hook::VTable SteamGameCoordinatorTable;
CSX::Hook::VTable SteamMatchMakingTable;
CSX::Hook::VTable SteamFriendsHook;


namespace ProfileChanger {
	bool enabled = true;
	int rank_id = 17;
	 int wins = 999666;
	 int cmd_friendly = 66666666;
	 int cmd_leader = 66666666;
	 int cmd_teaching = 66666666;
	 int level = 37;
	 int xp = 3694;
	 int ban = 0;
	 int time = 0;

}


//IClientMode* g_pClientMode = nullptr;
//IBaseClientDLL* g_pClient = nullptr;
//ISteamGameCoordinator* g_pSteamGameCoordinator = nullptr;


namespace SDK
{
	ISteamGameCoordinator* Interfaces::g_pSteamGameCoordinator = nullptr;
	ISteamUser* Interfaces::g_pSteamUser = nullptr;
	ISteamMatchmaking* Interfaces::g_pSteamMatchmaking = nullptr;
	ISteamFriends* Interfaces::g_pSteamFriends = nullptr;
	ISteamUser* Interfaces::SteamUser()
	{
		if (!Interfaces::g_pSteamUser) {
			SteamGameCoordinator();
		}

		return Interfaces::g_pSteamUser;
	}

	ISteamGameCoordinator* Interfaces::SteamGameCoordinator()
	{
		if (!g_pSteamGameCoordinator) {
			typedef uint32_t SteamPipeHandle;
			typedef uint32_t SteamUserHandle;

			SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
			SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();

			auto SteamClient = ((ISteamClient*(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();

			auto SteamHTTP = SteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, "STEAMHTTP_INTERFACE_VERSION002");
			g_pSteamUser = SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
			auto SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
			auto SteamInventory = SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
			g_pSteamGameCoordinator = (ISteamGameCoordinator*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
		}

		return g_pSteamGameCoordinator;
	}

	ISteamFriends * Interfaces::SteamFriends()
	{
		if (!g_pSteamFriends) {
			typedef uint32_t SteamPipeHandle;
			typedef uint32_t SteamUserHandle;

			SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
			SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();

			auto SteamClient = ((ISteamClient*(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();

			//auto SteamHTTP = SteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, "STEAMHTTP_INTERFACE_VERSION002");
			//g_pSteamUser = SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
			auto SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
			//auto SteamInventory = SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
			//g_pSteamMatchmaking = (ISteamMatchmaking*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamMatchMaking009");
			g_pSteamFriends = SteamFriends;
		}

		return g_pSteamFriends;
	}

	//SteamMatchMaking009
	ISteamMatchmaking* Interfaces::SteamMatchmaking()
	{
		if (!g_pSteamMatchmaking) {
			typedef uint32_t SteamPipeHandle;
			typedef uint32_t SteamUserHandle;

			SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
			SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();

			auto SteamClient = ((ISteamClient*(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();

			auto SteamHTTP = SteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, "STEAMHTTP_INTERFACE_VERSION002");
			g_pSteamUser = SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
			auto SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
			auto SteamInventory = SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
			g_pSteamMatchmaking = (ISteamMatchmaking*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamMatchMaking009");
		}

		return g_pSteamMatchmaking;
	}


	
}