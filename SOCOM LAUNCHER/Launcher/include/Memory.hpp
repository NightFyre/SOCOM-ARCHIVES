#pragma once
#include "../Helper.h"
#include "../resource.h"
#include "Launcher.hpp"

#define RELOC_FLAG32(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG64(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

#ifdef _WIN64
#define RELOC_FLAG RELOC_FLAG64
#else
#define RELOC_FLAG RELOC_FLAG32
#endif

namespace SOCOMLauncher {
	// Broihon Manual Map Method with Embedded Resource
	using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibFilename);
	using f_GetProcAddress = UINT_PTR(WINAPI*)(HINSTANCE hModule, const char* lpProcName);
	using f_DLL_ENTRY_POINT = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);
	struct MANUAL_MAPPING_DATA
	{
		f_LoadLibraryA		pLoadLibraryA;
		f_GetProcAddress	pGetProcAddress;
		HINSTANCE			hMod;
	};

	class Memory
	{
	public:


		explicit Memory();
		~Memory() noexcept = default;
		Memory(Memory const&) = delete;
		Memory(Memory&&) = delete;
		Memory& operator=(Memory const&) = delete;
		Memory& operator=(Memory&&) = delete;

		// GuidedHacking GameHacking Bible Part 1
		bool GetProcessList();
		DWORD GetProcId(const wchar_t* procName);
		void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
		void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
		uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
		uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

		// GuidedHAcking GameHacking Bible Part 2-3
		bool Inject(HANDLE Process, const char* Path, bool Sys = FALSE);
		bool customInjection(const wchar_t* ProcName, const char* Path, bool Sys = FALSE);
		bool ManualMap(HANDLE hProc, int GAME);
	};
	inline std::unique_ptr<Memory> g_Memory;
}