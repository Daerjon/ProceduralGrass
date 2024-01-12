#include "boundsSystem.h"

using namespace DirectX;
using namespace mini::Jelly;
mini::Jelly::BoundsSystem::BoundsSystem(
	DirectX::XMFLOAT3* points, DirectX::XMFLOAT3* velocities, int point_count,
	float extent, float elasticity_constant)
	:m_points(points), m_velocities(velocities), m_point_count(point_count),
	m_extent(extent), m_elasticity(elasticity_constant),
	m_sticky(true)
{
}


void mini::Jelly::BoundsSystem::update(float dt)
{
	for (int i = 0; i < m_point_count; i++)
	{
		bool sticky_collided = false;
		auto& p = m_points[i];
		auto& v = m_velocities[i];

		float* pp = (float*)&p;
		float* pv = (float*)&v;
		int iters = 0;
		bool bounced = false;
		do
		{
			bounced = false;
			for (int j = 0; j < 3; j++)
			{
				if (fabs(pp[j]) > m_extent)
				{
					if (pp[j] > 0)
					{
						pp[j] = m_extent - m_elasticity * (pp[j] - m_extent);
						pv[j] = -fabsf(pv[j]);
					}
					else
					{
						pp[j] = -m_extent - m_elasticity * (pp[j] + m_extent);
						pv[j] = fabsf(pv[j]);
					}

					if (m_sticky)
						sticky_collided = true;
					else
						pv[j] = m_elasticity * pv[j];
					iters++;
					if (iters < 30)
						bounced = true;
				}
			}
		} while (bounced);
		if(sticky_collided)
			for (int j = 0; j < 3; j++)
				pv[j] = m_elasticity * pv[j];

	}
}
