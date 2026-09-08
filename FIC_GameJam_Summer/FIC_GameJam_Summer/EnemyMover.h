#pragma once
#include "Enemy.h"

// X座標固定のまま、指定した範囲をY方向に往復移動する敵
class EnemyMover final : public Enemy
{
public:
	// x: 固定X座標, startY: 開始Y座標, minY/maxY: 往復範囲, initialDirection: 初期移動方向(+1:下 -1:上)
	EnemyMover(float x, float startY, float minY, float maxY, int initialDirection);
	~EnemyMover();

private:
	virtual void UpdateBehavior() override;

	float m_minY;
	float m_maxY;
	int m_direction;
};