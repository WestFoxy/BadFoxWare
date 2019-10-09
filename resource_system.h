#pragma once
#include "panorama_resource.h"
#include "Utils_fox.h"

class resource_system
{
public:
	int16_t check_hash(int16_t hash);
	panorama_resource * find_resource_by_name(const char* name, int64_t type = 0L);
	void add_resource_xml_js_css(const char* name, const char* text, int32_t length);
};