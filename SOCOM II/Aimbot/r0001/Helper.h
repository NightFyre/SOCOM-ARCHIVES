/// includes.h is my personal framework. Can be obtained by contributing to my efforts OR join the GuidedHacking Community to learn how to go about creating your own
/// PCSX2 GuidedHacking Thread: https://guidedhacking.com/threads/pcsx2-emulator-finddmaaddy-multilevel-pointer-function.19423/
#pragma once
#include "../../../INTERNAL/includes.h"

///Summary
//Game: SOCOM 2 patch.r0001

//  PROC & GAME VARIABLES
struct Vector3 { float x, y, z; };
class sealOBJECT
{
public:
    char pad_0000[20]; //0x0000
    void* playerNAME; //0x0014
    char pad_0018[4]; //0x0018
    Vector3 playerPOS; //0x001C
    void* coordsWRITE; //0x0028
    char pad_002C[156]; //0x002C
    int32_t teamID; //0x00C8
    char pad_00CC[3960]; //0x00CC
    float playerHEALTH; //0x1044
    char pad_1048[4]; //0x1048
}; //Size: 0x104C

namespace Process {
    HMODULE     hModule;
    uintptr_t   moduleBase;
    uintptr_t   textBase = (moduleBase + 0x1000);
}

namespace Vars {

    //  MENU
    string sPROMOD = " ";
    string sRFIX = " ";
    string sAMMO = " ";
    string sAIMBOT = " ";

    //  PLAYER
    bool bMENU = false;
    bool bPROMOD = false;
    bool bRFIX = false;
    bool bAMMO = false;
    bool bAIMBOT = false;
    bool LOOP = false;
}

namespace Offsets {

    //  Player Object
    int PlayerObjectPTR = 0x20440C38;

    //  Cheats
    int decAmmoCount = 0x205BF6A0;
    int decEquipmentCount = 0x205C1FE8;
    int wpnSpread = 0x205BD4E4;
    int wpnSpread2 = 0x205C2A0C;
    int wpnSpread3 = 0x205C36C4;
    int wpnRecoil = 0x205BF238;
    int AimAssistBool = 0x203DF1A0;
    int TargetLockDistance = 0x205AB368;

    //  Patches
    int recResetEE = 0x265570;
    int RenderFix = 0x2033CD6A;
    int Framerate1 = 0x203E13F8;
    int Framerate2 = 0x203E1400;
    int Fog = 0x203DE210;
    int MapBrightness1 = 0x204B4D4C;
    int MapBrightness2 = 0x204B4D5C;
    int MapBrightness3 = 0x204B4D6C;
}

//  CONSOLE
FILE* f;
void initFrameWork()
{
    //  CREATE CONSOLE
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitle(L"SOCOM-22");
    cMENU::_setWindow(20, 5);
    Sleep(100);
    cMENU::_setWindow(25, 7);
}

void UpdateMenu()
{
    //  Console UI Framework
    cMENU::_clearConsole();
    cMENU::_setConsole();
    std::cout << " ______________________ \n";
    std::cout << "|--------PATCHES-------|\n";
    std::cout << "|1) RENDER FIX     [" << Vars::sRFIX << "] |\n";
    std::cout << "|2) AIMBOT         [" << Vars::sAIMBOT << "] |\n";
    std::cout << "|______________________|\n";
    std::cout << "| PRESS [END] TO QUIT  |" << std::endl;
    Vars::bMENU = true;
}

//  CHEATS
void ProMod(bool ENABLED)
{
    if (ENABLED)
    {
        //  Apply Render Fix
        mem::PS2Nop((BYTE*)Offsets::RenderFix, 1);
        
        //  Disable Fog
        *(int*)Offsets::Fog = 0;
        
        //  Adjust framerate
        *(int*)Offsets::Framerate1 = 60;
        *(int*)Offsets::Framerate2 = 60;
        
        //  Adjust map brightness
        *(float*)Offsets::MapBrightness1 = 5;
        *(float*)Offsets::MapBrightness2 = 5;
        *(float*)Offsets::MapBrightness3 = 5;
        
        //  Recompile PCSX2 Memory
        // More information can be found at GuidedHacking.com , I have a thread where I give an overview on this function
        PCSX2RecompileMEM();
        
        //  Enabled indication for Menu
        Vars::sRFIX = "X";
        Vars::bMENU = FALSE;
    }
    else if (!ENABLED)
    {
        //  Restore defaults
        mem::Patch((BYTE*)Offsets::RenderFix, (BYTE*)"\x60", 1);
        *(int*)Offsets::Fog = 1;
        *(int*)Offsets::Framerate1 = 30;
        *(int*)Offsets::Framerate2 = 30;
        *(float*)Offsets::MapBrightness1 = 0;
        *(float*)Offsets::MapBrightness2 = 0;
        *(float*)Offsets::MapBrightness3 = 0;
        
        //  Recompile PCSX2 Memory
        PCSX2RecompileMEM();

        //  Disabled indication for Menu
        Vars::sRFIX = " ";
        Vars::bMENU = FALSE;
    }
}

