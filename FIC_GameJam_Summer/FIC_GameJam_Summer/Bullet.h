#pragma once
#include <vector>

class Bullet
{
public:
	// animHandles: あらかじめ分割ロード済みの弾アニメーション画像ハンドル配列(所有権はGameScene側)
	Bullet(float startX, float startY, const std::vector<int>& animHandles);
	~Bullet();

	void Update();
	void Draw() const;

	bool IsActive() const { return m_isActive; }

	// 対象との当たり判定(AABB)
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