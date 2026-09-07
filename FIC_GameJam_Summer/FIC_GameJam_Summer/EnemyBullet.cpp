#include "EnemyBullet.h"
#include <cassert>
#include <cmath>
#include <DxLib.h>

namespace
{
	constexpr float kEnemyBulletSpeed = 8.0f;
	constexpr float kHalfWidth = 20.0f;
	constexpr float kHalfHeight = 10.0f;
}

EnemyBullet::EnemyBullet(float startX, float startY) :
	m_handle(-1),
	m_x(startX),
	m_y(startY),
	m_isActive(true)
{
	m_handle = LoadGraph("Data/Bullet.png");
	assert(m_handle > 0);
}

EnemyBullet::~EnemyBullet()
{}

void EnemyBullet::Update()
{
	if (!m_isActive)
	{
		return;
	}

	m_x -= kEnemyBulletSpeed;

	if (m_x + kHalfWidth < 0)
	{
		m_isActive = false;
	}
}

void EnemyBullet::Draw() const
{
	if (!m_isActive)
	{
		return;
	}

	DrawGraph(
		static_cast<int>(m_x - kHalfWidth),
		static_cast<int>(m_y - kHalfHeight),
		m_handle,
		true);
}

bool EnemyBullet::CheckHit(float targetX, float targetY, float targetHalfWidth, float targetHalfHeight) const
{
	if (!m_isActive)
	{
		return false;
	}

	bool overlapX = std::abs(m_x - targetX) < (kHalfWidth + targetHalfWidth);
	bool overlapY = std::abs(m_y - targetY) < (kHalfHeight + targetHalfHeight);

	return overlapX && overlapY;
}