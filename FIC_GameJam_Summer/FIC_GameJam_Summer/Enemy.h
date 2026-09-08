#pragma once

class Enemy
{
public:
	// x, y: 最終的な(登場後に静止する)配置座標。実際の開始位置はここから画面右側へずらした位置になる
	Enemy(float x, float y, int handle, int hp, float scale, float collisionScale);
	virtual ~Enemy();

	// 入場演出込みの更新(GameSceneはこちらを呼ぶ)
	void Update();
	virtual void Draw() const;

	// 弾がヒットした際に呼び出す
	void TakeDamage(int damage);

	bool IsAlive() const { return m_hp > 0; }

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	// 見た目上の半分のサイズ(描画用)
	float GetHalfWidth() const;
	float GetHalfHeight() const;

	// 当たり判定用の半分のサイズ(見た目より小さめ)
	float GetCollisionHalfWidth() const;
	float GetCollisionHalfHeight() const;

	// このフレームで攻撃(弾発射)を行うかどうか
	bool IsShotTriggered() const { return m_shotTriggered; }

	// 入場演出中かどうか
	bool IsEntering() const { return m_isEntering; }

protected:
	// 各敵ごとの本来の行動(入場演出が終わった後に毎フレーム呼ばれる)
	virtual void UpdateBehavior() = 0;

	int m_handle;

	int m_graphWidth;
	int m_graphHeight;
	float m_scale;
	float m_collisionScale;	// 当たり判定を見た目に対してどれだけ縮小するか(0~1)

	float m_x;
	float m_y;

	int m_hp;

	bool m_shotTriggered;

private:
	float m_targetX;	// 入場演出の目標X座標(最終的な配置座標)
	bool m_isEntering;
};