#include "..//include/Game.hpp"
#include "..//include/Menu.hpp"
namespace SOCOM_CA {
	GameData::GameData()
	{
		Init();
#if DEVELOPER
		g_Console->printdbg("GameData::Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	GameVariables::GameVariables()
	{
#if DEVELOPER
		g_Console->printdbg("GameVariables::Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	void GameData::Init()
	{
		//	Establish Pointer Paths and base address
		BasePS2MemorySpace = *(uintptr_t*)dwEEMem;	//	Value of EEmem
		offset = *(int32_t*)dwEEMem;				//	Spacing for pointers
		return;
	}

	//	Get Process Window Information
	void GameVariables::Init()
	{
		g_GamePid = GetCurrentProcessId();
		g_GameHandle = GetCurrentProcess();
		g_GameWindow = GetForegroundWindow();

		RECT tempRECT;
		GetWindowRect(g_GameWindow, &tempRECT);
		g_GameWidth = tempRECT.right - tempRECT.left;
		g_GameHeight = tempRECT.bottom - tempRECT.top;
		//g_WindowSize{};

		char tempTitle[MAX_PATH];
		GetWindowTextA(g_GameWindow, tempTitle, sizeof(tempTitle));
		g_GameTitle = tempTitle;

		char tempClassName[MAX_PATH];
		GetClassNameA(g_GameWindow, tempClassName, sizeof(tempClassName));
		g_ClassName = tempClassName;

		char tempPath[MAX_PATH];
		GetModuleFileNameExA(g_GameHandle, NULL, tempPath, sizeof(tempPath));
		g_GamePath = tempPath;
#if DEVELOPER
		g_Console->printdbg("GameData::Init - Process Window Info Established\n", g_Console->color.pink);
#endif
	}


	//	Gets distance from Position A to Position B
	float GameData::GetDistanceTo3D_Object(Vector3 POS, Vector3 POS2)
	{
		float x = (POS2.x - POS.x);
		float y = (POS2.y - POS.y);
		float z = (POS2.z - POS.z);
		float distance = std::sqrt(x * x + y * y + z * z);
		return (distance);
	}

	//	Gets current game FPS
	void GameData::FPS()
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

	void GameData::Patch(BYTE* dst, BYTE* src, unsigned int size)
	{
		DWORD oldprotect;
		VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy(dst, src, size);
		VirtualProtect(dst, size, oldprotect, &oldprotect);
	}

	void GameData::Nop(BYTE* dst, unsigned int size)
	{
		DWORD oldprotect;
		VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memset(dst, 0x00, size);
		VirtualProtect(dst, size, oldprotect, &oldprotect);
	}

	uintptr_t GameData::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
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
	uintptr_t GameData::GetPS2Addr(int offset)
	{
		return (*(uintptr_t*)dwEEMem + offset);
	}

	///	
	/// <param name="offset">PS2 Offset</param>
	/// <param name="patch">Patch Value</param>
	/// <param name="type">1 = INT | 2 = FLOAT</param>
	void GameData::PS2Patch(uintptr_t offset, int patch, int type)
	{
		uintptr_t addr = GetPS2Addr(offset);
		switch (type) {
		case(1):
			*(int32_t*)addr = patch;
			break;
		case(2):
			*(float*)addr = patch;
			break;
		}
		return;
	}

	//void GameFunction::PS2Read(uintptr_t offset, int type)
	//{
	//	uintptr_t addr;

	//}

	const char* GameData::GetPlayerTeam(unsigned int value)
	{
		std::string output = "[DEBUG] ";
		if (value == g_GameData->Teams.SEALS)
			return "SEAL";
		else if (value == g_GameData->Teams.TERRORIST)
			return "TERRORIST";
		else if (value == g_GameData->Teams.SPECTATOR)
			return "SPECTATOR";
		else if (value == g_GameData->Teams.CAMPAIGN || value == g_GameData->Teams.CAMPAIGN2)
			return ("SEALS - CAMPAIGN");
		else if (value == g_GameData->Teams.TURRET || value == g_GameData->Teams.TURRET2)
			return ("TURRET");
		else return (output + std::to_string(value)).c_str();//"(unknown team id)";
	}

	// SOCOM CA Player PTR + Valid Check
	bool GameData::Update()
	{
		PlayerPTR = BasePS2MemorySpace + g_GameData->offsets.PlayerPTR;
		if (*(int32_t*)PlayerPTR == NULL) {
			isValid = FALSE;
			return FALSE;
		}
		SealObject = (PlayerObject*)((*(int32_t*)PlayerPTR) + BasePS2MemorySpace);
		SealName = (PlayerName*)(SealObject->NAME + BasePS2MemorySpace);
		//CharPhys = (PlayerPhysics*)(CharData->PlayerPositionPTR + BasePS2MemorySpace);
		isValid = TRUE;
		return TRUE;
	}

	bool GameData::EntityArrayUpdate()
	{
		//	Entity Object Array Pointer
		EntityArrayPTR = BasePS2MemorySpace + g_GameData->offsets.EntityArrayPTR;	//	0x?00000000 + 0x20442CEC = 7FF??0442CEC

		//	If object is null then we are not in a game.
		if (*(int32_t*)EntityArrayPTR == NULL || *(int32_t*)EntityArrayPTR == g_GameData->offsets.EntityArrayPTR)	return FALSE;
		uintptr_t entArray = *(int32_t*)EntityArrayPTR + BasePS2MemorySpace;
		uintptr_t baseEntity = entArray;															//	Store original position in the array as it loops back to the beginning
		int COUNT = 0;																				//	Establish count for Entity Array
		for (int i = 0; i <= 128 - 1; i = i + 1) {
			EntData[i] = (PlayerObject*)((*(int32_t*)(entArray + 0x08)) + BasePS2MemorySpace);		//	Creates pointer to entity object
			EntNames[i] = (PlayerName*)(EntData[i]->NAME + BasePS2MemorySpace);						//	Get Entity Namespace
			//EntPhys[i] = (EntityPhysics*)(EntData[i]->PlayerPositionPTR + BasePS2MemorySpace);	//	Get Entity Physics	(only required for SOCOM 2)
			entArray = *(int32_t*)entArray + BasePS2MemorySpace;									//	Get next entity in the array
			if (entArray == baseEntity)	break;														//	break out of loop
			COUNT++;
		}
		arraySize = COUNT;
		return TRUE;
	}

	//	VALID CHECK
	bool GameData::Valid()
	{
		PlayerPTR = BasePS2MemorySpace + g_GameData->offsets.PlayerPTR;
		if (*(int32_t*)PlayerPTR == NULL) {
			bpVALID = FALSE;
			return FALSE;
		}
		else {
			bpVALID = TRUE;
			return TRUE;
		}
	}

}