#pragma once
#include "../helper.hpp"

namespace SOCOM2 {

	struct TEAMS {
		unsigned int SEALS = 1073741825;
		unsigned int TERRORIST = 2147483904;
		unsigned int SPECTATOR = 65536;
		unsigned int TURRET = 1207959552;
		unsigned int CAMPAIGN = 2214592516;
		unsigned int CAMPAIGN2 = 2214592518;
	};

	struct Offsets {
		int p_Player = 0x44D648;		//
		int p_EntityArray = 0x442CEC;	//
		int p_WeaponPTR = 0x4B8A60;		//	

		int s_RenderFix = 0x35A2F8;		//	On = || Default = 
		int s_RenderFix2 = 0x35A320;	//	On = 00000000 || Default = 0x0C0D64C8 (202204360)
		int s_NVGFilter = 0x2169D8;		//	On = 0 || Default = 0x0C065DD0 (201743824)
		int s_FrameRate1 = 0x40C638;	//	On = || Default = 
		int s_FrameRate2 = 0x40C640;	//	On = || Default = 
		int Fog = 0x4093D0;
		int MapBrightness1 = 0x4B858C;	//0x4B4D4C;
		int MapBrightness2 = 0x4B859C;	//0x4B4D5C;
		int MapBrightness3 = 0x4B85AC;	//0x4B4D6C;
		int s_widescreen1 = 0x4A1DB0;	//	On = || Default = 0x3F800000 (1)
		int s_widescreen2 = 0x4A1DC0;	//	On = || Default = 0x44D80000 (1728.0)
		int s_widescreen3 = 0x4A1DC4;	//	On = || Default = 0x44E40000 (1824.0)
		int s_widescreen4 = 0x4A7CEC;	//	On = || Default = 0x3F800000 (1)
		int s_CompassX = 0x407688;		//	On = || Default = 0x440D4000 (565.0)
		int s_CompassY = 0x40768C;		//	On = || Default = 0x42C80000 (100.0)
		int s_crosshairR = 0x407720;	//	HEX Float
		int s_crosshairG = 0x407728;	//	Hex Float
		int s_crosshairB = 0x407730;	//	Hex Float

		//	CHEATS
		int decAmmoCount = 0x5C6288;
		int decEquipmentCount = 0x5C94B4;
		int PerfectWeapon = 0x5C5E20;
		int RapidFire = 0x5C81CC;
		int AimAssistBox = 0x2175E8;
		//int wpnSpread = 0x5BD4E4;
		//int wpnSpread2 = 0x5C2A0C;
		//int wpnSpread3 = 0x5C36C4;
		//int wpnRecoil = 0x5BF238;
		int AimAssistBool = 0x40A370;
		int TargetLockDistance = 0x5B0FF8;
	};

