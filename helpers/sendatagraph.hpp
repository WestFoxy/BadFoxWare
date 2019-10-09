#include "../MainInclude.hpp"
#include "../LagComp.h"
#include "../features/FakeLag.hpp"
#include "../hooks.hpp"
#include <intrin.h>  
#include <thread>
#include <mutex>
#include <iostream>
#include "../features/FakeLag.hpp"
//-------------------------------------------\\
#pragma once
#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2	