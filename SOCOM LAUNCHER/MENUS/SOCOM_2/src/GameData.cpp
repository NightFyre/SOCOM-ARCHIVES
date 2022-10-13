#include "../include/GameData.hpp"
#include "../include/Console.hpp"

namespace SOCOM2 {

	GameData::GameData() {
		Init();
	}

	void GameData::Init() 
	{
		//	Establish Pointer Paths and base address
		BasePS2MemorySpace = *(uintptr_t*)dwEEMem;	//	Value of EEmem
		offset = *(int32_t*)dwEEMem;				//	Spacing for pointers
		g_Console->printdbg("PS2 Memory Space: %llx\n", g_Console->color.DEFAULT, BasePS2MemorySpace);
		g_Console->printdbg("dwEEmem %X\n\n", g_Console->color.DEFAULT, offset);
		return;
	}

	/// <summary>
	/// Check Game Version
	/// </summary>
	/// <returns></returns>
	bool GameData::Version()
	{
		//	SOCOM 2 (r0004) Render Address Values
		int value1 = 274727131;		//	ORIGINAL
		int value2 = 268435675;		//	PATCHED
		if (*(int32_t*)(BasePS2MemorySpace + Offsets.s_RenderFix) != value1) {
			//	Display Error about game version compatibility for the time being
			g_Console->printdbg("[!] WRONG GAME VERSION DETECTED!\n", g_Console->color.red);
			g_Console->printdbg("[!] THIS IS ONLY FOR SOCOM 2 (patch r0004)\n", g_Console->color.red);
			printf("press [SPACE] key to exit . . .");
			return FALSE;
		}
		return TRUE;
	}

	bool GameData::Update()
	{
		PlayerPTR = BasePS2MemorySpace + g_GameData->Offsets.p_Player;
		if (*(int32_t*)PlayerPTR == NULL) {
			isValid = FALSE;
			return FALSE;
		}
		CharData = (PlayerObject*)((*(int32_t*)PlayerPTR) + BasePS2MemorySpace);
		CharName = (PlayerName*)(CharData->Name + BasePS2MemorySpace);
		CharPhys = (PlayerPhysics*)(CharData->PlayerPositionPTR + BasePS2MemorySpace);

		isValid = TRUE;
		return TRUE;
	}

	/// <summary>
	/// Updates the entity array
	/// </summary>
	/// <returns></returns>
	bool GameData::EntityArrayUpdate()
	{
		//	Entity Object Array Pointer
		EntityArrayPTR = BasePS2MemorySpace + g_GameData->Offsets.p_EntityArray;	//	0x?00000000 + 0x20442CEC = 7FF??0442CEC

		//	If object is null then we are not in a game.
		if (*(int32_t*)EntityArrayPTR == NULL || *(int32_t*)EntityArrayPTR == g_GameData->Offsets.p_EntityArray)	return FALSE;
		uintptr_t entArray = *(int32_t*)EntityArrayPTR + BasePS2MemorySpace;
		uintptr_t baseEntity = entArray;	//	Store original position in the array as it loops back to the beginning
		int COUNT = 0;						//	Establish count for Entity Array
		for (int i = 0; i <= 128 - 1; i = i + 1) {
			EntData[i] = (EntityObjects*)((*(int32_t*)(entArray + 0x08)) + BasePS2MemorySpace);		//	Creates pointer to entity object
			EntNames[i] = (EntityNames*)(EntData[i]->NamePTR + BasePS2MemorySpace);					//	Get Entity Namespace
			EntPhys[i] = (EntityPhysics*)(EntData[i]->PlayerPositionPTR + BasePS2MemorySpace);		//	Get Entity Physics
			entArray = *(int32_t*)entArray + BasePS2MemorySpace;									//	Get next entity in the array
			if (entArray == baseEntity)	break;														//	break out of loop
			COUNT++;
		}
		arraySize = COUNT;
		return TRUE;
	}

	/// <summary>
	/// Check if player is in a game
	/// </summary>
	/// <returns>In-Game Status</returns>
	bool GameData::Valid()
	{
		PlayerPTR = BasePS2MemorySpace + g_GameData->Offsets.p_Player;
		if (*(int32_t*)PlayerPTR == NULL)
			return FALSE;
		
		//	Establish Weapon Data
		uintptr_t Ptr = *(int32_t*)(BasePS2MemorySpace + g_GameData->Offsets.p_WeaponPTR); //  Resolve First Address   //0x01980650
		uintptr_t Addr = *(int32_t*)(g_GameData->BasePS2MemorySpace + Ptr);
		g_GameData->Bullet = (BulletObject*)(g_GameData->BasePS2MemorySpace + Addr);
		return TRUE;
	}
}
