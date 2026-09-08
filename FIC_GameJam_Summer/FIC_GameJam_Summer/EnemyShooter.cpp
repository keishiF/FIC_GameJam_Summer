#include "EnemyShooter.h"
#include "Player.h"
#include <cassert>
#include <cmath>
#include <DxLib.h>

namespace
{
	// プレイヤー追従型のHP
	constexpr int kShooterHp = 2;

	constexpr float kShooterScale = 2.0f;
	// 当たり判定の拡大率
	constexpr float kShooterCollisionScale = 0.6f;

	// プレイヤーのY座標へ近づく速度
	constexpr float kTrackSpeed = 3.0f;

	// 何フレームごとに撃つか
	constexpr int kShotInterval = 60;

	int LoadShooterGraph()
	{
		int handle = LoadGraph("Data/EnemyShooter.png");
		assert(handle > 0);
		return handle;
	}
}

EnemyShooter::EnemyShooter(float x, float y, float minY, float maxY, const Player& player) :
	Enemy(x, y, LoadShooterGraph(), kShooterHp, kShooterScale, kShooterCollisionScale),
	m_minY(minY),
	m_maxY(maxY),
	m_player(player),
	m_shotTimer(0)
{}

EnemyShooter::~EnemyShooter()
{}

void EnemyShooter::UpdateBehavior()
{
	// プレイヤーのY座標に軸を合わせるように上下移動
	float targetY = m_player.GetY();
	float diff = targetY - m_y;

	if (std::abs(diff) <= kTrackSpeed)
	{
		m_y = targetY;
	}
	else
	{
		m_y += (diff > 0.0f) ? kTrackSpeed : -kTrackSpeed;
	}

	// 移動可能範囲内にクランプ
	if (m_y < m_minY) m_y = m_minY;
	if (m_y > m_maxY) m_y = m_maxY;

	m_shotTriggered = false;

	++m_shotTimer;
	if (m_shotTimer >= kShotInterval)
	{
		m_shotTimer = 0;
		m_shotTriggered = true;
	}
}