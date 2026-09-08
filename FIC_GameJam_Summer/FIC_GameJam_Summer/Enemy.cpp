#include "Enemy.h"
#include <cmath>
#include <DxLib.h>

namespace
{
	constexpr float kEntranceOffsetX = 400.0f;	// 開始位置を最終位置からどれだけ右にずらすか(ここを書き換えれば入場演出の距離を調整できる)
	constexpr float kEntranceSpeed = 10.0f;	// 入場演出の移動速度(ここを書き換えれば速さを調整できる)
}

Enemy::Enemy(float x, float y, int handle, int hp, float scale, float collisionScale) :
	m_handle(handle),
	m_graphWidth(0),
	m_graphHeight(0),
	m_scale(scale),
	m_collisionScale(collisionScale),
	m_x(x + kEntranceOffsetX),
	m_y(y),
	m_hp(hp),
	m_shotTriggered(false),
	m_targetX(x),
	m_isEntering(true)
{
	GetGraphSize(m_handle, &m_graphWidth, &m_graphHeight);
}

Enemy::~Enemy()
{}

void Enemy::Update()
{
	if (m_isEntering)
	{
		float diff = m_targetX - m_x;

		if (std::abs(diff) <= kEntranceSpeed)
		{
			m_x = m_targetX;
			m_isEntering = false;
		}
		else
		{
			m_x += (diff > 0.0f) ? kEntranceSpeed : -kEntranceSpeed;
		}

		// 入場演出中は本来の行動(攻撃・移動パターンなど)を行わない
		return;
	}

	UpdateBehavior();
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
	return (m_graphWidth * m_scale) * 0.5f;
}

float Enemy::GetHalfHeight() const
{
	return (m_graphHeight * m_scale) * 0.5f;
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