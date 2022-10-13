#include "../include/D3D11Renderer.hpp"
#include "../include/Game.hpp"
#include "../include/Menu.hpp"
#include "../include/Hooking.hpp"
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace SOCOM2 {
	static uint64_t* MethodsTable = NULL;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="saturation"></param>
	/// <param name="value"></param>
	/// <param name="opacity"></param>
	float HSV_RAINBOW_SPEED = 0.0005;
	static float HSV_RAINBOW_HUE = 0;
	void SV_RAINBOW(float saturation, float value, float opacity)
	{
		using namespace SOCOM2;
		HSV_RAINBOW_HUE -= HSV_RAINBOW_SPEED;
		if (HSV_RAINBOW_HUE < -1.f) HSV_RAINBOW_HUE += 1.f;
		for (int i = 0; i < 860; i++)
		{
			float hue = HSV_RAINBOW_HUE + (1.f / (float)860) * i;
			if (hue < 0.f) hue += 1.f;
			g_Menu->dbg_RAINBOW = ImColor::HSV(hue, (saturation / 255), (value / 255), (opacity / 255));
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool D3D11Renderer::Hook()
	{
		if (InitHook()) {
			CreateHook(8, (void**)&oIDXGISwapChainPresent, HookPresent);
			return 1;
		}
		return 0;
	}

	/// <summary>
	/// 
	/// </summary>
	void D3D11Renderer::Unhook()
	{
		SetWindowLongPtr(g_GameVariables->m_GameWindow, GWLP_WNDPROC, (LONG_PTR)m_OldWndProc);
		DisableAll();
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool D3D11Renderer::InitHook()
	{
		if (InitWindow() == false) {
			return false;
		}

		HMODULE D3D11Module = GetModuleHandleA("d3d11.dll");

		D3D_FEATURE_LEVEL FeatureLevel;
		const D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

		DXGI_RATIONAL RefreshRate;
		RefreshRate.Numerator = 60;
		RefreshRate.Denominator = 1;

		DXGI_MODE_DESC BufferDesc;
		BufferDesc.Width = 100;
		BufferDesc.Height = 100;
		BufferDesc.RefreshRate = RefreshRate;
		BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC SampleDesc;
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		SwapChainDesc.BufferDesc = BufferDesc;
		SwapChainDesc.SampleDesc = SampleDesc;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.OutputWindow = WindowHwnd;
		SwapChainDesc.Windowed = 1;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain* SwapChain;
		ID3D11Device* Device;
		ID3D11DeviceContext* Context;
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevels, 1, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &Context) < 0)
		{
			DeleteWindow();
			return false;
		}

		MethodsTable = (uint64_t*)::calloc(205, sizeof(uint64_t));
		memcpy(MethodsTable, *(uint64_t**)SwapChain, 18 * sizeof(uint64_t));
		memcpy(MethodsTable + 18, *(uint64_t**)Device, 43 * sizeof(uint64_t));
		memcpy(MethodsTable + 18 + 43, *(uint64_t**)Context, 144 * sizeof(uint64_t));
		Sleep(1000);

		//	INIT NOTICE
		Beep(300, 300);

		MH_Initialize();
		SwapChain->Release();
		SwapChain = NULL;
		Device->Release();
		Device = NULL;
		Context->Release();
		Context = NULL;
		DeleteWindow();
		return true;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pSwapChain"></param>
	void D3D11Renderer::Overlay(IDXGISwapChain* pSwapChain)
	{
		if (!b_ImGui_Initialized)
			Init(pSwapChain);

		if (bWindowPosChanged)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			m_DeviceContext->ClearState();
			ImGui_ImplDX11_CreateDeviceObjects();
			bWindowPosChanged = FALSE;
		}

		SV_RAINBOW(169, 169, 200);
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame(1);
		ImGui::NewFrame();
		ImGui::GetIO().MouseDrawCursor = g_GameVariables->m_ShowMenu;


		// DRAW
		g_Menu->Draw();

		ImGui::EndFrame();
		ImGui::Render();
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hWnd"></param>
	/// <param name="msg"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	/// <returns></returns>
	LRESULT D3D11Renderer::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		/// <summary>
		/// Resize our overlay and imgui menu based on the game window resizing
		/// </summary>
		if (msg == WM_WINDOWPOSCHANGED) {
			WINDOWPOS* windowPositionStructure = reinterpret_cast<WINDOWPOS*>(lParam);
			WINDOWINFO windowInfo;
			windowInfo.cbSize = sizeof(windowInfo);

			GetWindowInfo(windowPositionStructure->hwnd, &windowInfo);
			if (windowInfo.dwStyle && WS_BORDER) {
				int32_t windowTitleHeight = (windowInfo.rcWindow.bottom - windowInfo.rcWindow.top) - (windowInfo.rcClient.bottom - windowInfo.rcClient.top);
				windowPositionStructure->y += windowTitleHeight;
				windowPositionStructure->cy -= windowTitleHeight;
			}

			ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(windowPositionStructure->cx), static_cast<float>(windowPositionStructure->cy));

			MoveWindow(g_D3DRenderer->WindowHwnd, windowPositionStructure->x + windowInfo.cxWindowBorders, windowPositionStructure->y - windowInfo.cyWindowBorders, windowPositionStructure->cx, windowPositionStructure->cy, false);
			g_D3DRenderer->bWindowPosChanged = TRUE;
		}

		if (g_GameVariables->m_ShowMenu) {		
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
			return TRUE;
		}
		return CallWindowProc((WNDPROC)g_D3DRenderer->m_OldWndProc, hWnd, msg, wParam, lParam);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pSwapChain"></param>
	/// <param name="SyncInterval"></param>
	/// <param name="Flags"></param>
	/// <returns></returns>
	HRESULT APIENTRY D3D11Renderer::HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (g_Killswitch) {
			g_Hooking->Unhook();
			g_GameVariables->m_ShowMenu = FALSE;
			g_D3DRenderer->oIDXGISwapChainPresent(pSwapChain, SyncInterval, Flags);
			g_Running = FALSE;
			return 0;
		}
		g_D3DRenderer->Overlay(pSwapChain);
		return g_D3DRenderer->oIDXGISwapChainPresent(pSwapChain, SyncInterval, Flags);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="swapChain"></param>
	/// <returns></returns>
	bool D3D11Renderer::Init(IDXGISwapChain* swapChain)
	{
		if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&m_Device))) {
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::GetIO().WantCaptureMouse;
			ImGui::GetIO().WantTextInput;
			ImGui::GetIO().WantCaptureKeyboard;
			io.ConfigFlags = ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.IniFilename = NULL;
			m_Device->GetImmediateContext(&m_DeviceContext);

			DXGI_SWAP_CHAIN_DESC Desc;
			swapChain->GetDesc(&Desc);
			g_GameVariables->m_GameWindow = Desc.OutputWindow;

			ID3D11Texture2D* BackBuffer;
			swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
			m_Device->CreateRenderTargetView(BackBuffer, NULL, &m_RenderTargetView);
			BackBuffer->Release();

			ImGui_ImplWin32_Init(g_GameVariables->m_GameWindow);
			ImGui_ImplDX11_Init(m_Device, m_DeviceContext);
			ImGui_ImplDX11_CreateDeviceObjects();
			ImGui::GetIO().ImeWindowHandle = g_GameVariables->m_GameWindow;
			m_OldWndProc = SetWindowLongPtr(g_GameVariables->m_GameWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
			b_ImGui_Initialized = TRUE;
			return 1;
		}
		b_ImGui_Initialized = FALSE;
		return 0;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool D3D11Renderer::InitWindow()
	{
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = DefWindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = GetModuleHandle(NULL);
		WindowClass.hIcon = NULL;
		WindowClass.hCursor = NULL;
		WindowClass.hbrBackground = NULL;
		WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = L"MJ";
		WindowClass.hIconSm = NULL;
		RegisterClassEx(&WindowClass);
		WindowHwnd = CreateWindow(WindowClass.lpszClassName, L"DirectX Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, WindowClass.hInstance, NULL);
		if (WindowHwnd == NULL) {
			return false;
		}
		return true;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool D3D11Renderer::DeleteWindow() 
	{
		DestroyWindow(WindowHwnd);
		UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
		if (WindowHwnd != NULL) {
			return false;
		}
		return true;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="Index"></param>
	/// <param name="Original"></param>
	/// <param name="Function"></param>
	/// <returns></returns>
	bool D3D11Renderer::CreateHook(uint16_t Index, void** Original, void* Function) 
	{
		assert(Index >= 0 && Original != NULL && Function != NULL);
		void* target = (void*)MethodsTable[Index];
		if (MH_CreateHook(target, Function, Original) != MH_OK || MH_EnableHook(target) != MH_OK) {
			return false;
		}
		return true;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="Index"></param>
	void D3D11Renderer::DisableHook(uint16_t Index) 
	{
		assert(Index >= 0);
		MH_DisableHook((void*)MethodsTable[Index]);
	}

	/// <summary>
	/// 
	/// </summary>
	void D3D11Renderer::DisableAll() 
	{
		MH_DisableHook(MH_ALL_HOOKS);
		free(MethodsTable);
		MethodsTable = NULL;
	}

	/// <summary>
	/// 
	/// </summary>
	D3D11Renderer::~D3D11Renderer()
	{
		Unhook();
	}
}