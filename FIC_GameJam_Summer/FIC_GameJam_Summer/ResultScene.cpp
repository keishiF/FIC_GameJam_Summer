#include "ResultScene.h"
#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// 基礎点(ここを書き換えれば調整できる)
	constexpr int kClearBaseScore = 10000;
	constexpr int kGameOverBaseScore = 0;

	// 残HPスコア: HP1につき何点か(固定値、ここを書き換えれば調整できる)
	constexpr int kHpScorePerPoint = 1000;

	// 残弾スコア: 残弾1発につき何点か(固定値、ここを書き換えれば調整できる)
	constexpr int kBulletScorePerPoint = 1500;

	// 各結果表示フェーズが自動的に切り替わるまでのフレーム数(最後のフェーズはOK待ち)
	constexpr int kPhaseDuration = 60;

	constexpr int kPhaseBaseScore = 0;
	constexpr int kPhaseHpScore = 1;
	constexpr int kPhaseBulletScore = 2;
	constexpr int kPhaseTotalScore = 3;
}

ResultScene::ResultScene(SceneController& controller, bool isClear, int hp, int remainingBullets, int totalBullets) :
	SceneBase(controller),
	m_isClear(isClear),
	m_hp(hp),
	m_remainingBullets(remainingBullets),
	m_totalBullets(totalBullets),
	m_baseScore(0),
	m_hpScore(0),
	m_bulletScore(0),
	m_totalScore(0),
	m_resultPhase(kPhaseBaseScore),
	m_phaseTimer(0),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&ResultScene::FadeInUpdate),
	m_draw(&ResultScene::FadeDraw)
{
	CalcScore();
}

ResultScene::~ResultScene()
{}

void ResultScene::Update()
{
	(this->*m_update)();
}

void ResultScene::Draw()
{
	(this->*m_draw)();
}

void ResultScene::CalcScore()
{
	m_baseScore = m_isClear ? kClearBaseScore : kGameOverBaseScore;
	m_hpScore = m_hp * kHpScorePerPoint;
	m_bulletScore = m_remainingBullets * kBulletScorePerPoint;

	m_totalScore = m_baseScore + m_hpScore + m_bulletScore;
}

void ResultScene::NormalUpdate()
{
	++m_blinkFrame;

	// 最終フェーズ(合計スコア表示)はOKボタンでフェードアウトへ
	if (m_resultPhase >= kPhaseTotalScore)
	{
		if (Input::GetInstance().IsPress("OK"))
		{
			m_update = &ResultScene::FadeOutUpdate;
			m_draw = &ResultScene::FadeDraw;
			m_fadeFrame = 0;
		}
		return;
	}

	// それ以外のフェーズは一定時間で自動的に次へ進む
	++m_phaseTimer;
	if (m_phaseTimer >= kPhaseDuration)
	{
		m_phaseTimer = 0;
		++m_resultPhase;
	}
}

void ResultScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &ResultScene::NormalUpdate;
		m_draw = &ResultScene::NormalDraw;
	}
}

void ResultScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		// ステージセレクトに戻る
		m_controller.ChangeScene(std::make_shared<StageSelectScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void ResultScene::NormalDraw()
{
	unsigned int bgColor = m_isClear ? 0x002040 : 0x400000;
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, bgColor, true);

	const char* titleText = m_isClear ? "STAGE CLEAR" : "GAME OVER";
	const char* resultLabel = m_isClear ? "クリア" : "クリアならず";

	DrawFormatString(static_cast<int>(Game::kScreenWidth * 0.5f - 100), 100, 0xffffff, "%s", titleText);
	DrawFormatString(static_cast<int>(Game::kScreenWidth * 0.5f - 150), 200, 0xffffff, "%s　　　%d pt", resultLabel, m_baseScore);

	if (m_resultPhase >= kPhaseHpScore)
	{
		DrawFormatString(static_cast<int>(Game::kScreenWidth * 0.5f - 150), 250, 0xffffff, "残HP　　　+%d pt", m_hpScore);
	}

	if (m_resultPhase >= kPhaseBulletScore)
	{
		DrawFormatString(static_cast<int>(Game::kScreenWidth * 0.5f - 150), 300, 0xffffff, "残弾数　　　+%d pt", m_bulletScore);
	}

	if (m_resultPhase >= kPhaseTotalScore)
	{
		DrawFormatString(static_cast<int>(Game::kScreenWidth * 0.5f - 150), 380, 0xffffff, "合計　　　%d pt", m_totalScore);

		if ((m_blinkFrame / 30) % 2 == 0)
		{
			DrawString(static_cast<int>(Game::kScreenWidth * 0.5f - 90), 450, "Press OK", 0xffffff);
		}
	}
}

void ResultScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}