#include "jellyApplication.h"
#include"imgui.h"

using namespace DirectX;

mini::Jelly::JellyApplication::JellyApplication(HINSTANCE instance)
	:MyDxApplication(instance,
		1280, 720,
		s_Jelly_window_title),
	m_viewFrustum{
			SIZE{1280, 720},
			XM_PIDIV2, 0.1f, 100
		},
	m_camera{ {},0.1f, 100, 2 },
	m_camera_speed{0.03f},

	m_simulation_step(0.005f),
	
	m_side_length(1.0f), m_impulse_strength(0.1f)
{
	m_cbView = create_buffer<XMFLOAT4X4>();
	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, m_viewFrustum.getProjectionMatrix());
	m_cbProj = create_buffer<XMFLOAT4X4>(m);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m(i, j) = 1;
	m_cbColor = create_buffer<XMFLOAT4X4>(m);
	XMStoreFloat4x4(&m,
		XMMatrixTranslation(0.0f, 0.0f, 40.0f) *
		XMMatrixRotationX(XM_PIDIV2) *
		XMMatrixScaling(0.005f, 0.005f, 0.005f) *
		XMMatrixTranslation(0.5f,0.5f,0.5f));
	m_cbModel = create_buffer<XMFLOAT4X4>(m);

	ID3D11Buffer* cb = m_cbView;
	m_device.context()->VSSetConstantBuffers(0, 1, &cb);
	m_device.context()->DSSetConstantBuffers(0, 1, &cb);
	cb = m_cbProj;
	m_device.context()->VSSetConstantBuffers(1, 1, &cb);
	m_device.context()->DSSetConstantBuffers(1, 1, &cb);
	cb = m_cbColor;
	m_device.context()->PSSetConstantBuffers(0, 1, &cb);
	cb = m_cbModel;
	m_device.context()->VSSetConstantBuffers(2, 1, &cb);
}

void mini::Jelly::JellyApplication::render()
{
	float clearColor[4] = { 0.7f, 0.6f, 0.9f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_renderTargetView.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1, 0);

	renderGui();
}

void mini::Jelly::JellyApplication::renderGui()
{
	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
}

void mini::Jelly::JellyApplication::update(utils::clock const& clock)
{
	float dt = clock.frame_time();
	updateControls(dt);
	updateCamera();

}

void mini::Jelly::JellyApplication::updateControls(float dt)
{
	auto& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		XMFLOAT2 dragL = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			m_camera.rotate(dragL.y * m_camera_speed, dragL.x * m_camera_speed);
		}
		XMFLOAT2 dragM = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		auto dragMvec = XMLoadFloat2(&dragM);
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
			dragMvec = XMVectorSetX(dragMvec, -XMVectorGetX(dragMvec));
			dragMvec = XMVectorSetZ(dragMvec, 0);
			dragMvec = XMVectorSetW(dragMvec, 0);
			m_camera.move_target(XMVector4Transform(dragMvec, XMMatrixTranspose(m_camera.view_matrix())) * m_camera_speed);
		}
		XMFLOAT2 dragR = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
		{
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
			m_camera.zoom(dragR.y * m_camera_speed);
		}
	}
	
}

void mini::Jelly::JellyApplication::updateCamera()
{
	auto& io = ImGui::GetIO();
	float d = 5.0f*io.DeltaTime;
	
	if (ImGui::IsKeyDown(ImGuiKey_LeftArrow))
		m_camera.rotate(0, -d);
	if (ImGui::IsKeyDown(ImGuiKey_RightArrow))
		m_camera.rotate(0, d);
	if (ImGui::IsKeyDown(ImGuiKey_DownArrow))
		m_camera.rotate(d, 0);
	if (ImGui::IsKeyDown(ImGuiKey_UpArrow))
		m_camera.rotate(-d, 0);

	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, m_camera.view_matrix());
	update_buffer(m_cbView, view);
}
