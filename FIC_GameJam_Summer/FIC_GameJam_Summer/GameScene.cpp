#include "Bullet.h"
#include "ClearScene.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "EnemyLooper.h"
#include "EnemyMover.h"
#include "EnemyShooter.h"
#include "game.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "HitEffect.h"
#include "Input.h"
#include "Player.h"
#include "SceneController.h"
#include "TitleScene.h"
#include <algorithm>
#include <cassert>
#include <string>
#include <DxLib.h>

namespace
{
	constexpr int kFadeInterval = 60;

	constexpr float kPi = 3.14159265358979323846f;

	// プレイヤーの弾アニメーション
	constexpr int kPlayerBulletFrameWidth = 256;
	constexpr int kPlayerBulletFrameHeight = 64;

	// 敵の弾アニメーション
	constexpr int kEnemyBulletFrameWidth = 128;
	constexpr int kEnemyBulletFrameHeight = 64;

	// 弾グラフィックのアニメーションの総数
	constexpr int kBulletAnimFrameNum = 7;
	// 1コマに使うフレーム数
	constexpr int kBulletAnimFrameCount = 6;

	// ステージごとの弾数上限
	constexpr int kStageBulletLimits[] = { 21, 22, 23, 24 };

	// 背景スクロール速度
	constexpr float kBgScrollSpeed = 2.0f;

	// 敵の出現X座標
	constexpr float kEnemySpawnX = 1000.0f;

	// 画面を縦4分割した際の各行の中心Y座標
	constexpr int kGridRows = 4;
	constexpr float kGridRowHeight = static_cast<float>(Game::kScreenHeight) / kGridRows;
	constexpr float kRowY[kGridRows] = {
		kGridRowHeight * 0.5f,	// 1番上
		kGridRowHeight * 1.5f,	// 上から2番目
		kGridRowHeight * 2.5f,	// 上から3番目
		kGridRowHeight * 3.5f,	// 1番下
	};

	// ステージ2用グリッド、左に1マス分追加
	constexpr float kGridColumnWidth = kGridRowHeight;
	constexpr float kRightColumnX = kEnemySpawnX;
	constexpr float kLeftColumnX = kEnemySpawnX - kGridColumnWidth;

	// ステージ2 ウェーブ2/3: ループ移動の軌道パラメータ
	constexpr float kLoopCenterX = (kLeftColumnX + kRightColumnX) / 2.0f;
	constexpr float kLoopCenterY = (kRowY[0] + kRowY[3]) / 2.0f;
	constexpr float kLoopRadiusX = (kRightColumnX - kLeftColumnX) / 2.0f;
	constexpr float kLoopRadiusY = (kRowY[3] - kRowY[0]) / 2.0f;

#ifdef _DEBUG
	// 当たり判定デバッグ表示用の矩形描画
	void DrawCollisionBox(float x, float y, float halfWidth, float halfHeight, unsigned int color)
	{
		DrawBox(
			static_cast<int>(x - halfWidth),
			static_cast<int>(y - halfHeight),
			static_cast<int>(x + halfWidth),
			static_cast<int>(y + halfHeight),
			color,
			false);
	}
#endif
}

