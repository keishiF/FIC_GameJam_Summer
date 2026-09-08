#pragma once
#include <vector>

class EnemyBullet
{
public:
	EnemyBullet(float startX, float startY, const std::vector<int>& animHandles);
	~EnemyBullet();

	void Update();
	void Draw() const;

	bool IsActive() const { return m_isActive; }

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	// 当たり判定用の半分のサイズ
	float GetCollisionHalfWidth() const;
	float GetCollisionHalfHeight() const;

	// 対象との当たり判定
	bool CheckHit(float targetX, float targetY, float targetHalfWidth, float targetHalfHeight) const;

	void Deactivate() { m_isActive = false; }

private:
	const std::vector<int>& m_animHandles;

	int m_animFrame;
	int m_animTimer;

	float m_x;
	float m_y;

	bool m_isActive;
};