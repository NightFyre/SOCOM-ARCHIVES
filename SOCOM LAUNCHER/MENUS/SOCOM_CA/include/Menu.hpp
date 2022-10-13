#pragma once
#include "../helper.h"
#include "..//include/Game.hpp"
#include "..//include/D3D11Window.hpp"
namespace SOCOM_CA {
	class Menu
	{
	public:
		ImColor dbg_RAINBOW{};				//	RAINBOW THEME COLORS
		bool dbg_RAINBOW_THEME = FALSE;		//	RAINBOW THEME BOOLEAN
		float sOpacity = 0.80;				//	Opacity for SunShade Window
		bool dbg_ALERTS = TRUE;

		//	INITIALIZE CLASS
		Menu()  noexcept = default;
		~Menu() noexcept = default;
		Menu(Menu const&) = delete;
		Menu(Menu&&) = delete;
		Menu& operator=(Menu const&) = delete;
		Menu& operator=(Menu&&) = delete;

		//	FORWARD DECLARE FUNCTIONS
		void Draw();
		void MainMenu();
		void HUD();
		void Loops();
		void SilentAimInfo(bool* p_open);

	private:
		bool m_StyleInit{};
	};
	inline std::unique_ptr<Menu> g_Menu;
}
