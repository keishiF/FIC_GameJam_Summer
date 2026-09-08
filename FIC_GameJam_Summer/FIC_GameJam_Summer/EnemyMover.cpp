#include "EnemyMover.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	// 移動型のHP(ここを書き換えるだけで硬さを調整できる)
	constexpr int kMoverHp = 1;

	constexpr float kMoverScale = 2.0f;
	constexpr float kMoverCollisionScale = 0.6f;
	constexpr float kMoverSpeed = 4.0f;	// 移動速度(ここを書き換えれば速さを調整できる)

	int LoadMoverGraph()
	{
		int handle = LoadGraph("Data/EnemyMover.png");
		assert(handle > 0);
		return handle;
	}
}

EnemyMover::EnemyMover(float x, float startY, float minY, float maxY, int initialDirection) :
	Enemy(x, startY, LoadMoverGraph(), kMoverHp, kMoverScale, kMoverCollisionScale),
	m_minY(minY),
	m_maxY(maxY),
	m_direction(initialDirection)
{}

EnemyMover::~EnemyMover()
{}

void EnemyMover::UpdateBehavior()
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