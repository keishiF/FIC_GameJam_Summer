#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include "TitleScene.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_titleHandle(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	/*m_titleHandle = LoadGraph("Data/UI/.png");
	assert(m_titleHandle >= 0);*/
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

	if (Input::GetInstance().IsPress("OK"))
	{
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void TitleScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
	}
}

void TitleScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<StageSelectScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void TitleScene::NormalDraw()
{
#ifdef _DEBUG
	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Title Scene", 0xffffff);
	}
#endif
}

void TitleScene::FadeDraw()
{
	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
