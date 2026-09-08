#pragma once
#include "Enemy.h"

class Player;
class EnemyShooter final : public Enemy
{
public:
	EnemyShooter(float x, float y, float minY, float maxY, const Player& player);
	~EnemyShooter();

private:
	virtual void UpdateBehavior() override;

	float m_minY;
	float m_maxY;
	const Player& m_player;

	int m_shotTimer;
};