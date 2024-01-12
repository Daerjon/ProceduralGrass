#pragma once
#include"dxDevice.h"
#include<string_view>

using namespace mini::directx;
namespace mini::Jelly
{
	class Wireframe
	{
	public:
		enum class Model { bezierCube, cube, cubeBinds };
		Wireframe(DxDevice& device, std::wstring_view shader_name, Model type);
		void update(DirectX::XMFLOAT3* points);
		void update(ID3D11Buffer* points);
		void render();
	private:
		void prepareMesh();

	private:
		dx_ptr<ID3D11InputLayout> m_inputLayout;
		dx_ptr<ID3D11VertexShader> vs;
		dx_ptr<ID3D11PixelShader> ps;
		dx_ptr<ID3D11DepthStencilState> m_depthStencilState;

		Model m_type;
		DirectX::XMFLOAT3* m_points;
		bool m_pointsDirty = true;
		dx_ptr<ID3D11Buffer> m_pointsBuffer;
		ID3D11Buffer* m_externPointsBuffer;
		dx_ptr<ID3D11Buffer> m_indexBuffer;

		DxDevice& m_device;
	};
}