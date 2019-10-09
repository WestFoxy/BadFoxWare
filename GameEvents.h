#pragma once
#include "MainInclude.hpp"
#include "GrenadesEsp.h"
#include "Bulletshow.h"

class FoxEventListener : public IGameEventListener2 {
public:
	FoxEventListener(const char *name) {
		g_GameEvents->AddListener(this, name, false);
	};

	~FoxEventListener() {
		g_GameEvents->RemoveListener(this);
	};

	virtual void FireGameEvent(IGameEvent* pEvent) {
		//int userid = pEvent->GetInt("userid");
		//int entid = pEvent->GetInt("entityid");
		float x = pEvent->GetFloat("x");
		float y = pEvent->GetFloat("y");
		float z = pEvent->GetFloat("z");

		Vector EventVec;
		EventVec.x = x;
		EventVec.y = y;
		EventVec.z = z;

		if(!strcmpi(pEvent->GetName(), "smokegrenade_detonate"))
			Smokes.emplace_back(x, y, z, 17.5f);
		else if(!strcmpi(pEvent->GetName(), "molotov_detonate"))
			Molotovs.emplace_back(x, y, z, 7.0f);
		else if (!strcmpi(pEvent->GetName(), "hegrenade_detonate"))
		{
			dlight_t* lpELight = g_pEffects->CL_AllocDlight(0);
			lpELight->color.b = 0;
			lpELight->color.g = 0;
			lpELight->color.r = 255;
			lpELight->color.exponent = 10.0f;
			lpELight->radius = 125;
			lpELight->decay = 15;
			lpELight->key = 0;
			lpELight->m_Direction = EventVec;
			lpELight->origin = EventVec;
			lpELight->die = g_GlobalVars->curtime + 20.0f;
		}
		//g_CVar->ConsoleColorPrintf(Color(255,0,0,255),"\n\n smoke_detonate:\n user_id: %d \n entity_id: %d \n %0.1f %0.1f %0.1f\n\n", userid, entid, x,y,z);

		
	};

	int GetEventDebugID() override {
		return EVENT_DEBUG_ID_INIT; // 42
	};
};

FoxEventListener* g_SmokeHitListener = nullptr;
FoxEventListener* g_MolotovHitListener = nullptr;
FoxEventListener* g_HeGrenadeListener = nullptr;