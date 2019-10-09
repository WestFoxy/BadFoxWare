
#include "panorama_resource.h"
#include "Utils_fox.h"

void panorama_resource_text::calc_hash()
{
	static const auto calc_hash_fn = Utils_fox.FindPattern("panorama.dll", "55 8B EC 53 8B D9 56 57 8B 7D 08 8B F2");

	int32_t hash = -1;
	int32_t size_data = this->size;
	const char* data = this->text;

	__asm
	{
		mov edx, data
		lea ecx, [hash]
		push size_data
		call calc_hash_fn
		add esp, 4
	};

	this->text_hash = ~hash;
}