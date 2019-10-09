#pragma once

#include <Windows.h>
#include <string>
#include <Psapi.h>
#include <vector>
#include <string>



#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "libprotoc.lib")

#define INRANGE(x, a, b)	(x >= a && x <= b)
#define GETBITS(x)			(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GETBYTE(x)			(GETBITS(x[0]) << 4 | GETBITS(x[1]))

#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#undef KEY
#undef BUFLEN

void HexDumpMemory(void *addr, int len);

std::string randomName(int length);

template <int XORSTART, int BUFLEN, int XREFKILLER>
class Xor
{
public:
	char s[BUFLEN];
	Xor(const char* xs);
#ifndef DEBUG_OPTION
	~Xor() { for (int i = 0; i < BUFLEN; i++)s[i] = 0; } // clear string from stack
#endif

private:
	Xor();
};

template <int XORSTART, int BUFLEN, int XREFKILLER>
Xor<XORSTART, BUFLEN, XREFKILLER>::Xor(const char* xs)
{
	int xvalue = XORSTART;
	int i = 0;
	for (; i < (BUFLEN - 1); i++) {
		s[i] = xs[i - XREFKILLER] ^ xvalue;
		xvalue += 1;
		xvalue %= 256;
	}
	s[BUFLEN - 1] = 0;
}


class FCUtils
{
public:
	DWORD WaitOnModuleHandle(std::string moduleName);
	bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask);
	DWORD FindPatternMask(std::string moduleName, BYTE* Mask, const char* szMask);
	DWORD FindPattern(std::string moduleName, std::string pattern);
	ULONG PatternSearch(std::string sModuleName, PBYTE pbPattern, std::string sMask, ULONG uCodeBase, ULONG uSizeOfCode);
	HMODULE GetModuleHandleSafe(const char* pszModuleName);
};

extern FCUtils Utils_fox;


template<typename T>
__forceinline static T vfunc(void *base, int index)
{
	DWORD *vTabella = *(DWORD**)base;
	return (T)vTabella[index];
}



typedef PVOID(__cdecl* oKeyValuesSystem)();

class KeyValues
{
public:
	PVOID operator new(size_t iAllocSize)
	{
		static oKeyValuesSystem KeyValuesSystemFn = (oKeyValuesSystem)GetProcAddress(GetModuleHandle("vstdlib.dll"), "KeyValuesSystem");
		auto KeyValuesSystem = KeyValuesSystemFn();

		typedef PVOID(__thiscall* oAllocKeyValuesMemory)(PVOID, int);
		return vfunc<oAllocKeyValuesMemory>(KeyValuesSystem, 1)(KeyValuesSystem, iAllocSize);
	}

	void operator delete(PVOID pMem)
	{
		static oKeyValuesSystem KeyValuesSystemFn = (oKeyValuesSystem)GetProcAddress(GetModuleHandle("vstdlib.dll"), "KeyValuesSystem");
		auto KeyValuesSystem = KeyValuesSystemFn();

		typedef void(__thiscall* oFreeKeyValuesMemory)(PVOID, PVOID);
		vfunc<oFreeKeyValuesMemory>(KeyValuesSystem, 2)(KeyValuesSystem, pMem);
	}

	const char* GetName()
	{
		static oKeyValuesSystem KeyValuesSystemFn = (oKeyValuesSystem)GetProcAddress(GetModuleHandle("vstdlib.dll"), "KeyValuesSystem");
		auto KeyValuesSystem = KeyValuesSystemFn();

		auto a2 = (DWORD)this;

		typedef const char*(__thiscall* oGetName)(PVOID, int);
		return vfunc<oGetName>(KeyValuesSystem, 4)(KeyValuesSystem, *(BYTE*)(a2 + 3) | (*(WORD*)(a2 + 18) << 8));
	}

	KeyValues* FindKey(const char *keyName, bool bCreate = false);
	void SetString(const char* keyName, const char* value);
	void InitKeyValues(const char* name);
	void SetUint64(const char* keyName, int value, int value2);
	const char* GetString(const char *keyName, const char *defaultValue);
	int GetInt(const char *keyName, int defaultValue);
	void SetInt(const char *keyName, int Value);
};



namespace KeyValues_Func
{
	inline void KeyValues_SetString(KeyValues* key, const char* value)
	{
		auto key_values_set_string = reinterpret_cast<void(__thiscall*)(void*, const char*)>(Utils_fox.FindPatternMask("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\xA1\x00\x00\x00\x00\x53\x56\x57\x8B\xF9\x8B\x08\x8B\x01", "xxxx????xxxxxxxxx"));
		key_values_set_string(key, value);
	}

