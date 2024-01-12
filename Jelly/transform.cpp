#include "transform.h"
using namespace DirectX;

mini::Jelly::Transform::Transform(DirectX::XMFLOAT3 location, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation)
	:m_position(location), m_scale(scale), m_rpy(rotation)
{
}

mini::Jelly::Transform::Transform()
	:m_position(0.0f,0,0), m_scale(1.0f,1,1),m_rpy(0,0,0)
{
}

DirectX::XMMATRIX mini::Jelly::Transform::get()
{
	return XMMatrixTransformation(
		{}, XMQuaternionIdentity(), XMLoadFloat3(&m_scale),
		{}, XMQuaternionRotationRollPitchYaw(m_rpy.x, m_rpy.y, m_rpy.z),
		XMLoadFloat3(&m_position)
	);
}
