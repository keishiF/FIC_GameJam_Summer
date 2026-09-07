#pragma once

class Player
{
public:
	explicit Player(float startX, float startY);
	~Player();

	void Update();
	void Draw() const;

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	float GetHalfWidth() const;
	float GetHalfHeight() const;

	// このフレームで攻撃ボタンが押されたか
	bool IsShotTriggered() const { return m_shotTriggered; }

	// 被弾処理(無敵時間中は無視される)
	void TakeDamage(int damage);

	int GetHp() const { return m_hp; }
	bool IsAlive() const { return m_hp > 0; }

private:
	// 移動状態
	enum class MoveState
	{
		Normal,
		Up,
		Down,
	};

	int m_normalHandle;
	int m_upHandle;
	int m_downHandle;

	int m_graphWidth;
	int m_graphHeight;
	float m_scale;

	MoveState m_moveState;

	float m_x;
	float m_y;

	bool m_shotTriggered;

	int m_hp;
	int m_invulnerableTimer;

	// 移動処理(上下左右フリー)
	void Move();

	// 現在の移動状態に応じたハンドルを取得する
	int GetCurrentHandle() const;
};