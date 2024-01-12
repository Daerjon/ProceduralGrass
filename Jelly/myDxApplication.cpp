#include "myDxApplication.h"
#include"imgui_impl_dx11.h"
#include"imgui_impl_win32.h"

using namespace mini::Jelly;

MyDxApplication::MyDxApplication(HINSTANCE instance, int window_width, int window_height, std::wstring_view window_title)
	:dx_app(instance, window_width, window_height, window_title)
{
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	bool valid = ImGui_ImplWin32_Init(get_window().handle());
	valid &= ImGui_ImplDX11_Init(m_device, m_device.context().get());
}

MyDxApplication::~MyDxApplication()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


int MyDxApplication::main_loop()
{
	auto& io = ImGui::GetIO();
	MSG msg = { nullptr };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_clock.query();
			update(m_clock);
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			m_device.context()->RSSetViewports(1, &m_viewport);
			ID3D11RenderTargetView* rtv = m_renderTargetView.get();
			m_device.context()->OMSetRenderTargets(1, &rtv, m_depthStencilView.get());

			render();
			ImGui::Render();
			auto drawData = ImGui::GetDrawData();
			ImGui_ImplDX11_RenderDrawData(drawData);
			m_device.swapChain().Present(1, 0);
		}
	}
	return static_cast<int>(msg.wParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
std::optional<LRESULT> mini::Jelly::MyDxApplication::process_message(mini::windows::message const& msg)
{
	if (ImGui_ImplWin32_WndProcHandler(get_window().handle(), msg.type, msg.w_param, msg.l_param) == TRUE)
		return std::optional<LRESULT>(0);
	else
		return std::nullopt;
}

