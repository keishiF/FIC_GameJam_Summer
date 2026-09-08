#pragma once

class Enemy
{
public:
	Enemy(float x, float y, int handle, int hp, float scale, float collisionScale);
	virtual ~Enemy();

	virtual void Update() = 0;
	virtual void Draw() const;

	// 弾がヒットした際に呼び出す
	void TakeDamage(int damage);

	bool IsAlive() const { return m_hp > 0; }

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	// 見た目上の半分のサイズ
	float GetHalfWidth() const;
	float GetHalfHeight() const;

	// 当たり判定用の半分のサイズ
	float GetCollisionHalfWidth() const;
	float GetCollisionHalfHeight() const;

	// このフレームで攻撃(弾発射)を行うかどうか
	bool IsShotTriggered() const { return m_shotTriggered; }

protected:
	int m_handle;

	int m_graphWidth;
	int m_graphHeight;
	float m_scale;
	float m_collisionScale;	// 当たり判定を見た目に対してどれだけ縮小するか

	float m_x;
	float m_y;

	int m_hp;

	bool m_shotTriggered;
};