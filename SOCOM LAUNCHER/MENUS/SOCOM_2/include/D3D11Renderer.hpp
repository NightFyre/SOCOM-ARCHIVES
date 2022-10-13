#pragma once
#include "../helper.hpp"

namespace SOCOM2 {
	class D3D11Renderer
	{
		typedef HRESULT(APIENTRY* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
		IDXGISwapChainPresent oIDXGISwapChainPresent = NULL;

	public:
		explicit D3D11Renderer() = default;
		~D3D11Renderer() noexcept;
		D3D11Renderer(D3D11Renderer const&) = delete;
		D3D11Renderer(D3D11Renderer&&) = delete;
		D3D11Renderer& operator=(D3D11Renderer const&) = delete;
		D3D11Renderer& operator=(D3D11Renderer&&) = delete;
		
		bool bWindowPosChanged = FALSE;
		bool m_Init{};
		bool b_ImGui_Initialized{};
		uint64_t m_OldWndProc{};

		//	Forward Dx11 Hook Declarations
		bool Init(IDXGISwapChain* swapChain);
		static HRESULT APIENTRY HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
		static LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//	CUSTOM FUNCTIONS		
		bool InitHook();
		bool Hook();
		void Unhook();

		bool InitWindow();
		bool DeleteWindow();

		bool CreateHook(uint16_t Index, void** Original, void* Function);
		void DisableHook(uint16_t Index);
		void DisableAll();

		//	RENDER STATE
		void Overlay(IDXGISwapChain* pSwapChain);
	
	private:
		WNDCLASSEX WindowClass;
		HWND WindowHwnd;

		ID3D11Device* m_Device{};
		ID3D11DeviceContext* m_DeviceContext{};
		ID3D11RenderTargetView* m_RenderTargetView{};
	};

	inline std::unique_ptr<D3D11Renderer> g_D3DRenderer;
}