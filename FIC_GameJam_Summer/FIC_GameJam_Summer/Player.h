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
	int m_handle;

	int m_graphWidth;	// 画像の幅
	int m_graphHeight;	// 画像の高さ
	float m_scale;		// 拡大率

	float m_x;
	float m_y;

	bool m_shotTriggered;

	// 移動処理
	void Move();
};