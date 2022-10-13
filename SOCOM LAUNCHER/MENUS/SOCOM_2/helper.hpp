#pragma once
//	Windows API
#include <windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

//	DX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

//	Imgui
#include "../../libs/ImGui/imgui.h"
#include "../../libs/ImGui/imgui_internal.h"
#include "../../libs/ImGui/ImGui_Impl_dx11.h"
#include "../../libs/ImGui/imgui_impl_win32.h"
 
//	String Association
#include <tchar.h>
#include <string>
#include <thread>
#include <chrono>

//	Others
#include <vector>
#include "../../libs/Minhook/MinHook.h"

namespace SOCOM2 {

	using namespace std::chrono_literals;
	inline HMODULE g_hModule{};
	inline LPCWSTR g_ModuleName{};
	inline std::atomic_bool g_Running = TRUE;
	inline std::atomic_bool g_Killswitch = FALSE;

	static uintptr_t dwGameBase = (uintptr_t)GetModuleHandle(g_ModuleName);
	static uintptr_t dwEEMem = (uintptr_t)GetProcAddress(g_hModule, "EEmem");


	//	Draw Declaration
	//auto gi_Draw = ImGui::GetForegroundDrawList();
	
	struct Vector2 {
		float x, y;
	};

	struct Vector3 {
		float x, y, z;
	};

	struct Vector4 {
		float x, y, z, w;
	};
}