#include "game.h"
#include "GameOverScene.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;
}

GameOverScene::GameOverScene(SceneController& controller) :
	SceneBase(controller),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&GameOverScene::FadeInUpdate),
	m_draw(&GameOverScene::FadeDraw)
{}

GameOverScene::~GameOverScene()
{}

void GameOverScene::Update()
{
	(this->*m_update)();
}

void GameOverScene::Draw()
{
	(this->*m_draw)();
}

void GameOverScene::NormalUpdate()
{
	++m_blinkFrame;

	if (Input::GetInstance().IsPress("OK"))
	{
		m_update = &GameOverScene::FadeOutUpdate;
		m_draw = &GameOverScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void GameOverScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &GameOverScene::NormalUpdate;
		m_draw = &GameOverScene::NormalDraw;
	}
}

void GameOverScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		// ステージセレクトに戻ってやり直せるようにする
		m_controller.ChangeScene(std::make_shared<StageSelectScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void GameOverScene::NormalDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x400000, true);

	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(
			Game::kScreenWidth / 2 - 100,
			Game::kScreenHeight / 2 - 10,
			"GAME OVER - Press OK",
			0xffffff);
	}
}

void GameOverScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}