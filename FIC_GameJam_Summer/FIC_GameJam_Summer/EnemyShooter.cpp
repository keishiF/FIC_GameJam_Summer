#include "EnemyShooter.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	// HP
	constexpr int kTurretHp = 2;

	constexpr float kTurretScale = 2.0f; // 拡大率
	// 当たり判定の拡大・縮小率
	constexpr float kTurretCollisionScale = 0.6f;

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
	Enemy(x, y, LoadTurretGraph(), kTurretHp, kTurretScale, kTurretCollisionScale),
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