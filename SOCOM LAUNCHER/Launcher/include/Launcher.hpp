#pragma once
#include "../helper.h"
#include "Console.hpp"
#include "DXWindow.hpp"
#include "Memory.hpp"
namespace SOCOMLauncher {

	struct Process {
		//	THIS PROCESS
		const char*		m_MenuTitle = "SOCOM LAUNCHER";
		const char*		m_OriginalModuleName = "SOCOMLauncher.exe";
		CHAR			m_filePATH[MAX_PATH];
		std::string		m_ModuleName;
		HINSTANCE		m_hInstance;
		PROCESSENTRY32	m_procENTRY;
		MODULEENTRY32	m_modENTRY;

		//	CHILD PROCESS
		PROCESS_INFORMATION pInfo;
		STARTUPINFO			sInfo;
		HANDLE				p_hProc{};
		uintptr_t			p_moduleBase{};
		DWORD				p_PID{};
		DWORD				p_exitCode{};
		const wchar_t* p_PATH{};
		const wchar_t* p_PARAMS{};
	};

	class Launcher
	{
	public:
		explicit Launcher();
		~Launcher() noexcept = default;
		Launcher(Launcher const&) = delete;
		Launcher(Launcher&&) = delete;
		Launcher& operator=(Launcher const&) = delete;
		Launcher& operator=(Launcher&&) = delete;

		void InitStyle();							//	Window Style
		void Draw();								//	Main draw function
		void MenuOptions();							//	
		void ProcSelect();							//	Process Select Window
		void CustomInjector();						//	
		bool createProcess(int Game);				//	Creates Process
		void clearProcess(bool TERMINATE);			//	Clears Process Information
	};

	class LauncherVariables
	{
	public:

		enum Titles {
			SOCOM1 = 0,
			SOCOM2 = 1,
			SOCOM3 = 2,
			SOCOMCA = 3,
		};

		//	EXTERNAL PROCESS & WINDOW 
		Process proc_T{};				//	Child Process Variables
		HINSTANCE hInstance{};			//	Variable for Process Icon

		const char* PCSX2_PATH = xorstr_("PCSX2x64.exe");
		const char* PCSX2_PARAMS{};

		//	LAUNCHER
		bool m_LauncherRunning{};		//	
		bool m_ShowWindow{};			//	
		bool m_DemoWindow{};			//	

		//	MENU VARIABLES & FLAGS
		bool m_MainMenu{};
		bool m_MenuOptions{};
		bool m_ProcSelect{};
		bool m_LaunchOptions{};
		bool m_Custom{};
		bool m_CustomInjector{};
		bool m_bX86 = FALSE;

		//	Selections and Window Related Vars
		int m_SelectedTitle{};
		bool bINJECT_ERROR = FALSE;

		// Rainbow Variables
		ImColor cRainbow;
		ImVec2 BUTTONS{ 200, 30 };		//	Button Size
		ImVec4 m_CustomColors{};
		std::vector<float> cfgColors{};

		//	INIT
		explicit LauncherVariables();
		~LauncherVariables() noexcept = default;
		LauncherVariables(LauncherVariables const&) = delete;
		LauncherVariables(LauncherVariables&&) = delete;
		LauncherVariables& operator=(LauncherVariables const&) = delete;
		LauncherVariables& operator=(LauncherVariables&&) = delete;
	};

	inline std::unique_ptr<Launcher> g_Launcher;
	inline std::unique_ptr<LauncherVariables> g_LauncherVariables;
}