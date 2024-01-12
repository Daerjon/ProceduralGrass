#pragma once
#include<DirectXMath.h>


namespace mini::Jelly
{
	class Transform
	{
	public:
		Transform(
			DirectX::XMFLOAT3 location,
			DirectX::XMFLOAT3 scale,
			DirectX::XMFLOAT3 rotation);
		Transform();

		DirectX::XMMATRIX get();

		DirectX::XMFLOAT3 m_position, m_scale, m_rpy;
	};
}