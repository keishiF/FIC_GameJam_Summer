#pragma once
#include "Enemy.h"

// 指定した中心点を軸に、楕円軌道でループ移動し続ける敵
class EnemyLooper final : public Enemy
{
public:
	EnemyLooper(float centerX, float centerY, float radiusX, float radiusY, int direction, float startAngle);
	~EnemyLooper();

	virtual void Update() override;

private:
	float m_centerX;
	float m_centerY;
	float m_radiusX;
	float m_radiusY;
	int m_direction;

	float m_angle;	// 現在の角度(ラジアン)
};