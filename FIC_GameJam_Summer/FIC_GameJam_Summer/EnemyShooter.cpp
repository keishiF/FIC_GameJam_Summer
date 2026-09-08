#include "EnemyShooter.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	// 据え置き型のHP(ここを書き換えるだけで硬さを調整できる)
	constexpr int kShooterHp = 2;

	constexpr float kShooterScale = 2.0f;
	constexpr float kShooterCollisionScale = 0.6f;	// 当たり判定を見た目の60%に縮小(調整可能)

	constexpr int kShotInterval = 90;	// 何フレームごとに撃つか(ここを書き換えれば攻撃頻度を調整できる)

	int LoadShooterGraph()
	{
		int handle = LoadGraph("Data/EnemyShooter.png");
		assert(handle > 0);
		return handle;
	}
}

EnemyShooter::EnemyShooter(float x, float y) :
	Enemy(x, y, LoadShooterGraph(), kShooterHp, kShooterScale, kShooterCollisionScale),
	m_shotTimer(0)
{}

EnemyShooter::~EnemyShooter()
{}

void EnemyShooter::UpdateBehavior()
{
	m_shotTriggered = false;

	++m_shotTimer;
	if (m_shotTimer >= kShotInterval)
	{
		m_shotTimer = 0;
		m_shotTriggered = true;
	}
}