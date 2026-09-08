#pragma once
#include "Enemy.h"

// 指定した中心点を軸に、楕円(または円)軌道でループ移動しながら攻撃する敵
class EnemyLooper final : public Enemy
{
public:
	// centerX/centerY: 軌道の中心, radiusX/radiusY: 軌道の半径(同じ値にすれば正円),
	// direction: 回転方向(+1:時計回り -1:反時計回り), startAngle: 開始角度(ラジアン、0:軌道の一番上 π:軌道の一番下)
	EnemyLooper(float centerX, float centerY, float radiusX, float radiusY, int direction, float startAngle);
	~EnemyLooper();

private:
	virtual void UpdateBehavior() override;

	float m_centerX;
	float m_centerY;
	float m_radiusX;
	float m_radiusY;
	int m_direction;

	float m_angle;	// 現在の角度(ラジアン)

	int m_shotTimer;
};