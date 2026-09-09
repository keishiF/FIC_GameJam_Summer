#pragma once

class Enemy
{
public:
	Enemy(float x, float y, int handle, int hp, float scale, float collisionScale);
	virtual ~Enemy();

	// 入場演出込みの更新
	void Update();
	virtual void Draw() const;

	// 弾がヒットした際に呼び出す
	void TakeDamage(int damage);

	bool IsAlive() const { return m_hp > 0; }

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	void SetY(float y) { m_y = y; }

	// 見た目上の半分のサイズ
	float GetHalfWidth() const;
	float GetHalfHeight() const;

	// 当たり判定用の半分のサイズ
	float GetCollisionHalfWidth() const;
	float GetCollisionHalfHeight() const;

	// このフレームで攻撃を行うかどうか
	bool IsShotTriggered() const { return m_shotTriggered; }

	// 入場演出中かどうか
	bool IsEntering() const { return m_isEntering; }

protected:
	virtual void UpdateBehavior() = 0;

	int m_handle;

	int m_graphWidth;
	int m_graphHeight;
	float m_scale;

	// 当たり判定を見た目に対してどれだけ縮小するか(0~1)
	float m_collisionScale;

	float m_x;
	float m_y;

	int m_hp;

	bool m_shotTriggered;

private:
	float m_targetX;
	bool m_isEntering;
};