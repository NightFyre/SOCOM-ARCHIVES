#pragma once
#include "../Helper.h"
#include "Injector.h"
#include "Launcher.hpp"

namespace SOCOMLauncher {
	class Memory
	{
	public:

		explicit Memory();
		~Memory() noexcept = default;
		Memory(Memory const&) = delete;
		Memory(Memory&&) = delete;
		Memory& operator=(Memory const&) = delete;
		Memory& operator=(Memory&&) = delete;

		bool GetProcessList();
		DWORD GetProcId(const wchar_t* procName);
		void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
		bool Inject(HANDLE Process, const char* Path, bool Sys = FALSE);
		void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
		bool customInjection(const wchar_t* ProcName, const char* Path, bool Sys = FALSE);
		uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
		uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	};
	inline std::unique_ptr<Memory> g_Memory;
}