#include "Utils_fox.h"

std::string randomName(int length) {

	char consonents[] = { 'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z' };
	char vowels[] = { 'a','e','i','o','u','y' };

	std::string name = "";

	int random = rand() % 2;
	int count = 0;

	for (int i = 0; i < length; i++) {

		if (random < 2 && count < 2) {
			name = name + consonents[rand() % 19];
			count++;
		}
		else {
			name = name + vowels[rand() % 5];
			count = 0;
		}

		random = rand() % 2;

	}

	return name;

}


void HexDumpMemory(void *addr, int len)
{
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				printf("  %s\n", buff);

			// Output the offset.
			printf("  %04X ", i);
		}

		// Now the hex code for the specific character.
		printf(" %02X", pc[i]);
		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
			buff[i % 16] = '.';
		}
		else {
			buff[i % 16] = pc[i];
		}

		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf("  %s\n", buff);
}

KeyValues* KeyValues::FindKey(const char *keyName, bool bCreate)
{
	static auto key_values_find_key = reinterpret_cast<KeyValues*(__thiscall*)(void*, const char*, bool)>(Utils_fox.FindPattern("client.dll", "55 8B EC 83 EC 1C 53 8B D9 85 DB"));
	return key_values_find_key(this, keyName, bCreate);
}

void KeyValues::SetString(const char* keyName, const char* value)
{
	auto key = FindKey(keyName, true);
	if (key)
	{
		static auto key_values_set_string = reinterpret_cast<void(__thiscall*)(void*, const char*)>(Utils_fox.FindPattern("client.dll", "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01"));
		key_values_set_string(key, value);
	}
}
void KeyValues::InitKeyValues(const char* name)
{
	static auto key_values = reinterpret_cast<void(__thiscall*)(void*, const char*)>(Utils_fox.FindPattern("client.dll", "55 8B EC 51 33 C0 C7 45 ? ? ? ? ? 56 8B F1 81 26 ? ? ? ? C6 46 03 ? 89 46 10 89 46 18 89 46 14 89 46 1C 89 46 04 89 46 08 89 46 0C FF 15 ? ? ? ? 6A 01 FF 75 08 8D 4D FC 8B 10 51 8B C8 FF 52 24 8B 0E 33 4D FC 81 E1 ? ? ? ? 31 0E 88 46 03"));
	key_values(this, name);
}
void KeyValues::SetUint64(const char* keyName, int value, int value2)
{
	static auto key_values_set_uint64 = reinterpret_cast<void(__thiscall*)(void*, const char*, int, int)>(Utils_fox.FindPattern("client.dll", "55 8B EC 56 6A 01 FF 75 08"));
	key_values_set_uint64(this, keyName, value, value2);
}
const char* KeyValues::GetString(const char *keyName, const char *defaultValue)
{
	static auto key_values_get_string = reinterpret_cast<const char*(__thiscall*)(void*, const char*, const char*)>(Utils_fox.FindPattern("client.dll", "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08"));
	return key_values_get_string(this, keyName, defaultValue);
}
int KeyValues::GetInt(const char *keyName, int defaultValue)
{
	static auto key_values_get_int = reinterpret_cast<int(__thiscall*)(void*, const char*, int)>(Utils_fox.FindPattern("client.dll", "55 8B EC 6A ? FF 75 08 E8 ? ? ? ? 85 C0 74 45"));
	return key_values_get_int(this, keyName, defaultValue);
}
void KeyValues::SetInt(const char *keyName, int Value)
{
	auto key_int = FindKey(keyName, true);
	if (key_int)
	{
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(key_int) + 0xC) = Value;
		*reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(key_int) + 0x10) = 2;
	}
}



DWORD FCUtils::WaitOnModuleHandle(std::string moduleName)
{
	DWORD ModuleHandle = NULL;

	while (!ModuleHandle)
	{
		ModuleHandle = (DWORD)GetModuleHandle(moduleName.c_str());
		if (!ModuleHandle)
			Sleep(50);
	}

	return ModuleHandle;
}
bool FCUtils::bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
{
	for (; *szMask; ++szMask, ++Mask, ++Data)
		if (*szMask == 'x' && *Mask != *Data)
			return false;

	return (*szMask) == 0;
}

DWORD FCUtils::FindPatternMask(std::string moduleName, BYTE* Mask, const char* szMask)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;

	for (DWORD c = 0; c < Length; c += 1)
		if (bCompare((BYTE*)(Address + c), Mask, szMask))
			return (DWORD)(Address + c);


	return 0;
}

DWORD FCUtils::FindPattern(std::string moduleName, std::string pattern)
{
	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;    //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}


ULONG FCUtils::PatternSearch(std::string sModuleName, PBYTE pbPattern, std::string sMask, ULONG uCodeBase, ULONG uSizeOfCode)
{
	BOOL bPatternDidMatch = FALSE;
	HMODULE hModule = GetModuleHandle(sModuleName.c_str());

	if (!hModule)
		return 0x0;

	PIMAGE_DOS_HEADER pDsHeader = PIMAGE_DOS_HEADER(hModule);
	PIMAGE_NT_HEADERS pPeHeader = PIMAGE_NT_HEADERS(LONG(hModule) + pDsHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pPeHeader->OptionalHeader;

	if (uCodeBase == 0x0)
		uCodeBase = (ULONG)hModule + pOptionalHeader->BaseOfCode;

	if (uSizeOfCode == 0x0)
		uSizeOfCode = pOptionalHeader->SizeOfCode;

	ULONG uArraySize = sMask.length();

	if (!uCodeBase || !uSizeOfCode || !uArraySize)
		return 0x0;

	for (size_t i = uCodeBase; i <= uCodeBase + uSizeOfCode; i++)
	{
		for (size_t t = 0; t < uArraySize; t++)
		{
			if (*((PBYTE)i + t) == pbPattern[t] || sMask.c_str()[t] == '?')
				bPatternDidMatch = TRUE;

			else
			{
				bPatternDidMatch = FALSE;
				break;
			}
		}

		if (bPatternDidMatch)
			return i;
	}

	return 0x0;
}

//typedef void(*CL_FullUpdate_t) (void);

//CL_FullUpdate_t CL_FullUpdate = (CL_FullUpdate_t)FindPattern(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, /*A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85*/Xor<0x6F, 43, 0xCCC757C2>("\x2E\x41\x51\x4D\x53\x4B\x55\x49\x57\x47\x59\x38\x42\x5C\x42\x5E\x40\xA0\xBE\xA2\xBC\xA4\xB0\xB0\xA7\xCE\xCF\xAA\xBE\xBC\xAD\xBF\xBB\xB0\xA9\xD0\xB3\xA7\xA1\xB6\xAF\xAD" + 0xCCC757C2).s);

HMODULE FCUtils::GetModuleHandleSafe(const char* pszModuleName)
{
	HMODULE hmModuleHandle = nullptr;

	do
	{
		hmModuleHandle = GetModuleHandle(pszModuleName);
		Sleep(1);
	} while (hmModuleHandle == nullptr);

	return hmModuleHandle;
}