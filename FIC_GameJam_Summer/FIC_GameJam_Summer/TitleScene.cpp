#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include "TextDraw.h"
#include "TitleScene.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// 「PRESS A BUTTON」「START」の表示位置・間隔(ここを書き換えれば配置を調整できる)
	constexpr int kPromptY = 560;
	constexpr int kStartY = 620;
	constexpr int kPromptBlinkInterval = 30;	// 点滅間隔(フレーム数)
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_titleHandle(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	m_titleHandle = LoadGraph("Data/Title.png");
	assert(m_titleHandle > 0);
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