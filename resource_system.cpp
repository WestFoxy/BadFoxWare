#include "resource_system.h"
#include "MainInclude.hpp"
int16_t resource_system::check_hash(int16_t hash)
{
	/*IDA*/
	auto pthis = reinterpret_cast<uintptr_t>(this) + 0x3C;
	auto result = *(DWORD*)(pthis + 0x10);
	if (result != -1)
	{
		auto v3 = *(DWORD *)(pthis + 4);
		auto v4 = hash;
		do
		{
			auto v5 = *(WORD*)(v3 + 24 * result + 0x10);
			if (v5 <= (unsigned __int16)v4)
			{
				if (v5 >= (unsigned __int16)v4)
					return result;
				result = *(DWORD*)(v3 + 24 * result + 4);
			}
			else
			{
				result = *(DWORD*)(v3 + 24 * result);
			}
		} while (result != -1);
	}
	return result;
}

panorama_resource* resource_system::find_resource_by_name(const char* name, int64_t type)
{
	return vfunc<panorama_resource*(__thiscall*)(void*, const char*, int64_t)>(this, 7)(this, name, type);
}

void resource_system::add_resource_xml_js_css(const char* name, const char* text, int32_t length)
{
	static const auto get_res_type_fn = reinterpret_cast<int64_t(__thiscall*)(const char*)>(Utils_fox.FindPattern("panorama.dll", "55 8B EC 81 EC ? ? ? ? 53 56 57 85 C9"));
	static const auto register_get_resource_fn = reinterpret_cast<void(__thiscall*)(uintptr_t, int16_t*, const char*)>(Utils_fox.FindPattern("panorama.dll", "55 8B EC 83 EC 0C 53 8B 5D 0C"));
	static const auto add_resource_fn = reinterpret_cast<void(__thiscall*)(uintptr_t, int16_t*, panorama_resource**)>(Utils_fox.FindPattern("panorama.dll", "55 8B EC 83 EC 0C 8B 45 08 53 56 57 66 8B 18 8B F1 8B 45 0C 66 89 5D F4 C7 45 ? ? ? ? ? C6 45 08 00 8B 00 89 45 FC 89 45 F8 8D 45 08 50 8D 45 0C 50 8D 45 F4 50 E8 ? ? ? ? 8B CE E8 ? ? ? ? FF 75 08 8B F8"));
	static const auto unkcall_fn = reinterpret_cast<void(__thiscall*)(uintptr_t, panorama_resource**, int16_t*)>(Utils_fox.FindPattern("panorama.dll", "55 8B EC 51 53 56 8B 75 08 57 8B F9"));

	int16_t hash = -1;
	int64_t type = get_res_type_fn(name);

	panorama_resource_text* data = reinterpret_cast<panorama_resource_text*>(g_MemAlloc->Alloc(16));
	panorama_resource* res = reinterpret_cast<panorama_resource*>(g_MemAlloc->Alloc(24));

	auto text_data = g_MemAlloc->Alloc(length);
	strcpy(reinterpret_cast<char*>(text_data), text);

	data->text = reinterpret_cast<const char*>(text_data);
	data->size = length;
	data->pad2 = 0;
	data->calc_hash();

	memcpy(res->resource_type, &type, 4);
	res->resource_type_id = *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(&type) + 0x4);
	res->data = data;
	res->pad = 0;
	res->pad2 = 0;
	res->pad3 = 0;

	register_get_resource_fn(reinterpret_cast<uintptr_t>(this) + 0x4, &hash, name);
	auto fixed_hash = this->check_hash(hash);

	if (fixed_hash == -1)
		add_resource_fn(reinterpret_cast<uintptr_t>(this) + 0x3C, &hash, &res);
	else
		*reinterpret_cast<panorama_resource**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(this) + 0x40) + 24 * fixed_hash + 0x14) = res;

	unkcall_fn(reinterpret_cast<uintptr_t>(this) + 0x60, &res, &hash);
}