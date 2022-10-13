#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define DEVELOPER 0
#include <windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <memory>
#include <iostream>
#include <tchar.h>
#include <string>
#include <thread>
#include <chrono>

#include <vector>
#include <shobjidl.h>	//	Open File Dialogue
#include <fstream>
#include <comdef.h> // Authenticate function string handler
#include <filesystem>

//	Internet
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#include <Wbemidl.h>
//#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "wbemuuid.lib")

//	DirectX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

//	ImGui
#include "../libs/ImGui(Docking)/imgui.h"
#include "../libs/ImGui(Docking)/imgui_internal.h"
#include "../libs/ImGui(Docking)/imgui_impl_win32.h"
#include "../libs/ImGui(Docking)/imgui_impl_dx11.h"
#include "../libs/StringEncryption/xorstr.hpp"

namespace SOCOMLauncher {
	using namespace std::chrono_literals;

	inline HMODULE g_Module{};
	inline LPCWSTR g_ModuleName{};
	inline std::atomic_bool g_Running = TRUE;
	static uintptr_t g_ModuleBase = (uintptr_t)GetModuleHandle(NULL);
}