GameScene::GameScene(SceneController& controller, int stageNo) :
	SceneBase(controller),
	m_stageNo(stageNo),
	m_stagebgHandle(-1),
	m_bgWidth(0),
	m_bgScrollX(0.0f),
	m_remainingBullets(0),
	m_currentWaveIndex(0),
	m_isGameOver(false),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	std::string bgPath = "Data/Stage" + std::to_string(m_stageNo) + "bg.png";
	m_stagebgHandle = LoadGraph(bgPath.c_str());
	assert(m_stagebgHandle > 0);

	int dummyHeight = 0;
	GetGraphSize(m_stagebgHandle, &m_bgWidth, &dummyHeight);

	// プレイヤー弾アニメーションを分割で読み込み
	{
		int handles[kBulletAnimFrameNum];
		int result = LoadDivGraph(
			"Data/PlayerBullet.png",
			kBulletAnimFrameNum,
			1, kBulletAnimFrameNum,
			kPlayerBulletFrameWidth, kPlayerBulletFrameHeight,
			handles);
		assert(result == 0);

		m_playerBulletAnimHandles.assign(handles, handles + kBulletAnimFrameCount);
	}

	// 敵弾アニメーションを分割で読み込み
	{
		int handles[kBulletAnimFrameNum];
		int result = LoadDivGraph(
			"Data/EnemyBullet.png",
			kBulletAnimFrameNum,
			1, kBulletAnimFrameNum,
			kEnemyBulletFrameWidth, kEnemyBulletFrameHeight,
			handles);
		assert(result == 0);

		m_enemyBulletAnimHandles.assign(handles, handles + kBulletAnimFrameCount);
	}

	// ヒットエフェクト画像を8枚まとめて読み込み
	m_hitEffectHandles.reserve(8);
	for (int i = 1; i <= 8; ++i)
	{
		std::string path = "Data/Hit" + std::to_string(i) + ".png";
		int handle = LoadGraph(path.c_str());
		assert(handle > 0);
		m_hitEffectHandles.push_back(handle);
	}

	m_player = std::make_unique<Player>(200.0f, Game::kScreenHeight / 2.0f);

	// ステージ番号に対応する弾数上限を適用
	int stageIndex = m_stageNo - 1;
	assert(stageIndex >= 0 && stageIndex < static_cast<int>(std::size(kStageBulletLimits)));
	m_remainingBullets = kStageBulletLimits[stageIndex];

	BuildWaveData();
	SpawnWave(m_currentWaveIndex);
}

GameScene::~GameScene()
{
}

void GameScene::Update()
{
	(this->*m_update)();
}

void GameScene::Draw()
{
	(this->*m_draw)();
}

void GameScene::NormalUpdate()
{
	++m_blinkFrame;

	UpdateBackgroundScroll();

	m_player->Update();

	TryShoot();

	UpdateBullets();
	UpdateEnemies();
	UpdateEnemyBullets();
	UpdateHitEffects();

	CheckCollisions();

	UpdateWaveProgress();

	// ウェーブ進行処理内で既にステージクリアへ切り替わっている場合はここで終了
	if (m_update != &GameScene::NormalUpdate)
	{
		return;
	}

	// プレイヤーのHPが0になったらゲームオーバー
	if (!m_player->IsAlive())
	{
		m_isGameOver = true;
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
		return;
	}

	// 弾切れかつ、まだ全ウェーブクリアしていない場合はゲームオーバー
	if (m_remainingBullets <= 0 && m_bullets.empty())
	{
		m_isGameOver = true;
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void GameScene::FadeInUpdate()
{
	// フェードイン中も背景は動かしておく
	UpdateBackgroundScroll();

	if (--m_fadeFrame <= 0)
	{
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
	}
}

void GameScene::FadeOutUpdate()
{
	// フェードアウト中も背景は動かしておく
	UpdateBackgroundScroll();

	if (m_fadeFrame++ >= kFadeInterval)
	{
		if (m_isGameOver)
		{
			m_controller.ChangeScene(std::make_shared<GameOverScene>(m_controller));
		}
		else
		{
			m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller));
		}

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void GameScene::UpdateBackgroundScroll()
{
	m_bgScrollX -= kBgScrollSpeed;

	// 画像の実際の幅を基準にループさせる(画面幅ではなく画像幅を使うのがポイント)
	if (m_bgScrollX <= -static_cast<float>(m_bgWidth))
	{
		m_bgScrollX += static_cast<float>(m_bgWidth);
	}
}

void GameScene::DrawBackground() const
{
	int offsetX = static_cast<int>(m_bgScrollX);

	// 2枚並べて描画し、繋ぎ目が見えないようにループさせる(画像の実際の幅で並べる)
	DrawGraph(offsetX, 0, m_stagebgHandle, true);
	DrawGraph(offsetX + m_bgWidth, 0, m_stagebgHandle, true);
}

void GameScene::BuildWaveData()
{
	m_waves.clear();

	switch (m_stageNo)
	{
	case 1:
	{
		WaveData wave1;
		wave1.enemies = {
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[1] },
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[2] },
		};

		WaveData wave2;
		wave2.enemies = {
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[0] },
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[3] },
		};

		WaveData wave3;
		wave3.enemies = {
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[1] },
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[3] },
		};

		m_waves = { wave1, wave2, wave3 };
		break;
	}
	case 2:
	{
		WaveData wave1;
		wave1.enemies = {
			// 左グリッド: 上から下へ移動開始
			{ EnemyType::Mover, kLeftColumnX, kRowY[0], 0.0f, 0.0f, 1, 0.0f },
			// 右グリッド: 下から上へ移動開始
			{ EnemyType::Mover, kRightColumnX, kRowY[3], 0.0f, 0.0f, -1, 0.0f },
		};

		WaveData wave2;
		wave2.enemies = {
			// 上から出て時計回りにループ
			{ EnemyType::Looper, kLoopCenterX, kLoopCenterY, kLoopRadiusX, kLoopRadiusY, 1, 0.0f },
			// 下から出て、上の個体と常に反対側を保ちながら同じ方向にループ
			{ EnemyType::Looper, kLoopCenterX, kLoopCenterY, kLoopRadiusX, kLoopRadiusY, 1, kPi },
		};

		WaveData wave3;
		wave3.enemies = {
			// ウェーブ2と逆回転
			{ EnemyType::Looper, kLoopCenterX, kLoopCenterY, kLoopRadiusX, kLoopRadiusY, -1, 0.0f },
			{ EnemyType::Looper, kLoopCenterX, kLoopCenterY, kLoopRadiusX, kLoopRadiusY, -1, kPi },
		};

		m_waves = { wave1, wave2, wave3 };
		break;
	}
	default:
	{
		// ステージ3・4のウェーブデータも決まり次第ここに追加する
		// 仮でステージ1と同じ内容にしておく
		WaveData tempWave;
		tempWave.enemies = {
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[1] },
			{ EnemyType::Shooter, kEnemySpawnX, kRowY[2] },
		};
		m_waves = { tempWave };
		break;
	}
	}
}

