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

		dx_ptr<ID3D11PixelShader> m_test_ps;
		dx_ptr<ID3D11VertexShader> m_test_vs;
		dx_ptr<ID3D11ComputeShader> m_grass_cs;
		dx_ptr<ID3D11InputLayout > m_inputLayout;
		dx_ptr<ID3D11Buffer> m_bladeBuffer;

		dx_ptr<ID3D11Buffer> m_CS1DataBuffer;
		ID3D11UnorderedAccessView* m_BuffDataUAV;
		ID3D11ShaderResourceView* m_BuffDataSRV;
		dx_ptr<ID3D11Buffer> m_DataBuff;

		dx_ptr<ID3D11Buffer> m_CS1Number;
		ID3D11UnorderedAccessView* m_BuffNumberUAV;
		ID3D11ShaderResourceView* m_BuffNumberSRV;

		ID3D11ShaderResourceView* ppSRViewnullptr[1] = { nullptr };
		ID3D11UnorderedAccessView* ppUAViewnullptr[2] = { nullptr, nullptr };
		ID3D11UnorderedAccessView* ppUAView[2] = { m_BuffDataUAV, m_BuffNumberUAV };


		float m_side_length, m_impulse_strength;
		struct {
			DirectX::XMFLOAT3
				p[pt_cnt+8+8]{}, v[pt_cnt]{}, f[pt_cnt+8]{};
		} m_cube;

		float m_simulation_step;
		float m_simulation_time_left{};

		
	};
}