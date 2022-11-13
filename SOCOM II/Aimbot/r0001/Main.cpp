// More detailed information can be in the linked header file
#include "Helper.h"

//  MAIN THREAD
DWORD WINAPI iniThread(HMODULE hModule)
{
    //  Init Console Framework
    initFrameWork();

    //  ESTABLISH PROC VARIABLES
    Process::moduleBase = (uintptr_t)GetModuleHandleA(NULL);
    sealOBJECT* sealObject = (sealOBJECT*)(mem::offsetPS2MEM(Offsets::PlayerObjectPTR));

    //  Main Hack Loop
    while (true)
    {
        //  Console Menu
        if (!Vars::bMENU)
            UpdateMenu();

        //  EXIT
        if (GetAsyncKeyState(VK_END) & 1) break;

        //  Render Fix + Pro Mod
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            Vars::bRFIX = !Vars::bRFIX;
            if (Vars::bRFIX)
                ProMod(TRUE);
            else
                ProMod(FALSE);
        }

        //  AIMBOT
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            Vars::bAIMBOT = !Vars::bAIMBOT;
            if (Vars::bAIMBOT)
                AimBot(TRUE);
            else
                AimBot(FALSE);
        }

        // LOOPS
        if (Vars::bRFIX)
            Brightness();
    }

    //  CLEANUP and EJECT
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

// DLL ATTACH
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)iniThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}