#pragma once
#include"myDxApplication.h"

#include"camera.h"
#include"viewFrustrum.h"



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

		void updateCamera();

		orbit_camera m_camera;
		float m_camera_speed;
		ViewFrustrum m_viewFrustum;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbView;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbProj;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbModel;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbColor;


		float m_side_length, m_impulse_strength;
		struct {
			DirectX::XMFLOAT3
				p[pt_cnt+8+8]{}, v[pt_cnt]{}, f[pt_cnt+8]{};
		} m_cube;

		float m_simulation_step;
		float m_simulation_time_left{};

		
	};
}