void GameScene::SpawnWave(int waveIndex)
{
	if (waveIndex < 0 || waveIndex >= static_cast<int>(m_waves.size()))
	{
		return;
	}

	const WaveData& wave = m_waves[waveIndex];

	for (const auto& spawnInfo : wave.enemies)
	{
		switch (spawnInfo.type)
		{
		case EnemyType::Shooter:
			m_enemies.push_back(std::make_unique<EnemyShooter>(spawnInfo.x, spawnInfo.y));
			break;

		case EnemyType::Mover:
			m_enemies.push_back(std::make_unique<EnemyMover>(
				spawnInfo.x, spawnInfo.y, kRowY[0], kRowY[3], spawnInfo.direction));
			break;

		case EnemyType::Looper:
			m_enemies.push_back(std::make_unique<EnemyLooper>(
				spawnInfo.x, spawnInfo.y, spawnInfo.radiusX, spawnInfo.radiusY, spawnInfo.direction, spawnInfo.startAngle));
			break;
		}
	}
}

void GameScene::TryShoot()
{
	if (!m_player->IsShotTriggered())
	{
		return;
	}

	// 残弾が無ければ発射できない
	if (m_remainingBullets <= 0)
	{
		return;
	}

	m_bullets.push_back(std::make_unique<Bullet>(m_player->GetX(), m_player->GetY(), m_playerBulletAnimHandles));
	--m_remainingBullets;
}

void GameScene::UpdateBullets()
{
	for (auto& bullet : m_bullets)
	{
		bullet->Update();
	}

	// 非アクティブな弾をまとめて削除
	m_bullets.erase(
		std::remove_if(m_bullets.begin(), m_bullets.end(),
			[](const std::unique_ptr<Bullet>& bullet) { return !bullet->IsActive(); }),
		m_bullets.end());
}

void GameScene::UpdateEnemies()
{
	for (auto& enemy : m_enemies)
	{
		if (!enemy->IsAlive())
		{
			continue;
		}

		enemy->Update();

		if (enemy->IsShotTriggered())
		{
			m_enemyBullets.push_back(std::make_unique<EnemyBullet>(enemy->GetX(), enemy->GetY(), m_enemyBulletAnimHandles));
		}
	}
}

void GameScene::UpdateEnemyBullets()
{
	for (auto& enemyBullet : m_enemyBullets)
	{
		enemyBullet->Update();
	}

	m_enemyBullets.erase(
		std::remove_if(m_enemyBullets.begin(), m_enemyBullets.end(),
			[](const std::unique_ptr<EnemyBullet>& enemyBullet) { return !enemyBullet->IsActive(); }),
		m_enemyBullets.end());
}

void GameScene::UpdateHitEffects()
{
	for (auto& hitEffect : m_hitEffects)
	{
		hitEffect->Update();
	}

	m_hitEffects.erase(
		std::remove_if(m_hitEffects.begin(), m_hitEffects.end(),
			[](const std::unique_ptr<HitEffect>& hitEffect) { return !hitEffect->IsActive(); }),
		m_hitEffects.end());
}

