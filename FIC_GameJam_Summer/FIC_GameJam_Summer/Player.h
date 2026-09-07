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

	// このフレームで攻撃ボタンが押されたか
	bool IsShotTriggered() const { return m_shotTriggered; }

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

	int m_graphWidth;	// 画像の幅
	int m_graphHeight;	// 画像の高さ
	float m_scale;		// 拡大率

	MoveState m_moveState;

	float m_x;
	float m_y;

	bool m_shotTriggered;

	// 移動処理
	void Move();

	// 現在の移動状態に応じたハンドルを取得する
	int GetCurrentHandle() const;
};