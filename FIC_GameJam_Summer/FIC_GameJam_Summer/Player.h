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

	// 見た目上の半分のサイズ
	float GetHalfWidth() const;
	float GetHalfHeight() const;

	// 当たり判定用の半分のサイズ
	float GetCollisionHalfWidth() const;
	float GetCollisionHalfHeight() const;

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
	float m_collisionScale;	// 当たり判定を見た目に対してどれだけ縮小するか

	MoveState m_moveState;

	float m_x;
	float m_y;

	bool m_shotTriggered;

	int m_hp;
	int m_invulnerableTimer;

	// 移動処理
	void Move();

	// 現在の移動状態に応じたハンドルを取得する
	int GetCurrentHandle() const;
};