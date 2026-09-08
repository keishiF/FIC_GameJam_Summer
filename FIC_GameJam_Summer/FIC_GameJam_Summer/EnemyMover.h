#pragma once
#include "Enemy.h"

// X座標固定のまま、指定した範囲をY方向に往復移動する敵
class EnemyMover final : public Enemy
{
public:
	EnemyMover(float x, float startY, float minY, float maxY, int initialDirection);
	~EnemyMover();

	virtual void Update() override;

private:
	float m_minY;
	float m_maxY;
	int m_direction;
};