	struct PlayerObject
	{
		//	wpnAuto: 0xDF8
		//	RapidFire: 0xDFC
		char pad_0000[20];	//0x0000
		int32_t Name;		//0x0014
		char pad_0018[4];	//0x0018
		Vector3 Position;	//0x001C
		int PlayerPositionPTR;	//0x0028
		char pad_002C[156];	//0x002C
		int TeamID;			//0x00C8
		char pad_00CC[1224];//0x00CC
		int ShotsFired;		//0x0594
		char pad_0598[64];	//0x0598
		float WeaponBounce;	//0x05D8
		float WeaponSpread;	//0x05DC
		char pad_05E0[36];	//0x05E0
		float WeaponRecoil;	//0x0604
		float ScopeSway1;	//0x0608
		float ScopeSway2;	//0x060C
		char pad_0610[12];	//0x0610
		float ScopeRecoil1;	//0x061C
		float ScopeRecoil2;	//0x0620
		char pad_0624[400];	//0x0624
		int PrimaryMag1;	//0x07B4
		int PrimaryMag2;	//0x07B8
		int PrimaryMag3;	//0x07BC
		int PrimaryMag4;	//0x07C0
		int PrimaryMag5;	//0x07C4
		int PrimaryMag6;	//0x07C8
		int PrimaryMag7;	//0x07CC
		int PrimaryMag8;	//0x07D0
		int PrimaryMag9;	//0x07D4
		int PrimaryMag10;	//0x07D8
		int SecondaryMag1;	//0x07DC
		int SecondaryMag2;	//0x07E0
		int SecondaryMag3;	//0x07E4
		int SecondaryMag4;	//0x07E8
		int SecondaryMag5;	//0x07EC
		int SecondaryMag6;	//0x07F0
		int SecondaryMag7;	//0x07F4
		int SecondaryMag8;	//0x07F8
		int SecondaryMag9;	//0x07FC
		int SecondaryMag10;	//0x0800
		int EquipmentSlot1;	//0x0804
		char pad_0808[36];	//0x0808
		int EquipmentSlot2;	//0x082C
		char pad_0830[36];	//0x0830
		int EquipmentSlot3;	//0x0854
		char pad_0858[1440]; //0x0858
		int WeaponAuto;		//0x0DF8
		int WeaponRapid;	//0x0DFC
		char pad_00DFF[44];		//0x0DFF
		int32_t crosshairSPREAD; //0x0E2C
		int32_t crosshairSPREAD2; //0x0E30
		char pad_0E34[320]; //0x0E34
		int32_t gunHOT; //0x0F74
		char pad_0F78[204]; //0x0F78
		float playerHEALTH; //0x1044
	}; //Size: 0x1048

	struct PlayerName {
		char NAME[16];
	};

	struct PlayerPhysics {
		float rotate;	//0x0000
		char pad_0004[4];	//0x0004
		float rotate2;	//0x0008
		char pad_000C[20];	//0x000C
		float rotate3;	//0x0020
		char pad_0024[4];	//0x0024
		float rotate4;	//0x0028
		char pad_002C[4];	//0x002C
		Vector3 edit_POSITION;	//0x0030
	};	//Size: 0x003C

	struct EntityObjects {
		char pad_0000[20];	//0x0000
		int32_t NamePTR;	//0x0014
		char pad_0018[4];	//0x0018
		Vector3 Position;	//0x001C
		int PlayerPositionPTR;	//0x0028
		char pad_002C[156];	//0x002C
		int TeamID;			//0x00C8
		char pad_00CC[1292];//0x00CC
		float WeaponBounce;	//0x05D8
		float WeaponSpread;	//0x05DC
		char pad_05E0[36];	//0x05E0
		float WeaponRecoil;	//0x0604
		float ScopeSway1;	//0x0608
		float ScopeSway2;	//0x060C
		char pad_0610[12];	//0x0610
		float ScopeRecoil1;	//0x061C
		float ScopeRecoil2;	//0x0620
		char pad_0624[400];	//0x0624
		int PrimaryMag1;	//0x07B4
		int PrimaryMag2;	//0x07B8
		int PrimaryMag3;	//0x07BC
		int PrimaryMag4;	//0x07C0
		int PrimaryMag5;	//0x07C4
		int PrimaryMag6;	//0x07C8
		int PrimaryMag7;	//0x07CC
		int PrimaryMag8;	//0x07D0
		int PrimaryMag9;	//0x07D4
		int PrimaryMag10;	//0x07D8
		int SecondaryMag1;	//0x07DC
		int SecondaryMag2;	//0x07E0
		int SecondaryMag3;	//0x07E4
		int SecondaryMag4;	//0x07E8
		int SecondaryMag5;	//0x07EC
		int SecondaryMag6;	//0x07F0
		int SecondaryMag7;	//0x07F4
		int SecondaryMag8;	//0x07F8
		int SecondaryMag9;	//0x07FC
		int SecondaryMag10;	//0x0800
		int EquipmentSlot1;	//0x0804
		char pad_0808[36];	//0x0808
		int EquipmentSlot2;	//0x082C
		char pad_0830[36];	//0x0830
		int EquipmentSlot3;	//0x0854
		char pad_0858[2028];	//0x0858
		float Health;	//0x1044
	};

