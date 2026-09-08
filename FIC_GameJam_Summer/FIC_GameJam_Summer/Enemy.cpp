#include "Enemy.h"
#include <DxLib.h>

Enemy::Enemy(float x, float y, int handle, int hp, float scale, float collisionScale) :
	m_handle(handle),
	m_graphWidth(0),
	m_graphHeight(0),
	m_scale(scale),
	m_collisionScale(collisionScale),
	m_x(x),
	m_y(y),
	m_hp(hp),
	m_shotTriggered(false)
{
	GetGraphSize(m_handle, &m_graphWidth, &m_graphHeight);
}

Enemy::~Enemy()
{
}

void Enemy::TakeDamage(int damage)
{
	m_hp -= damage;
	if (m_hp < 0)
	{
		m_hp = 0;
	}
}

float Enemy::GetHalfWidth() const
{
	return (m_graphWidth * m_scale) / 2.0f;
}

float Enemy::GetHalfHeight() const
{
	return (m_graphHeight * m_scale) / 2.0f;
}

float Enemy::GetCollisionHalfWidth() const
{
	return GetHalfWidth() * m_collisionScale;
}

float Enemy::GetCollisionHalfHeight() const
{
	return GetHalfHeight() * m_collisionScale;
}

void Enemy::Draw() const
{
	float halfWidth = GetHalfWidth();
	float halfHeight = GetHalfHeight();

	DrawExtendGraph(
		static_cast<int>(m_x - halfWidth),
		static_cast<int>(m_y - halfHeight),
		static_cast<int>(m_x + halfWidth),
		static_cast<int>(m_y + halfHeight),
		m_handle,
		true);
}