#include "jellyApplication.h"
#include"imgui.h"
#include <chrono>

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
	m_time(0.0f),
	m_simulation_step(0.005f),
	
	m_side_length(1.0f), m_impulse_strength(0.1f),
	m_s{
		1.0f/7, 1.0f/7,
		64.0f, 20.0f, 2.5f, 1.0f
	}
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
	m_cbTime = create_buffer<XMFLOAT4, 2>();
	m_cbGroup = create_buffer<XMFLOAT4, 2>();

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
	cb = m_cbTime;
	m_device.context()->CSSetConstantBuffers(0, 1, &cb);
	cb = m_cbGroup;
	m_device.context()->CSSetConstantBuffers(1, 1, &cb);


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

	rasterizer_info ri{};
	ri.CullMode = D3D11_CULL_NONE;
	m_rasterizerState = m_device.CreateRasterizerState(ri);

	auto vsBytes = m_device.LoadByteCode(L"Test" L"VS.cso");
	m_test_vs = m_device.CreateVertexShader(vsBytes);

	D3D11_INPUT_ELEMENT_DESC empty;
	m_grassinputLayout = m_device.CreateInputLayout(&empty, 0, vsBytes);

	std::vector<inputElement> poss;
	for(int x = 0; x<100; x++)
		for (int z = 0; z < 100; z++)
		{
			poss.push_back(
				inputElement{
					XMFLOAT3{ static_cast<float>(x) / 5,0.0f,static_cast<float>(z) / 5 }
				});
		}

	//m_bladeBuffer = m_device.CreateVertexBuffer(poss.data(), poss.size());
	//m_bladeBuffer = m_device.CreateVertexBuffer<float>(0);

	auto psBytes = m_device.LoadByteCode(L"Test"  L"PS.cso");
	m_test_ps = m_device.CreatePixelShader(psBytes);

	auto csBytes = m_device.LoadByteCode(L"Grass"  L"CS.cso");
	m_grass_cs = m_device.CreateComputeShader(csBytes);


	vsBytes = m_device.LoadByteCode(L"Ground" L"VS.cso");
	m_ground_vs = m_device.CreateVertexShader(vsBytes);

	D3D11_INPUT_ELEMENT_DESC groundLayout[] =
	{
		{/*LPCSTR SemanticName*/ "Position",
		/*UINT SemanticIndex*/ 0,
		/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32G32B32_FLOAT,
		/*UINT InputSlot*/ 0,
		/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
		/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_VERTEX_DATA,
		/*UINT InstanceDataStepRate*/ 0}
	};
	m_groundinputLayout = m_device.CreateInputLayout(groundLayout, 1, vsBytes);
	float groundExtent = m_s.worldSize/2;
	XMFLOAT3 quad[] =
	{
		{-groundExtent, 0, -groundExtent},
		{groundExtent, 0, -groundExtent},
		{-groundExtent, 0, groundExtent},
		{groundExtent, 0, groundExtent}
	};
	m_groundBuffer = m_device.CreateVertexBuffer(quad);
	psBytes = m_device.LoadByteCode(L"Ground" L"PS.cso");
	m_ground_ps = m_device.CreatePixelShader(psBytes);
	auto dsBytes = m_device.LoadByteCode(L"Ground" L"DS.cso");
	m_ground_ds = m_device.CreateDomainShader(dsBytes);
	auto hsBytes = m_device.LoadByteCode(L"Ground" L"HS.cso");
	m_ground_hs = m_device.CreateHullShader(hsBytes);

	/*ID3D11UnorderedAccessView* ppUAView[2] = { m_BuffDataUAV, m_BuffNumberUAV };
	m_device.context()->CSSetUnorderedAccessViews(0, 2, ppUAView, nullptr);
	m_device.context()->CSSetShader(m_grass_cs.get(), NULL, 0);
	m_device.context()->Dispatch(1, 1, 1);
	ID3D11UnorderedAccessView* ppUAViewnullptr[2] = { nullptr, nullptr };
	m_device.context()->CSSetUnorderedAccessViews(0, 2, ppUAViewnullptr, nullptr);*/

	m_device.context()->RSSetState(m_rasterizerState.get());
}

