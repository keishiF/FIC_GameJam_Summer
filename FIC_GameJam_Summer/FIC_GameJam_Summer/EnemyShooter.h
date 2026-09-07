#pragma once
#include "Enemy.h"

class EnemyShooter final : public Enemy
{
public:
	EnemyShooter(float x, float y);
	~EnemyShooter();

	virtual void Update() override;

private:
	int m_shotTimer;
};