void AimBot(bool ENABLED)
{
    if (ENABLED)
    {
        //  Enable Aim Assist
        mem::Patch((BYTE*)Offsets::AimAssistBool, (BYTE*)"\x01\x00\x00\x00", 4);

        //  Aim Assist Target Distance
        mem::PS2Nop((BYTE*)Offsets::TargetLockDistance, 4);

        //  Perfect Shot
        mem::PS2Nop((BYTE*)Offsets::wpnRecoil, 4);
        mem::PS2Nop((BYTE*)Offsets::wpnSpread, 4);
        mem::PS2Nop((BYTE*)Offsets::wpnSpread2, 4);
        mem::PS2Nop((BYTE*)Offsets::wpnSpread3, 4);

        //  Infinite Ammo (Ballistics and Equipment)
        mem::Patch((BYTE*)Offsets::decAmmoCount, (BYTE*)"\x00\x00\x63\x24", 4);
        mem::Patch((BYTE*)Offsets::decEquipmentCount, (BYTE*)"\x00\x00\x42\x24", 4);

        //  Recompile PCSX2 Memory
        // More information can be found at GuidedHacking.com , I have a thread where I give an overview on this function
        PCSX2RecompileMEM();

        //  Enabled indication for Menu
        Vars::sAIMBOT = "X";
        Vars::bMENU = FALSE;
    }
    else if (!ENABLED)
    {
        //  Restore Defaults
        mem::PS2Nop((BYTE*)Offsets::AimAssistBool, 4);
        mem::Patch((BYTE*)Offsets::TargetLockDistance, (BYTE*)"\xDC\x40\x0A\x0C", 4);
        mem::Patch((BYTE*)Offsets::wpnRecoil, (BYTE*)"\x18\x08\xA2\xAE", 4);
        mem::Patch((BYTE*)Offsets::wpnSpread, (BYTE*)"\x08\x00\x00\xE6", 4);
        mem::Patch((BYTE*)Offsets::wpnSpread2, (BYTE*)"\x4C\x08\x40\xE6", 4);
        mem::Patch((BYTE*)Offsets::wpnSpread3, (BYTE*)"\x04\x00\x00\x45", 4);
        mem::Patch((BYTE*)Offsets::decAmmoCount, (BYTE*)"\xFF\xFF\x63\x24", 4);
        mem::Patch((BYTE*)Offsets::decEquipmentCount, (BYTE*)"\xFF\xFF\x42\x24", 4);

        //  Recompile PCSX2 Memory
        PCSX2RecompileMEM();
        
        //  Disabled indication for Menu
        Vars::sAIMBOT = " ";
        Vars::bMENU = FALSE;
    }
}

void Brightness()
{
    //  Check if player is in game
    if (*(int*)Offsets::PlayerObjectPTR != NULL)
    {
        //  Check if brightness has reset itself
        if (*(float*)Offsets::MapBrightness1 == 0)
        {
            //  Re-Apply our patch
            *(float*)Offsets::MapBrightness1 = 5;
            *(float*)Offsets::MapBrightness2 = 5;
            *(float*)Offsets::MapBrightness3 = 5;
        }
    }
    else if (*(float*)Offsets::MapBrightness1 != 0)
    {
        //  Restore default brightness for menus
        *(float*)Offsets::MapBrightness1 = 0;
        *(float*)Offsets::MapBrightness2 = 0;
        *(float*)Offsets::MapBrightness3 = 0;
    }
}