void GameScene::CheckCollisions()
{
	// 自弾 vs 敵
	for (auto& bullet : m_bullets)
	{
		if (!bullet->IsActive())
		{
			continue;
		}

		for (auto& enemy : m_enemies)
		{
			if (!enemy->IsAlive())
			{
				continue;
			}

			if (bullet->CheckHit(enemy->GetX(), enemy->GetY(), enemy->GetCollisionHalfWidth(), enemy->GetCollisionHalfHeight()))
			{
				enemy->TakeDamage(1);

				// ヒットエフェクトを弾の位置に生成
				m_hitEffects.push_back(std::make_unique<HitEffect>(bullet->GetX(), bullet->GetY(), m_hitEffectHandles));

				bullet->Deactivate();
				break;	// 1発の弾は1体にしか当たらない
			}
		}
	}

	// 敵弾 vs プレイヤー
	for (auto& enemyBullet : m_enemyBullets)
	{
		if (!enemyBullet->IsActive())
		{
			continue;
		}

		if (enemyBullet->CheckHit(m_player->GetX(), m_player->GetY(), m_player->GetCollisionHalfWidth(), m_player->GetCollisionHalfHeight()))
		{
			m_player->TakeDamage(1);
			enemyBullet->Deactivate();
		}
	}

	// 死亡した敵を削除
	m_enemies.erase(
		std::remove_if(m_enemies.begin(), m_enemies.end(),
			[](const std::unique_ptr<Enemy>& enemy) { return !enemy->IsAlive(); }),
		m_enemies.end());
}

void GameScene::UpdateWaveProgress()
{
	// まだ敵が残っている場合は何もしない
	if (!m_enemies.empty())
	{
		return;
	}

	++m_currentWaveIndex;

	if (m_currentWaveIndex < static_cast<int>(m_waves.size()))
	{
		SpawnWave(m_currentWaveIndex);
		return;
	}

	// 全ウェーブクリア → ステージクリアへ
	m_isGameOver = false;
	m_update = &GameScene::FadeOutUpdate;
	m_draw = &GameScene::FadeDraw;
	m_fadeFrame = 0;
}

void GameScene::NormalDraw()
{
	DrawBackground();

	m_player->Draw();

	for (const auto& bullet : m_bullets)
	{
		bullet->Draw();
	}

	for (const auto& enemy : m_enemies)
	{
		enemy->Draw();
	}

	for (const auto& enemyBullet : m_enemyBullets)
	{
		enemyBullet->Draw();
	}

#ifdef _DEBUG
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Game Scene", 0xffffff);
	}

	for (const auto& hitEffect : m_hitEffects)
	{
		hitEffect->Draw();
	}

	DrawFormatString(0, 20, 0xffffff, "残弾: %d", m_remainingBullets);
	DrawFormatString(0, 40, 0xffffff, "HP: %d", m_player->GetHp());
	DrawFormatString(0, 60, 0xffffff, "Wave: %d / %d", m_currentWaveIndex + 1, static_cast<int>(m_waves.size()));
	DrawFormatString(0, 80, 0xffffff, "Enemies: %d", static_cast<int>(m_enemies.size()));

	// 当たり判定デバッグ表示
	DrawCollisionBox(m_player->GetX(), m_player->GetY(), m_player->GetCollisionHalfWidth(), m_player->GetCollisionHalfHeight(), 0x00ff00);

	for (const auto& enemy : m_enemies)
	{
		DrawCollisionBox(enemy->GetX(), enemy->GetY(), enemy->GetCollisionHalfWidth(), enemy->GetCollisionHalfHeight(), 0xff0000);
	}

	for (const auto& bullet : m_bullets)
	{
		DrawCollisionBox(bullet->GetX(), bullet->GetY(), bullet->GetCollisionHalfWidth(), bullet->GetCollisionHalfHeight(), 0x00ffff);
	}

	for (const auto& enemyBullet : m_enemyBullets)
	{
		DrawCollisionBox(enemyBullet->GetX(), enemyBullet->GetY(), enemyBullet->GetCollisionHalfWidth(), enemyBullet->GetCollisionHalfHeight(), 0xffff00);
	}
#endif
}

void GameScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}