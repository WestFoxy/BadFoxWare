#pragma once

#include "SDK_lobby.h"
#include "Utils_fox.h"
#include <inttypes.h>

void RenderLobby();
EGCResults __fastcall Hook_RetrieveMessage(void* ecx, void* edx, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize);
void __fastcall Hook_SendLobbyChatMsg(void* ecx, void* edx, CSteamID steamIDLobby, void *pvMsgBody, int cubMsgBody);
void SetupLobbyHooks();

class CFoxSystem
{
public:
	void PostRetrieveMessage(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);

};

inline void SetClantag(const char *tag)
{

	static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))Utils_fox.FindPattern("engine.dll", "53 56 57 8B DA 8B F9 FF 15");
	//if (!fnClantagChanged)
	//	return;

	fnClantagChanged(tag, tag);
}

extern CFoxSystem FoxSystem;
extern CSteamID steam_id_global;
extern uint64 local_player_xiud;


extern std::vector <CSteamID>friends;

struct LobbyPlayer
{

	uint64 xuid;
};
extern std::vector<LobbyPlayer> players;