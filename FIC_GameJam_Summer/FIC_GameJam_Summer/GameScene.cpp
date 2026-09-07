#include "Bullet.h"
#include "ClearScene.h"
#include "game.h"
#include "GameScene.h"
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

	constexpr int kBulletFrameWidth = 256;
	constexpr int kBulletFrameHeight = 64;
	constexpr int kBulletAnimFrameNum = 7;
	constexpr int kBulletAnimFrameCount = 6;

	// ステージごとの弾数上限
	constexpr int kStageBulletLimits[] = { 10, 8, 6, 5 };
}

GameScene::GameScene(SceneController& controller, int stageNo) :
	SceneBase(controller),
	m_stageNo(stageNo),
	m_stagebgHandle(-1),
	m_remainingBullets(0),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	std::string bgPath = "Data/Stage" + std::to_string(m_stageNo) + "bg.png";
	m_stagebgHandle = LoadGraph(bgPath.c_str());
	assert(m_stagebgHandle > 0);

	// 弾アニメーション読み込み
	int handles[kBulletAnimFrameNum];
	int result = LoadDivGraph(
		"Data/Bullet.png",
		kBulletAnimFrameNum,
		1, kBulletAnimFrameNum,
		kBulletFrameWidth, kBulletFrameHeight,
		handles);
	assert(result == 0);

	m_bulletAnimHandles.assign(handles, handles + kBulletAnimFrameCount);

	m_player = std::make_unique<Player>(200.0f, Game::kScreenHeight / 2.0f);

	// ステージ番号に対応する弾数上限を適用
	int stageIndex = m_stageNo - 1;
	assert(stageIndex >= 0 && stageIndex < static_cast<int>(std::size(kStageBulletLimits)));
	m_remainingBullets = kStageBulletLimits[stageIndex];
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

	m_player->Update();

	TryShoot();

	UpdateBullets();

	/*if (Input::GetInstance().IsPress("OK"))
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}*/
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
		m_controller.ChangeScene(std::make_shared<ClearScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
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

	// 非アクティブな弾をまとめて削除
	m_bullets.erase(
		std::remove_if(m_bullets.begin(), m_bullets.end(),
			[](const std::unique_ptr<Bullet>& bullet) { return !bullet->IsActive(); }),
		m_bullets.end());
}

void GameScene::NormalDraw()
{
	DrawGraph(0, 0, m_stagebgHandle, true);

	m_player->Draw();

	for (const auto& bullet : m_bullets)
	{
		bullet->Draw();
	}

	// 残弾数の表示
	DrawFormatString(20, 40, 0xffffff, "残弾: %d", m_remainingBullets);

#ifdef _DEBUG
	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Game Scene", 0xffffff);
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