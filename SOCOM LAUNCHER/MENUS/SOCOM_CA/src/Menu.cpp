#include "..//include/Menu.hpp"
namespace SOCOM_CA {

    //  RAGE KILL EVERYTHING
    void SilentAim(Vector3 Coords)
    {
        *(float*)g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_X) = Coords.x;
        *(float*)g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Y) = Coords.y;
        *(float*)g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Z) = Coords.z;
    }

    void distSilentAim(float Distance)
    {
        if (!g_GameData->Valid()) {
            g_GameData->bDIST_SILENTAIM = FALSE;
            return;
        }

        if (g_GameData->SealObject->Health == NULL) return;

        // Establish Projectile
        uintptr_t ProjectileX = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_X);
        uintptr_t ProjectileY = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Y);
        uintptr_t ProjectileZ = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Z);

        //// Loop Array
        for (int i = 0; i < g_GameData->arraySize; i++) {

            //  TEAMID CHECK
            if (g_GameData->EntData[i]->TeamID == g_GameData->Teams.TURRET) continue;
            if (g_GameData->EntData[i]->TeamID == g_GameData->Teams.TURRET2) continue;
            if (g_GameData->EntData[i]->TeamID == -1) continue;
            if (g_GameData->EntData[i]->TeamID == g_GameData->SealObject->TeamID) continue;

            //  Check Health
            if (g_GameData->EntData[i]->Health <= NULL) continue;

            //  Check Distance
            if (g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position) > Distance) continue;

            // Send projectile0 to Target
            *(float*)ProjectileX = g_GameData->EntData[i]->Position.x;
            *(float*)ProjectileY = g_GameData->EntData[i]->Position.y;
            *(float*)ProjectileZ = g_GameData->EntData[i]->Position.z;
        }
    }

    void TeleporttoCrosshair()
    {
        if (!g_GameData->Valid()) return;
        uintptr_t ProjectileX = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_X);
        uintptr_t ProjectileY = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Y);
        uintptr_t ProjectileZ = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Z);
        if (*(float*)ProjectileX == NULL && *(float*)ProjectileY == NULL && *(float*)ProjectileZ == NULL) return;
        g_GameData->SealObject->Position.x = *(float*)ProjectileX;
        g_GameData->SealObject->Position.y = *(float*)ProjectileY;
        g_GameData->SealObject->Position.z = *(float*)ProjectileZ;
        *(float*)ProjectileX = 0;
        *(float*)ProjectileY = 0;
        *(float*)ProjectileZ = 0;
    }

    void SilentAim_private()
    {
        if (!g_GameData->Valid()) return;

        // Establish Projectile
        uintptr_t ProjectileX = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_X);
        uintptr_t ProjectileY = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Y);
        uintptr_t ProjectileZ = g_GameData->GetPS2Addr(g_GameData->offsets.PROJECTILE_COORDS_Z);

        //// Loop Array
        for (int i = 0; i < g_GameData->arraySize; i++) {
            //  Check Team
            if (g_GameData->EntData[i]->TeamID == g_GameData->SealObject->TeamID) continue;

            //  Check Health
            if (g_GameData->EntData[i]->Health == NULL) continue;

            //  Check Distance
            //if (g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position) <= 100) continue;

            // Send projectile0 to Target
            *(float*)ProjectileX = g_GameData->EntData[i]->Position.x;
            *(float*)ProjectileY = g_GameData->EntData[i]->Position.y;
            *(float*)ProjectileZ = g_GameData->EntData[i]->Position.z;
        }
    }

    //  TEST Online Entity Manager
    void EntityManager(bool* TSilent, unsigned int TEAM)
    {
        for (int i = 0; i <= g_GameData->arraySize - 1; i++)
        {
            //  Health Filter
            if (g_GameData->bALIVE_FILTER && g_GameData->EntData[i]->Health == NULL) {
                if (TSilent)
                    TSilent = FALSE;
                continue;
            }

            if (g_GameData->EntData[i]->TeamID != TEAM) continue;
            std::string OBJECT_NAME = std::string(g_GameData->EntNames[i]->NAME) + "##" + std::to_string(i);
            std::string SILENT_AIM = "POOP ON##" + std::to_string(i);
            std::string TELEPORT = "TELEPORT TO PLAYER##" + std::to_string(i);
            std::string TELEPORT2 = "TELEPORT TARGET TO PLAYER##" + std::to_string(i);
            if (ImGui::CollapsingHeader(OBJECT_NAME.c_str())) {
                ImGui::Text("[debug] TEAM ID: %X", g_GameData->EntData[i]->TeamID);
                ImGui::Text("TEAM: %s", g_GameData->GetPlayerTeam(g_GameData->EntData[i]->TeamID));
                ImGui::Text("HEALTH: %.0f", g_GameData->EntData[i]->Health * 100);
                ImGui::Text("DISTANCE: %f", g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position));
                ImGui::Text("POS-X: %f", g_GameData->EntData[i]->Position.x);
                ImGui::Text("POS-Y: %f", g_GameData->EntData[i]->Position.y);
                ImGui::Text("POS-Z: %f", g_GameData->EntData[i]->Position.z);
                if (ImGui::Checkbox(SILENT_AIM.c_str(), TSilent)) g_Console->LogEvent("[+] MATCHMANAGER::MagicBullet - ", TSilent);
                if (TSilent)
                    g_GameData->TARGET = g_GameData->EntData[i]->Position;
                if (ImGui::Button(TELEPORT.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                    g_GameData->SealObject->Position = g_GameData->EntData[i]->Position;
                if (ImGui::Button(TELEPORT2.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                    g_GameData->EntData[i]->Position = g_GameData->SealObject->Position;
                ImGui::Separator();
            }
        }
    }

    //  TEST Single Player Entity Manager
    void EntityManager_SP(bool* TSilent, unsigned int TEAM, unsigned int TEAM2)
    {
        for (int i = 0; i <= g_GameData->arraySize - 1; i++)
        {
            //  HEALTH CHECK w/ TSILENT CHECK
            if (g_GameData->bALIVE_FILTER && g_GameData->EntData[i]->Health == NULL) {
                if (TSilent)
                    TSilent = FALSE;
                continue;
            }
            if (g_GameData->EntData[i]->TeamID == TEAM) continue;
            if (g_GameData->EntData[i]->TeamID == TEAM2) continue;
            std::string OBJECT_NAME = std::string(g_GameData->EntNames[i]->NAME) + "##" + std::to_string(i);
            std::string SILENT_AIM = "MAGIC BULLET##" + std::to_string(i);
            std::string TELEPORT = "TELEPORT TO TARGET##" + std::to_string(i);
            std::string TELEPORT2 = "TELEPORT TARGET TO PLAYER##" + std::to_string(i);
            if (ImGui::CollapsingHeader(OBJECT_NAME.c_str())) {
                ImGui::Text("[debug] TEAM ID: %X", g_GameData->EntData[i]->TeamID);
                ImGui::Text("TEAM: %s", g_GameData->GetPlayerTeam(g_GameData->EntData[i]->TeamID));
                ImGui::Text("HEALTH: %.0f", g_GameData->EntData[i]->Health * 100);
                ImGui::Text("DISTANCE: %f", g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position));
                ImGui::Text("POS-X: %f", g_GameData->EntData[i]->Position.x);
                ImGui::Text("POS-Y: %f", g_GameData->EntData[i]->Position.y);
                ImGui::Text("POS-Z: %f", g_GameData->EntData[i]->Position.z);
                if (ImGui::Checkbox(SILENT_AIM.c_str(), TSilent)) g_Console->LogEvent("[+] MATCHMANAGER::MagicBullet - ", TSilent);
                if (TSilent)
                    g_GameData->TARGET = g_GameData->EntData[i]->Position;
                if (ImGui::Button(TELEPORT.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                    g_GameData->SealObject->Position = g_GameData->EntData[i]->Position;
                if (ImGui::Button(TELEPORT2.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                    g_GameData->EntData[i]->Position = g_GameData->SealObject->Position;
                ImGui::Separator();
            }
        }
    }

	namespace Styles {
        void InitStyle()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec4* colors = ImGui::GetStyle().Colors;

            //	STYLE PROPERTIES
            style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
            style.FrameRounding = 1.0f;
            style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
            style.WindowRounding = 4.0f;
            style.FrameBorderSize = 0.75f;
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
            style.TabBorderSize = 1;
            style.ChildRounding = 4;
            style.PopupRounding = 4;
            style.ScrollbarRounding = 9;
            style.LogSliderDeadzone = 4;
            style.TabRounding = 0;
            colors[ImGuiCol_Text] = ImVec4(0.69f, 0.62f, 0.34f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.12f, 0.08f, 0.84f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.58f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
            colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.23f, 0.27f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.38f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.00f, 0.63f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.23f, 0.00f, 0.53f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.27f, 0.00f, 0.63f, 0.78f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.27f, 0.00f, 0.63f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.27f, 0.30f, 0.37f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.39f, 0.00f, 0.90f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.55f, 0.68f, 0.34f, 0.40f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.33f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.00f, 0.90f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.27f, 0.00f, 0.63f, 0.76f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.00f, 0.63f, 0.86f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.00f, 0.90f, 1.00f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.27f, 0.00f, 0.63f, 0.78f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.27f, 0.00f, 0.63f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.27f, 0.40f);
            colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
            colors[ImGuiCol_TabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.19f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.27f, 0.40f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.22f, 0.27f, 0.70f);
            colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.27f, 0.00f, 0.63f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.27f, 0.00f, 0.63f, 1.00f);
            colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
            colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.00f, 0.63f, 0.43f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.08f, 0.12f, 0.08f, 0.05f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

            //	COLORS
            if (g_Menu->dbg_RAINBOW_THEME) {
                colors[ImGuiCol_Separator] = ImVec4(g_Menu->dbg_RAINBOW);
                colors[ImGuiCol_TitleBg] = ImVec4(0, 0, 0, 1.0f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0, 0, 0, 1.0f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0, 0, 0, 1.0f);
            }
            else {
                colors[ImGuiCol_TitleBg] = ImVec4(0.24f, 0.31f, 0.24f, 1.00f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.24f, 0.31f, 0.24f, 1.00f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
                colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
            }
        }
	}

	namespace Tabs {
        void TABMain()
        {
            ImGui::TextCentered("WEAPON");
            ImGui::Spacing();
            if (ImGui::Toggle("SEMI-PERFECT SHOT", &g_GameData->bPERFECTSHOT2)) g_Console->LogEvent("[+] ENHANCEMENT::WEAPON::SEMI PERFECT SHOT - ", g_GameData->bPERFECTSHOT2);
            if (ImGui::Toggle("MAGIC BULLET", &g_GameData->bDIST_SILENTAIM)) g_Console->LogEvent("[+] ENHANCEMENT::WEAPONL::DISTANCE SILENT AIM - ", g_GameData->bDIST_SILENTAIM);
            if (g_GameData->bDIST_SILENTAIM) {
                ImGui::SameLine();
                ImGui::Text("     ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                if (ImGui::SliderFloat("DISTANCE", &g_GameData->AIM_DISTANCE, 0, 200, "%.02f")) printf("MAGIC BULLET DISTANCE: [%f]\r", g_GameData->AIM_DISTANCE);
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::TextCentered("ENVIRONMENT");
            ImGui::Spacing();
            if (ImGui::Toggle("NO FOG", &g_GameData->bFOG))
            {
                switch (g_GameData->bFOG) {
                case(TRUE):
                    g_GameData->PS2Patch(g_GameData->offsets.FOG, 0, 1);
                    break;
                case(FALSE):
                    g_GameData->PS2Patch(g_GameData->offsets.FOG, 1, 1);
                    break;
                }
                g_Console->LogEvent("[+] ENHANCEMENT::ENVIRONMENT::FOG - ", g_GameData->bFOG);
            }
            if (ImGui::Toggle("FPS", &g_GameData->bFPS))
            {
                switch (g_GameData->bFPS) {
                case(TRUE):
                    g_GameData->PS2Patch(g_GameData->offsets.FPS, 60, 1);
                    break;
                case(FALSE):
                    g_GameData->PS2Patch(g_GameData->offsets.FPS, 30, 1);
                    break;
                }
                g_Console->LogEvent("[+] ENHANCEMENT::ENVIRONMENT::FPS - ", g_GameData->bFPS);
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::TextCentered("RAGE");
            ImGui::Spacing();
            if (ImGui::Toggle("PERFECT SHOT", &g_GameData->bPERFECTSHOT)) g_Console->LogEvent("[+] ENHANCEMENT::RAGE::PERFECT SHOT - ", g_GameData->bPERFECTSHOT);
            if (ImGui::Toggle("SILENT AIM", &g_GameData->bSILENTAIM)) g_Console->LogEvent("[+] ENHANCEMENT::RAGE::SILENT AIM - ", g_GameData->bSILENTAIM);
            if (ImGui::Toggle("TELEPORT TO PROJECTILE", &g_GameData->TELEPORT_TO_PROJECTILE)) g_Console->LogEvent("[+] ENHANCEMENT::RAGE::TELEPORT TO PROJECTILE - ", g_GameData->TELEPORT_TO_PROJECTILE);
        }

        void TABEntities()
        {
            if (!g_GameData->Valid()) {
                ImGui::TextColored(ImColor(169, 169, 0, 200), "[!] PLAYER OBJECT INVALID OR NOT IN A MATCH");
                return;
            }
            ImGui::TextCentered("ONLY OPEN 1 ENTITY INFO AT A TIME!", TRUE, ImColor(200, 0, 0, 200));
            if (!g_GameVariables->m_Multiplayer)
                if (ImGui::Checkbox("CAMPAIGN", &g_GameVariables->m_SinglePlayer)) g_Console->LogEvent("[+] MATCHMANAGER::Campaign - ", g_GameVariables->m_SinglePlayer);

            if (!g_GameVariables->m_SinglePlayer)
                if (ImGui::Checkbox("ONLINE", &g_GameVariables->m_Multiplayer)) g_Console->LogEvent("[+] MATCHMANAGER::Online - ", g_GameVariables->m_Multiplayer);

            //ImGui::Text("MATCH STATS");
            //ImGui::Text("SEAL TEAM: %d", g_GameData->arraySize);
            //ImGui::Text("enemies: %d", g_GameData->arraySize);
            //ImGui::Text("total entities: %d", g_GameData->arraySize);
            //ImGui::Text("alive entities: %d", g_GameData->arraySize);
            //ImGui::Separator();

            if (g_GameVariables->m_SinglePlayer) 
            {   
                if (ImGui::Checkbox("ALIVE PLAYERS", &g_GameData->bALIVE_FILTER)) g_Console->LogEvent("[+] MATCHMANAGER::AlivePlayers - ", g_GameData->bALIVE_FILTER);
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextCentered("SEAL TEAM");
                ImGui::Spacing();
                for (int i = 0; i <= g_GameData->arraySize; i++)
                {
                    //  Health Filter
                    if (g_GameData->bALIVE_FILTER && g_GameData->EntData[i]->Health == NULL) {
                        //  Toggle Silent Aim Off
                        if (g_GameData->bSILENTAIM_SEAL)
                            g_GameData->bSILENTAIM_SEAL = FALSE;
                        continue;
                    }
                    if ((g_GameData->EntData[i]->TeamID == g_GameData->Teams.CAMPAIGN) || (g_GameData->EntData[i]->TeamID == g_GameData->Teams.CAMPAIGN2)) 
                    {
                        std::string OBJECT_NAME = std::string(g_GameData->EntNames[i]->NAME) + "##" + std::to_string(i);
                        std::string SILENT_AIM = "POOP ON##" + std::to_string(i);
                        std::string TELEPORT = "TELEPORT TO TARGET##" + std::to_string(i);
                        std::string TELEPORT2 = "TELEPORT TARGET TO PLAYER##" + std::to_string(i);
                        if (ImGui::CollapsingHeader(OBJECT_NAME.c_str())) {
                            ImGui::Text("[debug] TEAM ID: %X", g_GameData->EntData[i]->TeamID);
                            ImGui::Text("TEAM: %s", g_GameData->GetPlayerTeam(g_GameData->EntData[i]->TeamID));
                            ImGui::Text("HEALTH: %.0f", g_GameData->EntData[i]->Health * 100);
                            ImGui::Text("DISTANCE: %f", g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position));
                            ImGui::Text("POS-X: %f", g_GameData->EntData[i]->Position.x);
                            ImGui::Text("POS-Y: %f", g_GameData->EntData[i]->Position.y);
                            ImGui::Text("POS-Z: %f", g_GameData->EntData[i]->Position.z);
                            ImGui::Checkbox(SILENT_AIM.c_str(), &g_GameData->bSILENTAIM_SEAL);
                            if (g_GameData->bSILENTAIM_SEAL)
                                g_GameData->TARGET = g_GameData->EntData[i]->Position;
                            if (ImGui::Button(TELEPORT.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                                g_GameData->SealObject->Position = g_GameData->EntData[i]->Position;
                            if (ImGui::Button(TELEPORT2.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                                g_GameData->EntData[i]->Position = g_GameData->SealObject->Position;
                            ImGui::Separator();
                        }
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextCentered("ENEMY FORCES");
                ImGui::Spacing();
                EntityManager_SP(&g_GameData->bSILENTAIM_TERR, g_GameData->Teams.CAMPAIGN, g_GameData->Teams.CAMPAIGN2);
            }
            else if (g_GameVariables->m_Multiplayer)
            {
                if (ImGui::Checkbox("ALIVE PLAYERS", &g_GameData->bALIVE_FILTER)) g_Console->LogEvent("[+] MATCHMANAGER::AlivePlayers - ", g_GameData->bALIVE_FILTER);
                ImGui::TextCentered("HOST OPTIONS");
                ImGui::Spacing();
                if (ImGui::Toggle("FORCE START", &g_GameData->bFORCESTART)) {
                    switch (g_GameData->bFORCESTART) {
                    case(TRUE):
                        g_GameData->PS2Patch(g_GameData->offsets.FORCE_START, 2, 1);
                        break;
                    case(FALSE):
                        g_GameData->PS2Patch(g_GameData->offsets.FORCE_START, 11880836, 1);
                        break;
                    }
                    g_Console->LogEvent("[+] MATCHMANAGER::HOST::FORCE START - ", g_GameData->bFORCESTART);
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextCentered("SEALS");
                ImGui::Spacing();
                EntityManager(&g_GameData->bSILENTAIM_SEAL, g_GameData->Teams.SEALS);
                //for (int i = 0; i <= g_GameData->arraySize - 1; i++)
                //{
                //    //  Health Filter
                //    if (g_GameData->bALIVE_FILTER && g_GameData->EntData[i]->Health == NULL) {
                //        if (g_GameData->bSILENTAIM_SEAL)
                //            g_GameData->bSILENTAIM_SEAL = FALSE;
                //        continue;
                //    }
                //    if (g_GameData->EntData[i]->TeamID == g_GameData->Teams.SEALS) {
                //        std::string OBJECT_NAME = "[" + std::to_string(i) + "] " + std::string(g_GameData->EntNames[i]->NAME);
                //        std::string SILENT_AIM = "[" + std::to_string(i) + "] " + "POOP ON";
                //        if (ImGui::CollapsingHeader(OBJECT_NAME.c_str())) {
                //            ImGui::Text("TEAM: %s", g_GameData->GetPlayerTeam(g_GameData->EntData[i]->TeamID));
                //            ImGui::Text("HEALTH: %.0f", g_GameData->EntData[i]->Health * 100);
                //            ImGui::Text("DISTANCE: %f", g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position));
                //            ImGui::Text("POS-X: %f", g_GameData->EntData[i]->Position.x);
                //            ImGui::Text("POS-Y: %f", g_GameData->EntData[i]->Position.y);
                //            ImGui::Text("POS-Z: %f", g_GameData->EntData[i]->Position.z);
                //            ImGui::Checkbox(SILENT_AIM.c_str(), &g_GameData->bSILENTAIM_SEAL);
                //            if (g_GameData->bSILENTAIM_SEAL)
                //                g_GameData->TARGET = g_GameData->EntData[i]->Position;
                //            if (ImGui::Button("TELEPORT TO PLAYER", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                //                g_GameData->SealObject->Position = g_GameData->EntData[i]->Position;
                //        }
                //    }
                //}
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::TextCentered("TERRRORISTS");
                ImGui::Spacing();
                EntityManager(&g_GameData->bSILENTAIM_TERR, g_GameData->Teams.TERRORIST);
                //for (int i = 0; i <= g_GameData->arraySize - 1; i++)
                //{
                //    //  Health Filter
                //    if (g_GameData->bALIVE_FILTER && g_GameData->EntData[i]->Health == NULL)
                //    {
                //        if (g_GameData->bSILENTAIM_TERR)
                //            g_GameData->bSILENTAIM_TERR = FALSE;
                //        continue;
                //    }
                //    if (g_GameData->EntData[i]->TeamID == g_GameData->Teams.TERRORIST) {
                //        std::string OBJECT_NAME = "[" + std::to_string(i) + "] " + std::string(g_GameData->EntNames[i]->NAME);
                //        std::string SILENT_AIM = "[" + std::to_string(i) + "] " + "POOP ON";
                //        if (ImGui::CollapsingHeader(OBJECT_NAME.c_str())) {
                //            ImGui::Text("TEAM: %s", g_GameData->GetPlayerTeam(g_GameData->EntData[i]->TeamID));
                //            ImGui::Text("HEALTH: %.0f", g_GameData->EntData[i]->Health * 100);
                //            ImGui::Text("DISTANCE: %f", g_GameData->GetDistanceTo3D_Object(g_GameData->EntData[i]->Position, g_GameData->SealObject->Position));
                //            ImGui::Text("POS-X: %f", g_GameData->EntData[i]->Position.x);
                //            ImGui::Text("POS-Y: %f", g_GameData->EntData[i]->Position.y);
                //            ImGui::Text("POS-Z: %f", g_GameData->EntData[i]->Position.z);
                //            ImGui::Checkbox(SILENT_AIM.c_str(), &g_GameData->bSILENTAIM_TERR);
                //            if (g_GameData->bSILENTAIM_TERR)
                //                g_GameData->TARGET = g_GameData->EntData[i]->Position;
                //            if (ImGui::Button("TELEPORT TO PLAYER", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
                //                g_GameData->SealObject->Position = g_GameData->EntData[i]->Position;
                //        }
                //    }
                //}
            }
        }

        void TABAbout()
        {
            ImGui::Text("SOCOM CA MENU (PREVIEW)");
            ImGui::Text("BUILD VERSION: v0.0.5");
            ImGui::Text("BUILD DATE: 6/24/2022");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Checkbox("IMGUI DEMO", &g_GameVariables->m_ShowDemo)) g_Console->LogEvent("[+] DEBUG::ImGuiDemo::Show/Hide - ", g_Console->m_ShowConsole);
            if (ImGui::Checkbox("SHOW CONSOLE", &g_Console->m_ShowConsole)) g_Console->LogEvent("[+] DEBG::Console::Show/Hide - ", g_Console->m_ShowConsole);
            if (ImGui::Checkbox("RGB MODE", &g_Menu->dbg_RAINBOW_THEME)) g_Console->LogEvent("[+] DEBUG::MENU::RGB THEME - ", g_Menu->dbg_RAINBOW_THEME);
            if (ImGui::Checkbox("CONTROLLER INPUT", &g_GameVariables->m_CONTROLLER)) g_Console->LogEvent("[+] DEBUG::MENU::CONTROLLER INPUT - ", g_GameVariables->m_CONTROLLER);
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            //if (ImGui::Button("RESIZE WINDOW", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) g_D3D11Window->ResizeWindow();
            if (ImGui::Button("UNHOOK DLL", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) {
#if DEVELOPER
                g_Console->printdbg("\n\n[+] UNHOOK INITIALIZED [+]\n\n", g_Console->color.red);
#endif
                g_KillSwitch = TRUE;
            }
        }
	}

    void Menu::Draw()
    {
        if (!g_GameVariables->m_ShowDemo)
            Styles::InitStyle();

        if (g_GameVariables->m_ShowMenu)
            MainMenu();

        if (g_GameVariables->m_ShowHud)
            HUD();

        if (g_GameData->bDIST_SILENTAIM)
            SilentAimInfo(&g_GameData->bDIST_SILENTAIM);

        if (g_GameVariables->m_ShowDemo)
            ImGui::ShowDemoWindow();

        switch (g_Console->m_ShowConsole) {
        case(TRUE):
            if (!g_Console->ACTIVE) {
                ShowWindow(g_Console->g_hWnd, SW_SHOW);
                g_Console->ACTIVE = TRUE;
            }
            break;
        case(FALSE):
            if (g_Console->ACTIVE) {
                ShowWindow(g_Console->g_hWnd, SW_HIDE);
                g_Console->ACTIVE = FALSE;
            }
            break;
        }
    }

	void Menu::MainMenu()
	{
        if (g_Menu->dbg_RAINBOW_THEME) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(g_Menu->dbg_RAINBOW));
        }
        if (!ImGui::Begin("SOCOM::CA", &g_GameVariables->m_ShowMenu, 96))
        {
            ImGui::End();
            return;
        }
        if (g_Menu->dbg_RAINBOW_THEME) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("ENHANCEMENTS"))
            {
                Tabs::TABMain();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("MATCH MANAGER"))
            {
                Tabs::TABEntities();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("DEBUG"))
            {
                Tabs::TABAbout();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
	}

	void Menu::HUD()
	{
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        if (!ImGui::Begin("SOCOM CA Internal Launch Window", NULL, 103))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("SOCOM CA MENU LOADED");
        ImGui::Text("PRESS [NUM0], [NUMPAD0] OR [L3 + R3] TO SHOW THE MENU");
        ImGui::Separator();
        ImGui::TextColored(ImColor(255, 0, 0, 255), "MENU v0.0.5 | Released: Friday June 24, 2022");
        ImGui::End();
	}

    void Menu::SilentAimInfo(bool* p_open)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        if (!ImGui::Begin("SILENT AIM INFO", NULL, 103))
        {
            ImGui::End();
            return;
        }

        if (g_GameData->bDIST_SILENTAIM) {

            // Loop entity array
            for (int i = 0; i < g_GameData->arraySize; i++) {

                //  Check Team ID
                if (g_GameData->EntData[i]->TeamID == -1) continue;
                if (g_GameData->EntData[i]->TeamID == g_GameData->Teams.TURRET) continue;
                if (g_GameData->EntData[i]->TeamID == g_GameData->Teams.TURRET2) continue;
                if (g_GameData->EntData[i]->TeamID == g_GameData->SealObject->TeamID) continue;

                //  Check Health
                if (g_GameData->EntData[i]->Health == NULL) continue;

                // Check Distance
                float distance = g_GameData->GetDistanceTo3D_Object(g_GameData->SealObject->Position, g_GameData->EntData[i]->Position);
                if (distance > g_GameData->AIM_DISTANCE) continue;

                ImGui::Text("%s: ", g_GameData->EntNames[i]->NAME);
                ImGui::SameLine();
                ImGui::TextColored(ImColor(0, 255, 0, 255), "%.2f", distance);
                ImGui::Text("HEALTH: %.2f", (g_GameData->EntData[i]->Health * 100));
                ImGui::Text("TeamID: %X", g_GameData->EntData[i]->TeamID);
                ImGui::Separator();
            }
        }
    }

	void Menu::Loops()
	{
        //  Perfect Shot
        if (g_GameData->bpVALID) {
            if (g_GameData->bPERFECTSHOT) {
                g_GameData->SealObject->chairBloom = NULL;
                g_GameData->SealObject->ChairSpread = NULL;
                g_GameData->SealObject->VerticalRecoil = NULL;
                g_GameData->SealObject->ScopeRecoil = NULL;
                g_GameData->SealObject->ScopeHoriKick = NULL;
                g_GameData->SealObject->ScopeHoriSway = NULL;
                g_GameData->SealObject->ScopeVertKick = NULL;
                g_GameData->SealObject->ScopeVertSway = NULL;
            }
        }

        //  Semi Perfect Shot
        if (g_GameData->bpVALID) {
            if (g_GameData->bPERFECTSHOT2) {

                //  
                g_GameData->SealObject->chairBloom = (g_GameData->SealObject->chairBloom / 1.0001);
                g_GameData->SealObject->ChairSpread = (g_GameData->SealObject->ChairSpread / 1.0001);
                g_GameData->SealObject->VerticalRecoil = (g_GameData->SealObject->VerticalRecoil / 1.1);

                //  Scope Perfect Shot
                g_GameData->SealObject->ScopeRecoil = NULL;
                g_GameData->SealObject->ScopeHoriKick = NULL;
                g_GameData->SealObject->ScopeHoriSway = NULL;
                g_GameData->SealObject->ScopeVertKick = NULL;
                g_GameData->SealObject->ScopeVertSway = NULL;
            }
        }

        if (g_GameData->bSILENTAIM)
            SilentAim_private();

        if (g_GameData->bSILENTAIM_SEAL)
            SilentAim(g_GameData->TARGET);

        if (g_GameData->bSILENTAIM_TERR)
            SilentAim(g_GameData->TARGET);

        if (g_GameData->bDIST_SILENTAIM)
            distSilentAim(g_GameData->AIM_DISTANCE);

        if (g_GameData->TELEPORT_TO_PROJECTILE)
            TeleporttoCrosshair();
	}
}