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

	m_bc{ m_device, L"Jelly" },
	m_bcWireframe{ m_device, L"Line", Wireframe::Model::bezierCube },
	m_ctrlWireframe{ m_device, L"Line", Wireframe::Model::cubeBinds },
	m_playgroundWireframe{ m_device, L"Line", Wireframe::Model::cube },
	m_deformedModel{m_device, L"Morphed", "models\\Teapot.3ds"},

	m_simulation_step(0.005f),
	m_ds{ m_cube.p, m_cube.v, m_cube.f, pt_cnt, 0.1f, 0.4f, -10 },
	m_side_length(1.0f), m_impulse_strength(0.1f),

	m_springsys_edge{ m_cube.p, m_cube.f, pt_cnt, m_side_length/3, 75 },
	m_springsys_diag{ m_cube.p, m_cube.f, pt_cnt, m_side_length * sqrtf(2)/3, m_springsys_edge.m_elasticity },
	m_springsys_ctrl{ m_cube.p, m_cube.f, pt_cnt+8, 0, 20 },

	m_bs{m_cube.p, m_cube.v, pt_cnt, 1,0.9f }
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
	m_cbBezier = create_buffer<XMFLOAT4, 64>();

	
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
	cb = m_cbBezier;
	m_device.context()->VSSetConstantBuffers(3, 1, &cb);

	

	int i = 0;
	for (int z = 0; z < 4; z++)
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
			{
				if (x < 3) m_springs_e.push_back({ i,i + 1 });
				if (y < 3) m_springs_e.push_back({ i,i + 4 });
				if (z < 3) m_springs_e.push_back({ i,i + 16 });

				if (x < 3 && y < 3)	m_springs_d.push_back({ i, i + 1 + 4 });
				if (x < 3 && z < 3)	m_springs_d.push_back({ i, i + 1 + 16 });
				if (y < 3 && z < 3)	m_springs_d.push_back({ i, i + 4 + 16 });
											 
				if (x > 0 && y < 3)	m_springs_d.push_back({ i, i - 1 + 4 });
				if (y > 0 && z < 3)	m_springs_d.push_back({ i, i - 4 + 16 });
				if (z > 0 && x < 3)	m_springs_d.push_back({ i, i - 16 + 1 });
				i++;
			}
	i = 0;
	for (int z = 0; z < 2; z++)
		for (int y = 0; y < 2; y++)
			for (int x = 0; x < 2; x++)
			{
				m_springs_c.push_back({ (int)pt_cnt+i, x*3 + y*12 + z*48 });
				i++;
			}
	resetJelly();
	updateControl();
	updatePlayground();
}

void mini::Jelly::JellyApplication::render()
{
	float clearColor[4] = { 0.7f, 0.6f, 0.9f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_renderTargetView.get(), clearColor);
	m_device.context()->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1, 0);

	m_deformedModel.render();
	if (m_show_bezier)
		m_bc.render();
	if (m_show_wireframe)
		m_bcWireframe.render();
	if (m_show_control_frame)
		m_ctrlWireframe.render();
	if (m_show_playground)
		m_playgroundWireframe.render();

	renderGui();
}

void mini::Jelly::JellyApplication::renderGui()
{
	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::Begin("Control box");
	bool m = ImGui::DragFloat3("Position", (float*)&m_control_transform.m_position, 0.1f);
	m |= ImGui::DragFloat3("Rotation", (float*)&m_control_transform.m_rpy, 0.02f);
	m |= ImGui::DragFloat3("Scale", (float*)&m_control_transform.m_scale, 0.1f);
	ImGui::DragFloat("Spring strength", &m_springsys_ctrl.m_elasticity, 0.1f);
	ImGui::Checkbox("Enable control box", &m_control_box_enable);
	if (m)
		updateControl();
	ImGui::End(); // Control box

	ImGui::Begin("Jelly cube");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() - 185);
	if (ImGui::DragFloat("Internal spring strength", &m_springsys_edge.m_elasticity, 0.1f)
		&& m_edge_diag_strength_lock)
		m_springsys_diag.m_elasticity = m_springsys_edge.m_elasticity;
	if (m_edge_diag_strength_lock)
		ImGui::LabelText("Diagonal spring strength", "%.3f", m_springsys_diag.m_elasticity);
	else
		ImGui::DragFloat("Diagonal spring strength", &m_springsys_diag.m_elasticity, 0.1f);
	if (ImGui::Checkbox("Lock both spring types", &m_edge_diag_strength_lock))
		m_springsys_diag.m_elasticity = m_springsys_edge.m_elasticity;
	if (ImGui::DragFloat("Mass of single point", &m_ds.m_mass, 0.1f))
		m_ds.m_invmass = 1.0f / m_ds.m_mass;
	ImGui::DragFloat("Impulse", &m_impulse_strength, 0.01f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Space", { 70,0 }) || ImGui::IsKeyPressed(ImGuiKey_Space, false))
		m_ds.displace(m_impulse_strength);
	ImGui::DragFloat("Drag", &m_ds.m_damping, 0.01f);
	ImGui::DragFloat3("Gravity", (float*)&m_ds.m_gravity, 0.1f);
	ImGui::End(); // Jelly cube


	ImGui::Begin("Miscellaneous");
	ImGui::Checkbox("Show wireframe", &m_show_wireframe);
	ImGui::Checkbox("Show bezier", &m_show_bezier);
	ImGui::Checkbox("Show control frame", &m_show_control_frame);
	ImGui::Checkbox("Show room", &m_show_playground);
	ImGui::End(); // Miscellaneous

	ImGui::Begin("Playground");
	if (ImGui::DragFloat("Extent", &m_bs.m_extent, 0.01f))
		updatePlayground();
	ImGui::DragFloat("Elasticity", &m_bs.m_elasticity, 0.01f);
	ImGui::Checkbox("Sticky", &m_bs.m_sticky);
	ImGui::End(); // Playground
}

