#pragma once
#include<DirectXMath.h>


namespace mini::Jelly
{
	class BoundsSystem
	{
	public:
		BoundsSystem(DirectX::XMFLOAT3* points, DirectX::XMFLOAT3* velocities, int point_count,
			float extent, float elasticity_constant);
		void update(float dt);

		bool m_sticky;
	private:

		int m_point_count;
		DirectX::XMFLOAT3* m_points, * m_velocities;
	public:
		float m_extent, m_elasticity;
	};
}