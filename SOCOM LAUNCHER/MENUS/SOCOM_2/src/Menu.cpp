#include "../include/Menu.hpp"
#include "../include/Console.hpp"
#include "../include/Hooking.hpp"
#include "../include/D3D11Renderer.hpp"
#include "../include/GameData.hpp"
#include "../include/Game.hpp"
#include "../include/Memory.hpp"
static ImVec4 color = ImVec4(200.0f / 255.0f, 200.0f / 255.0f, 24.0f / 255.0f, 200.0f / 255.0f);
namespace SOCOM2 {
    //template<typename T> inline void GameFunctions::PS2Patch2(uintptr_t Addr, T Patch);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="Text:">[+] GAMEFIXES::MENU::HARRY62 RENDER FIX - </param>
    /// <param name="Addr:">g_GameData->Offsets.s_RenderFix</param>
    /// <param name="Value:">268435675</param>
    /// <param name="Flag">g_GameData->b_RFIX</param>
    /// <param name="Type:">INT(1) or FLOAT(2)</param>
    //void Switch(std::string Text, uintptr_t Addr, int Value, int Value2, bool Flag, int Type)
    //{
    //    switch (Flag) {
    //    case(TRUE):
    //        g_GameFunctions->PS2Patch(Addr, Value, Type);
    //        break;
    //    case(FALSE):
    //        g_GameFunctions->PS2Patch(Addr, Value2, Type);
    //        break;
    //    }
    //    g_Console->LogEvent(Text, Flag);
    //    return;
    //}

    /// ESP TESTS
    bool WorldToScreen(Vector3 pos, Vector2& screen, float* matrix, int windowWidth, int windowHeight)
    {
        //Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
        Vector4 clipCoords;
        clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
        clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
        clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
        clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

        if (clipCoords.w < 0.1f)
            return false;

        //perspective division, dividing by clip.W = Normalized Device Coordinates
        Vector3 NDC;
        NDC.x = clipCoords.x / clipCoords.w;
        NDC.y = clipCoords.y / clipCoords.w;
        NDC.z = clipCoords.z / clipCoords.w;

        screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
        screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
        return true;
    }

    void ESP(BYTE Pos, Vector2 WindowSize)
    {
        if (!g_GameData->Valid())
            return;

        if (g_GameVariables->INPUT_VIEWMATRIX < NULL || g_GameVariables->INPUT_VIEWMATRIX >= 0xFFFFFFFF) {
            g_Console->printdbg("[!] ESP:: INPUT_VIEWMATRIX ; OUT OF BOUNDS!\n", g_Console->color.red);
            return;
        }

        if (g_GameVariables->VIEWMATRIX_TEST < NULL || g_GameVariables->VIEWMATRIX_TEST >= 0xFFFFFFFF) {
            g_Console->printdbg("[!] ESP:: VIEWMATRIX_TEST ; OUT OF BOUNDS!\n", g_Console->color.red);
            return;
        }

        if (!g_GameVariables->bLOOP)
            g_GameVariables->VIEWMATRIX_TEST = g_GameVariables->INPUT_VIEWMATRIX;

        //	Window Dimensions & Position
        float X = WindowSize.x;
        float Y = WindowSize.y;
        Vector2 pos = { ImGui::GetMainViewport()->GetCenter().x, ImGui::GetMainViewport()->GetCenter().y };

        /// <summary>
        /// Establish Position to draw from
        ImVec2 drawPosition{};
        switch (Pos) {
            //	Heaven
        case(0):
            drawPosition = { pos.x, 0 };
            break;

            //	Hell
        case(1):
            drawPosition = { pos.x, Y };
            break;

            // Corner
        case(2):
            drawPosition = { 0, 0 };
            break;

            // Center
        case(3):
            drawPosition = { pos.x, pos.y };
            break;
        }
        uintptr_t ViewMatrix = g_GameData->BasePS2MemorySpace + g_GameVariables->VIEWMATRIX_TEST;
        memcpy(&g_Menu->Matrix, (BYTE*)ViewMatrix, sizeof(g_Menu->Matrix));
        for (int i = 0; i <= g_GameData->arraySize - 1; i = i + 1) 
        {
            
            //  FILTER
            if (g_GameData->EntData[i]->Health == NULL) continue;

            //  DRAW
            Vector2 vScreen;
            if (WorldToScreen(g_GameData->EntData[i]->Position, vScreen, g_Menu->Matrix, WindowSize.x, WindowSize.y)) {
                ImGui::GetBackgroundDrawList()->AddLine(drawPosition, ImVec2(vScreen.x, vScreen.y), ImColor(255, 255, 255, 255), 0.3f);
                ImGui::GetBackgroundDrawList()->AddText(ImVec2(vScreen.x, vScreen.y), ImColor(255, 255, 255, 255), g_GameData->EntNames[i]->NAME);
            }
        }
    }
    
