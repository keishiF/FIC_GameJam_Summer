#pragma once
#include <vector>

class HitEffect
{
public:
	HitEffect(float x, float y, const std::vector<int>& animHandles);
	~HitEffect();

	void Update();
	void Draw() const;

	bool IsActive() const { return m_isActive; }

private:
	const std::vector<int>& m_animHandles;

	int m_animFrame;
	int m_animTimer;

	float m_x;
	float m_y;

	bool m_isActive;
};