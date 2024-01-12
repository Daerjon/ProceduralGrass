#pragma once
#include"dxDevice.h"
#include<string_view>

using namespace mini::directx;
namespace mini::Jelly
{
	class BezierCube
	{
	public:
		BezierCube(DxDevice& device, std::wstring_view shader_name);
		void update(DirectX::XMFLOAT3 *points);
		void render();
	private:
		void prepareMesh();

	private:
		dx_ptr<ID3D11InputLayout> m_inputLayout;
		dx_ptr<ID3D11VertexShader> vs;
		dx_ptr<ID3D11DomainShader> ds;
		dx_ptr<ID3D11HullShader> hs;
		dx_ptr<ID3D11PixelShader> ps;
		
		DirectX::XMFLOAT3 *m_points;
		bool m_pointsDirty = true;
	public:
		dx_ptr<ID3D11Buffer> m_pointsBuffer;
	private:
		dx_ptr<ID3D11Buffer> m_indexBuffer;

		DxDevice& m_device;
	};
}