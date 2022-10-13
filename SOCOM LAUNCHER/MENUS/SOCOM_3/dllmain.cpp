// dllmain.cpp : Defines the entry point for the DLL application.
#include "helper.h"
#include "include/Console.hpp"
#include "include/Game.hpp"
#include "include/D3D11Window.hpp"
#include "include/Hooking.hpp"

void BackgroundWorker()
{
    using namespace SOCOM3;
    while (g_Running) {
        g_Menu->Loops();
        std::this_thread::sleep_for(1ms);
    }
    return;
}

DWORD WINAPI MainThread()
{
    using namespace SOCOM3;
    g_Console = std::make_unique<Console>();

    //	INIT INFO
    g_Console->InitializeConsole("SOCOM 3 - INTERNAL DEBUG");
    while (GetAsyncKeyState(VK_INSERT) == 0)
        Sleep(60);

    ///  ESTABLISH GAME DATA   
    g_GameVariables = std::make_unique<GameVariables>();
    g_GameData = std::make_unique<GameData>();

    ///  CREATE WINDOW AND ESTABLISH HOOKS
    g_D3D11Window = std::make_unique<D3D11Window>();
    g_Hooking = std::make_unique<Hooking>();

    ///  INITIALIZE HOOK
    g_Menu = std::make_unique<Menu>();
    g_Hooking->Hook();
    std::thread UPDATE(BackgroundWorker);
    g_Console->printdbg("\n\n[+] MENU READY\n | Show/Hide with [NUMPAD0] or [NUM0]\n\n", g_Console->color.green);
    
    ///  RENDER LOOP
    while (g_Running)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1) g_GameVariables->m_ShowMenu = !g_GameVariables->m_ShowMenu;
    }

    ///  EXIT
    UPDATE.join();
    FreeLibraryAndExitThread(g_hModule, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwCallReason, LPVOID lpReserved)
{
    using namespace SOCOM3;
    g_hModule = hModule;

    switch (dwCallReason) {
        case (DLL_PROCESS_ATTACH): {
            DisableThreadLibraryCalls(hModule);
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, g_hModule, NULL, NULL);
            break;
        }
        case (DLL_PROCESS_DETACH): {
            g_KillSwitch = TRUE;
            break;
        }
    }
    return TRUE;
}