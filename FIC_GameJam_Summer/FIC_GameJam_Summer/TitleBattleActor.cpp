#include "TitleBattleActor.h"
#include <iterator>
#include <DxLib.h>

namespace
{
	constexpr float kMoveSpeed = 2.0f;
	constexpr int kMoveDuration = 30;	// 何フレームごとに移動方向を選び直すか(ここを書き換えれば調整できる)

	// 発射間隔の候補(この中からランダムに選ぶ、ここを書き換えれば調整できる)
	constexpr int kShotIntervalCandidates[] = { 15, 30, 45 };
}

TitleBattleActor::TitleBattleActor(float x, float startY, float minY, float maxY, int handle, float scale) :
	m_x(x),
	m_y(startY),
	m_minY(minY),
	m_maxY(maxY),
	m_handle(handle),
	m_scale(scale),
	m_graphWidth(0),
	m_graphHeight(0),
	m_moveDirection(0),
	m_moveTimer(kMoveDuration),	// 開始直後に方向を選び直させるため、上限値にしておく
	m_shotTimer(0),
	m_nextShotInterval(0),
	m_shotTriggered(false)
{
	GetGraphSize(m_handle, &m_graphWidth, &m_graphHeight);
	PickNextShotInterval();
}

TitleBattleActor::~TitleBattleActor()
{}

void TitleBattleActor::PickNewDirection()
{
	// ランダムに上(-1)か下(+1)かを選ぶ
	m_moveDirection = (GetRand(1) == 0) ? -1 : 1;
	m_moveTimer = 0;
}

void TitleBattleActor::PickNextShotInterval()
{
	int candidateCount = static_cast<int>(std::size(kShotIntervalCandidates));
	int index = GetRand(candidateCount - 1);
	m_nextShotInterval = kShotIntervalCandidates[index];
	m_shotTimer = 0;
}

void TitleBattleActor::Update()
{
	// 移動方向を一定間隔(30フレーム)で選び直す
	++m_moveTimer;
	if (m_moveTimer >= kMoveDuration)
	{
		PickNewDirection();
	}

	m_y += kMoveSpeed * m_moveDirection;

	if (m_y < m_minY) m_y = m_minY;
	if (m_y > m_maxY) m_y = m_maxY;

	// 発射判定(15/30/45フレームからランダムに選んだ間隔で発射)
	m_shotTriggered = false;
	++m_shotTimer;
	if (m_shotTimer >= m_nextShotInterval)
	{
		m_shotTriggered = true;
		PickNextShotInterval();
	}
}

void TitleBattleActor::Draw() const
{
	float halfWidth = (m_graphWidth * m_scale) / 2.0f;
	float halfHeight = (m_graphHeight * m_scale) / 2.0f;

	DrawExtendGraph(
		static_cast<int>(m_x - halfWidth),
		static_cast<int>(m_y - halfHeight),
		static_cast<int>(m_x + halfWidth),
		static_cast<int>(m_y + halfHeight),
		m_handle,
		true);
}