#include "game.h"
#include "GameScene.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include <cassert>
#include <cmath>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;
	constexpr int kStageNum = 4;
	constexpr int kStageBoxSize = 100;
	constexpr int kStageBoxMargin = 60;
	constexpr int kCursorColor = 0xff0000;

	constexpr int kPlayerSize = 40;
	// 目標位置への近づき具合(0~1、大きいほど速い)
	constexpr float kMoveLerpRate = 0.3f; 
}

StageSelectScene::StageSelectScene(SceneController& controller) :
	SceneBase(controller),
	m_playerHandle(-1),
	m_playerX(0.0f),
	m_playerY(Game::kScreenHeight * 0.5f),
	m_selectStage(0),
	m_facingRight(true),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&StageSelectScene::FadeInUpdate),
	m_draw(&StageSelectScene::FadeDraw)
{
	m_playerHandle = LoadGraph("Data/Player.png");
	assert(m_playerHandle > 0);

	// 初期位置はステージ1の位置
	m_playerX = GetStageCenterX(m_selectStage);
}

StageSelectScene::~StageSelectScene()
{
}

void StageSelectScene::Update()
{
	(this->*m_update)();
}

void StageSelectScene::Draw()
{
	(this->*m_draw)();
}

void StageSelectScene::NormalUpdate()
{
	++m_blinkFrame;

	if (Input::GetInstance().IsTrigger("RIGHT"))
	{
		if (m_selectStage < kStageNum - 1)
		{
			++m_selectStage;
		}
		m_facingRight = true;
	}
	else if (Input::GetInstance().IsTrigger("LEFT"))
	{
		if (m_selectStage > 0)
		{
			--m_selectStage;
		}
		m_facingRight = false;
	}

	UpdatePlayerPosition();

	if (Input::GetInstance().IsPress("OK"))
	{
		m_update = &StageSelectScene::FadeOutUpdate;
		m_draw = &StageSelectScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void StageSelectScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &StageSelectScene::NormalUpdate;
		m_draw = &StageSelectScene::NormalDraw;
	}
}

void StageSelectScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		// 選択したステージ番号を渡してGameSceneへ遷移
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_selectStage + 1));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

float StageSelectScene::GetStageCenterX(int stageIndex) const
{
	int totalWidth = kStageBoxSize * kStageNum + kStageBoxMargin * (kStageNum - 1);
	int startX = static_cast<int>((Game::kScreenWidth - totalWidth) * 0.5f);
	int boxX = startX + stageIndex * (kStageBoxSize + kStageBoxMargin);

	return static_cast<float>(boxX + kStageBoxSize * 0.5f);
}

void StageSelectScene::UpdatePlayerPosition()
{
	float targetX = GetStageCenterX(m_selectStage);

	// 目標位置へ少しずつ近づける
	m_playerX += (targetX - m_playerX) * kMoveLerpRate;

	// ほぼ到達したらぴったり合わせる(誤差の蓄積防止)
	if (std::abs(targetX - m_playerX) < 0.5f)
	{
		m_playerX = targetX;
	}
}

void StageSelectScene::NormalDraw()
{
	// 背景(仮)
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x202020, true);

	int totalWidth = kStageBoxSize * kStageNum + kStageBoxMargin * (kStageNum - 1);
	int startX = static_cast<int>((Game::kScreenWidth - totalWidth) * 0.5f);
	int y = static_cast<int>((Game::kScreenHeight - kStageBoxSize) * 0.5f);

	for (int i = 0; i < kStageNum; ++i)
	{
		int x = startX + i * (kStageBoxSize + kStageBoxMargin);

		DrawBox(x, y, x + kStageBoxSize, y + kStageBoxSize, 0xffffff, false);
		DrawFormatString(static_cast<int>(x + kStageBoxSize * 0.5f - 8), 
			static_cast<int>(y + kStageBoxSize * 0.5f - 8), 
			0xffffff, "%d", i + 1);

		// 選択中のステージを強調表示
		if (i == m_selectStage && (m_blinkFrame / 10) % 2 == 0)
		{
			DrawBox(x - 5, y - 5, x + kStageBoxSize + 5, y + kStageBoxSize + 5, kCursorColor, false);
		}
	}

	// プレイヤーキャラクター描画
	int drawX = static_cast<int>(m_playerX - kPlayerSize * 0.5f);
	int drawY = static_cast<int>(m_playerY - kPlayerSize * 0.5f);

	if (m_facingRight)
	{
		DrawGraph(drawX, drawY, m_playerHandle, true);
	}
	else
	{
		// 左右反転して描画
		DrawTurnGraph(drawX, drawY, m_playerHandle, true);
	}

#ifdef _DEBUG
	DrawString(0, 0, "Stage Select Scene", 0xffffff);
#endif
}

void StageSelectScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}