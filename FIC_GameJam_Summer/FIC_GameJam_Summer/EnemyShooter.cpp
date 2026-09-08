#include "EnemyShooter.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	// HP
	constexpr int kShooterHp = 2;

	constexpr float kShooterScale = 2.0f; // 拡大率
	// 当たり判定の拡大・縮小率
	constexpr float kShooterCollisionScale = 0.75f;

	// 何フレームごとに撃つか
	constexpr int kShotInterval = 90;

	int LoadTurretGraph()
	{
		int handle = LoadGraph("Data/EnemyShooter.png");
		assert(handle > 0);
		return handle;
	}
}

EnemyShooter::EnemyShooter(float x, float y) :
	Enemy(x, y, LoadTurretGraph(), kShooterHp, kShooterScale, kShooterCollisionScale),
	m_shotTimer(0)
{
}

EnemyShooter::~EnemyShooter()
{
}

void EnemyShooter::Update()
{
	m_shotTriggered = false;

	++m_shotTimer;
	if (m_shotTimer >= kShotInterval)
	{
		m_shotTimer = 0;
		m_shotTriggered = true;
	}
}