#include "Player.h"
#include "game.h"
#include "Input.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kPlayerSpeed = 5.0f;
	constexpr float kPlayerScale = 2.5f; // 拡大率
}

Player::Player(float startX, float startY) :
	m_handle(-1),
	m_graphWidth(0),
	m_graphHeight(0),
	m_scale(kPlayerScale),
	m_x(startX),
	m_y(startY),
	m_shotTriggered(false)
{
	m_handle = LoadGraph("Data/Player.png");
	assert(m_handle > 0);

	// 実際の画像サイズを取得
	GetGraphSize(m_handle, &m_graphWidth, &m_graphHeight);
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
	if (Input::GetInstance().IsPress("UP"))
	{
		m_y -= kPlayerSpeed;
	}
	if (Input::GetInstance().IsPress("DOWN"))
	{
		m_y += kPlayerSpeed;
	}

	// 画面外へ出ないようクランプ
	float halfWidth = (m_graphWidth * m_scale) / 2.0f;
	float halfHeight = (m_graphHeight * m_scale) / 2.0f;

	if (m_x < halfWidth) m_x = halfWidth;
	if (m_x > Game::kScreenWidth - halfWidth) m_x = Game::kScreenWidth - halfWidth;
	if (m_y < halfHeight) m_y = halfHeight;
	if (m_y > Game::kScreenHeight - halfHeight) m_y = Game::kScreenHeight - halfHeight;
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
		m_handle,
		true);
}