void mini::Jelly::JellyApplication::render()
{
	float clearColor[4] = { 0.15f, 0.7f, 0.85f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_renderTargetView.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1, 0);


	m_device.context()->VSSetShader(m_ground_vs.get(), nullptr, 0);
	m_device.context()->PSSetShader(m_ground_ps.get(), nullptr, 0);
	m_device.context()->HSSetShader(m_ground_hs.get(), nullptr, 0);
	m_device.context()->DSSetShader(m_ground_ds.get(), nullptr, 0);
	ID3D11Buffer* vb[] = { m_groundBuffer.get() };
	const UINT stride[] = { sizeof(XMFLOAT3) };
	const UINT offset[] = { 0 };
	m_device.context()->IASetVertexBuffers(0, 1, vb, stride, offset);
	m_device.context()->IASetInputLayout(m_groundinputLayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_device.context()->Draw(4, 0);

	doGrass();
	renderGui();
}

void mini::Jelly::JellyApplication::renderGui()
{
	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	{
		ImGui::Begin("Settings");
		ImGui::PushItemWidth(100);
		ImGui::LabelText("FPS:", "%.1f", m_fps);
		float sc = m_s.timeScale * 100;
		ImGui::DragFloat("Wind speed:", &sc, 0.01f, 0, 0, "%.2f");
		m_s.timeScale = sc / 100;
		sc = 1/m_s.noiseScale;
		ImGui::DragFloat("Wind blow scaling:", &sc, 0.01f, 0.01f, 0, "%.2f");
		m_s.noiseScale = 1 / sc;
		int tmp = (int)m_s.worldSize;
		ImGui::DragInt("World size:", &tmp, 1, 1, 99999, "%d", ImGuiSliderFlags_AlwaysClamp);
		m_s.worldSize = (float)tmp;
		tmp = (int)(m_s.maxGroups/2);
		ImGui::DragInt("Amount of grass:", &tmp, 1, 1, 40, "%d", ImGuiSliderFlags_AlwaysClamp);
		m_s.maxGroups = (float)tmp*2;
		ImGui::DragFloat("Grass clump size:", &m_s.clumpSize, 0.01f, 0.01f, 0, "%.2f");
		ImGui::DragFloat("Grass clumping factor:", &m_s.clumping, 0.01f, 0, 0, "%.2f");
		sc = m_viewFrustum.farPlane();
		if (ImGui::DragFloat("Far plane:", &sc, 0.1f, m_viewFrustum.nearPlane(), 100, "%.1f"))
		{
			m_viewFrustum.setFarPlane(sc);
			XMFLOAT4X4 m;
			XMStoreFloat4x4(&m, m_viewFrustum.getProjectionMatrix());
			update_buffer(m_cbProj, m);
		}

		ImGui::PopItemWidth();
		ImGui::End();
	}
}

void mini::Jelly::JellyApplication::update(utils::clock const& clock)
{
	XMFLOAT4 t[] = {
		{ m_time, // time
		m_s.timeScale, // time scale
		m_s.noiseScale, // noise scale
		0.0f }, // free
		{ m_s.worldSize, // world size
		m_s.maxGroups, // max amount of render groups in one dimension
		m_s.clumpSize, // clump size
		m_s.clumping} // clumping factor
	};
	update_buffer(m_cbTime, t);
	float dt = clock.frame_time();
	m_fps = clock.fps();
	m_time += dt;
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

void mini::Jelly::JellyApplication::doGrass()
{
	m_device.context()->VSSetShader(m_test_vs.get(), nullptr, 0);
	m_device.context()->PSSetShader(m_test_ps.get(), nullptr, 0);
	m_device.context()->HSSetShader(nullptr, nullptr, 0);
	m_device.context()->DSSetShader(nullptr, nullptr, 0);
	m_device.context()->CSSetShader(m_grass_cs.get(), NULL, 0);


	m_device.context()->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	m_device.context()->IASetInputLayout(m_grassinputLayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	float minGroupSize = m_s.worldSize / m_s.maxGroups;
	
	std::vector<uint8_t> skip((size_t)((m_s.maxGroups)* (m_s.maxGroups)));

	XMFLOAT4 eye = m_camera.camera_position();
	m_camera.zoom(5);
	auto mats = m_camera.view_matrix() * m_viewFrustum.getProjectionMatrix();
	m_camera.zoom(-5);
	for (int k = 128; k > 0; k >>= 1)
	{
		for (int i = 0; i < m_s.maxGroups; i += k)
			for (int j = 0; j < m_s.maxGroups; j += k)
			{
				{
					if (skip[(size_t)(i * m_s.maxGroups + j)])
						continue;
					int lod = (int)fminf(fminf((float)k, m_s.maxGroups - j), m_s.maxGroups - i);
					float mind = 0;
					bool in = false;
					for (int ii = i; ii <= i + lod; ii += lod)
						for (int jj = j; jj <= j + lod; jj += lod)
						{
							XMFLOAT4 center = { ii * minGroupSize - m_s.worldSize / 2, 0,jj * minGroupSize - m_s.worldSize / 2, 1 };
							float dist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&center) - XMLoadFloat4(&eye)));
							auto v = XMVector4Transform(XMLoadFloat4(&center), mats);
							v = v / XMVectorGetW(v);
							if (fabsf(XMVectorGetX(v)) < 1 && fabsf(XMVectorGetY(v)) < 1 && fabsf(XMVectorGetZ(v)) < 1)
								in = true;
							mind = fminf(mind, dist);
						}
					if (!in || mind < 15 * (lod >> 1))
						continue;

					for (int ii = i; ii < i + lod; ii++)
						for (int jj = j; jj < j + lod; jj++)
							skip[(size_t)(ii * m_s.maxGroups + jj)] = 1;
					float groupSize = lod * minGroupSize;

					XMFLOAT4 center = { (i + lod * 0.5f) * minGroupSize - m_s.worldSize / 2, 0,(j + lod * 0.5f) * minGroupSize - m_s.worldSize / 2, 1 };

					XMFLOAT4 g[] = {
						{ center.x - groupSize / 2, center.z - groupSize / 2, groupSize, (float)lod },
						eye
					};
					update_buffer(m_cbGroup, g);
				}
				ID3D11UnorderedAccessView* ppUAView[2] = { m_BuffDataUAV, m_BuffNumberUAV };
				m_device.context()->CSSetUnorderedAccessViews(0, 2, ppUAView, nullptr);
				m_device.context()->Dispatch(1, 1, 1);
				ID3D11UnorderedAccessView* ppUAViewnullptr[2] = { nullptr, nullptr };
				m_device.context()->CSSetUnorderedAccessViews(0, 2, ppUAViewnullptr, nullptr);

				m_device.context()->VSSetShaderResources(1, 1, &m_BuffDataSRV);
				m_device.context()->DrawInstanced(15, MaxBlades, 0, 0);
				ID3D11ShaderResourceView* ppSRViewnullptr[1] = { nullptr };
				m_device.context()->VSSetShaderResources(1, 1, ppSRViewnullptr);
			}
	}
}
