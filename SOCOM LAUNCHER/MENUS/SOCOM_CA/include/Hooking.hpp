#pragma once
#include "../helper.h"
#include "..//include/Console.hpp"
#include "..//include/Game.hpp"
#include "..//include/D3D11Window.hpp"

namespace SOCOM_CA {
	class Hooking
	{
	public:

		explicit Hooking();
		~Hooking() noexcept;
		Hooking(Hooking const&) = delete;
		Hooking(Hooking&&) = delete;
		Hooking& operator=(Hooking const&) = delete;
		Hooking& operator=(Hooking&&) = delete;

		void Hook();
		void Unhook();
		void FindHooks();

		static void __fastcall HookInput(uint64_t a1, uint64_t a2);

		//	Get Static Address
		__int64 GetAddr(INT64 addr);

		// Hook Pointers
		void* m_OriginalInputHandler{};
		uint64_t m_InputHandler{};

		void* m_OriginalW2SHandler{};
		INT64 m_W2SHandler{};

		INT64 TRUE_W2S = 0;
		static void __fastcall HookW2S(INT64 a1, INT64 a2, INT64 a3, void* a4);

		void* m_OriginalOnLeftDCLick{};
		INT64 m_OnLeftDClick{};
		static void __fastcall HookFullscreenToggle(INT64 a1);

		void* m_OriginalResetEE{};
		INT64 m_ResetEE{};
		static void __fastcall recResetEE();
	};
	inline std::unique_ptr<Hooking> g_Hooking;
}