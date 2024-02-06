#include "jellyApplication.h"
#include"imgui.h"

#include"renderLayout.h"
#include "SingleBlade.h"

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

	buffer_info csDescData = buffer_info(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, MaxBlades * sizeof(SingleBladeSt), csDescData.Usage = D3D11_USAGE_DEFAULT);
	csDescData.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	csDescData.StructureByteStride = sizeof(SingleBladeSt);
	m_CS1DataBuffer = m_device.CreateBuffer(csDescData);
	CreateBufferUAV(m_device, m_CS1DataBuffer.get(), &m_BuffDataUAV);
	CreateBufferSRV(m_device, m_CS1DataBuffer.get(), &m_BuffDataSRV);

	buffer_info desc = buffer_info(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, sizeof(unsigned int));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(unsigned int);
	m_CS1Number = m_device.CreateBuffer(desc);
	CreateBufferUAV(m_device, m_CS1Number.get(), &m_BuffNumberUAV);
	CreateBufferSRV(m_device, m_CS1Number.get(), &m_BuffNumberSRV);

	auto vsBytes = m_device.LoadByteCode(L"Test" L"VS.cso");
	m_test_vs = m_device.CreateVertexShader(vsBytes);


	m_inputLayout = m_device.CreateInputLayout(inputLayout, 12, vsBytes);

	std::vector<inputElement> poss;
	for(int x = 0; x<100; x++)
		for (int z = 0; z < 100; z++)
		{
			poss.push_back(
				inputElement{
					XMFLOAT3{ static_cast<float>(x) / 5,0.0f,static_cast<float>(z) / 5 }
				});
		}

	m_bladeBuffer = m_device.CreateVertexBuffer(poss.data(), poss.size());
	//m_bladeBuffer = m_device.CreateVertexBuffer<float>(0);

	auto psBytes = m_device.LoadByteCode(L"Test"  L"PS.cso");
	m_test_ps = m_device.CreatePixelShader(psBytes);

	auto csBytes = m_device.LoadByteCode(L"Grass"  L"CS.cso");
	m_grass_cs = m_device.CreateComputeShader(csBytes);
}

void mini::Jelly::JellyApplication::render()
{
	float clearColor[4] = { 0.7f, 0.6f, 0.9f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_renderTargetView.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1, 0);

	m_device.context()->VSSetShader(m_test_vs.get(), nullptr, 0);
	m_device.context()->PSSetShader(m_test_ps.get(), nullptr, 0);

	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	ID3D11Buffer* buf = CreateAndCopyToBuf(m_device, m_device.context().get(), m_CS1DataBuffer.get());
	ID3D11Buffer* vb[] = { nullptr, m_bladeBuffer.get()};
	//const UINT strides[] = { sizeof(XMFLOAT3) };
	const UINT strides[] = { 0,sizeof(inputElement)};
	const UINT offsets[] = { 0,0 };
	m_device.context()->IASetVertexBuffers(0, 2, vb, strides, offsets);
	m_device.context()->VSSetShaderResources(1, 1, &m_BuffDataSRV);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_device.context()->IASetInputLayout(m_inputLayout.get());
	m_device.context()->DrawInstanced(15,256,0,0);
	m_device.context()->VSSetShaderResources(2, 1, ppSRViewnullptr);
	renderGui();
}

void mini::Jelly::JellyApplication::renderGui()
{
	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
}

void mini::Jelly::JellyApplication::update(utils::clock const& clock)
{
	float dt = clock.frame_time();
	m_device.context()->CSSetUnorderedAccessViews(0, 2, ppUAView, nullptr);
	m_device.context()->CSSetShader(m_grass_cs.get(), NULL, 0);
	m_device.context()->Dispatch(1, 1, 1);
	m_device.context()->CSSetUnorderedAccessViews(0, 2, ppUAViewnullptr, nullptr);
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
