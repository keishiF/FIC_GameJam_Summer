#pragma once
#include "Enemy.h"

class EnemyShooter final : public Enemy
{
public:
	EnemyShooter(float x, float y);
	~EnemyShooter();

private:
	virtual void UpdateBehavior() override;

	int m_shotTimer;
};