    /// SILENT AIM TESTS
    void RageSilentAim() 
    {
        //  Loop Entity Array
        for (int i = 0; i <= g_GameData->arraySize - 1; i = i + 1) {
            
            // Distance to target
            if (g_GameFunctions->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->CharData->Position) >= 50) continue;

            ////  TEAM CHECK
            //if (g_GameData->EntData[i]->TeamID != g_GameData->Teams.CAMPAIGN || g_GameData->EntData[i]->TeamID != g_GameData->Teams.CAMPAIGN2) continue;

            // HP CHECK
            if (g_GameData->EntData[i]->Health == NULL) continue;

            // SEND BULLET TO TARGET
            g_GameData->Bullet->Projectile.x = g_GameData->EntData[i]->Position.x;
            g_GameData->Bullet->Projectile.y = (g_GameData->EntData[i]->Position.y + (float)5);
            g_GameData->Bullet->Projectile.z = g_GameData->EntData[i]->Position.z;
        }
    }

    void SilentAim(Vector3 pos)
    {
        // SEND BULLET TO TARGET
        g_GameData->Bullet->Projectile.x = pos.x;
        g_GameData->Bullet->Projectile.y = (pos.y + (float)5);
        g_GameData->Bullet->Projectile.z = pos.z;
    }

    namespace Styles {
        void InitStyle()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec4* colors = ImGui::GetStyle().Colors;
            style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
            style.WindowPadding = ImVec2(8.00f, 8.00f);
            style.FramePadding = ImVec2(5.00f, 2.00f);
            style.CellPadding = ImVec2(6.00f, 6.00f);
            style.ItemSpacing = ImVec2(6.00f, 6.00f);
            style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
            style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
            style.IndentSpacing = 25;
            style.ScrollbarSize = 15;
            style.GrabMinSize = 10;
            style.WindowBorderSize = 1;
            style.ChildBorderSize = 1;
            style.PopupBorderSize = 1;
            style.FrameBorderSize = 1;
            style.TabBorderSize = 1;
            style.WindowRounding = 7;
            style.ChildRounding = 4;
            style.FrameRounding = 3;
            style.PopupRounding = 4;
            style.ScrollbarRounding = 9;
            style.GrabRounding = 3;
            style.LogSliderDeadzone = 4;
            style.TabRounding = 2;

            //  SOCOM THEME
            colors[ImGuiCol_Text] = ImVec4(0.70f, 0.71f, 0.71f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.31f, 0.33f, 0.79f);
            colors[ImGuiCol_Button] = ImVec4(0.08f, 0.56f, 0.58f, 0.29f);
            colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.41f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.21f, 0.28f, 0.31f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.74f, 0.74f, 0.29f, 0.40f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.00f, 0.63f, 0.43f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.74f, 0.74f, 0.29f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

            /*
            //  SOCOM THEME
            colors[ImGuiCol_Text] = ImVec4(0.70f, 0.71f, 0.71f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.31f, 0.33f, 0.39f);
            colors[ImGuiCol_Button] = ImVec4(0.08f, 0.56f, 0.58f, 0.29f);
            colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.41f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.21f, 0.28f, 0.31f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.74f, 0.74f, 0.29f, 0.40f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.00f, 0.63f, 0.43f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.74f, 0.74f, 0.29f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
            */
            if (g_Menu->dbg_RAINBOW_THEME) {
                colors[ImGuiCol_Separator] = g_Menu->dbg_RAINBOW;
                colors[ImGuiCol_TabActive] = g_Menu->dbg_RAINBOW;
            }
            else {
                colors[ImGuiCol_Separator] = ImVec4(1.00f, 0.90f, 0.45f, 1.00f);
                colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
            }
        }
    }

    namespace Tabs {

        void TABGameFixes()
        {
            /// INTENDED FEATURES
            //  RENDER FIX
            //  FRAMERATE
            //  WIDESCREEN
            //  MAP BRIGHTNESS
            //  NVG FIX (Bismo)

            if (ImGui::Toggle("Harry62 RenderFix", &g_GameData->b_RFIX)) {
                switch (g_GameData->b_RFIX) {
                case(TRUE):
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_RenderFix, 268435675, 1);
                    break;
                case(FALSE):
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_RenderFix, 274727131, 1);
                    break;
                }
                g_Console->LogEvent("[+] GAMEFIXES::MENU::HARRY62 RENDER FIX - ", g_GameData->b_RFIX);
            }

            if (ImGui::Toggle("PUllReq RenderFix", &g_GameData->b_RFIX2)) {
                if (g_GameData->b_RFIX2)
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_RenderFix2, NULL, 1);
                else if (!g_GameData->b_RFIX2)
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_RenderFix2, 202204360, 1);
                g_Console->LogEvent("[+] GAMEFIXES::MENU::NIGHTFYRE RENDER FIX - ", g_GameData->b_RFIX2);
            }

            if (ImGui::Toggle("60 FPS", &g_GameData->bFPS)) {
                switch (g_GameData->bFPS) {
                case(TRUE):
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_FrameRate1, 60, 1);
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_FrameRate2, 60, 1);
                    break;
                case(FALSE):
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_FrameRate1, 30, 1);
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.s_FrameRate2, 30, 1);
                    break;
                }
                g_Console->LogEvent("[+] GAMEFIXES::MENU::60FPS - ", g_GameData->bFPS);
            }

            if (ImGui::Toggle("NO FOG", &g_GameData->bFOG)) {
                switch (g_GameData->bFOG) {
                case(TRUE):
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.Fog, 0, 1);
                    break;
                case(FALSE):
                    g_GameFunctions->PS2Patch(g_GameData->Offsets.Fog, 1, 1);
                    break;
                }
                g_Console->LogEvent("[+] GAMEFIXES::MENU::NO FOG - ", g_GameData->bFOG);
            }

            if (ImGui::Toggle("IMPROVED NVG", &g_GameData->bNVGFILTER)) {
                switch (g_GameData->bNVGFILTER) {
                case(TRUE):
                    PS2Patch<int32_t>(g_GameData->Offsets.s_NVGFilter, 0);
                    break;
                case(FALSE):
                    PS2Patch<int32_t>(g_GameData->Offsets.s_NVGFilter, 0x0C065DD0);
                    break;
                }
                g_Hooking->recResetEE();
                g_Console->LogEvent("[+] GAMEFIXES::MENU::IMPROVED NVG - ", g_GameData->bNVGFILTER);
            }

            if (ImGui::Toggle("WIDESCREEN", &g_GameData->bWIDESCREEN)) { 
                //switch (g_GameData->bWIDESCREEN) {
                //case(TRUE):
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen1, 0.8);
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen2, 1800.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen3, 1820.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen4, 1.15);
                //    PS2Patch<float>(g_GameData->Offsets.s_CompassX, 630);
                //    PS2Patch<float>(g_GameData->Offsets.s_CompassY, 85);
                //    break;
                //case(FALSE):
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen1, 1.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen2, 1728.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen3, 1824.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_widescreen4, 1.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_CompassX, 565.0);
                //    PS2Patch<float>(g_GameData->Offsets.s_CompassY, 100.0);
                //    break;
                //}
                //g_Hooking->recResetEE();
                //g_Console->LogEvent("[+] GAMEFIXES::MENU::WIDESCREEN - ", g_GameData->bWIDESCREEN);
            }
            if (g_GameData->bWIDESCREEN)
            {
                ImGui::SliderFloat("HUD SCALE", &*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_widescreen1), 0, 2, "%0.2f");
                ImGui::SliderFloat("HUD PLACEMENT X", &*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_widescreen2), 1600, 1900, "%0.2f");
                ImGui::SliderFloat("HUD PLACEMENT Y", &*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_widescreen3), 1600, 1900, "%0.2f");
                ImGui::SliderFloat("CAMERA SCALE", &*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_widescreen4), 0, 2, "%0.2f");
                ImGui::SliderFloat("COMPASS X", &*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_CompassX), 0, 1000, "%0.2f");
                ImGui::SliderFloat("COMPASS Y", &*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_CompassY), 0, 500, "%0.2f");
            }

            if (ImGui::Toggle("GameWorld BRIGHTNESS", &g_GameData->bBRIGHTNESS)) g_Console->LogEvent("[+] GAMEFIXES::MENU::BRIGHTNESS - ", g_GameData->bBRIGHTNESS);
            if (g_GameData->bBRIGHTNESS) {
                ImGui::SliderFloat("##MAP", &g_GameData->e_Brightness, 0, 30);
                g_GameData->buf = g_GameData->e_Brightness;
                *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.MapBrightness1) = (float)g_GameData->e_Brightness;
                g_Console->printdbg("MAP BRIGHTNESS: [%f]\r", g_Console->color.DEFAULT, g_GameData->buf);
            }
        }

        void TABCharacterStats()
        {
            if (!g_GameData->Valid()) {
                ImGui::Text("[!] PLAYER OBJECT INVALID");
                return;
            }

            //  Automatically Splits the content region in half
            ImGui::BeginColumns("TEST_COLUMNS", 2);
            {
                ImGui::Text("PLAYER INFO");
                ImGui::Spacing();
                ImGui::Text("Name: %s", g_GameData->CharName->NAME);
                ImGui::Text("Team: %s", g_GameFunctions->GetPlayerTeam(g_GameData->CharData->TeamID));
                ImGui::Text("Health: %.2f", g_GameData->CharData->playerHEALTH * 100);

                ImGui::Text("PosX   ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::SliderFloat("##PosX", &g_GameData->CharPhys->edit_POSITION.x, g_GameData->CharPhys->edit_POSITION.x - 50, g_GameData->CharPhys->edit_POSITION.x + 50, "%.2f");

                ImGui::Text("PosY   ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::SliderFloat("##PosY", &g_GameData->CharPhys->edit_POSITION.y, g_GameData->CharPhys->edit_POSITION.y - 50, g_GameData->CharPhys->edit_POSITION.y + 50, "%.2f");

                ImGui::Text("PosZ   ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::SliderFloat("##PosZ", &g_GameData->CharPhys->edit_POSITION.z, g_GameData->CharPhys->edit_POSITION.z - 50, g_GameData->CharPhys->edit_POSITION.z + 50, "%.2f");
                ImGui::Spacing();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::Combo("##a", &g_GameData->SELECTED_WEAPON, g_GameData->LIST_OPTIONS, IM_ARRAYSIZE(g_GameData->LIST_OPTIONS));
                switch (g_GameData->SELECTED_WEAPON) {
                case(0):
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::Combo("##b", &g_GameData->bSELECTED_PRIMARY, g_GameData->LIST_pMAGS, IM_ARRAYSIZE(g_GameData->LIST_pMAGS));
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::InputInt("##b1", &g_GameData->SELECTED_MAG_COUNT);
                    break;
                case(1):
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::Combo("##c", &g_GameData->bSELECTED_SECONDARY, g_GameData->LIST_sMAGS, IM_ARRAYSIZE(g_GameData->LIST_sMAGS));
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::InputInt("##c1", &g_GameData->SELECTED_MAG_COUNT);
                    break;
                case(2):
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::Combo("##d", &g_GameData->bSELECTED_EQSLOT, g_GameData->LIST_eqSLOTS, IM_ARRAYSIZE(g_GameData->LIST_eqSLOTS));
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
                    ImGui::InputInt("##d1", &g_GameData->SELECTED_MAG_COUNT);
                    break;
                }
                if (ImGui::Button("SEND", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
                    switch (g_GameData->bSELECTED_PRIMARY) {
                    case(0):
                        g_GameData->CharData->PrimaryMag1 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(1):
                        g_GameData->CharData->PrimaryMag2 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(2):
                        g_GameData->CharData->PrimaryMag3 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(3):
                        g_GameData->CharData->PrimaryMag4 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(4):
                        g_GameData->CharData->PrimaryMag5 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(5):
                        g_GameData->CharData->PrimaryMag6 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(6):
                        g_GameData->CharData->PrimaryMag7 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(7):
                        g_GameData->CharData->PrimaryMag8 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(8):
                        g_GameData->CharData->PrimaryMag9 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(9):
                        g_GameData->CharData->PrimaryMag10 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    }
                    switch (g_GameData->bSELECTED_SECONDARY) {
                    case(0):
                        g_GameData->CharData->SecondaryMag1 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(1):
                        g_GameData->CharData->SecondaryMag2 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(2):
                        g_GameData->CharData->SecondaryMag3 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(3):
                        g_GameData->CharData->SecondaryMag4 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(4):
                        g_GameData->CharData->SecondaryMag5 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(5):
                        g_GameData->CharData->SecondaryMag6 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(6):
                        g_GameData->CharData->SecondaryMag7 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(7):
                        g_GameData->CharData->SecondaryMag8 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(8):
                        g_GameData->CharData->SecondaryMag9 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(9):
                        g_GameData->CharData->SecondaryMag10 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    }
                    switch (g_GameData->bSELECTED_EQSLOT) {
                    case(0):
                        g_GameData->CharData->EquipmentSlot1 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(1):
                        g_GameData->CharData->EquipmentSlot2 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    case(2):
                        g_GameData->CharData->EquipmentSlot3 = g_GameData->SELECTED_MAG_COUNT;
                        break;
                    }
                }
            }
            ImGui::NextColumn();
            {
                ImGui::Text("WEAPON INFO");
                ImGui::Spacing();
                //ImGui::Text("Primary: ");
                //ImGui::Text("Secondary: ");
                //ImGui::Text("EQ1: ");
                //ImGui::Text("EQ2: ");
                //ImGui::Text("EQ3: ");
                if (ImGui::Toggle("CROSSHAIR COLOR", &g_GameData->bFOGCOLOR)) g_Console->LogEvent("[+] CHARSTATS::MENU::CROSSHAIR COLOR - ", g_GameData->bFPS);
                ImGui::SameLine();
                ImGui::ColorEdit4("CUSTOM##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | g_Menu->misc_flags);
                float a = (float)(color.x * 255);
                float b = (float)(color.y * 255);
                float c = (float)(color.z * 255);
                if (*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_crosshairR) != a
                    || *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_crosshairG) != b
                    || *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_crosshairB) != c)
                {
                    if (g_GameData->bFOGCOLOR) {
                        *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_crosshairR) = a;
                        *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_crosshairG) = b;
                        *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.s_crosshairB) = c;
                    }
                }
                if (ImGui::Toggle("RAPID FIRE", &g_GameData->bRAPIDFIRE)) g_Console->LogEvent("[+] CHARSTATS::MENU::RAPID FIRE - ", g_GameData->bRAPIDFIRE);
                if (ImGui::Toggle("AUTOMATIC GUNS", &g_GameData->bAUTOGUNS)) g_Console->LogEvent("[+] CHARSTATS::MENU::AUTOMATIC GUNS - ", g_GameData->bAUTOGUNS);
                if (ImGui::Toggle("INFINITE AMMO", &g_GameData->bINFINITEAMMO)) {
                    g_Console->LogEvent("[+] CHARSTATS::MENU:: INFINITE AMMO ; ", g_GameData->bINFINITEAMMO);
                    if (g_GameData->bINFINITEAMMO) {
                        g_GameData->AMMO_ARRAY[0] = g_GameData->CharData->PrimaryMag1;
                        g_GameData->AMMO_ARRAY[1] = g_GameData->CharData->PrimaryMag2;
                        g_GameData->AMMO_ARRAY[2] = g_GameData->CharData->PrimaryMag3;
                        g_GameData->AMMO_ARRAY[3] = g_GameData->CharData->SecondaryMag1;
                        g_GameData->AMMO_ARRAY[4] = g_GameData->CharData->SecondaryMag2;
                        g_GameData->AMMO_ARRAY[5] = g_GameData->CharData->SecondaryMag3;
                        g_GameData->AMMO_ARRAY[6] = g_GameData->CharData->EquipmentSlot1;
                        g_GameData->AMMO_ARRAY[7] = g_GameData->CharData->EquipmentSlot2;
                        g_GameData->AMMO_ARRAY[8] = g_GameData->CharData->EquipmentSlot3;
                        g_GameData->CURRENT_SHOTS_FIRED = g_GameData->CharData->ShotsFired;
                    }
                    else if (!g_GameData->bINFINITEAMMO) {
                        g_GameData->CharData->PrimaryMag1 = g_GameData->AMMO_ARRAY[0];
                        g_GameData->CharData->PrimaryMag2 = g_GameData->AMMO_ARRAY[1];
                        g_GameData->CharData->PrimaryMag3 = g_GameData->AMMO_ARRAY[2];
                        g_GameData->CharData->SecondaryMag1 = g_GameData->AMMO_ARRAY[3];
                        g_GameData->CharData->SecondaryMag2 = g_GameData->AMMO_ARRAY[4];
                        g_GameData->CharData->SecondaryMag3 = g_GameData->AMMO_ARRAY[5];
                        g_GameData->CharData->EquipmentSlot1 = g_GameData->AMMO_ARRAY[6];
                        g_GameData->CharData->EquipmentSlot2 = g_GameData->AMMO_ARRAY[7];
                        g_GameData->CharData->EquipmentSlot3 = g_GameData->AMMO_ARRAY[8];

                        for (int i = 0; i < IM_ARRAYSIZE(g_GameData->AMMO_ARRAY); i++) {
                            g_GameData->AMMO_ARRAY[i] = NULL;
                        }
                        g_GameData->CURRENT_SHOTS_FIRED = NULL;
                    }
                }
                if (ImGui::Toggle("PERFECT SHOT", &g_GameData->bPERFECTSHOT)) g_Console->LogEvent("[+] CHARSTATS::MENU::PERFECT SHOT - ", g_GameData->bPERFECTSHOT);
                if (ImGui::Toggle("PERFECT SCOPE", &g_GameData->bPERFECTSCOPE)) g_Console->LogEvent("[+] CHARSTATS::MENU::PERFECT SCOPE - ", g_GameData->bPERFECTSCOPE);
                if (ImGui::Toggle("AIMBOT", &g_GameData->bAIMBOT)) {
                    g_Console->LogEvent("[+] CHARSTATS::MENU::AIMBOT - ", g_GameData->bAIMBOT);
                    if (g_GameData->bAIMBOT) {
                        //  Turn Aim Assist Bool Off
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.AimAssistBool), (BYTE*)"\x01\x00\x00\x00", 4);

                        //  Target Distance
                        g_GameFunctions->Nop((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.TargetLockDistance), 4);

                        //  Perfect Shot
                        g_GameFunctions->Nop((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.PerfectWeapon), 4);

                        //  Infinite Ammo
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.decAmmoCount), (BYTE*)"\x00\x00\x63\x24", 4);
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.decEquipmentCount), (BYTE*)"\x00\x00\x42\x24", 4);
                        g_Hooking->recResetEE();
                    }
                    else if (!g_GameData->bAIMBOT) {
                        //  Turn Aim Assist Bool Off
                        g_GameFunctions->Nop((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.AimAssistBool), 4);

                        //  Target Distance
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.TargetLockDistance), (BYTE*)"\xDC\x40\x0A\x0C", 4);

                        //  Perfect Shot
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.PerfectWeapon), (BYTE*)"\x18\x08\xA2\xAE", 4);

                        //  Infinite Ammo
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.decAmmoCount), (BYTE*)"\xFF\xFF\x63\x24", 4);
                        g_GameFunctions->Patch((BYTE*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.decEquipmentCount), (BYTE*)"\xFF\xFF\x42\x24", 4);
                        g_Hooking->recResetEE();
                    }
                }
                ImGui::Toggle("SILENT AIM", &g_GameData->bSILENTAIM_RAGE);
            }
            ImGui::EndColumns();
        }

        void TABEntityStats()
        {
            if (!g_GameData->Valid()) {
                ImGui::Text("[!] PLAYER OBJECT INVALID");
                return;
            }

            ImGui::Text("LOBBY INFORMATION");
            ImGui::Text("ROOM NAME: ");
            ImGui::Text("HOST: ");
            ImGui::Text("MAP: ");
            ImGui::Text("MAP: ");

            ImGui::Button("FORCE START MATCH", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20));
            ImGui::Button("ALL MAPS RESPAWN", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20));
            ImGui::Button("ENDLESS MATCH", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20));

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();


            ImGui::BeginChild("##ENTITYARRAY", ImVec2(ImGui::GetContentRegionAvailWidth() - 3, 500), TRUE, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("ENTITIES: %d", g_GameData->arraySize);
            for (int i = 0; i <= g_GameData->arraySize - 1; i = i + 1) {
                ImGui::PushID(i);
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                std::string OBJECT_TEXT = g_GameData->EntNames[i]->NAME;
                if (ImGui::CollapsingHeader(OBJECT_TEXT.c_str()))
                {
                    ImGui::BeginColumns("TEST_COLUMNS", 2);
                    {
                        ImGui::Text("ENTITY INFO");
                        ImGui::Spacing();
                        ImGui::Text("HEALTH: %.2f", g_GameData->EntData[i]->Health * 100);
                        ImGui::Text("TEAM: %s", g_GameFunctions->GetPlayerTeam(g_GameData->EntData[i]->TeamID));
                        ImGui::Text("POS-X: %f", g_GameData->EntData[i]->Position.x);
                        ImGui::Text("POS-Y: %f", g_GameData->EntData[i]->Position.y);
                        ImGui::Text("POS-Z: %f", g_GameData->EntData[i]->Position.z);
                        ImGui::Text("DISTANCE: %f", g_GameFunctions->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->CharData->Position));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::Toggle("POOP ON", &g_Menu->bSILENTAIM);
                        if (g_Menu->bSILENTAIM && g_GameData->EntData[i]->Health != NULL)
                            g_GameData->SavedCoords = g_GameData->EntData[i]->Position;
                        else if (g_Menu->bSILENTAIM != FALSE)
                            g_Menu->bSILENTAIM = FALSE;

                        if (ImGui::Button("TELEPORT TO CROSSHAIR", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
                            g_GameData->EntPhys[i]->edit_POSITION = g_GameData->CharPhys->edit_POSITION;
                            g_Console->printdbg("[+] MENU::EntityList; TELEPORT TO CROSSHAIR\n", g_Console->color.green);
                            g_Console->printdbg("ORIGINAL COORDINATES:\nPosX: %f\nPosY: %f\nPosZ: %f\n\n", g_Console->color.DEFAULT, g_GameData->EntData[i]->Position.x,
                                g_GameData->EntData[i]->Position.y, g_GameData->EntData[i]->Position.z);
                            g_Console->printdbg("NEW COORDINATES:\nPosX: %f\nPosY: %f\nPosZ: %f\n\n", g_Console->color.DEFAULT, g_GameData->CharData->Position.x,
                                g_GameData->CharData->Position.y, g_GameData->CharData->Position.z);
                        }
                        ImGui::Spacing();
                        if (ImGui::Button("TELEPORT TO TARGET", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
                            g_GameData->CharPhys->edit_POSITION = g_GameData->EntPhys[i]->edit_POSITION;
                            g_Console->printdbg("[+] MENU::EntityList; TELEPORT TO TARGET\n", g_Console->color.green);
                            g_Console->printdbg("ORIGINAL COORDINATES:\nPosX: %f\nPosY: %f\nPosZ: %f\n\n", g_Console->color.DEFAULT, g_GameData->CharData->Position.x,
                                g_GameData->CharData->Position.y, g_GameData->CharData->Position.z);
                            g_Console->printdbg("NEW COORDINATES:\nPosX: %f\nPosY: %f\nPosZ: %f\n\n", g_Console->color.DEFAULT, g_GameData->EntData[i]->Position.x,
                                g_GameData->EntData[i]->Position.y, g_GameData->EntData[i]->Position.z);
                        }
                    }
                    ImGui::EndColumns();
                }
                ImGui::PopID();
                ImGui::Spacing();
            }
            ImGui::EndChild();
        }

        void TABVisuals()
        {
            if (ImGui::Toggle("DRAW FPS", &g_Menu->dbg_FPS)) g_Console->LogEvent("[+] VISUALS::MENU::DRAW FPS - ", g_Menu->dbg_FPS);
            if (g_Menu->b_FPS) {
                g_Menu->dbg_FPS = TRUE;
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50);
                ImGui::InputFloat("##", &g_Menu->f_FPS, 0, 999);
                ImGui::SameLine();
                if (ImGui::Button("+"))
                    g_Menu->f_FPS = g_Menu->f_FPS + 1;
                ImGui::SameLine();
                if (ImGui::Button("-"))
                    g_Menu->f_FPS = g_Menu->f_FPS - 1;
                ImGui::SameLine();
                if (ImGui::Button("RESET"))
                    g_Menu->f_FPS = 60;
            }

            if (ImGui::Toggle("DRAW CROSSHAIR", &g_Menu->m_dbgCrosshair)) g_Console->LogEvent("[+] VISUALS::MENU::CROSSHAIR - ", g_Menu->m_dbgCrosshair);
            if (g_Menu->m_dbgCrosshair)
            {
                if (ImGui::Toggle("RGB Crosshair", &g_Menu->m_RGB_CROSSHAIR)) g_Console->LogEvent("[+] VISUALS::MENU::RGB CROSSHAIR - ", g_Menu->m_RGB_CROSSHAIR);

                //  CROSSHAIR COLOR
                ImGui::Text("CROSSHAIR COLOR");
                ImGui::SameLine();
                ImGui::ColorEdit4("CUSTOM##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | g_Menu->misc_flags);
                ImColor cBuf{ (float)color.x, (float)color.y, (float)color.z };
                if (ImColor(g_Menu->dbg_crosshair_color) != ImColor(cBuf))
                    g_Menu->dbg_crosshair_color = ImColor(cBuf);

                //  SIZE
                ImGui::Text("CROSSHAIR SIZE:      ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::SliderFloat("##SIZE ", &g_Menu->dbg_crosshair_radius, 1, 100, "%.2f");
                ImGui::Spacing();

                ImGui::Text("CROSSHAIR SEGMENTS:  ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::SliderInt("##SIZE2 ", &g_Menu->dbg_crosshair_segments, 0, 12, "%.2f");
                ImGui::Spacing();

                ImGui::Text("CROSSHAIR THICKNESS: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75);
                ImGui::SliderFloat("##SIZE3 ", &g_Menu->dbg_crosshair_thickness, 0, 10, "%.2f");
            }
            ImGui::Separator();

            //  ESP VIEW MATRIX DEBUG
            ImGui::Text("VIEWMATRIX TESTER");
            ImGui::Spacing();
            ImGui::Text("ADDRESS: %llX", (g_GameData->BasePS2MemorySpace + g_GameVariables->INPUT_VIEWMATRIX));
            ImGui::Text("INPUT: %d", g_GameVariables->INPUT_VIEWMATRIX);
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("WINDOW WIDTH", &g_GameData->m_dbg_window.x);
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("WINDOW HEIGHT", &g_GameData->m_dbg_window.y);
            ImGui::Text("ADDRESS (INT): ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputInt("##VIEWMATRIX _TEST_INPUT", &g_GameData->matrixBUF);    // Input buffer
            ImGui::SameLine();
            if (ImGui::Button("SEND", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) g_GameVariables->INPUT_VIEWMATRIX = g_GameData->matrixBUF;
            if (g_GameVariables->INPUT_VIEWMATRIX == NULL || g_GameVariables->INPUT_VIEWMATRIX > 0xFFFFFFFF)
                ImGui::TextColored(ImColor(255, 0, 0, 255), "INVALID INPUT!");
            if (ImGui::Toggle("ESP TEST", &g_Menu->b_ESP)) g_Console->LogEvent("[+] VISUALS::MENU::ESP - ", g_Menu->b_ESP);
            if (g_Menu->b_ESP) {
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("POSITION", &g_GameData->m_dbg_draw_pos, "Heaven\0Hell\0Corner\0Center\0");
            }
            ImGui::Separator();

            ImGui::Text("AUTOMATIC MODE");
            ImGui::Spacing();
            ImGui::Text("COUNT: %d", g_GameVariables->count);
            ImGui::Text("ADDRESS(int): %d", g_GameVariables->VIEWMATRIX_TEST);
            ImGui::Text("ADDRESS: %llX", (g_GameData->BasePS2MemorySpace + g_GameVariables->VIEWMATRIX_TEST));
            if (ImGui::Button("- 4")) g_GameVariables->VIEWMATRIX_TEST = g_GameVariables->VIEWMATRIX_TEST - 4;
            ImGui::SameLine();
            if (ImGui::Button("+ 4")) g_GameVariables->VIEWMATRIX_TEST = g_GameVariables->VIEWMATRIX_TEST + 4;
            ImGui::SameLine();
            if (ImGui::Button("RESET")) g_GameVariables->VIEWMATRIX_TEST = g_GameData->matrixBUF;
            ImGui::SameLine();
            if (ImGui::Toggle("LOOP", &g_GameVariables->bLOOP)) {
                if (g_GameVariables->bLOOP)
                    g_GameVariables->VIEWMATRIX_TEST = g_GameData->matrixBUF;
                g_Console->LogEvent("[+] VISUALS::ViewMatrixFinder::Loop - ", g_GameVariables->bLOOP);
            }
        }

        void TABDebugOptions()
        {
            ImGui::Text("SOCOM 2 MENU (PREVIEW)");
            ImGui::Text("BUILD VERSION: alpha-0.0.5");
            ImGui::Text("BUILD DATE: 6/24/2022");
            ImGui::Text("GAME VERSION: r0004");
            ImGui::Separator();

            if (ImGui::Toggle("RGB THEME", &g_Menu->dbg_RAINBOW_THEME)) g_Console->LogEvent("[+] DEBUG::MENU::RGB THEME - ", g_Menu->dbg_RAINBOW_THEME);
            ImGui::Spacing();
            if (ImGui::Toggle("SHOW CONSOLE | ", &g_Console->m_ShowConsole)) g_Console->LogEvent("[+] DEBUG::MENU::ShowConsole - ", g_Console->m_ShowConsole);
            ImGui::SameLine();
            if (ImGui::Checkbox("Verbose Logging", &g_Console->verbose)) g_Console->LogEvent("[+] DEBUG::CONSOLE::VerboseLogging - ", g_Console->verbose);
            ImGui::Spacing();
            if (ImGui::Toggle("CONTROLLER INPUT", &g_GameVariables->m_CONTROLLER)) g_Console->LogEvent("[+] DEBUG::MENU::ControllerInput - ", g_GameVariables->m_CONTROLLER);
            ImGui::Spacing();
            if (ImGui::Toggle("SHOW IMGUI::DEMO WINDOW", &g_GameVariables->m_ShowDemo)) g_Console->LogEvent("[+] DEBUG::MENU::ShowDemoWindow - ", g_GameVariables->m_ShowDemo);
            ImGui::Spacing();
            if (ImGui::Toggle("SHOW IMGUI::STYLE EDITOR", &g_GameVariables->m_ShowStyleEditor)) g_Console->LogEvent("[+] DEBUG::ImGui::ShowStyleEditor - ", g_GameVariables->m_ShowStyleEditor);
            ImGui::Separator();
            if (ImGui::Button("UPDATE ENTITY ARRAY", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) {
                g_GameData->EntityArrayUpdate();
                printf("MENU:: UpdateEntityArray");
            }
            if (ImGui::Button("INITIALIZE STYLE", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) {
                Styles::InitStyle();
                g_Console->LogEvent("DEBUG::InitializeStyle - ", NULL);
            }
            if (ImGui::Button("UNHOOK MODULE", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) g_Killswitch = TRUE;
            ImGui::Separator();
            if (ImGui::Button("QUIT GAME", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) TerminateProcess(g_GameVariables->m_GameHandle, EXIT_SUCCESS);
        }
    }

    //  MAIN MENU LOOP
    void Menu::Draw()
    {
        //  For style editing
        if (!g_GameVariables->m_ShowDemo && !g_GameVariables->m_ShowStyleEditor)
            Styles::InitStyle();

        // MAIN WINDOW
        if (g_GameVariables->m_ShowMenu)
            MainMenu();

        //  NOTIFICATION WINDOW
        if (g_GameVariables->m_ShowHud)
            HUD();

        //  IMGUI EDITORS
        if (g_GameVariables->m_ShowDemo)
            ImGui::ShowDemoWindow(&g_GameVariables->m_ShowDemo);
        if (g_GameVariables->m_ShowStyleEditor)
            ImGui::ShowStyleEditor();

        //  CONSOLE SWITCH
        switch (g_Console->m_ShowConsole) {
        case(TRUE):
            if (!g_Console->ACTIVE) {
                ::ShowWindow(GetConsoleWindow(), SW_SHOW);
                g_Console->ACTIVE = TRUE;

            }
            break;
        case(FALSE):
            if (g_Console->ACTIVE) {
                ::ShowWindow(GetConsoleWindow(), SW_HIDE);
                g_Console->ACTIVE = FALSE;
            }
            break;
        }

        //  DEBUG FPS
        if (dbg_FPS)
            g_GameFunctions->FPS();

        //  DEBUG DRAW CROSSHAIR
        if (m_dbgCrosshair) {
            if (m_RGB_CROSSHAIR)
                ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMainViewport()->GetCenter(), g_Menu->dbg_crosshair_radius, g_Menu->dbg_RAINBOW, g_Menu->dbg_crosshair_segments, g_Menu->dbg_crosshair_thickness);
            else if (!m_RGB_CROSSHAIR)
                ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMainViewport()->GetCenter(), g_Menu->dbg_crosshair_radius, ImColor(dbg_crosshair_color), g_Menu->dbg_crosshair_segments, g_Menu->dbg_crosshair_thickness);
        }

        if (b_ESP) {
            ESP(g_GameData->m_dbg_draw_pos, g_GameData->m_dbg_window);
            if (GetAsyncKeyState(VK_LEFT) & 1) g_GameVariables->INPUT_VIEWMATRIX = (g_GameVariables->INPUT_VIEWMATRIX - 4);
            if (GetAsyncKeyState(VK_RIGHT) & 1) g_GameVariables->INPUT_VIEWMATRIX = (g_GameVariables->INPUT_VIEWMATRIX + 4);
        }

    }

    void Menu::MainMenu()
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
        if (dbg_RAINBOW_THEME) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(g_Menu->dbg_RAINBOW));
        }
        if (!ImGui::Begin("SOCOM 2", &g_GameVariables->m_ShowMenu, 96))
        {
            ImGui::End();
            return;
        }
        if (dbg_RAINBOW_THEME) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("GAME FIXES"))
            {
                Tabs::TABGameFixes();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("PLAYER"))
            {
                Tabs::TABCharacterStats();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("VISUALS"))
            {
                Tabs::TABVisuals();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("MATCH MANAGER"))
            {
                Tabs::TABEntityStats();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("DEBUG"))
            {
                Tabs::TABDebugOptions();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    //  NOTIFICATION WINDOW
    void Menu::HUD()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        if (!ImGui::Begin("SOCOM 2 Internal Launch Window", NULL, 103))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("SOCOM 2 MENU LOADED");
        ImGui::Text("PRESS [NUM0], [NUMPAD0] OR [L3 + R3] TO SHOW THE MENU");
        ImGui::Separator();
        ImGui::TextColored(ImColor(255, 0, 0, 255), "MENU v0.0.5 | Released: Friday June 24, 2022");
        ImGui::End();
    }

    void Menu::MagicBulletInfo(bool* p_open)
    {

    }

    void Menu::Loops()
    {
        //  ENVIRONMENT
        if (g_GameData->bBRIGHTNESS) {
            if (*(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.MapBrightness1) != (float)g_GameData->e_Brightness)
                *(float*)g_GameFunctions->GetPS2Addr(g_GameData->Offsets.MapBrightness1) = (float)g_GameData->e_Brightness;
        }

        //  WEAPON 
        if (g_GameData->bRAPIDFIRE)
            g_GameData->CharData->WeaponRapid = NULL;

        if (g_GameData->bAUTOGUNS)
            g_GameData->CharData->WeaponAuto = NULL;

        if (g_GameData->bINFINITEAMMO)
        {
            //  There is a bullets shot offset, 
            //  we will need to freeze the value with our ammo count
            //  Store our current amo first so we can restore original data after
            g_GameData->CharData->PrimaryMag1 = 1;
            g_GameData->CharData->PrimaryMag2 = 1;
            g_GameData->CharData->PrimaryMag3 = 1;
            g_GameData->CharData->SecondaryMag1 = 1;
            g_GameData->CharData->SecondaryMag2 = 1;
            g_GameData->CharData->SecondaryMag3 = 1;
            g_GameData->CharData->EquipmentSlot1 = 1;
            g_GameData->CharData->EquipmentSlot2 = 1;
            g_GameData->CharData->EquipmentSlot3 = 1;
            g_GameData->CharData->ShotsFired = g_GameData->CURRENT_SHOTS_FIRED;
        }

        if (g_GameData->bPERFECTSHOT)
        {
            if (g_GameData->CharData->gunHOT == 1) {
                g_GameData->CharData->WeaponRecoil = NULL;
                g_GameData->CharData->WeaponBounce = NULL;
                g_GameData->CharData->WeaponSpread = NULL;
                g_GameData->CharData->crosshairSPREAD = NULL;
                g_GameData->CharData->crosshairSPREAD2 = NULL;
                g_GameData->CharData->ScopeRecoil1 = NULL;
                g_GameData->CharData->ScopeRecoil2 = NULL;
                g_GameData->CharData->ScopeSway1 = NULL;
                g_GameData->CharData->ScopeSway2 = NULL;
            }
        }

        if (g_GameData->bPERFECTSCOPE) {
            g_GameData->CharData->ScopeRecoil1 = NULL;
            g_GameData->CharData->ScopeRecoil2 = NULL;
            g_GameData->CharData->ScopeSway1 = NULL;
            g_GameData->CharData->ScopeSway2 = NULL;
        }

        if (g_Menu->bSILENTAIM)
            SilentAim(g_GameData->SavedCoords);

        if (g_GameData->bSILENTAIM_RAGE)
            RageSilentAim();
    }
}