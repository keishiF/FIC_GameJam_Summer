#include "Bullet.h"
#include "EnemyBullet.h"
#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include "TextDraw.h"
#include "TitleBattleActor.h"
#include "TitleScene.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// 「PRESS A BUTTON」「START」の表示位置・間隔(ここを書き換えれば配置を調整できる)
	constexpr int kPromptY = 560;
	constexpr int kStartY = 620;
	constexpr int kPromptBlinkInterval = 30;	// 点滅間隔(フレーム数)

	// 弾アニメーション(ゲーム本編と同じシート構成: 縦7分割、最後の1コマは空白)
	constexpr int kPlayerBulletFrameWidth = 256;
	constexpr int kPlayerBulletFrameHeight = 64;
	constexpr int kEnemyBulletFrameWidth = 128;
	constexpr int kEnemyBulletFrameHeight = 64;
	constexpr int kBulletAnimFrameNum = 7;
	constexpr int kBulletAnimFrameCount = 6;

	// 背景演出用アクターの配置・見た目(ここを書き換えれば調整できる)
	constexpr float kPlayerActorX = 200.0f;
	constexpr float kEnemyActorX = 1080.0f;
	constexpr float kActorScale = 2.0f;
	constexpr float kActorMoveMinY = 60.0f;
	constexpr float kActorMoveMaxY = 690.0f;
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_titleHandle(-1),
	m_bgHandle(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	m_bgHandle = LoadGraph("Data/StageTitlebg.png");
	assert(m_bgHandle > 0);

	m_titleHandle = LoadGraph("Data/Title.png");
	assert(m_titleHandle > 0);

	// プレイヤー側弾アニメーションを読み込み
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

	// 敵側弾アニメーションを読み込み
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

	int playerHandle = LoadGraph("Data/Player.png");
	assert(playerHandle > 0);

	int enemyHandle = LoadGraph("Data/EnemyShooter.png");
	assert(enemyHandle > 0);

	m_playerActor = std::make_unique<TitleBattleActor>(
		kPlayerActorX, Game::kScreenHeight / 2.0f, kActorMoveMinY, kActorMoveMaxY, playerHandle, kActorScale);

	m_enemyActor = std::make_unique<TitleBattleActor>(
		kEnemyActorX, Game::kScreenHeight / 2.0f, kActorMoveMinY, kActorMoveMaxY, enemyHandle, kActorScale);
}

TitleScene::~TitleScene()
{
	DeleteGraph(m_titleHandle);
}

void TitleScene::Update()
{
	(this->*m_update)();
}

void TitleScene::Draw()
{
	(this->*m_draw)();
}

void TitleScene::NormalUpdate()
{
	++m_blinkFrame;

	UpdateBattleBackground();

	if (Input::GetInstance().IsPress("OK"))
	{
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void TitleScene::FadeInUpdate()
{
	UpdateBattleBackground();

	if (--m_fadeFrame <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
	}
}

void TitleScene::FadeOutUpdate()
{
	UpdateBattleBackground();

	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<StageSelectScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void TitleScene::UpdateBattleBackground()
{
	m_playerActor->Update();
	m_enemyActor->Update();

	if (m_playerActor->IsShotTriggered())
	{
		m_bullets.push_back(std::make_unique<Bullet>(m_playerActor->GetX(), m_playerActor->GetY(), m_playerBulletAnimHandles));
	}

	if (m_enemyActor->IsShotTriggered())
	{
		m_enemyBullets.push_back(std::make_unique<EnemyBullet>(m_enemyActor->GetX(), m_enemyActor->GetY(), m_enemyBulletAnimHandles));
	}

	for (auto& bullet : m_bullets)
	{
		bullet->Update();
	}
	m_bullets.erase(
		std::remove_if(m_bullets.begin(), m_bullets.end(),
			[](const std::unique_ptr<Bullet>& bullet) { return !bullet->IsActive(); }),
		m_bullets.end());

	for (auto& enemyBullet : m_enemyBullets)
	{
		enemyBullet->Update();
	}
	m_enemyBullets.erase(
		std::remove_if(m_enemyBullets.begin(), m_enemyBullets.end(),
			[](const std::unique_ptr<EnemyBullet>& enemyBullet) { return !enemyBullet->IsActive(); }),
		m_enemyBullets.end());
}

void TitleScene::DrawBattleBackground() const
{
	m_playerActor->Draw();
	m_enemyActor->Draw();

	for (const auto& bullet : m_bullets)
	{
		bullet->Draw();
	}

	for (const auto& enemyBullet : m_enemyBullets)
	{
		enemyBullet->Draw();
	}
}

void TitleScene::NormalDraw()
{
	DrawGraph(0, 0, m_bgHandle, true);

	// 背景演出(プレイヤー側・敵側の撃ち合い、ロゴより奥に表示する)
	DrawBattleBackground();

	// ロゴ(画像自体が透過付きで中央上に配置される構図になっている)
	DrawGraph(0, 0, m_titleHandle, true);

	// PRESS A BUTTON / START(点滅、Bell MTフォント、中央揃え)
	if ((m_blinkFrame / kPromptBlinkInterval) % 2 == 0)
	{
		const char* promptText = "PRESS A BUTTON";
		int promptWidth = TextDraw::GetTextWidth(promptText, TextDraw::FontType::Title);
		TextDraw::DrawOutlinedText((Game::kScreenWidth - promptWidth) / 2, kPromptY, promptText, 0xffffff, 0x000000, TextDraw::FontType::Title);

		const char* startText = "START";
		int startWidth = TextDraw::GetTextWidth(startText, TextDraw::FontType::Title);
		TextDraw::DrawOutlinedText((Game::kScreenWidth - startWidth) / 2, kStartY, startText, 0xffffff, 0x000000, TextDraw::FontType::Title);
	}

#ifdef _DEBUG
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Title Scene", 0xffffff);
	}
#endif
}

void TitleScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}