#include "EnemyLooper.h"
#include <cassert>
#include <cmath>
#include <DxLib.h>

namespace
{
	// 移動型のHP(ここを書き換えるだけで硬さを調整できる)
	constexpr int kLooperHp = 1;

	constexpr float kLooperScale = 2.0f;
	constexpr float kLooperCollisionScale = 0.6f;
	constexpr float kAngularSpeed = 0.03f;	// 1フレームあたりの角速度(ラジアン、大きいほど速く回る)

	int LoadLooperGraph()
	{
		int handle = LoadGraph("Data/EnemyMover.png");
		assert(handle > 0);
		return handle;
	}

	// 角度から軌道上のX座標を計算
	float CalcX(float centerX, float radiusX, float angle)
	{
		return centerX + radiusX * std::sin(angle);
	}

	// 角度から軌道上のY座標を計算
	float CalcY(float centerY, float radiusY, float angle)
	{
		return centerY - radiusY * std::cos(angle);
	}
}

EnemyLooper::EnemyLooper(float centerX, float centerY, float radiusX, float radiusY, int direction, float startAngle) :
	Enemy(
		CalcX(centerX, radiusX, startAngle),
		CalcY(centerY, radiusY, startAngle),
		LoadLooperGraph(), kLooperHp, kLooperScale, kLooperCollisionScale),
	m_centerX(centerX),
	m_centerY(centerY),
	m_radiusX(radiusX),
	m_radiusY(radiusY),
	m_direction(direction),
	m_angle(startAngle)
{}

EnemyLooper::~EnemyLooper()
{}

void EnemyLooper::UpdateBehavior()
{
	m_angle += kAngularSpeed * m_direction;

	m_x = CalcX(m_centerX, m_radiusX, m_angle);
	m_y = CalcY(m_centerY, m_radiusY, m_angle);
}