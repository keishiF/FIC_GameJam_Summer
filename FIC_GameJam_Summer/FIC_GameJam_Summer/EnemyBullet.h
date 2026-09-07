#pragma once

class EnemyBullet
{
public:
	EnemyBullet(float startX, float startY);
	~EnemyBullet();

	void Update();
	void Draw() const;

	bool IsActive() const { return m_isActive; }

	// 対象との当たり判定(AABB)
	bool CheckHit(float targetX, float targetY, float targetHalfWidth, float targetHalfHeight) const;

	void Deactivate() { m_isActive = false; }

private:
	int m_handle;

	float m_x;
	float m_y;

	bool m_isActive;
};