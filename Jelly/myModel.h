#pragma once
#include"dxDevice.h"
#include"modelLoader.h"
#include"inputLayoutManager.h"
#include<string_view>

using namespace mini::directx;
namespace mini::Jelly
{
	class MyModel
	{
	public:
		MyModel(DxDevice& device,
			std::wstring_view shader_name, std::string_view model_name);
		void render();

	private:
		ID3D11InputLayout* m_inputLayout;
		dx_ptr<ID3D11VertexShader> vs;
		dx_ptr<ID3D11PixelShader> ps;

	private:

		Model m_jellyModel;
		ModelLoader m_modelLoader;
		InputLayoutManager m_inputLayoutManager;
		DxDevice& m_device;
	};
}