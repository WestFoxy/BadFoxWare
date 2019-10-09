#define NOMINMAX

#include "XorStr.h"

#include <Windows.h>
#include "KitParser.h"
#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "MainInclude.hpp"
#include "Utils_fox.h"
#include <thread>
#include <chrono>
#include <fstream>

#include "hooks.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "hooks.hpp"
#include "panorama_resource.h"
#include "resource_system.h"
#include "AWMenu.h"
#include "FoxyMenu.h"
#include "SkinChanger.h"

vfunc_hook panorama_hook;
vfunc_hook panorama_hook2;

std::vector<std::string>mat_names;
std::vector<MaterialHandle_t>mat_handles;

typedef int(__fastcall* hkPanorama_t)(resource_system* thisptr, void* edx);
int __fastcall hkPanorama(resource_system* thisptr, void* edx)
{
	std::cout << "hkPanorama before\n";
	static auto oHkPanorama = panorama_hook.get_original<hkPanorama_t>(0);

	int res = oHkPanorama(thisptr, edx);

	std::cout << "hkPanorama\n";

	return res;
}
#include "resources_panorama.h"

typedef int(__fastcall* hkPanorama2_t)(void* ecx, void* edx,char* path, char ** xml_ptr,
	int a3, DWORD a4, DWORD a5);
auto FileReplacer(char * path, char ** xml_ptr, DWORD & size) -> bool {
	if (!strcmp(path, "panorama\\scripts\\mainmenu_news.js")) {
		/*std::stringstream ss;
		std::ifstream stream("D:\\mainmenu_news.js");

		ss << stream.rdbuf();*/

		size = sizeof(mainmenu_news_js);
		strcpy(*xml_ptr, mainmenu_news_js);
		//printf("replaced mainmenu_news.js\n");
	}
	if (!strcmp(path, "panorama\\styles\\mainmenu_news.css")) {
		/*std::stringstream ss;
		std::ifstream stream("D:\\mainmenu_news.css");

		ss << stream.rdbuf();*/

		size = sizeof(mainmenu_news_css);
		strcpy(*xml_ptr, mainmenu_news_css);
		//printf("replaced mainmenu_news.css\n");
	}
	if (!strcmp(path, "panorama\\scripts\\mainmenu.js")) {
		/*std::stringstream ss;
		std::ifstream stream("D:\\mainmenu.js");

		ss << stream.rdbuf();*/

		size = sizeof(mainmenu_js);
		strcpy(*xml_ptr, mainmenu_js);
		//printf("replaced mainmenu.js\n");
	}
	if (!strcmp(path, "panorama\\layout\\mainmenu.xml")) {
		std::stringstream ss;
		//std::ifstream stream("D:\\mainmenu.xml");

		ss << mainmenu_xml;

		size = ss.str().size();
		strcpy(*xml_ptr, ss.str().c_str());
		//printf("replaced characteranims.js\n");
	}
	//panorama\scripts\common\characteranims.js
	//panorama\scripts\mainmenu.js
	return false;
}
auto __fastcall hkReadFile(void* ecx, void* edx, char* path, char ** xml_ptr,
	int a3, DWORD a4, DWORD a5) -> bool {
	static auto ofunc = panorama_hook2.get_original<hkPanorama2_t>(0);


	auto &size = *(DWORD*)((DWORD)xml_ptr + 0x10);
	auto &size2 = *(DWORD*)((DWORD)xml_ptr + 0x1c);

	//Utils::ConsolePrint("%s\n", path);
	//printf("%s\n", path);

	auto ofunc_ret = ofunc(ecx, edx,path, xml_ptr, a3, a4, a5);
	
	auto hk_ret = FileReplacer(path, xml_ptr, size);

	size2 = size;

	return ofunc_ret || hk_ret;
}

void SetupPanoramaHook()
{
	bool module_found = false;
	while(!module_found)
	{
		__try
		{
			while (!GetModuleHandleA("panorama.dll"))
			{
				Sleep(5);
			}
			module_found = true;
			Sleep(700);
			bool called = false;
			void* v4;
			while (!called)
			{
				__try
				{

					void* dword_64A79558;
					do {
						dword_64A79558 = **(void***)(Utils::PatternScan(
							GetModuleHandleA("panorama.dll"),
							"55 8B EC 83 EC 30 83 65 FC FC 53 57 8B F9 C7 45 D0 00 00 00 00 8B 0D ?? ?? ?? ?? 66 C7 45 E4 00") + 23
							);
						Sleep(1);
					} while (!dword_64A79558);
					v4 = (void*)CallVFunction<int(__thiscall*)(void*)>((void*)dword_64A79558, 28)((void*)dword_64A79558);
					called = true;

				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{

				}
			}

			panorama_hook2.setup(v4);
			panorama_hook2.hook_index(0, hkReadFile);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{

		}
	}

	return;

	
}

FCUtils Utils_fox;
DWORD WINAPI OnDllAttach(LPVOID base)
{
	// 
	// Wait at most 10s for the main game modules to be loaded.
	// 


#ifdef _DEBUG
	Utils::AttachConsole();
#endif
	/*AllocConsole();
	AttachConsole(GetCurrentProcessId());
	auto _out = GetStdHandle(STD_OUTPUT_HANDLE);


	SetConsoleMode(_out,
		ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);


	freopen("CON", "w", stdout);*/
	//SetupPanoramaHook();


	if (Utils::WaitForModules(20000, { L"serverbrowser.dll" }))
	{
		return false;
	}
	try {


		printf("Fox framework started\n");
		
		//Utils::ConsolePrint("Initializing...\n");
		//Utils::ConsolePrint("aureware\n");
		Interfaces::Initialize();
		//Interfaces::Dump();

		NetvarSys::Get().Initialize();
		InputSys::Get().Initialize();
		//Menu::Get().Initialize();
		HitMarkerEvent::Get().RegisterSelf();
		BulletImpactEvent::Get().RegisterSelf();;
		
		InitializeKits();
		NetMngr::Get().init();
		NetMngr::Get().hookProp(XorStr("CBaseViewModel"), XorStr("m_nSequence"), Proxies::nSequence, o_nSequence);

		Hooks::Initialize();
		Beep(1000, 50);

		g_Options.knifemodel = 5;



		InputSys::Get().RegisterHotkey(VK_INSERT, [base]()//i like pause break but i changed to INS
			{

				g_LisaMenu.Toggle();
			});

		while (!g_Unload)
			Sleep(10000);

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

	}
	catch (const std::exception & ex) {

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}
}

BOOL WINAPI OnDllDetach()
{
#ifdef _DEBUG
    Utils::DetachConsole();
#endif
	HitMarkerEvent::Get().UnregisterSelf();
	BulletImpactEvent::Get().UnregisterSelf();

    Hooks::Shutdown();

    //Menu::Get().Shutdown();
    return TRUE;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        case DLL_PROCESS_DETACH:
            if(lpvReserved == nullptr)
                return OnDllDetach();
            return TRUE;
        default:
            return TRUE;
    }
}
