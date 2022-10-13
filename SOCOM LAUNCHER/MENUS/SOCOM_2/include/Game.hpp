#pragma once
#include "../helper.hpp"

namespace SOCOM2 {
	class GameVariables
	{
	public:
		explicit GameVariables();
		~GameVariables() noexcept = default;
		GameVariables(GameVariables const&) = delete;
		GameVariables(GameVariables&&) = delete;
		GameVariables& operator=(GameVariables const&) = delete;
		GameVariables& operator=(GameVariables&&) = delete;

		//	Dx & ImGui
		int m_GamePid{};
		HANDLE m_GameHandle{};
		HWND m_GameWindow{};
		int m_GameWidth;
		int m_GameHeight;
		ImVec2 m_WindowSize{};
		LPCSTR m_GameTitle;
		LPCSTR m_ClassName;
		LPCSTR m_GamePath;
		uintptr_t m_ModuleBase;
		//IDXGISwapChain3* m_Swapchain{};

		//	MENU RELATED
		bool m_ShowMenu{};
		bool m_ShowDemo{};
		bool m_ShowHud{};
		bool m_ShowStyleEditor{};
		bool m_CONTROLLER{};

		bool bLOOP{};
		int count{};
		int VIEWMATRIX_TEST{};
		int INPUT_VIEWMATRIX{};
		void ResizeWindow();
	};

	class GameFunctions
	{
	public:
		explicit GameFunctions();
		~GameFunctions() noexcept = default;
		GameFunctions(GameFunctions const&) = delete;
		GameFunctions(GameFunctions&&) = delete;
		GameFunctions& operator=(GameFunctions const&) = delete;
		GameFunctions& operator=(GameFunctions&&) = delete;

		//uintptr_t p2addy(uintptr_t PTR, std::vector<unsigned int> OFFSETS);
		float GetDistanceTo3D_Object(Vector3 POS, Vector3 POS2);
		void FPS();
		void Patch(BYTE* dst, BYTE* src, unsigned int size);
		void Nop(BYTE* dst, unsigned int size);
		uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
		uintptr_t GetPS2Addr(uintptr_t addr);
		void PS2Patch(uintptr_t offset, int patch, int type);
		const char* GetPlayerTeam(unsigned int value);
	};

	inline std::unique_ptr<GameVariables> g_GameVariables;
	inline std::unique_ptr<GameFunctions> g_GameFunctions;

}