#include "wireframe.h"

using namespace DirectX;

constexpr UINT cubeIdx[] = {
	0,1,
	2,3,
	4,5,
	6,7,

	0,2,
	1,3,
	4,6,
	5,7,
	
	0,4,
	1,5,
	2,6,
	3,7
};
std::vector<UINT> bezierCubeIdx{};
#define PUSH_2(v,a,b)do{v.push_back(a);v.push_back(b);}while(0)
static void prepareBezierCubeIdx()
{
	for(int z = 0; z<4; z++)
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
			{
				int i = x + 4 * y + 16 * z;
				if (x < 3)
					PUSH_2(bezierCubeIdx, i, i + 1);
				if (y < 3)
					PUSH_2(bezierCubeIdx, i, i + 4);
				if (z < 3)
					PUSH_2(bezierCubeIdx, i, i + 16);
			}
}
mini::Jelly::Wireframe::Wireframe(DxDevice& device, std::wstring_view shader_name, Model type)
	:m_device(device), m_points(nullptr), m_type(type), m_externPointsBuffer(nullptr)
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

	auto psBytes = m_device.LoadByteCode(std::wstring(shader_name) + L"PS.cso");
	ps = m_device.CreatePixelShader(psBytes);

	depth_stencil_info dsi;
	dsi.DepthEnable = false;
	m_depthStencilState = m_device.CreateDepthStencilState(dsi);

	m_pointsBuffer = m_device.CreateVertexBuffer<XMFLOAT3>(72);

	switch (type)
	{
	case Model::bezierCube:
		if (bezierCubeIdx.size() == 0)
			prepareBezierCubeIdx();
		m_indexBuffer = m_device.CreateIndexBuffer(bezierCubeIdx);
		break;
	case Model::cube:
		m_indexBuffer = m_device.CreateIndexBuffer(cubeIdx);
		break;
	case Model::cubeBinds:
		auto idx = std::vector<UINT>();
		for (int i = 0; i < 24; i++)
			idx.push_back(cubeIdx[i]+64);

		for (int z = 0; z < 2; z++)
			for (int y = 0; y < 2; y++)
				for (int x = 0; x < 2; x++)
					PUSH_2(idx, 64 + (x + y * 2 + z * 4), x * 3 + y * 12 + z * 48);

		m_indexBuffer = m_device.CreateIndexBuffer(idx);
		break;
	}
}

void mini::Jelly::Wireframe::update(DirectX::XMFLOAT3* points)
{
	m_points = points;
	m_externPointsBuffer = nullptr;
	m_pointsDirty = true;
}

void mini::Jelly::Wireframe::update(ID3D11Buffer* points)
{
	if(m_pointsBuffer)
		m_pointsBuffer.release();
	m_externPointsBuffer = points;
	m_points = nullptr;
	m_pointsDirty = false;
}

void mini::Jelly::Wireframe::render()
{
	if (m_pointsDirty)
	{
		prepareMesh();
		m_pointsDirty = false;
	}

	m_device.context()->VSSetShader(vs.get(), nullptr, 0);
	m_device.context()->HSSetShader(nullptr, nullptr, 0);
	m_device.context()->DSSetShader(nullptr, nullptr, 0);
	m_device.context()->PSSetShader(ps.get(), nullptr, 0);

	m_device.context()->OMSetDepthStencilState(m_depthStencilState.get(), 0);

	ID3D11Buffer* vb[] = { m_pointsBuffer.get() };
	if (*vb == nullptr)*vb = m_externPointsBuffer;
	const UINT strides[] = { sizeof(XMFLOAT3) };
	const UINT offsets[] = { 0 };
	m_device.context()->IASetVertexBuffers(0, 1, vb, strides, offsets);
	m_device.context()->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_device.context()->IASetInputLayout(m_inputLayout.get());

	int idxn = 0;
	switch (m_type)
	{
	case Model::bezierCube:
		idxn = 2 * 3 * 3 * 16;
		break;
	case Model::cube:
		idxn = 2 * 3 * 1 * 4;
		break;
	case Model::cubeBinds:
		idxn = 2 * 3 * 1 * 4 + 16;
		break;
	}
	m_device.context()->DrawIndexed(idxn, 0, 0);
}


void mini::Jelly::Wireframe::prepareMesh()
{
	D3D11_MAPPED_SUBRESOURCE map;
	m_device.context()->Map(m_pointsBuffer.get(),
		0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	int vtxn = 0;
	switch (m_type)
	{
	case Model::bezierCube:
		vtxn = 64;
		break;
	case Model::cube:
		vtxn = 8;
		break;
	case Model::cubeBinds:
		vtxn = 64 + 8;
		break;
	}
	memcpy(map.pData, m_points, vtxn * sizeof(XMFLOAT3));
	m_device.context()->Unmap(m_pointsBuffer.get(), 0);
}
