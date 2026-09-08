#include "EnemyMover.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	// HP
	constexpr int kPatrolHp = 1;

	constexpr float kMoverScale = 2.0f;
	constexpr float kMoverCollisionScale = 0.6f;
	constexpr float kMoverSpeed = 4.0f;

	int LoadPatrolGraph()
	{
		int handle = LoadGraph("Data/EnemyMover.png");
		assert(handle > 0);
		return handle;
	}
}

EnemyMover::EnemyMover(float x, float startY, float minY, float maxY, int initialDirection) :
	Enemy(x, startY, LoadPatrolGraph(), kPatrolHp, kMoverScale, kMoverCollisionScale),
	m_minY(minY),
	m_maxY(maxY),
	m_direction(initialDirection)
{
}

EnemyMover::~EnemyMover()
{
}

void EnemyMover::Update()
{
	m_y += kMoverSpeed * m_direction;

	// 上下端に到達したら反転
	if (m_y <= m_minY)
	{
		m_y = m_minY;
		m_direction = 1;
	}
	else if (m_y >= m_maxY)
	{
		m_y = m_maxY;
		m_direction = -1;
	}
}