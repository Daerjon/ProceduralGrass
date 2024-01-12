#pragma once
#include"myDxApplication.h"

#include"camera.h"
#include"viewFrustrum.h"

#include"transform.h"

#include"bezierCube.h"
#include"wireframe.h"
#include"myModel.h"

#include"springSystem.h"
#include"dynamicSystem.h"
#include"boundsSystem.h"


using namespace mini::directx;
namespace mini::Jelly
{
	constexpr size_t pt_cnt = 64;

	class JellyApplication : public MyDxApplication
	{
		static constexpr std::wstring_view s_Jelly_window_title{L"Żelek"};
	public:
		JellyApplication(HINSTANCE instance);
	private:
		void render() override;
		void renderGui();
		void update(utils::clock const& clock) override;
		void updateControls(float dt);

		void updateControl();
		void updatePlayground();
		void resetJelly();
		void updateCamera();

		orbit_camera m_camera;
		float m_camera_speed;
		ViewFrustrum m_viewFrustum;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbView;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbProj;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbModel;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbColor;
		ConstantBuffer<DirectX::XMFLOAT4, 64> m_cbBezier;


		float m_side_length, m_impulse_strength;
		struct {
			DirectX::XMFLOAT3
				p[pt_cnt+8+8]{}, v[pt_cnt]{}, f[pt_cnt+8]{};
		} m_cube;
		Transform m_control_transform{};
		std::vector<DirectX::XMINT2> m_springs_e;
		std::vector<DirectX::XMINT2> m_springs_d;
		std::vector<DirectX::XMINT2> m_springs_c;

		float m_simulation_step;
		float m_simulation_time_left{};

		bool m_show_wireframe = true, m_show_bezier = true;
		bool m_show_control_frame = true, m_show_playground = true;
		Wireframe m_ctrlWireframe;
		Wireframe m_playgroundWireframe;
		Wireframe m_bcWireframe;
		BezierCube m_bc;
		MyModel m_deformedModel;

		DynamicSystem m_ds;
		bool m_edge_diag_strength_lock = true;
		SpringSystem m_springsys_edge, m_springsys_diag;
		bool m_control_box_enable = true;
		SpringSystem m_springsys_ctrl;
		BoundsSystem m_bs;

	};
}