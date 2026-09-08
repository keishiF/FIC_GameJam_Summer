#include "Player.h"
#include "game.h"
#include "Input.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kPlayerSpeed = 5.0f;
	constexpr float kPlayerScale = 2.5f; // 拡大率
	// 当たり判定の拡大・縮小率
	constexpr float kPlayerCollisionScale = 0.4f;

	// プレイヤーの最大HP
	constexpr int kMaxHp = 3;
	// 被弾クールタイム
	constexpr int kInvulnerableFrame = 90;
}

Player::Player(float startX, float startY) :
	m_normalHandle(-1),
	m_upHandle(-1),
	m_downHandle(-1),
	m_graphWidth(0),
	m_graphHeight(0),
	m_scale(kPlayerScale),
	m_collisionScale(kPlayerCollisionScale),
	m_moveState(MoveState::Normal),
	m_x(startX),
	m_y(startY),
	m_shotTriggered(false),
	m_hp(kMaxHp),
	m_invulnerableTimer(0)
{
	m_normalHandle = LoadGraph("Data/Player.png");
	assert(m_normalHandle > 0);

	m_upHandle = LoadGraph("Data/PlayerUp.png");
	assert(m_upHandle > 0);

	m_downHandle = LoadGraph("Data/PlayerDown.png");
	assert(m_downHandle > 0);

	// 画像サイズを取得
	GetGraphSize(m_normalHandle, &m_graphWidth, &m_graphHeight);
}

Player::~Player()
{
}

void Player::Update()
{
	Move();

	m_shotTriggered = Input::GetInstance().IsTrigger("Attack");

	if (m_invulnerableTimer > 0)
	{
		--m_invulnerableTimer;
	}
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

	// 上下の移動状態を判定
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

	// 画面外へ出ないようクランプ
	float halfWidth = GetHalfWidth();
	float halfHeight = GetHalfHeight();

	if (m_x < halfWidth) m_x = halfWidth;
	if (m_x > Game::kScreenWidth - halfWidth) m_x = Game::kScreenWidth - halfWidth;
	if (m_y < halfHeight) m_y = halfHeight;
	if (m_y > Game::kScreenHeight - halfHeight) m_y = Game::kScreenHeight - halfHeight;
}

float Player::GetHalfWidth() const
{
	return (m_graphWidth * m_scale) / 2.0f;
}

float Player::GetHalfHeight() const
{
	return (m_graphHeight * m_scale) / 2.0f;
}

float Player::GetCollisionHalfWidth() const
{
	return GetHalfWidth() * m_collisionScale;
}

float Player::GetCollisionHalfHeight() const
{
	return GetHalfHeight() * m_collisionScale;
}

void Player::TakeDamage(int damage)
{
	if (m_invulnerableTimer > 0)
	{
		return;
	}

	m_hp -= damage;
	if (m_hp < 0)
	{
		m_hp = 0;
	}

	m_invulnerableTimer = kInvulnerableFrame;
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
	// 無敵時間中は点滅させる
	if (m_invulnerableTimer > 0 && (m_invulnerableTimer / 4) % 2 == 0)
	{
		return;
	}

	float halfWidth = GetHalfWidth();
	float halfHeight = GetHalfHeight();

	DrawExtendGraph(
		static_cast<int>(m_x - halfWidth),
		static_cast<int>(m_y - halfHeight),
		static_cast<int>(m_x + halfWidth),
		static_cast<int>(m_y + halfHeight),
		GetCurrentHandle(),
		true);
}