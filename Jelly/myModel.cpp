#include "myModel.h"

mini::Jelly::MyModel::MyModel(DxDevice& device,
	std::wstring_view shader_name, std::string_view model_name)
	:
	m_device{ device },
	m_modelLoader{ device },
	m_inputLayoutManager{ device }
{
	m_jellyModel = m_modelLoader.LoadFromFile(std::string(model_name), m_inputLayoutManager);
	auto vsBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"VS.cso");
	vs = m_device.CreateVertexShader(vsBytes);


	auto psBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"PS.cso");
	ps = m_device.CreatePixelShader(psBytes);

	auto id = m_inputLayoutManager.registerSignatureID(vsBytes);
	m_inputLayout = m_inputLayoutManager.getLayout(m_jellyModel.getMeshSignatureID(0), id).get();
}

void mini::Jelly::MyModel::render()
{
	m_device.context()->VSSetShader(vs.get(), nullptr, 0);
	m_device.context()->HSSetShader(nullptr, nullptr, 0);
	m_device.context()->DSSetShader(nullptr, nullptr, 0);
	m_device.context()->PSSetShader(ps.get(), nullptr, 0);

	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	m_device.context()->IASetInputLayout(m_inputLayout);
	m_jellyModel.getMesh(0).Render(m_device.context());

}
