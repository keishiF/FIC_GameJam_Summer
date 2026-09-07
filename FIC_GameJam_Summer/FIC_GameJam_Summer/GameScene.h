#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>

class Physics;
class StageObjectManager;
class GameObjectManager;
class UIManager;
class Player;
class Bullet;
class Enemy;
class EnemyBullet;

class GameScene final : public SceneBase
{
private:
	// 敵の種類
	enum class EnemyType
	{
		Turret,
		// Mover, // 今後実装予定
	};

	// 1体分の出現情報
	struct EnemySpawnInfo
	{
		EnemyType type;
		float y;	// 出現Y座標(X座標は固定)
	};

	// 1ウェーブ分の出現情報
	struct WaveData
	{
		std::vector<EnemySpawnInfo> enemies;
	};

public:
	explicit GameScene(SceneController& controller, int stageNo);
	~GameScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_stageNo;
	int m_stage1bgHandle;

	std::vector<int> m_bulletAnimHandles;

	std::unique_ptr<Player> m_player;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	std::vector<std::unique_ptr<EnemyBullet>> m_enemyBullets;

	int m_remainingBullets;	// このステージで残っている弾数

	std::vector<WaveData> m_waves;	// このステージの全ウェーブ情報
	int m_currentWaveIndex;		// 現在のウェーブ番号

	bool m_isGameOver;				// フェードアウト後の遷移先を分けるためのフラグ

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(GameScene::*)();
	using DrawFunc_t = void(GameScene::*)();
	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	// 通常時更新処理
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();

	// このステージのウェーブデータを構築する
	void BuildWaveData();
	// 指定したウェーブの敵を生成する
	void SpawnWave(int waveIndex);

	// 弾を発射できるか判定し、発射処理を行う
	void TryShoot();
	// 弾の生成・更新・削除
	void UpdateBullets();

	// 敵の更新・敵の弾生成トリガー確認
	void UpdateEnemies();
	// 敵の弾の更新・削除
	void UpdateEnemyBullets();

	// 自弾と敵、敵弾とプレイヤーの当たり判定、および死亡した敵の削除
	void CheckCollisions();

	// ウェーブ進行(現在のウェーブの敵が全滅したら次のウェーブへ、全滅済みならステージクリアへ)
	void UpdateWaveProgress();
};