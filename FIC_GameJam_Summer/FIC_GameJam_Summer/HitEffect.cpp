#include "HitEffect.h"
#include <DxLib.h>

namespace
{
	constexpr int kAnimInterval = 3;		// 何フレームごとにコマを切り替えるか(お好みで調整)
	constexpr float kHitEffectScale = 4.0f;	// 表示拡大率(ここを書き換えれば大きさを調整できる)
}

HitEffect::HitEffect(float x, float y, const std::vector<int>& animHandles) :
	m_animHandles(animHandles),
	m_animFrame(0),
	m_animTimer(0),
	m_x(x),
	m_y(y),
	m_isActive(true)
{}

HitEffect::~HitEffect()
{}

void HitEffect::Update()
{
	if (!m_isActive)
	{
		return;
	}

	++m_animTimer;
	if (m_animTimer >= kAnimInterval)
	{
		m_animTimer = 0;
		++m_animFrame;

		// 最後のコマまで再生し終えたら非アクティブ化(ループさせない使い切りエフェクト)
		if (m_animFrame >= static_cast<int>(m_animHandles.size()))
		{
			m_isActive = false;
		}
	}
}

void HitEffect::Draw() const
{
	if (!m_isActive)
	{
		return;
	}

	int handle = m_animHandles[m_animFrame];

	int width = 0;
	int height = 0;
	GetGraphSize(handle, &width, &height);

	float halfWidth = (width * kHitEffectScale) * 0.5f;
	float halfHeight = (height * kHitEffectScale) * 0.5f;

	DrawExtendGraph(
		static_cast<int>(m_x - halfWidth),
		static_cast<int>(m_y - halfHeight),
		static_cast<int>(m_x + halfWidth),
		static_cast<int>(m_y + halfHeight),
		handle,
		true);
}