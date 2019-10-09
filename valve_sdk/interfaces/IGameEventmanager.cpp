
#include "IGameEventmanager.hpp"
#include "../sdk.hpp"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#define EVENT_HOOK(x)
#define TICK_INTERVAL (g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / TICK_INTERVAL))





