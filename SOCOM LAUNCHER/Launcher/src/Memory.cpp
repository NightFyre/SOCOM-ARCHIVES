#include "../include//Memory.hpp"

namespace SOCOMLauncher {

	Memory::Memory()
	{
		return;
	}

	void Memory::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
	{
		DWORD oldprotect;
		VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		WriteProcessMemory(hProcess, dst, src, size, nullptr);
		VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
		return;
	}

	void Memory::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
	{
		BYTE* nopArray = new BYTE[size];
		memset(nopArray, 0x90, size);

		PatchEx(dst, nopArray, size, hProcess);
		delete[] nopArray;
		return;
	}

	uintptr_t Memory::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
	{
		uintptr_t addr = ptr;
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
			addr += offsets[i];
		}
		return addr;
	}

	DWORD Memory::GetProcId(const wchar_t* procName)
	{
		DWORD procId = 0;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 procEntry;
			procEntry.dwSize = sizeof(procEntry);

			if (Process32First(hSnap, &procEntry))
			{
				do
				{
					if (!_wcsicmp(procEntry.szExeFile, procName))
					{
						procId = procEntry.th32ProcessID;
						break;
					}
				} while (Process32Next(hSnap, &procEntry));
			}
		}
		CloseHandle(hSnap);
		return procId;
	}

	uintptr_t Memory::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
	{
		uintptr_t modBaseAddr = 0;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPPROCESS, procId);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(modEntry);
			if (Module32First(hSnap, &modEntry))
			{
				do
				{
					if (!_wcsicmp(modEntry.szModule, modName))
					{
						modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnap, &modEntry));
			}
		}
		CloseHandle(hSnap);
		return modBaseAddr;
	}

	bool Memory::GetProcessList()
	{
		HANDLE hProcessSnap;
		HANDLE hProcess;
		PROCESSENTRY32 pe32;
		DWORD dwPriorityClass;

		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return FALSE;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32)) {
			CloseHandle(hProcessSnap);
			return FALSE;
		}

		//	Get all running processes
		do {
			std::wstring buf = pe32.szExeFile;
			std::string c(buf.begin(), buf.end());
			g_Console->ActiveProcesses.push_back(c);
		} while (Process32Next(hProcessSnap, &pe32));
		CloseHandle(hProcessSnap);
		return TRUE;
	}

	bool Memory::Inject(HANDLE Process, const char* Path, bool Sys)
	{
		bool buf;
		LPTHREAD_START_ROUTINE thread{};
		HMODULE hKernel32 = GetModuleHandleA("Kernel32");
		if (hKernel32 == NULL)
			return FALSE;

		void* addr = VirtualAllocEx(Process, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (addr == NULL)
			return FALSE;

		buf = WriteProcessMemory(Process, addr, Path, strlen(Path) + 1, NULL);
		//	FORK FOR [x86 | x64]
		switch (Sys) {
		case(TRUE):			//	x86	
			thread = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
			break;
		case(FALSE):		//	x64
			thread = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
			break;
		}

		HANDLE hThread = CreateRemoteThread(Process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, addr, NULL, NULL);
		if (!hThread) {
			buf = VirtualFreeEx(Process, addr, NULL, MEM_RELEASE);
			return FALSE;
		}

		CloseHandle(hThread);
		return TRUE;
	}

	bool Memory::customInjection(const wchar_t* ProcName, const char* Path, bool Sys)
	{
		g_LauncherVariables->proc_T.p_PID = g_Memory->GetProcId(ProcName);
		if (g_LauncherVariables->proc_T.p_PID == NULL) {
			MessageBoxA(NULL, "INJECTION FAILED\nCAUSE: (PID WAS NULL)", "ERROR", MB_ICONERROR);
			return FALSE;
		}

		g_LauncherVariables->proc_T.p_hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_LauncherVariables->proc_T.p_PID);
		if (g_LauncherVariables->proc_T.p_hProc == NULL) {
			MessageBoxA(NULL, "INJECTION FAILED\nCAUSE: (COULDN'T OBTAIN HANDLE)", "ERROR", MB_ICONERROR);
			return FALSE;
		}

		if (!g_Memory->Inject(g_LauncherVariables->proc_T.p_hProc, Path, Sys)) {
			MessageBoxA(NULL, "INJECTION FAILED\nCAUSE: (INJECTION ERROR)", "ERROR", MB_ICONERROR);
			return FALSE;
		}
		return TRUE;
	}
}