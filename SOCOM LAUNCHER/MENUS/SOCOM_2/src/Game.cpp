#include "../include/Game.hpp"
#include "../include/Menu.hpp"
#include "../include/GameData.hpp"
namespace SOCOM2 {
	GameVariables::GameVariables()
	{
		m_GamePid = GetCurrentProcessId();
		m_GameHandle = GetCurrentProcess();
		m_GameWindow = GetForegroundWindow();
		m_ModuleBase = (uintptr_t)GetModuleHandle(NULL);

		RECT TempRect;
		GetWindowRect(m_GameWindow, &TempRect);
		m_GameWidth = TempRect.right - TempRect.left;
		m_GameHeight = TempRect.bottom - TempRect.top;
		//m_WindowSize = { m_GameWidth, m_GameHeight };

		char TempTitle[MAX_PATH];
		GetWindowTextA(m_GameWindow, TempTitle, sizeof(TempTitle));
		m_GameTitle = TempTitle;

		char TempClassName[MAX_PATH];
		GetClassNameA(m_GameWindow, TempClassName, sizeof(TempClassName));
		m_ClassName = TempClassName;

		char TempPath[MAX_PATH];
		GetModuleFileNameExA(m_GameHandle, NULL, TempPath, sizeof(TempPath));
		m_GamePath = TempPath;
	}

	void GameVariables::ResizeWindow()
	{
		m_GameWindow = GetForegroundWindow();
		
		RECT TempRect;
		GetWindowRect(m_GameWindow, &TempRect);
		m_GameWidth = TempRect.right - TempRect.left;
		m_GameHeight = TempRect.bottom - TempRect.top;
	}

	GameFunctions::GameFunctions()
	{
		return;
	}

	//	Gets distance from Position A to Position B
	float GameFunctions::GetDistanceTo3D_Object(Vector3 POS, Vector3 POS2)
	{
		float x = (POS2.x - POS.x);
		float y = (POS2.y - POS.y);
		float z = (POS2.z - POS.z);
		float distance = std::sqrt(x * x + y * y + z * z);
		return (distance);
	}

	//	Gets current game FPS
	void GameFunctions::FPS()
	{
		//  CREDIT: XBOX360LSBEST
		static bool init = false;
		static char text[64] = "FPS: unknown";
		static int frame = 0;
		static clock_t time = clock();
		frame++;
		if (clock() - time >= 1000)
		{
			memset(text, 0, sizeof(text));
			sprintf_s(text, "FPS: %i\n", frame);
			frame = 0;
			time = clock();
			init = true;
		}

		if (init && text[0])	//	CASE SWITCH FOR 3rd BOOLEAN
			if (g_Menu->dbg_RAINBOW_THEME)
				ImGui::GetBackgroundDrawList()->AddText(ImVec2(20, 5), ImColor(g_Menu->dbg_RAINBOW), text);
			else if (!g_Menu->dbg_RAINBOW_THEME) {
				
				///	x01337
				//ImVec2 pos, rect_size;
				//pos = ImVec2(20, 5);
				//rect_size = ImVec2(10/*x*/, 10/*y*/); // change to what ever size you want
				//draw->AddRect(pos, { pos.x + rect_size.x, pos.y + rect_size.y }, ImColor(255, 255, 255));

				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(15, 20), ImVec2(80, 20), ImColor(0, 0, 0, 255));
				ImGui::GetForegroundDrawList()->AddText(ImVec2(19, 5), ImColor(0, 0, 0, 255), text);
				ImGui::GetForegroundDrawList()->AddText(ImVec2(21, 5), ImColor(0, 0, 0, 255), text);
				ImGui::GetForegroundDrawList()->AddText(ImVec2(20, 5), ImColor(255, 255, 0, 255), text);
			}
	}

	void GameFunctions::Patch(BYTE* dst, BYTE* src, unsigned int size)
	{
		DWORD oldprotect;
		VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy(dst, src, size);
		VirtualProtect(dst, size, oldprotect, &oldprotect);
	}

	void GameFunctions::Nop(BYTE* dst, unsigned int size)
	{
		DWORD oldprotect;
		VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memset(dst, 0x00, size);
		VirtualProtect(dst, size, oldprotect, &oldprotect);
	}

	uintptr_t GameFunctions::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
	{
		uintptr_t addr = ptr;
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			addr = *(uintptr_t*)addr;
			addr += offsets[i];
		}
		return addr;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="offset"></param>
	/// <returns></returns>
	uintptr_t GameFunctions::GetPS2Addr(uintptr_t offset)
	{
		return (*(uintptr_t*)dwEEMem + offset);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="offset">PS2 Offset</param>
	/// <param name="patch">Patch Value</param>
	/// <param name="type">1 = INT | 2 = FLOAT</param>
	void GameFunctions::PS2Patch(uintptr_t offset, int patch, int type)
	{
		uintptr_t addr = GetPS2Addr(offset);
	
		if (type == 1)
			*(int32_t*)addr = patch;
		else if (type == 2)
			*(float*)addr = patch;
		return;
	}

	//void GameFunction::PS2Read(uintptr_t offset, int type)
	//{
	//	uintptr_t addr;

	//}

	const char* GameFunctions::GetPlayerTeam(unsigned int value)
	{
		if (value == g_GameData->Teams.SEALS)
			return "SEAL";
		else if (value == g_GameData->Teams.TERRORIST)
			return "TERRORIST";
		else if (value == g_GameData->Teams.SPECTATOR)
			return "SPECTATOR";
		else if (value == g_GameData->Teams.CAMPAIGN)
			return ("SEALS - CAMPAIGN");
		else return "(null)";
	}
}