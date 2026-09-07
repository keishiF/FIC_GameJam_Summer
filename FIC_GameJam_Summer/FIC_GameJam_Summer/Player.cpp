#include "Player.h"
#include "game.h"
#include "Input.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kPlayerSpeed = 5.0f;
	constexpr float kPlayerScale = 2.5f;	// 表示拡大率(お好みで調整してください)
}

Player::Player(float startX, float startY) :
	m_normalHandle(-1),
	m_upHandle(-1),
	m_downHandle(-1),
	m_graphWidth(0),
	m_graphHeight(0),
	m_scale(kPlayerScale),
	m_moveState(MoveState::Normal),
	m_x(startX),
	m_y(startY),
	m_shotTriggered(false)
{
	m_normalHandle = LoadGraph("Data/Player.png");
	assert(m_normalHandle > 0);

	m_upHandle = LoadGraph("Data/PlayerUp.png");
	assert(m_upHandle > 0);

	m_downHandle = LoadGraph("Data/PlayerDown.png");
	assert(m_downHandle > 0);

	// 実際の画像サイズを取得しておく(中心座標計算・クランプ・描画すべての基準にする)
	GetGraphSize(m_normalHandle, &m_graphWidth, &m_graphHeight);
}

Player::~Player()
{
}

void Player::Update()
{
	Move();

	m_shotTriggered = Input::GetInstance().IsTrigger("Attack");
}

void Player::Move()
{
	if (Input::GetInstance().IsPress("LEFT"))
	{
		m_x -= kPlayerSpeed;
	}
	if (Input::GetInstance().IsPress("RIGHT"))
	{
		m_x += kPlayerSpeed;
	}

	// 上下の移動状態を判定(両方押された場合はUP優先)
	if (Input::GetInstance().IsPress("UP"))
	{
		m_y -= kPlayerSpeed;
		m_moveState = MoveState::Up;
	}
	else if (Input::GetInstance().IsPress("DOWN"))
	{
		m_y += kPlayerSpeed;
		m_moveState = MoveState::Down;
	}
	else
	{
		m_moveState = MoveState::Normal;
	}

	// 実際の表示サイズ(拡大後)を基準に画面外へ出ないようクランプ
	float halfWidth = (m_graphWidth * m_scale) / 2.0f;
	float halfHeight = (m_graphHeight * m_scale) / 2.0f;

	if (m_x < halfWidth) m_x = halfWidth;
	if (m_x > Game::kScreenWidth - halfWidth) m_x = Game::kScreenWidth - halfWidth;
	if (m_y < halfHeight) m_y = halfHeight;
	if (m_y > Game::kScreenHeight - halfHeight) m_y = Game::kScreenHeight - halfHeight;
}

int Player::GetCurrentHandle() const
{
	switch (m_moveState)
	{
	case MoveState::Up:
		return m_upHandle;
	case MoveState::Down:
		return m_downHandle;
	case MoveState::Normal:
	default:
		return m_normalHandle;
	}
}

void Player::Draw() const
{
	float halfWidth = (m_graphWidth * m_scale) / 2.0f;
	float halfHeight = (m_graphHeight * m_scale) / 2.0f;

	DrawExtendGraph(
		static_cast<int>(m_x - halfWidth),
		static_cast<int>(m_y - halfHeight),
		static_cast<int>(m_x + halfWidth),
		static_cast<int>(m_y + halfHeight),
		GetCurrentHandle(),
		true);
}