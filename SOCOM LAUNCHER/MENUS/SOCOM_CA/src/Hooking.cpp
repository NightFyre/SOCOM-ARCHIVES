#include "..//include/Hooking.hpp"
#include "..//include/Memory.hpp"
namespace SOCOM_CA {
	Hooking::Hooking()
	{
		MH_Initialize();
#if DEVELOPER
		g_Console->printdbg("Hooking::Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	Hooking::~Hooking()
	{
		MH_RemoveHook(MH_ALL_HOOKS);
	}

	void Hooking::HookFullscreenToggle(INT64 a1)
	{
		if (g_GameVariables->m_ShowMenu)
			return;

		reinterpret_cast<decltype(&HookFullscreenToggle)>(g_Hooking->m_OriginalOnLeftDCLick)(a1);
	}

	void Hooking::recResetEE()
	{
		reinterpret_cast<decltype(&recResetEE)>(g_Hooking->m_OriginalResetEE)();
	}

	void Hooking::FindHooks()
	{
		m_InputHandler = NULL; //Signature("48 8B C4 41 56 48 81 EC ? ? ? ? 48 C7 40 ? ? ? ? ? 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 0F 29 70 E8 0F 29 78 D8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 50 ?").Scan().As<uint64_t>();
		m_W2SHandler = NULL; //GetAddr(g_Menu->ptr_W2S_FUNCTION);
		m_OnLeftDClick = Signature("48 8B 05 ? ? ? ? 80 B8 ? ? ? ? ? 74 0C").Scan().As<uint64_t>();
		m_ResetEE = Signature("80 3D ? ? ? ? ? 74 13 B8 ? ? ? ? 86").Scan().As<uint64_t>();	// 80 3D 09 18 E8 0C 00 74 13 B8 01 00 00 00 86 05 DF 0B E8 0C C6 05 F6 17 E8 0C 01
	}

	void Hooking::Hook()
	{
		g_GameVariables->Init();
		g_D3D11Window->Hook();
		FindHooks();

		if (m_OnLeftDClick)
			MH_CreateHook((LPVOID)m_OnLeftDClick, &HookFullscreenToggle, &m_OriginalOnLeftDCLick);

		if (m_ResetEE)
			MH_CreateHook((LPVOID)m_ResetEE, &recResetEE, &m_OriginalResetEE);

		MH_EnableHook(MH_ALL_HOOKS);
#if DEVELOPER
		g_Console->printdbg("Hooking::Hook Initialized\n", g_Console->color.pink);
#endif

		//	Clear Console
		//	Hide Console
		//  Show an ImGui notification bubble to the user
		//	Enable Controller Input
		//	Enable our main loop
		system("cls");
		g_Console->ACTIVE = TRUE;
		g_Console->m_ShowConsole = FALSE;
		g_GameVariables->m_ShowHud = TRUE;
		g_GameVariables->m_CONTROLLER = TRUE;
		g_Running = TRUE;
		return;
	}

	void Hooking::Unhook()
	{
		g_D3D11Window->Unhook();
		MH_DisableHook((LPVOID)m_OnLeftDClick);	//	Restore double click functionality
		MH_RemoveHook(MH_ALL_HOOKS);
		g_Console->DestroyConsole();
		return;
	}
}
