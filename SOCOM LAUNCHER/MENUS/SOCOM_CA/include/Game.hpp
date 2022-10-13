#pragma once
#include "../helper.h"
#include "Console.hpp"

namespace SOCOM_CA {

	struct PlayerObject
	{
		char pad_0000[20];	//0x0000
		int32_t NAME;	//0x0014
		char pad_0018[4];	//0x0018
		Vector3 Position;	//0x001C
		char pad_0028[176];	//0x0028
		int TeamID;	//0x00D8
		char pad_00DC[1544];	//0x00DC
		float chairBloom;	//0x06E4
		char pad_06E8[44];	//0x06E8
		float ScopeRecoil;	//0x0714
		char pad_0718[316];	//0x0718
		float ChairSpread;	//0x0854
		float something;	//0x0858
		float ScopeVertSway;	//0x085C
		float ScopeHoriSway;	//0x0860
		char pad_0864[4];	//0x0864
		float VerticalRecoil;	//0x0868
		float ScopeVertKick;	//0x086C
		float ScopeHoriKick;	//0x0870
		char pad_0874[372];	//0x0874
		float Health;	//0x09E8
	};	//Size: 0x09EC

	struct PlayerName {
		char NAME[16];
	};

	struct TEAMS {
		unsigned int SEALS = 1073741825;
		unsigned int TERRORIST = 2147483904;
		unsigned int SPECTATOR = 65536;
		unsigned int TURRET = 1207959552;
		unsigned int TURRET2 = 3221225472;
		unsigned int CAMPAIGN = 2214592518;
		unsigned int CAMPAIGN2 = 2214592522;
	};

	struct Offsets {
		int PlayerPTR = 0x709D98;
		int EntityArrayPTR = 0x709788;
		int SUNFIX = 0x19F480;		
		int FPS = 0x6AA770;
		int DNAS1 = 0x29bd90;	//	DEFAULT: 27BDFF90 | PATCH: 03E00008
		int DNAS2 = 0x29bd94;	//	DEFAULT: FFBF0030 | PATCH: 00000000
		int FOG = 0x6A2E40;
		int FALL_DAMAGE = 0x0;
		int FORCE_START = 0xA7675C;
		int PROJECTILE_COORDS_X = 0xCFC834;
		int PROJECTILE_COORDS_Y = 0xCFC838;
		int PROJECTILE_COORDS_Z = 0xCFC83c;
	};

	class GameData
	{
	public:
		// Player Objects
		PlayerObject* SealObject{};
		PlayerName* SealName{}; 
		//PlayerPhysics* SealPhysics{};

		//	Entity Objects
		PlayerObject* EntData[128];
		PlayerName* EntNames[128];
		//PlayerPhysics* EntPhys[128];

		//	Data Structs
		Offsets offsets{};
		TEAMS Teams{};

		//
		uintptr_t BasePS2MemorySpace{};
		uintptr_t PlayerPTR{};
		uintptr_t EntityArrayPTR{};
		int arraySize{};
		int offset{};
		bool isValid{};


		//	Cheats
		bool bFOG = FALSE;
		bool bFPS = FALSE;
		bool bSUNFIX = FALSE;
		bool bFORCESTART = FALSE;
		bool bDNASBYPASS = FALSE;
		bool bSONARESP = FALSE;
		bool bPERFECTSHOT = FALSE;
		bool bPERFECTSHOT2 = FALSE;
		bool bSILENTAIM = FALSE;
		bool bDIST_SILENTAIM = FALSE;
		bool bSILENTAIM_SEAL = FALSE;
		bool bSILENTAIM_TERR = FALSE;
		bool bALIVE_FILTER = FALSE;
		bool TELEPORT_TO_PROJECTILE = FALSE;
		bool bFALLDAMAGE = FALSE;


		float AIM_DISTANCE = 0;
		Vector3 TARGET{};

		//	Checks
		bool bpVALID = FALSE;	//	Player In Game bool check


		explicit GameData();
		~GameData() noexcept = default;
		GameData(GameData const&) = delete;
		GameData(GameData&&) = delete;
		GameData& operator=(GameData const&) = delete;
		GameData& operator=(GameData&&) = delete;

		void Init();
		float GetDistanceTo3D_Object(Vector3 POS, Vector3 POS2);
		void FPS();
		void Patch(BYTE* dst, BYTE* src, unsigned int size);
		void Nop(BYTE* dst, unsigned int size);
		uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
		uintptr_t GetPS2Addr(int addr);
		void PS2Patch(uintptr_t offset, int patch, int type);
		const char* GetPlayerTeam(unsigned int value);


		bool Update();
		bool EntityArrayUpdate();
		bool Valid();
	};

	class GameVariables
	{
	public:

		//	Dx & ImGui
		int g_GamePid{};
		HANDLE g_GameHandle{};
		HWND g_GameWindow{};
		int g_GameWidth;
		int g_GameHeight;
		ImVec2 g_WindowSize{};
		LPCSTR g_GameTitle;
		LPCSTR g_ClassName;
		LPCSTR g_GamePath;

		//	MENU RELATED
		bool m_ShowMenu{};
		bool m_ShowHud{};
		bool m_ShowDemo{};
		bool m_ShowConsole{};
		bool m_CONTROLLER{};


		//	
		bool m_SinglePlayer{};
		bool m_Multiplayer{};


		explicit GameVariables();
		~GameVariables() noexcept = default;
		GameVariables(GameVariables const&) = delete;
		GameVariables(GameVariables&&) = delete;
		GameVariables& operator=(GameVariables const&) = delete;
		GameVariables& operator=(GameVariables&&) = delete;

		void Init();
	};

	inline std::unique_ptr<GameData> g_GameData;
	inline std::unique_ptr<GameVariables> g_GameVariables;
}
