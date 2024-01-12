#include "dxApplication.h"
#include "dxstructures.h"

using namespace mini::directx;

dx_app::dx_app(HINSTANCE instance, int window_width, int window_height, std::wstring_view window_title)
	: window_app{ instance, window_width, window_height, window_title }, m_device{ get_window()},
	m_viewport{ get_window().client_size() }

{
	auto windowSize = get_window().client_size();
	auto backBufferTexture = m_device.swapChain().GetBuffer();
	
	m_depthStencilView = m_device.CreateDepthStencilView(windowSize);
	m_renderTargetView = m_device.CreateRenderTargetView(backBufferTexture);
	//m_device.context()->PSSetShaderResources(0, 0, nullptr);

}


int dx_app::main_loop()
{
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

			render();
			m_device.swapChain().Present(0, DXGI_PRESENT_ALLOW_TEARING);
		}
	}
	return static_cast<int>(msg.wParam);
}

void dx_app::render()
{
	float clearColor[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_renderTargetView.get(), clearColor);

}
