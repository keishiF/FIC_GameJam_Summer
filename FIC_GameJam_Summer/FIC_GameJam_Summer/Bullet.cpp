#include "Bullet.h"
#include "game.h"
#include <cmath>
#include <DxLib.h>

namespace
{
	constexpr float kBulletSpeed = 12.0f;
	constexpr int kBulletWidth = 256;
	constexpr int kBulletHeight = 64;
	// 何フレームごとにアニメーションのコマを切り替えるか
	constexpr int kAnimInterval = 4;

	// 当たり判定用のサイズ
	constexpr float kCollisionHalfWidth = 40.0f;
	constexpr float kCollisionHalfHeight = 16.0f;
}

Bullet::Bullet(float startX, float startY, const std::vector<int>& animHandles) :
	m_animHandles(animHandles),
	m_animFrame(0),
	m_animTimer(0),
	m_x(startX),
	m_y(startY),
	m_isActive(true)
{
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	if (!m_isActive)
	{
		return;
	}

	m_x += kBulletSpeed;

	// 画面外に出たら非アクティブ化
	if (m_x - kBulletWidth / 2.0f > Game::kScreenWidth)
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

void Bullet::Draw() const
{
	if (!m_isActive)
	{
		return;
	}

	DrawGraph(
		static_cast<int>(m_x - kBulletWidth / 2.0f),
		static_cast<int>(m_y - kBulletHeight / 2.0f),
		m_animHandles[m_animFrame],
		true);
}

float Bullet::GetCollisionHalfWidth() const
{
	return kCollisionHalfWidth;
}

float Bullet::GetCollisionHalfHeight() const
{
	return kCollisionHalfHeight;
}

bool Bullet::CheckHit(float targetX, float targetY, float targetHalfWidth, float targetHalfHeight) const
{
	if (!m_isActive)
	{
		return false;
	}

	bool overlapX = std::abs(m_x - targetX) < (kCollisionHalfWidth + targetHalfWidth);
	bool overlapY = std::abs(m_y - targetY) < (kCollisionHalfHeight + targetHalfHeight);

	return overlapX && overlapY;
}