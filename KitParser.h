#pragma once

#include <algorithm>
#include <codecvt>
#include <vector>
#include <string>
#include "Utils_fox.h"
#include "ILocalize.hpp"

struct Kit_t
{
	int id;
	std::string name;

	auto operator < (const Kit_t& other) const -> bool
	{
		return name < other.name;
	}
};

extern std::vector<Kit_t> k_skins;
extern std::vector<Kit_t> k_gloves;
extern std::vector<Kit_t> k_stickers;

extern auto InitializeKits() -> void;