void mini::Jelly::JellyApplication::update(utils::clock const& clock)
{
	float dt = clock.frame_time();
	m_simulation_time_left += dt;
	while (m_simulation_time_left > m_simulation_step)
	{
		m_springsys_edge.update(m_springs_e.data(), (int)m_springs_e.size());
		m_springsys_diag.update(m_springs_d.data(), (int)m_springs_d.size());
		if (m_control_box_enable)
			m_springsys_ctrl.update(m_springs_c.data(), (int)m_springs_c.size());
		float dt = m_simulation_step;

		m_ds.update(dt);
		m_bs.update(dt);
		m_simulation_time_left -= dt;
	}
	m_bc.update(m_cube.p);
	m_bcWireframe.update(m_cube.p);
	m_ctrlWireframe.update(m_cube.p);
	m_playgroundWireframe.update(&m_cube.p[pt_cnt + 8]);
	{
		XMFLOAT4 arr[64];
		for (int i = 0; i < 64; i++)
			arr[i] = { m_cube.p[i].x, m_cube.p[i].y, m_cube.p[i].z, 0 };
		m_cbBezier.Update(m_device.context(), arr);
	}
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
	
	if (ImGui::IsKeyPressed(ImGuiKey_Q, false))
		m_show_bezier = !m_show_bezier;
	if (ImGui::IsKeyPressed(ImGuiKey_W, false))
		m_show_wireframe = !m_show_wireframe;
	if (ImGui::IsKeyPressed(ImGuiKey_E, false))
		m_show_control_frame = !m_show_control_frame;

	if (ImGui::IsKeyPressed(ImGuiKey_X, false))
		resetJelly();
}

void mini::Jelly::JellyApplication::updateControl()
{
	int i = 0;
	for (int z = 0; z < 2; z++)
		for (int y = 0; y < 2; y++)
			for (int x = 0; x < 2; x++)
			{
				XMVECTOR v{ (float)x,(float)y,(float)z };
				v -= XMVECTOR{ 1,1,1 }*0.5f;
				v = v * m_side_length;
				v = XMVector3Transform(v, m_control_transform.get());
				XMStoreFloat3(&m_cube.p[pt_cnt + i], v);
				i++;
			}
}

void mini::Jelly::JellyApplication::updatePlayground()
{
	int i = 0;
	for (int z = 0; z < 2; z++)
		for (int y = 0; y < 2; y++)
			for (int x = 0; x < 2; x++)
			{
				XMVECTOR v{ (float)x,(float)y,(float)z };
				v -= XMVECTOR{ 1,1,1 }*0.5f;
				v = v * m_bs.m_extent*2.0f;
				XMStoreFloat3(&m_cube.p[pt_cnt + 8 + i], v);
				i++;
			}
}

void mini::Jelly::JellyApplication::resetJelly()
{
	int i = 0;
	for (int z = 0; z < 4; z++)
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
			{
				XMVECTOR v{ (float)x,(float)y,(float)z };
				v = v / 3 - XMVECTOR{ 1,1,1 }*0.5f;
				v = v * m_side_length;
				XMStoreFloat3(&m_cube.p[i], v + 0.0f * XMVECTOR{ 0,1,0 });
				XMStoreFloat3(&m_cube.v[i], 0 * XMVECTOR{ 1,-x*0.1f,0 });
				i++;
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
