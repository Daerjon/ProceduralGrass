#include "dynamicSystem.h"
#include<memory>

using namespace mini::Jelly;
using namespace DirectX;

mini::Jelly::DynamicSystem::DynamicSystem(
	DirectX::XMFLOAT3* points, DirectX::XMFLOAT3* velocities, DirectX::XMFLOAT3* forces, int point_count,
	float mass, float damping_constant, float gravity)
	:m_points(points), m_velocities(velocities), m_forces(forces),
	m_point_count(point_count),
	m_damping(damping_constant), m_mass(mass), m_invmass(1.0f/mass),
	m_gravity{ 0,gravity,0 }
{

}

void mini::Jelly::DynamicSystem::update(float dt)
{
	for (int i = 0; i < m_point_count; i++)
	{
		auto p = XMLoadFloat3(&m_points[i]);
		auto v = XMLoadFloat3(&m_velocities[i]);
		auto f = XMLoadFloat3(&m_forces[i]);

		XMStoreFloat3(&m_points[i],
			p + dt * v
		);
		float speedSq = XMVectorGetX(XMVector3Length(v));
		f += -v * m_damping;
		auto gravity = XMLoadFloat3(&m_gravity);
		XMStoreFloat3(&m_velocities[i],
			v  + dt * (f * m_invmass + gravity)
		);
	}
	memset(m_forces, 0, sizeof(XMFLOAT3) * m_point_count);
}

void mini::Jelly::DynamicSystem::displace(float strength)
{
	for (int i = 0; i < m_point_count; i++)
	{
		auto f = XMLoadFloat3(&m_points[i]);
		XMFLOAT3 r =
		{
			rand() * 2.0f / RAND_MAX - 1,
			rand() * 2.0f / RAND_MAX - 1,
			rand() * 2.0f / RAND_MAX - 1
		};


		XMStoreFloat3(&m_points[i],
			f + XMVector3Normalize(XMLoadFloat3(&r))*strength
		);
	}
}
