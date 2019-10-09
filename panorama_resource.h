#pragma once
#include "Utils_fox.h"

struct panorama_resource_text
{
public:
	int32_t text_hash;
	const char* text;
	uint32_t size;
	int32_t pad2;

	void calc_hash();
};

class panorama_resource
{
public:
	panorama_resource_text* data;
	int32_t pad;
	char resource_type[4];
	int32_t resource_type_id;
	int32_t pad2;
	int32_t pad3;
};