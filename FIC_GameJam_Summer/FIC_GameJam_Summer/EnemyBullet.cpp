#include "EnemyBullet.h"
#include <cmath>
#include <DxLib.h>

namespace
{
	constexpr float kEnemyBulletSpeed = 8.0f;
	constexpr int kEnemyBulletWidth = 128;
	constexpr int kEnemyBulletHeight = 64;
	// 何フレームごとにアニメーションのコマを切り替えるか
	constexpr int kAnimInterval = 4;

	// 当たり判定用のサイズ
	constexpr float kCollisionHalfWidth = 30.0f;
	constexpr float kCollisionHalfHeight = 12.0f;
}

EnemyBullet::EnemyBullet(float startX, float startY, const std::vector<int>& animHandles) :
	m_animHandles(animHandles),
	m_animFrame(0),
	m_animTimer(0),
	m_x(startX),
	m_y(startY),
	m_isActive(true)
{
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Update()
{
	if (!m_isActive)
	{
		return;
	}

	m_x -= kEnemyBulletSpeed;

	if (m_x + kEnemyBulletWidth / 2.0f < 0)
	{
		m_isActive = false;
	}

	// アニメーション更新
	++m_animTimer;
	if (m_animTimer >= kAnimInterval)
	{
		m_animTimer = 0;
		m_animFrame = (m_animFrame + 1) % static_cast<int>(m_animHandles.size());
	}
}

void EnemyBullet::Draw() const
{
	if (!m_isActive)
	{
		return;
	}

	DrawGraph(
		static_cast<int>(m_x - kEnemyBulletWidth / 2.0f),
		static_cast<int>(m_y - kEnemyBulletHeight / 2.0f),
		m_animHandles[m_animFrame],
		true);
}

float EnemyBullet::GetCollisionHalfWidth() const
{
	return kCollisionHalfWidth;
}

float EnemyBullet::GetCollisionHalfHeight() const
{
	return kCollisionHalfHeight;
}

bool EnemyBullet::CheckHit(float targetX, float targetY, float targetHalfWidth, float targetHalfHeight) const
{
	if (!m_isActive)
	{
		return false;
	}

	bool overlapX = std::abs(m_x - targetX) < (kCollisionHalfWidth + targetHalfWidth);
	bool overlapY = std::abs(m_y - targetY) < (kCollisionHalfHeight + targetHalfHeight);

	return overlapX && overlapY;
}