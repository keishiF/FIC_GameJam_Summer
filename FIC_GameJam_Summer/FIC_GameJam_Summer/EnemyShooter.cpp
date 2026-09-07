#include "EnemyShooter.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	// 据え置き型のHP(ここを書き換えるだけで硬さを調整できる)
	constexpr int kTurretHp = 2;

	constexpr float kTurretScale = 2.0f;
	constexpr int kShotInterval = 90;	// 何フレームごとに撃つか(ここを書き換えれば攻撃頻度を調整できる)

	int LoadTurretGraph()
	{
		int handle = LoadGraph("Data/EnemyShooter.png");
		assert(handle > 0);
		return handle;
	}
}

EnemyShooter::EnemyShooter(float x, float y) :
	Enemy(x, y, LoadTurretGraph(), kTurretHp, kTurretScale),
	m_shotTimer(0)
{}

EnemyShooter::~EnemyShooter()
{}

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