#include "HitEffect.h"
#include "HitEffect.h"
#include <DxLib.h>

namespace
{
	constexpr int kAnimInterval = 3;	// 何フレームごとにコマを切り替えるか(お好みで調整)
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

	DrawGraph(
		static_cast<int>(m_x - width / 2.0f),
		static_cast<int>(m_y - height / 2.0f),
		handle,
		true);
}