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

	void LoadFileInResource(int name, int type, DWORD& size, const char*& data)
	{
		HMODULE handle = ::GetModuleHandle(NULL);
		HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name),
			MAKEINTRESOURCE(type));
		HGLOBAL rcData = ::LoadResource(handle, rc);
		size = ::SizeofResource(handle, rc);
		data = static_cast<const char*>(::LockResource(rcData));
	}
	
	void __stdcall Shellcode(MANUAL_MAPPING_DATA* pData);
	bool Memory::ManualMap(HANDLE hProc, int GAME)
	{
		DWORD Size = 0;
		const char* rdata = NULL;
		BYTE* pSrcData = nullptr;
		IMAGE_NT_HEADERS* pOldNtHeader = nullptr;
		IMAGE_OPTIONAL_HEADER* pOldOptHeader = nullptr;
		IMAGE_FILE_HEADER* pOldFileHeader = nullptr;
		BYTE* pTargetBase = nullptr;

		switch (GAME) {
		case(0): LoadFileInResource(IDR_SOCOM, inSOCOM, Size, rdata); break;
		case(1): LoadFileInResource(IDR_SOCOM2, inSOCOM2, Size, rdata); break;
		case(2): LoadFileInResource(IDR_SOCOM3, inSOCOM3, Size, rdata); break;
		case(3): LoadFileInResource(IDR_SOCOMCA, inSOCOMCA, Size, rdata); break;
		case(4): return FALSE;
		}



		pSrcData = new BYTE[static_cast<UINT_PTR>(Size)];
		::memcpy(pSrcData, rdata, Size);

		if (reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_magic != 0x5A4D) //"MZ"
		{
			delete[] pSrcData;
			return false;
		}

		pOldNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(pSrcData + reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_lfanew);
		pOldOptHeader = &pOldNtHeader->OptionalHeader;
		pOldFileHeader = &pOldNtHeader->FileHeader;

#ifdef _WIN64
		if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_AMD64)
		{
			delete[] pSrcData;
			return false;
		}
#else
		if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_I386)
		{
			delete[] pSrcData;
			return false;
		}
#endif

		pTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, reinterpret_cast<void*>(pOldOptHeader->ImageBase), pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
		if (!pTargetBase)
		{
			pTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, nullptr, pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
			if (!pTargetBase)
			{
				delete[] pSrcData;
				return false;
			}
		}

		MANUAL_MAPPING_DATA data{ 0 };
		data.pLoadLibraryA = LoadLibraryA;
		data.pGetProcAddress = reinterpret_cast<f_GetProcAddress>(GetProcAddress);

		auto* pSectionHeader = IMAGE_FIRST_SECTION(pOldNtHeader);
		for (UINT i = 0; i != pOldFileHeader->NumberOfSections; ++i, ++pSectionHeader)
		{
			if (pSectionHeader->SizeOfRawData)
			{
				if (!WriteProcessMemory(hProc, pTargetBase + pSectionHeader->VirtualAddress, pSrcData + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData, nullptr))
				{
					delete[] pSrcData;
					VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
					return false;
				}
			}
		}

		memcpy(pSrcData, &data, sizeof(data));
		WriteProcessMemory(hProc, pTargetBase, pSrcData, 0x1000, nullptr);

		delete[] pSrcData;

		void* pShellcode = VirtualAllocEx(hProc, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!pShellcode)
		{
			VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
			return false;
		}

		WriteProcessMemory(hProc, pShellcode, Shellcode, 0x1000, nullptr);

		HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(pShellcode), pTargetBase, 0, nullptr);
		if (!hThread)
		{
			VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
			VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);
			return false;
		}

		CloseHandle(hThread);

		HINSTANCE hCheck = NULL;
		while (!hCheck)
		{
			MANUAL_MAPPING_DATA data_checked{ 0 };
			ReadProcessMemory(hProc, pTargetBase, &data_checked, sizeof(data_checked), nullptr);
			hCheck = data_checked.hMod;
			Sleep(10);
		}

		VirtualFreeEx(hProc, pShellcode, 0, MEM_RELEASE);

		return true;
	}



	void __stdcall Shellcode(MANUAL_MAPPING_DATA* pData)
	{
		if (!pData)
			return;

		BYTE* pBase = reinterpret_cast<BYTE*>(pData);
		auto* pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pData)->e_lfanew)->OptionalHeader;

		auto _LoadLibraryA = pData->pLoadLibraryA;
		auto _GetProcAddress = pData->pGetProcAddress;
		auto _DllMain = reinterpret_cast<f_DLL_ENTRY_POINT>(pBase + pOpt->AddressOfEntryPoint);

		BYTE* LocationDelta = pBase - pOpt->ImageBase;
		if (LocationDelta)
		{
			if (!pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
				return;

			auto* pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			while (pRelocData->VirtualAddress)
			{
				UINT AmountOfEntries = (pRelocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
				WORD* pRelativeInfo = reinterpret_cast<WORD*>(pRelocData + 1);

				for (UINT i = 0; i != AmountOfEntries; ++i, ++pRelativeInfo)
				{
					if (RELOC_FLAG(*pRelativeInfo))
					{
						UINT_PTR* pPatch = reinterpret_cast<UINT_PTR*>(pBase + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
						*pPatch += reinterpret_cast<UINT_PTR>(LocationDelta);
					}
				}
				pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(pRelocData) + pRelocData->SizeOfBlock);
			}
		}

		if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
		{
			auto* pImportDescr = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
			while (pImportDescr->Name)
			{
				char* szMod = reinterpret_cast<char*>(pBase + pImportDescr->Name);
				HINSTANCE hDll = _LoadLibraryA(szMod);

				ULONG_PTR* pThunkRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->OriginalFirstThunk);
				ULONG_PTR* pFuncRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->FirstThunk);

				if (!pThunkRef)
					pThunkRef = pFuncRef;

				for (; *pThunkRef; ++pThunkRef, ++pFuncRef)
				{
					if (IMAGE_SNAP_BY_ORDINAL(*pThunkRef))
					{
						*pFuncRef = _GetProcAddress(hDll, reinterpret_cast<char*>(*pThunkRef & 0xFFFF));
					}
					else
					{
						auto* pImport = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pBase + (*pThunkRef));
						*pFuncRef = _GetProcAddress(hDll, pImport->Name);
					}
				}
				++pImportDescr;
			}
		}

		if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
		{
			auto* pTLS = reinterpret_cast<IMAGE_TLS_DIRECTORY*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
			auto* pCallback = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(pTLS->AddressOfCallBacks);
			for (; pCallback && *pCallback; ++pCallback)
				(*pCallback)(pBase, DLL_PROCESS_ATTACH, nullptr);
		}

		_DllMain(pBase, DLL_PROCESS_ATTACH, nullptr);

		pData->hMod = reinterpret_cast<HINSTANCE>(pBase);
	}
}