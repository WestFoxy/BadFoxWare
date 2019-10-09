#include "valve_sdk/csgostructs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>
void UnlockCursor()
{
	typedef void(__thiscall* OriginalFn)(PVOID);
	return CallVFunction<OriginalFn>(this, 66)(this);
}