#include "bezierCube.h"
#include<string>

constexpr UINT idx[] = {
	/* 0, 1, 2, 3,
	 4, 5, 6, 7,
	 8, 9,10,11,
	12,13,14,15,

	16,17,18,19,
	20,21,22,23,
	24,25,26,27,
	28,29,30,31,

	32,33,34,35,
	36,37,38,39,
	40,41,42,43,
	44,45,46,47,

	48,49,50,51,
	52,53,54,55,
	56,57,58,59,
	60,61,62,63,
*/
		12,13,14,15,
		 8, 9,10,11,
		 4, 5, 6, 7,
		 0, 1, 2, 3,

		60,61,62,63,
		44,45,46,47,
		28,29,30,31,
		12,13,14,15,

		63,59,55,51,
		47,43,39,35,
		31,27,23,19,
		15,11, 7, 3,

		51,50,49,48,
		35,34,33,32,
		19,18,17,16,
		 3, 2, 1, 0,

		48,52,56,60,
		32,36,40,44,
		16,20,24,28,
		 0, 4, 8,12,

		63,62,61,60,
		59,58,57,56,
		55,54,53,52,
		51,50,49,48
};

using namespace DirectX;
mini::Jelly::BezierCube::BezierCube(DxDevice& device, std::wstring_view shader_name)
	:m_device(device), m_points(nullptr)
{
	auto vsBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"VS.cso");
	vs = m_device.CreateVertexShader(vsBytes);

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{/*LPCSTR SemanticName*/ "POSITION",
		/*UINT SemanticIndex*/ 0,
		/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32G32B32_FLOAT,
		/*UINT InputSlot*/ 0,
		/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
		/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_VERTEX_DATA,
		/*UINT InstanceDataStepRate*/ 0}
	};
	m_inputLayout = m_device.CreateInputLayout(inputLayout, 1, vsBytes);

	auto hsBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"HS.cso");
	hs = m_device.CreateHullShader(hsBytes);
	auto dsBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"DS.cso");
	ds = m_device.CreateDomainShader(dsBytes);

	auto psBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"PS.cso");
	ps = m_device.CreatePixelShader(psBytes);

	m_pointsBuffer = m_device.CreateVertexBuffer<XMFLOAT3>(64);

	m_indexBuffer = m_device.CreateIndexBuffer(idx);
}

void mini::Jelly::BezierCube::update(DirectX::XMFLOAT3 *points)
{
	m_points = points;
	m_pointsDirty = true;
}

void mini::Jelly::BezierCube::render()
{
	if (m_pointsDirty)
	{
		prepareMesh();
		m_pointsDirty = false;
	}

	m_device.context()->VSSetShader(vs.get(), nullptr, 0);
	m_device.context()->HSSetShader(hs.get(), nullptr, 0);
	m_device.context()->DSSetShader(ds.get(), nullptr, 0);
	m_device.context()->PSSetShader(ps.get(), nullptr, 0);

	m_device.context()->OMSetDepthStencilState(nullptr, 0);

	ID3D11Buffer *vb[] = { m_pointsBuffer.get() };
	const UINT strides[] = { sizeof(XMFLOAT3) };
	const UINT offsets[] = { 0 };
	m_device.context()->IASetVertexBuffers(0, 1, vb, strides, offsets);
	m_device.context()->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	m_device.context()->IASetInputLayout(m_inputLayout.get());
	
	m_device.context()->DrawIndexed(16*6, 0, 0);
}

void mini::Jelly::BezierCube::prepareMesh()
{
	D3D11_MAPPED_SUBRESOURCE map;
	m_device.context()->Map(m_pointsBuffer.get(),
		0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, m_points, 64 * sizeof(XMFLOAT3));
	m_device.context()->Unmap(m_pointsBuffer.get(), 0);
}
