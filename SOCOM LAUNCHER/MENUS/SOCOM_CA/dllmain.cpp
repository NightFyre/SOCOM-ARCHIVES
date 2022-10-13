#include "helper.h"
#include "include//Console.hpp"
#include "include//Game.hpp"
#include "include//D3D11Window.hpp"
#include "include//Hooking.hpp"
#include "include//Menu.hpp"
void Initialize();
void BackgroundWorker();

DWORD WINAPI MainThread()
{
    using namespace SOCOM_CA;
    Initialize();
    std::thread UPDATE(BackgroundWorker);

    while (g_Running) {
        if (GetAsyncKeyState(0x30) & 1)
        {
            //  Initial Notification Window
            if (g_GameVariables->m_ShowHud)
                g_GameVariables->m_ShowHud = FALSE;

            //  Show/Hide Window
            g_GameVariables->m_ShowMenu ^= 1;
        }
    }

    UPDATE.join();
    FreeLibraryAndExitThread(g_hModule, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    using namespace SOCOM_CA;
    g_hModule = hModule;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(g_hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, g_hModule, NULL, NULL);
        break;
    case DLL_PROCESS_DETACH:
        g_KillSwitch = TRUE;
        break;
    }
    return TRUE;
}

void Initialize() {
    using namespace SOCOM_CA;

    //	Initialize Console
    g_Console = std::make_unique<Console>();
    g_Console->InitializeConsole("SOCOM CA - INTERNAL DEBUG");
    g_Console->printdbg("[+] SOCOM INTERNAL (PREVIEW)\n", g_Console->color.yellow);
    g_Console->printdbg("[+] BUILD VERSION: alpha-0.0.5\n", g_Console->color.yellow);
    g_Console->printdbg("[+] BUILD DATE: 6/24/22\n", g_Console->color.yellow);
    g_Console->printdbg("[+] Created bv NightFyre\n\n", g_Console->color.yellow);

    g_Console->printdbg("[!] THIS IS ONLY FOR SOCOM COMBINED ASSUALT patch 1.4\n", g_Console->color.red);
    g_Console->printdbg("[!] THIS IS A PREVIEW BUILD\n", g_Console->color.red);
    g_Console->printdbg("[!] PLEASE DON'T INJECT UNTIL YOU HAVE LAUNCHED THE GAME [INSERT]\n\n", g_Console->color.red);

    g_Console->printdbg("PCSX2 LAUNCHED\n", g_Console->color.green);
    g_Console->printdbg("PLEASE LAUNCH SOCOM COMBINED ASSAULT & TOGGLE FULLSCREEN\n\n", g_Console->color.DEFAULT);
    while (GetAsyncKeyState(VK_INSERT) == 0)
        Sleep(60);
    system("cls");

    //  ESTABLISH GAME VARIABLES AND FUNCTIONS
    g_GameVariables = std::make_unique<GameVariables>();
    g_GameData = std::make_unique<GameData>();

    //  ESTABLISH VARIABLES FOR MENU & D3DWINDOW
    g_D3D11Window = std::make_unique<D3D11Window>();
    g_Hooking = std::make_unique<Hooking>();

    //  INITIALIZE THE HOOK
    g_Menu = std::make_unique<Menu>();
    g_Hooking->Hook();
    return;
}

void BackgroundWorker()
{
    using namespace SOCOM_CA;
    while (g_Running)
    {
        if (g_bgCOUNTER >= 5000) {

            g_bgCOUNTER = NULL;
        }

        if (g_GameData->Valid()) {
            g_GameData->Update();
            g_GameData->EntityArrayUpdate();
        }

        g_Menu->Loops();
        g_bgCOUNTER++;
        std::this_thread::sleep_for(1ms);
    }
    return;
}