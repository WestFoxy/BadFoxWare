#pragma once
#include "../MainInclude.hpp"
class CPanel
{
public:
	const char* GetName(unsigned int Panel)
	{
		typedef const char*(__thiscall* Fn)(void*, unsigned int);
		return CallVFunction<Fn>(this, 36)(this, Panel);
	}

};