	struct EntityNames {
		char NAME[16];
	};

	struct EntityPhysics {
		float rotate;	//0x0000
		char pad_0004[4];	//0x0004
		float rotate2;	//0x0008
		char pad_000C[20];	//0x000C
		float rotate3;	//0x0020
		char pad_0024[4];	//0x0024
		float rotate4;	//0x0028
		char pad_002C[4];	//0x002C
		Vector3 edit_POSITION;	//0x0030
	};	//Size: 0x003C

	struct BulletObject
	{
		char pad_0000[84];	//0x0000
		Vector3 Projectile;	//0x0054
		char pad_0060[56];	//0x0060
	};	//Size: 0x0098

	class GameData {
	public:
		//	
		PlayerObject* CharData{};
		PlayerName* CharName{};
		PlayerPhysics* CharPhys{};
		
		//	Static Size, We will ommit empty entries and flush contents when we are done / before updating
		EntityObjects* EntData[128];
		EntityNames* EntNames[128];
		EntityPhysics* EntPhys[128];

		//	Data Structs
		TEAMS Teams{};
		Offsets Offsets{};
		BulletObject* Bullet{};

		//	
		uintptr_t BasePS2MemorySpace{};
		uintptr_t PlayerPTR{};
		uintptr_t EntityArrayPTR{};
		int arraySize{};
		int offset{};
		bool isValid{};

		//	Variables for various cheats
		bool b_RFIX{};
		bool b_RFIX2{};
		bool bSILENTAIM_RAGE = FALSE;
		Vector3 SavedCoords{};

		///  CHARACTER TAB
		//  Booleans
		bool bRAPIDFIRE{};
		bool bAUTOGUNS{};
		bool bINFINITEAMMO{};
		bool bPERFECTSHOT{};
		bool bPERFECTSCOPE{};
		bool bAIMBOT{};
		bool bFPS{};
		bool bFOG{};
		bool bNVGFILTER{};
		bool bWIDESCREEN{};
		bool bBRIGHTNESS{};
		bool bFOGCOLOR{};

		//  WEAPON MANAGER
		int SELECTED_WEAPON{};
		int bSELECTED_PRIMARY{};
		int bSELECTED_SECONDARY{};
		int bSELECTED_EQSLOT{};
		int SELECTED_MAG_COUNT{};
		const char* LIST_OPTIONS[3]{ "PRIMARY", "SECONDARY", "EQUIPMENT" };
		const char* LIST_pMAGS[10]{ "MAG 1", "MAG 2", "MAG 3", "MAG 4", "MAG 5", "MAG 6", "MAG 7", "MAG 8", "MAG 9", "MAG 10" };
		const char* LIST_sMAGS[10]{ "MAG 1", "MAG 2", "MAG 3", "MAG 4", "MAG 5", "MAG 6", "MAG 7", "MAG 8", "MAG 9", "MAG 10" };
		const char* LIST_eqSLOTS[3]{ "SLOT 1", "SLOT 2", "SLOT 3" };


		//  INFINITE AMMO
		int AMMO_ARRAY[9]{};
		int CURRENT_SHOTS_FIRED{};

		float e_Brightness{};
		float buf{};
		int matrixBUF{};
		int m_dbg_draw_pos;
		Vector2 m_dbg_window{};

		explicit GameData();
		~GameData() noexcept = default;
		GameData(GameData const&) = delete;
		GameData(GameData&&) = delete;
		GameData& operator=(GameData const&) = delete;
		GameData& operator=(GameData&&) = delete;

		void Init();
		bool Version();
		bool Update();
		bool EntityArrayUpdate();
		bool Valid();
		
	};
	inline std::unique_ptr<GameData> g_GameData;
}