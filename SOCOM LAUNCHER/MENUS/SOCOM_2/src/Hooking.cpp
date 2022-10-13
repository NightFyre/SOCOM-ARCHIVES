#include "../include/Hooking.hpp"
#include "../include/Console.hpp"
#include "../include/Game.hpp"
#include "../include/D3D11Renderer.hpp"
#include "../include/Memory.hpp"
namespace SOCOM2 {

	//	Get Function Address
	__int64 Hooking::GetAddr(INT64 addr)
	{
		if (g_GameVariables->m_ModuleBase == NULL)
			g_GameVariables->m_ModuleBase = (uintptr_t)GetModuleHandleA(NULL);
		return (__int64)g_GameVariables->m_ModuleBase + addr;
	}

	//	MinHook
	void Hooking::HookInput(uint64_t a1, uint64_t a2)
	{
		//if (g_GameVariables->m_ShowMenu)
		//	return;

		//reinterpret_cast<decltype(&HookInput)>(g_Hooking->m_OriginalInputHandler)(a1, a2);
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

	void Hooking::HookW2S(INT64 a1, INT64 a2, INT64 a3, void* a4)
	{
		//if (g_Menu->dbgPrintW2S) {
		//	printf("### FUNCTION CALLED : WorldToScreen\n");
		//	printf("| a1 :  %llX\n", a1);
		//	printf("| a2 :  %llX\n", a2);
		//	printf("| a3 :  %llX\n", a3);
		//	printf("| a4 :  %p\n", a4);
		//	printf("-----------------------------------\n\n");
		//	g_Menu->dbgPrintW2S = FALSE;
		//}
		//g_Hooking->TRUE_W2S = a3;
		//reinterpret_cast<decltype(&HookW2S)>(g_Hooking->m_OriginalW2SHandler)(a1, a2, a3, a4);
	}

	void Hooking::FindHooks()
	{
		m_InputHandler = NULL; //Signature("48 8B C4 41 56 48 81 EC ? ? ? ? 48 C7 40 ? ? ? ? ? 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 0F 29 70 E8 0F 29 78 D8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 50 ?").Scan().As<uint64_t>();
		m_W2SHandler = NULL; //GetAddr(g_Menu->ptr_W2S_FUNCTION);
		m_OnLeftDClick = Signature("48 8B 05 ? ? ? ? 80 B8 ? ? ? ? ? 74 0C").Scan().As<uint64_t>();
		m_ResetEE = Signature("80 3D ? ? ? ? ? 74 13 B8 ? ? ? ? 86").Scan().As<uint64_t>();	// 80 3D 09 18 E8 0C 00 74 13 B8 01 00 00 00 86 05 DF 0B E8 0C C6 05 F6 17 E8 0C 01
	}

	Hooking::Hooking()
	{
		MH_Initialize();
	}

	Hooking::~Hooking()
	{
		MH_RemoveHook(MH_ALL_HOOKS);
	}

	void Hooking::Hook()
	{
		g_D3DRenderer->Hook();
		FindHooks();

		if (m_OnLeftDClick)
			MH_CreateHook((LPVOID)m_OnLeftDClick, &HookFullscreenToggle, &m_OriginalOnLeftDCLick);

		if (m_ResetEE)
			MH_CreateHook((LPVOID)m_ResetEE, &recResetEE, &m_OriginalResetEE);

		MH_EnableHook(MH_ALL_HOOKS);
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
		g_D3DRenderer->Unhook();
		MH_DisableHook((LPVOID)m_OnLeftDClick);
		MH_RemoveHook(MH_ALL_HOOKS);
		g_Console->DestroyConsole();
		return;
	}
}