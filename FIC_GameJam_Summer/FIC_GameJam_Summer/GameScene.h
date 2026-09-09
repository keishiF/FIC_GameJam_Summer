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
class HitEffect;

class GameScene final : public SceneBase
{
private:
	// 敵の種類
	enum class EnemyType
	{
		Shooter,
		Mover,
		Looper,
	};

	// 1体分の出現情報
	struct EnemySpawnInfo
	{
		EnemyType type;
		float x;			// 出現X座標、Looperでは中心X座標
		float y;			// 出現Y座標、Looperでは中心Y座標
		float radiusX;		// Looper専用: 横方向の半径(他タイプでは未使用)
		float radiusY;		// Looper専用: 縦方向の半径(他タイプでは未使用)
		int direction;		// Mover: 初期移動方向(+1:下 -1:上) / Looper: 回転方向(+1:時計回り -1:反時計回り)
		float startAngle;	// Looper専用: 開始角度(ラジアン、他タイプでは未使用)
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
	int m_stagebgHandle;
	int m_bgWidth;	// 背景画像の実際の横幅

	float m_bgScrollX;	// 背景スクロール用オフセット

	std::vector<int> m_playerBulletAnimHandles;
	std::vector<int> m_enemyBulletAnimHandles;
	std::vector<int> m_hitEffectHandles;

	std::unique_ptr<Player> m_player;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	std::vector<std::unique_ptr<EnemyBullet>> m_enemyBullets;
	std::vector<std::unique_ptr<HitEffect>> m_hitEffects;

	int m_totalBullets;		// このステージの弾数上限
	int m_remainingBullets;	// このステージで残っている弾数

	std::vector<WaveData> m_waves;	// このステージの全ウェーブ情報
	int m_currentWaveIndex;		// 現在のウェーブ番号

	bool m_isWaitingNextWave;	// 次のウェーブ出現まで待機中かどうか
	int m_waveDelayTimer;		// 待機残りフレーム数

	// フェードアウト後の遷移先を分けるためのフラグ
	bool m_isGameOver;

	int m_heartIconHandle;
	int m_ammoIconHandle;

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

	// クリア演出: プレイヤーを画面中央へ移動させる
	void ClearMoveToCenterUpdate();
	// クリア演出: プレイヤーを画面右へ飛ばす
	void ClearFlyOutUpdate();

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
	// Shooter同士が近づきすぎている場合に押し合わせる
	void ApplyShooterRepulsion();
	// 敵の弾の更新・削除
	void UpdateEnemyBullets();
	// ヒットエフェクトの更新・削除
	void UpdateHitEffects();

	// 自弾と敵、敵弾とプレイヤーの当たり判定、および死亡した敵の削除
	void CheckCollisions();

	// ウェーブ進行
	void UpdateWaveProgress();

	// 背景スクロールのオフセット更新
	void UpdateBackgroundScroll();
	// 背景を2枚並べてループ描画する
	void DrawBackground() const;

	// 残HP・残弾数のUI描画
	void DrawUI() const;
};