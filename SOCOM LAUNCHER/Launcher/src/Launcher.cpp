#include "../include//Launcher.hpp"
#include "../resource.h"
namespace SOCOMLauncher {

    void TextCentered(std::string text) {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text.c_str());
    }

    LauncherVariables::LauncherVariables()
    {
        m_LauncherRunning = TRUE;
        m_ShowWindow = TRUE;
        hInstance;

        //  MENU WINDOW FLAG
        m_MainMenu = TRUE;
        m_MenuOptions = FALSE;
        m_ProcSelect = FALSE;
        m_LaunchOptions = FALSE;
        m_Custom = FALSE;
        return;
    }

    Launcher::Launcher()
    {
        g_LauncherVariables->proc_T.m_ModuleName = g_Console->CurrentProcName(g_LauncherVariables->proc_T.m_filePATH);
        InitStyle();
    }

    void Launcher::InitStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = ImGui::GetStyle().Colors;
        ImGui::StyleColorsDark();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.FrameRounding = 1.0f;
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
        style.WindowRounding = 4.0f;
        style.FrameBorderSize = 0.75f;

        /// CUSTOM USER COLORS
        colors[ImGuiCol_Border] = g_LauncherVariables->m_CustomColors;
        colors[ImGuiCol_BorderShadow] = g_LauncherVariables->m_CustomColors;

        //	COLORS
        colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.29f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.00f, 0.63f, 0.43f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.70f);
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
        colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.90f);
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
        return;
    }

    //  MAIN MENU
    void Launcher::ProcSelect()
    {
        ImGui::Spacing();

        //  SOCOM 1
        /*
        if (ImGui::Button("SOCOM 1", g_LauncherVariables->BUTTONS)) {
            g_Launcher->createProcess(g_LauncherVariables->SOCOM1);
            return;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        */

        //  SOCOM 2
        if (ImGui::Button("SOCOM 2", g_LauncherVariables->BUTTONS)) {
            g_Launcher->createProcess(g_LauncherVariables->SOCOM2);
            return;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        
        //  SOCOM 3
        /*
        if (ImGui::Button("SOCOM 3", g_LauncherVariables->BUTTONS)) {
            g_Launcher->createProcess(g_LauncherVariables->SOCOM3);
            return;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        */
        
        //  SOCOM CA
        if (ImGui::Button("SOCOM CA", g_LauncherVariables->BUTTONS)) {
            g_Launcher->createProcess(g_LauncherVariables->SOCOMCA);
            return;
        }

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(g_LauncherVariables->cRainbow));
        ImGui::Separator();
        ImGui::PopStyleColor();
        ImGui::Spacing();

        //  CUSTOM INJECTOR
        if (ImGui::Button("CUSTOM INJECTION", g_LauncherVariables->BUTTONS)) {
            g_LauncherVariables->m_MainMenu = FALSE;
            g_LauncherVariables->m_CustomInjector = TRUE;
            g_Memory->GetProcessList();
            return;
        }

        ImGui::Spacing();

        //  MENU OPTIONS
        if (ImGui::Button("OPTIONS", g_LauncherVariables->BUTTONS)) {
            g_LauncherVariables->m_MainMenu = FALSE;
            g_LauncherVariables->m_MenuOptions = TRUE;
            return;
        }

        ImGui::Spacing();

        //  EXIT
        if (ImGui::Button("EXIT", g_LauncherVariables->BUTTONS)) g_Running = FALSE;
    }

    //  MENU OPTIONS
    void Launcher::MenuOptions()
    {
        TextCentered("MENU COLORS");
        ImGui::ColorEdit4("##BUTTON-BORDERS", (float*)&g_LauncherVariables->m_CustomColors);
        ImGui::Spacing();
        if (ImGui::Button("SHOW IMGUI::DEMO", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1])))
            g_LauncherVariables->m_DemoWindow = !g_LauncherVariables->m_DemoWindow;
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(g_LauncherVariables->cRainbow));
        ImGui::Separator();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        if (ImGui::Button("BACK", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1]))) {
            g_LauncherVariables->m_MainMenu = TRUE;
            g_LauncherVariables->m_MenuOptions = FALSE;
        }
        ImGui::Spacing();
        if (ImGui::Button("EXIT", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1]))) g_Running = FALSE;
    }

    //  CUSTOM DLL INJECTION
    char buf[128]{};
    char buf2[128]{};
    bool isFocused = FALSE;
    static bool isOpen = FALSE;
    static bool isOpen2 = FALSE;
    void Launcher::CustomInjector()
    {
        /// <summary>
        /// PROCESS
        ImGui::SetNextItemWidth(175);
        ImGui::InputTextWithHint("##PROCESSNAME", "PROCESS NAME", buf, 128);
        ImGui::SameLine();

        //  AUTOCOMPLETE 
        isFocused = ImGui::IsItemFocused();
        isOpen |= ImGui::IsItemActive();
        if (isOpen)
        {
            ImGui::SetNextWindowPos({ ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y });
            ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 80 });
            if (ImGui::Begin("##popup", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip))
            {
                ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
                isFocused |= ImGui::IsWindowFocused();
                for (int i = 0; i < g_Console->ActiveProcesses.size(); i++) {
                    if (strstr(g_Console->ActiveProcesses[i].c_str(), buf) == NULL)
                        continue;
                    if (ImGui::Selectable(g_Console->ActiveProcesses[i].c_str()) || (ImGui::IsItemFocused() && ImGui::IsKeyPressedMap(ImGuiKey_Enter)))
                    {
                        strcpy(buf, g_Console->ActiveProcesses[i].c_str());
                        isOpen = FALSE;
                    }
                }
            }
            ImGui::End();
            isOpen &= isFocused;
        }

        //  OPEN PROCESS SELECTION WINDOW
        if (ImGui::Button("...##2"))
            isOpen2 = TRUE;

        if (isOpen2) {
            if (!ImGui::Begin("ACTIVE PROCESSES", &isOpen2, ImGuiWindowFlags_NoCollapse)) {
                ImGui::End();
                isOpen2 = FALSE;
            }
            for (int i = 0; i < g_Console->ActiveProcesses.size(); i++) {
                if (ImGui::Selectable(g_Console->ActiveProcesses[i].c_str())) {
                    strcpy(buf, g_Console->ActiveProcesses[i].c_str());
                    isOpen2 = FALSE;
                }
            }
            ImGui::End();
        }
        ImGui::Spacing();
        ImGui::Spacing();

        /// <summary>
        /// FILE
        ImGui::SetNextItemWidth(175);
        ImGui::InputTextWithHint("##DLL PATH", "DLL PATH", buf2, 128);
        ImGui::SameLine();

        //  OPEN FILE SELECTION
        if (ImGui::Button("...")) {
            if (g_Console->openFile()) {
                if (g_Console->result.c_str() != NULL) {
                    auto convert = std::strlen(g_Console->result.c_str());
                    if (std::size(buf2) > convert)
                        std::memcpy(buf2, g_Console->result.c_str(), convert);
                }
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();

        //ImGui::Checkbox("x86", &g_LauncherVariables->m_bX86);

        //  INJECT
        if (ImGui::Button("INJECT DLL", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1]))) {
            if (buf != NULL && buf2 != NULL) {
                if (g_Memory->customInjection(g_Console->Char2WChar(buf), buf2, g_LauncherVariables->m_bX86)) {
                    memset(buf, 0, 128);
                    memset(buf2, 0, 128);
                    g_LauncherVariables->bINJECT_ERROR = FALSE;
                    return;
                }
            }
            g_LauncherVariables->bINJECT_ERROR = TRUE;
            memset(buf, 0, 128);
            memset(buf2, 0, 128);
        }
        if (g_LauncherVariables->bINJECT_ERROR)
            ImGui::TextColored(ImColor(255, 0, 0, 255), "[!] AN ERROR HAS OCCURED\n[!] PLEASE TRY AGAIN");

        //ImGui::Spacing();
        //if (ImGui::Button("UNLOAD DLL", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1]))) {  }

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(g_LauncherVariables->cRainbow));
        ImGui::Separator();
        ImGui::PopStyleColor();
        ImGui::Spacing();

        if (ImGui::Button("BACK", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1]))) {
            g_Console->ActiveProcesses.clear();
            isOpen = FALSE;
            isOpen2 = FALSE;
            g_LauncherVariables->bINJECT_ERROR = FALSE;
            g_LauncherVariables->m_MainMenu = TRUE;
            g_LauncherVariables->m_CustomInjector = FALSE;
        }
        ImGui::Spacing();
        if (ImGui::Button("EXIT", ImVec2(ImGui::GetWindowContentRegionWidth(), g_LauncherVariables->BUTTONS[1]))) g_Running = FALSE;
    }

    //  MAIN DRAW LOOP
    void Launcher::Draw()
    {
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(g_LauncherVariables->cRainbow));
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(g_LauncherVariables->cRainbow));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(g_LauncherVariables->cRainbow));
        if (!ImGui::Begin(g_LauncherVariables->proc_T.m_MenuTitle, (bool*)&g_Running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::End();
            return;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();


        if (!g_LauncherVariables->m_DemoWindow)
            InitStyle();

        //  MAIN WINDOW
        if (g_LauncherVariables->m_MainMenu)
            ProcSelect();

        if (g_LauncherVariables->m_MenuOptions)
            MenuOptions();

        if (g_LauncherVariables->m_CustomInjector)
            CustomInjector();

        //  CLEAN EXIT AFTER LAUNCH
        if (g_LauncherVariables->proc_T.p_PID != NULL) {
            clearProcess(FALSE);
            g_Running = FALSE;
        }

        if (g_LauncherVariables->m_DemoWindow)
            ImGui::ShowDemoWindow(&g_LauncherVariables->m_DemoWindow);

        ImGui::End();
    }

    //  Create Process
    bool Launcher::createProcess(int Game)
    {
        ZeroMemory(&g_LauncherVariables->proc_T.sInfo, sizeof(g_LauncherVariables->proc_T.sInfo));
        g_LauncherVariables->proc_T.sInfo.cb = sizeof(g_LauncherVariables->proc_T.sInfo);
        ZeroMemory(&g_LauncherVariables->proc_T.pInfo, sizeof(g_LauncherVariables->proc_T.pInfo));
        if (!CreateProcessW(xorstr_(L"PCSX2x64.exe"),
            NULL,
            NULL, NULL, FALSE, 0, NULL, NULL,
            &g_LauncherVariables->proc_T.sInfo,
            &g_LauncherVariables->proc_T.pInfo
        ))
            return FALSE;

        SuspendThread(g_LauncherVariables->proc_T.pInfo.hThread);
        ResumeThread(g_LauncherVariables->proc_T.pInfo.hThread);
        g_LauncherVariables->proc_T.p_PID = GetProcessId(g_LauncherVariables->proc_T.pInfo.hProcess);
        g_LauncherVariables->proc_T.p_hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_LauncherVariables->proc_T.p_PID);

        //  Method for injection . . .
        if (!g_Memory->ManualMap(g_LauncherVariables->proc_T.p_hProc, Game)) {
            MessageBoxA(NULL, xorstr_("THE ATTEMPT TO LAUNCH WITH MENU HAS FAILED"), xorstr_("ERROR"), NULL);
            return FALSE;
        }
        return TRUE;
    }

    //  Clear Process Information
    void Launcher::clearProcess(bool TERMINATE)
    {
        // Close Process
        if (TERMINATE)
            TerminateProcess(g_LauncherVariables->proc_T.p_hProc, EXIT_SUCCESS);

        //  Clear Info
        if (!g_LauncherVariables->proc_T.pInfo.hThread != NULL) {
            CloseHandle(g_LauncherVariables->proc_T.pInfo.hThread);
            CloseHandle(g_LauncherVariables->proc_T.pInfo.hProcess);
            Sleep(100);
        }

        g_LauncherVariables->proc_T.p_exitCode = NULL;
        g_LauncherVariables->proc_T.p_hProc = NULL;
        g_LauncherVariables->proc_T.p_PID = NULL;
        g_LauncherVariables->proc_T.p_moduleBase = NULL;
        g_LauncherVariables->proc_T.pInfo;
        g_LauncherVariables->proc_T.sInfo;
        return;
    }
}