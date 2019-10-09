#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "MainInclude.hpp"

#include "imgui/imgui.h"

namespace ImGui
{
	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values);
}

class FoxMenu
{
private:
	ConVar* cl_mouseenable = 0;
	bool _visible = false;
	int _startX;
	int _startY;
public:
	IDirect3DTexture9* groupBox_tex = 0;
	IDirect3DTexture9* logo_tex = 0;
	IDirect3DTexture9* downshadow_tex = 0;
	IDirect3DTexture9* deagle_tex = 0;
	IDirect3DTexture9* scar20_tex = 0;
	IDirect3DTexture9* awp_tex = 0;
	IDirect3DTexture9* ak47_tex = 0;
	IDirect3DTexture9* ssg08_tex = 0;
	IDirect3DTexture9* ump_tex = 0;
	IDirect3DTexture9* glock_tex = 0;
	IDirect3DTexture9* redfox_tex = 0;

	ImFont* TahomaFont = nullptr;

	void Run();
	void Init(IDirect3DDevice9* dev);
	void OnDeviceLost();
	void OnDeviceReset();
	void Hide();
	void Show();
	void Toggle();
	bool IsVisible() const { return _visible; }
};

extern FoxMenu g_LisaMenu;