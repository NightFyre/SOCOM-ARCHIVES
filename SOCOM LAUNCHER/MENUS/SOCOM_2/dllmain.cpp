#include "helper.hpp"
#include "include/Console.hpp"
#include "include/Game.hpp"
#include "include/D3D11Renderer.hpp"
#include "include/Hooking.hpp"
#include "include/GameData.hpp"
#include "include/Menu.hpp"

void BackgroundWorker()
{
	using namespace SOCOM2;
	while (g_Running) {
		if (g_GameData->Valid())
		{
			g_GameData->Update();
			g_GameData->EntityArrayUpdate();
		}
		g_Menu->Loops();
		std::this_thread::sleep_for(5s);
		std::this_thread::yield();
	}
	return;
}

void ViewMatrixFinder()
{
	using namespace SOCOM2;
	while (g_Running) {
		if (g_GameVariables->bLOOP) {
			g_GameVariables->VIEWMATRIX_TEST = g_GameVariables->VIEWMATRIX_TEST + 4;
			g_GameVariables->count++;
		}
		else if (g_GameVariables->count > 0) {
			g_Console->printdbg("ViewMatrixFinder:: Loop Finished Executing . . . Logging Results . . .\n", g_Console->color.green);
			printf("Times Looped: [%d]\nFinal Result: [%llX]\n", g_GameVariables->count, (g_GameData->BasePS2MemorySpace + g_GameVariables->VIEWMATRIX_TEST));
			g_GameVariables->count = NULL;
		}
		std::this_thread::sleep_for(100ms);
		std::this_thread::yield();
	}
	return;
}

void MainThread()
{
	using namespace SOCOM2;
	//	Hook debug console
	g_Console = std::make_unique<Console>();
	g_Console->InitializeConsole("SOCOM 2 DEBUG CONSOLE");
	while (GetAsyncKeyState(VK_INSERT) == 0)
		Sleep(60);
	system("cls");

	//	GAME DECLARATION
	g_GameData = std::make_unique<GameData>();
	if (!g_GameData->Version()) {
		//while (GetAsyncKeyState(VK_SPACE) == 0)
		//	Sleep(100);
		g_Console->DestroyConsole();
		FreeLibraryAndExitThread(g_hModule, EXIT_SUCCESS);
		return;
	}

	//	Establish GameVariables and Functions
	g_GameVariables = std::make_unique<GameVariables>();
	g_GameFunctions = std::make_unique<GameFunctions>();
	g_Console->m_ShowConsole = FALSE;

	//	Establish variables for Menu and DXRenderer
	g_Menu = std::make_unique<Menu>();
	g_D3DRenderer = std::make_unique<D3D11Renderer>();

	//	Initialize the hook
	g_Hooking = std::make_unique<Hooking>();
	g_Hooking->Hook();
	
	//	Initialize update thread
	std::thread GDATA_UPDATE(BackgroundWorker);
	std::thread MATRIXHELPER(ViewMatrixFinder);

	//	Render menu
	while (g_Running)
	{
		if (GetAsyncKeyState(0x30) & 1)
		{
			//  Initial Notification Window
			if (g_GameVariables->m_ShowHud)
				g_GameVariables->m_ShowHud = FALSE;

			//	SHOW/HIDE WINDOW
			g_GameVariables->m_ShowMenu ^= 1;
		}
		std::this_thread::sleep_for(1ms);
		std::this_thread::yield();
	}

	//  EXIT THREAD
	GDATA_UPDATE.join();
	std::this_thread::sleep_for(250ms);
	MATRIXHELPER.join();
	std::this_thread::sleep_for(500ms);
	FreeLibraryAndExitThread(g_hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	using namespace SOCOM2;
	g_hModule = hModule;
	switch (ul_reason_for_call) {
	case(DLL_PROCESS_ATTACH):
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
		break;
	case(DLL_PROCESS_DETACH):
		g_Killswitch = TRUE;
		break;
	}
	return TRUE;
}