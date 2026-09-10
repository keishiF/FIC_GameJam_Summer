#pragma once

// タイトル画面の背景演出用: 上下移動しながら一定間隔で弾を撃つだけの軽量なアクター
// (ゲーム本編のPlayer/EnemyShooterとは独立した、見た目だけの存在)
class TitleBattleActor
{
public:
	// x: 固定X座標, startY: 開始Y座標, minY/maxY: 移動範囲, handle: 表示画像ハンドル, scale: 表示拡大率
	TitleBattleActor(float x, float startY, float minY, float maxY, int handle, float scale);
	~TitleBattleActor();

	void Update();
	void Draw() const;

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	// このフレームで弾を撃つべきかどうか
	bool IsShotTriggered() const { return m_shotTriggered; }

private:
	float m_x;
	float m_y;
	float m_minY;
	float m_maxY;

	int m_handle;
	float m_scale;

	int m_graphWidth;
	int m_graphHeight;

	int m_moveDirection;	// 現在の移動方向(+1:下 -1:上)
	int m_moveTimer;		// 移動方向を切り替えるまでのカウント

	int m_shotTimer;
	int m_nextShotInterval;	// 次に撃つまでのフレーム数(15/30/45からランダム選択)

	bool m_shotTriggered;

	// 移動方向をランダムに選び直す
	void PickNewDirection();
	// 次の発射間隔をランダムに選び直す
	void PickNextShotInterval();
};