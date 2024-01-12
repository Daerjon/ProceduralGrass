#include "springSystem.h"

using namespace mini::Jelly;
using namespace DirectX;


mini::Jelly::SpringSystem::SpringSystem(
	DirectX::XMFLOAT3* points, DirectX::XMFLOAT3* forces, int point_count,
	 float spring_length, float elasticity_constant)
	:m_points(points), m_forces(forces), m_point_count(point_count),
	m_spring_length(spring_length),
	m_elasticity(elasticity_constant)
{
}

void mini::Jelly::SpringSystem::update(XMINT2* springs, int spring_count)
{
	for (int i = 0; i < spring_count; i++)
	{
		auto p1 = XMLoadFloat3(&m_points[springs[i].x]);
		auto p2 = XMLoadFloat3(&m_points[springs[i].y]);

		auto diff = p2 - p1;
		auto len = XMVectorGetX(XMVector3Length(diff));
		if (len < 1e-6)
			continue;

		auto forceMag = (len - m_spring_length) * m_elasticity;

		auto force = diff * (forceMag / len);

		auto f1 = XMLoadFloat3(&m_forces[springs[i].x]);
		auto f2 = XMLoadFloat3(&m_forces[springs[i].y]);

		XMStoreFloat3(&m_forces[springs[i].x], f1 + force);
		XMStoreFloat3(&m_forces[springs[i].y], f2 - force);
	}
}
