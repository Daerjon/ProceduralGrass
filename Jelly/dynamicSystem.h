#pragma once
#include<DirectXMath.h>


namespace mini::Jelly
{
	class DynamicSystem
	{
	public:
		class DynamicSystem(
			DirectX::XMFLOAT3* points, DirectX::XMFLOAT3* velocities, DirectX::XMFLOAT3* forces, int point_count,
			float mass, float damping_constant, float gravity);
		void update(float dt);
		void displace(float strength);
	private:

		int m_point_count;
		DirectX::XMFLOAT3* m_points, *m_velocities, *m_forces;
	public:
		DirectX::XMFLOAT3 m_gravity;
		float m_mass, m_invmass, m_damping;
	};
}