	inline void GameFunc_InitKeyValues(KeyValues* key, const char* name)
	{
		auto key_values = reinterpret_cast<void(__thiscall*)(void*, const char*)>(Utils_fox.FindPatternMask("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x51\x33\xC0\xC7\x45\x00\x00\x00\x00\x00\x56\x8B\xF1\x81\x26\x00\x00\x00\x00\xC6\x46\x03\x00\x89\x46\x10\x89\x46\x18\x89\x46\x14\x89\x46\x1C\x89\x46\x04\x89\x46\x08\x89\x46\x0C\xFF\x15\x00\x00\x00\x00\x6A\x01\xFF\x75\x08\x8D\x4D\xFC\x8B\x10\x51\x8B\xC8\xFF\x52\x24\x8B\x0E\x33\x4D\xFC\x81\xE1\x00\x00\x00\x00\x31\x0E\x88\x46\x03", "xxxxxxxx?????xxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxx????xxxxx"));
		key_values(key, name);
	}

	inline void KeyValues_SetUint64(KeyValues* key, const char* keyName, int value, int value2)
	{
		auto key_values_set_uint64 = reinterpret_cast<void(__thiscall*)(void*, const char*, int, int)>(Utils_fox.FindPatternMask("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x56\x6A\x01\xFF\x75\x08", "xxxxxxxxx"));
		key_values_set_uint64(key, keyName, value, value2);
	}

	inline const char* KeyValues_GetString(KeyValues* key, const char *keyName, const char *defaultValue)
	{
		auto key_values_get_string = reinterpret_cast<const char*(__thiscall*)(void*, const char*, const char*)>(Utils_fox.FindPatternMask("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xE4\xC0\x81\xEC\x00\x00\x00\x00\x53\x8B\x5D\x08", "xxxxxxxx????xxxx"));
		return key_values_get_string(key, keyName, defaultValue);
	}

	inline int KeyValues_GetInt(KeyValues* key, const char *keyName, int defaultValue)
	{
		auto key_values_get_int = reinterpret_cast<int(__thiscall*)(void*, const char*, int)>(Utils_fox.FindPatternMask("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x6A\x00\xFF\x75\x08\xE8\x00\x00\x00\x00\x85\xC0\x74\x45", "xxxxxxxxx????xxxx"));
		return key_values_get_int(key, keyName, defaultValue);
	}

	inline KeyValues* KeyValues_FindKey(KeyValues* key, const char *keyName, bool bCreate = false)
	{
		auto key_values_find_key = reinterpret_cast<KeyValues*(__thiscall*)(void*, const char*, bool)>(Utils_fox.FindPatternMask("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x1C\x53\x8B\xD9\x85\xDB", "xxxxxxxxxxx"));
		return key_values_find_key(key, keyName, bCreate);
	}

	inline void KeyValues_SetInt(KeyValues* key, const char *keyName, int Value)
	{
		auto key_int = KeyValues_FindKey(key, keyName);
		if (key_int)
		{
			*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(key_int) + 0xC) = Value;
			*reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(key_int) + 0x10) = 2;
		}
	}
}



class PlayerLocal
{
public:
	int GetXUIDLow()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	int GetXUIDHigh()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0xC);
	}

	const char* GetName()
	{
		typedef const char*(__thiscall* tPaster)(void*);
		return vfunc<tPaster>(this, 2)(this);
	}
};

class PlayerManager
{
public:
	PlayerLocal* GetLocalPlayer(int un)
	{
		typedef PlayerLocal*(__thiscall* tTo)(void*, int);
		return vfunc<tTo>(this, 1)(this, un);
	}
};

class CMatchSystem
{
public:
	PlayerManager* GetPlayerManager()
	{
		typedef PlayerManager*(__thiscall* tKapis)(void*);
		return vfunc<tKapis>(this, 0)(this);
	}
};

class CMatchSessionOnlineHost;

class CMatchFramework
{
public:
	/*CMatchEventsSubscription* GetEventsSubscription()
	{
		typedef CMatchEventsSubscription*(__thiscall* tGetEventsSubscription)(void*);
		return call_vfunc<tGetEventsSubscription>(this, 11)(this);
	}*/

	CMatchSessionOnlineHost* GetMatchSession()
	{
		typedef CMatchSessionOnlineHost*(__thiscall* tGetMatchSession)(void*);
		return vfunc<tGetMatchSession>(this, 13)(this);
	}

	CMatchSystem* GetMatchSystem()
	{
		typedef CMatchSystem*(__thiscall* tGetMatchSystem)(void*);
		return vfunc<tGetMatchSystem>(this, 15)(this);
	}
};


class CMatchSessionOnlineHost
{
public:

	void Command(KeyValues *vl)
	{
		typedef void(__thiscall* FoxFox)(void*, KeyValues*);
		vfunc<FoxFox>(this, 3)(this, vl);

		return;
	}
	KeyValues* GetSessionSettings()
	{
		typedef KeyValues*(__thiscall* tGetSessionSettings)(void*);
		return vfunc<tGetSessionSettings>(this, 1)(this);
	}
	void UpdateSessionSettings(KeyValues* packet)
	{
		typedef void(__thiscall* tUpdateSessionSettings)(void*, KeyValues*);
		vfunc<tUpdateSessionSettings>(this, 2)(this, packet);
	}
};

extern CMatchFramework *g_MatchFramework;

