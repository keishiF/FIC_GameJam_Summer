#include "Bullet.h"
#include "ClearScene.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "game.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "Input.h"
#include "Player.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "EnemyShooter.h"
#include <algorithm>
#include <cassert>
#include <string>
#include <DxLib.h>

namespace
{
	constexpr int kFadeInterval = 60;

	constexpr int kBulletFrameWidth = 256;
	constexpr int kBulletFrameHeight = 64;
	constexpr int kBulletSheetFrameCount = 7;	// シート内の総コマ数(最後の1コマは空白)
	constexpr int kBulletAnimFrameCount = 6;	// アニメーションに使うコマ数(空白コマは除く)

	// ステージごとの弾数上限(インデックス0 = ステージ1、1 = ステージ2 ...)
	constexpr int kStageBulletLimits[] = { 10, 13, 16, 12 };

	// 敵の出現X座標(画面右側で固定)
	constexpr float kEnemySpawnX = 1000.0f;

	// 画面を縦4分割した際の各行の中心Y座標(表示はしない、配置計算のみに使用)
	constexpr int kGridRows = 4;
	constexpr float kGridRowHeight = static_cast<float>(Game::kScreenHeight) / kGridRows;
	constexpr float kRowY[kGridRows] = {
		kGridRowHeight * 0.5f,	// 1番上
		kGridRowHeight * 1.5f,	// 上から2番目
		kGridRowHeight * 2.5f,	// 上から3番目
		kGridRowHeight * 3.5f,	// 1番下
	};
}

GameScene::GameScene(SceneController& controller, int stageNo) :
	SceneBase(controller),
	m_stageNo(stageNo),
	m_stage1bgHandle(-1),
	m_remainingBullets(0),
	m_currentWaveIndex(0),
	m_isGameOver(false),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	std::string bgPath = "Data/Stage" + std::to_string(m_stageNo) + "bg.png";
	m_stage1bgHandle = LoadGraph(bgPath.c_str());
	assert(m_stage1bgHandle > 0);

	// 弾アニメーションを7コマ分割で読み込み
	int handles[kBulletSheetFrameCount];
	int result = LoadDivGraph(
		"Data/Bullet.png",
		kBulletSheetFrameCount,
		1, kBulletSheetFrameCount,
		kBulletFrameWidth, kBulletFrameHeight,
		handles);
	assert(result == 0);

	// 空白の最終コマを除いてアニメーション用配列に格納
	m_bulletAnimHandles.assign(handles, handles + kBulletAnimFrameCount);

	m_player = std::make_unique<Player>(200.0f, Game::kScreenHeight / 2.0f);

	// ステージ番号(1始まり)に対応する弾数上限を適用
	int stageIndex = m_stageNo - 1;
	assert(stageIndex >= 0 && stageIndex < static_cast<int>(std::size(kStageBulletLimits)));
	m_remainingBullets = kStageBulletLimits[stageIndex];

	BuildWaveData();
	SpawnWave(m_currentWaveIndex);
}

GameScene::~GameScene()
{}

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

	m_player->Update();

	TryShoot();

	UpdateBullets();
	UpdateEnemies();
	UpdateEnemyBullets();

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

	// 弾切れ(画面上の弾も含めて全て無くなった)かつ、まだ全ウェーブクリアしていない場合はゲームオーバー
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
	if (--m_fadeFrame <= 0)
	{
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
	}
}

void GameScene::FadeOutUpdate()
{
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

void GameScene::BuildWaveData()
{
	m_waves.clear();

	switch (m_stageNo)
	{
	case 1:
	{
		WaveData wave1;
		wave1.enemies = {
			{ EnemyType::Turret, kRowY[1] },
			{ EnemyType::Turret, kRowY[2] },
		};

		WaveData wave2;
		wave2.enemies = {
			{ EnemyType::Turret, kRowY[0] },
			{ EnemyType::Turret, kRowY[3] },
		};

		WaveData wave3;
		wave3.enemies = {
			{ EnemyType::Turret, kRowY[1] },
			{ EnemyType::Turret, kRowY[3] },
		};

		m_waves = { wave1, wave2, wave3 };
		break;
	}
	default:
	{
		// TODO: ステージ2以降のウェーブデータも決まり次第ここに追加する
		// 仮でステージ1と同じ内容にしておく(0件のままだと即クリア扱いになってしまうため)
		WaveData tempWave;
		tempWave.enemies = {
			{ EnemyType::Turret, kRowY[1] },
			{ EnemyType::Turret, kRowY[2] },
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
		case EnemyType::Turret:
			m_enemies.push_back(std::make_unique<EnemyShooter>(kEnemySpawnX, spawnInfo.y));
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

	m_bullets.push_back(std::make_unique<Bullet>(m_player->GetX(), m_player->GetY(), m_bulletAnimHandles));
	--m_remainingBullets;
}

void GameScene::UpdateBullets()
{
	for (auto& bullet : m_bullets)
	{
		bullet->Update();
	}

	// 非アクティブな弾をまとめて削除(erase-removeイディオム)
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
			m_enemyBullets.push_back(std::make_unique<EnemyBullet>(enemy->GetX(), enemy->GetY()));
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

			if (bullet->CheckHit(enemy->GetX(), enemy->GetY(), enemy->GetHalfWidth(), enemy->GetHalfHeight()))
			{
				enemy->TakeDamage(1);
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

		if (enemyBullet->CheckHit(m_player->GetX(), m_player->GetY(), m_player->GetHalfWidth(), m_player->GetHalfHeight()))
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
	DrawGraph(0, 0, m_stage1bgHandle, true);

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

	// 残弾数・HP表示(仮のUI、後で見た目を調整してください)
	DrawFormatString(20, Game::kScreenHeight - 60, 0xffffff, "残弾: %d", m_remainingBullets);
	DrawFormatString(20, Game::kScreenHeight - 30, 0xffffff, "HP: %d", m_player->GetHp());

#ifdef _DEBUG
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Game Scene", 0xffffff);
	}

	DrawFormatString(0, 20, 0xffffff, "Bullets on screen: %d", static_cast<int>(m_bullets.size()));
	DrawFormatString(0, 40, 0xffffff, "Wave: %d / %d", m_currentWaveIndex + 1, static_cast<int>(m_waves.size()));
	DrawFormatString(0, 60, 0xffffff, "Enemies: %d", static_cast<int>(m_enemies.size()));
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