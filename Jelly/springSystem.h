#pragma once
#include<DirectXMath.h>


namespace mini::Jelly
{
	class SpringSystem
	{
	public:
		SpringSystem(DirectX::XMFLOAT3 *points, DirectX::XMFLOAT3 *forces, int point_count,
			float spring_length, float elasticity_constant);
		void update(DirectX::XMINT2* springs, int spring_count);
	private:

		int m_point_count;
		DirectX::XMFLOAT3* m_points,* m_forces;
		float m_spring_length;
	public:
		float m_elasticity;
	};
}