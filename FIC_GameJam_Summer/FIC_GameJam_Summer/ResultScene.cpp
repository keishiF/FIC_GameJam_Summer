#include "ResultScene.h"
#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include "TextDraw.h"
#include "AudioManager.h"
#include <cstdio>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// 基礎点(ここを書き換えれば調整できる)
	constexpr int kClearBaseScore = 10000;
	constexpr int kGameOverBaseScore = 0;

	// 残り体力スコア: 1につき何点か(固定値、ここを書き換えれば調整できる)
	constexpr int kHpScorePerPoint = 1000;

	// 残弾スコア: 残弾1発につき何点か(固定値、ここを書き換えれば調整できる)
	constexpr int kBulletScorePerPoint = 1500;

	// 各結果表示フェーズが自動的に切り替わるまでのフレーム数(最後のフェーズはOK待ち)
	constexpr int kPhaseDuration = 60;

	constexpr int kPhaseBaseScore = 0;
	constexpr int kPhaseHpScore = 1;
	constexpr int kPhaseBulletScore = 2;
	constexpr int kPhaseTotalScore = 3;

	// レイアウト用の固定座標(ここを書き換えれば配置を調整できる)
	constexpr int kTitleY = 100;
	constexpr int kLabelX = 420;			// ラベルの左揃え位置
	constexpr int kValueRightX = 760;		// 数値の右揃え位置(ここに右端を合わせる)
	constexpr int kPtGapX = 15;			// 数値の右端から"pt"までの隙間

	constexpr int kLineStartY = 220;
	constexpr int kLineHeight = 70;		// Scoreフォント(サイズ40)に合わせて少し広げた
	constexpr int kTotalExtraGap = 30;		// 合計行の前に空ける追加の余白
	constexpr int kLabelYNudge = 8;			// ラベル(小さいフォント)を値(大きいフォント)と縦中央っぽく揃えるための微調整

	constexpr unsigned int kTextColor = 0xffffff;
	constexpr unsigned int kEdgeColor = 0x000000;

	// ラベル + 右揃え数値 + "pt" の1行を描画する
	// labelFontType: ラベル部分に使うフォント, valueFontType: 数値+ptに使うフォント
	void DrawScoreLine(int y, const char* label, int score, bool withPlusSign,
		TextDraw::FontType labelFontType, TextDraw::FontType valueFontType)
	{
		// ラベルは値より小さいフォントの場合が多いので、少し下にずらして視覚的な中心を合わせる
		int labelY = (valueFontType != TextDraw::FontType::Default) ? y + kLabelYNudge : y;

		TextDraw::DrawOutlinedText(kLabelX, labelY, label, kTextColor, kEdgeColor, labelFontType);

		char valueText[32];
		if (withPlusSign)
		{
			snprintf(valueText, sizeof(valueText), "+%d", score);
		}
		else
		{
			snprintf(valueText, sizeof(valueText), "%d", score);
		}

		int valueWidth = TextDraw::GetTextWidth(valueText, valueFontType);
		TextDraw::DrawOutlinedText(kValueRightX - valueWidth, y, valueText, kTextColor, kEdgeColor, valueFontType);

		TextDraw::DrawOutlinedText(kValueRightX + kPtGapX, y, "pt", kTextColor, kEdgeColor, valueFontType);
	}
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

	// 結果画面のBGMを再生
	if (m_isClear)
	{
		AudioManager::GetInstance().PlayBGM(AudioManager::BGM::Clear);
	}
	else
	{
		AudioManager::GetInstance().PlayBGM(AudioManager::BGM::GameOver);
	}
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

	// タイトルはBell MTフォント(大)、中央揃え ※変更なし
	const char* titleText = m_isClear ? "STAGE CLEAR" : "GAME OVER";
	int titleWidth = TextDraw::GetTextWidth(titleText, TextDraw::FontType::Title);
	TextDraw::DrawOutlinedText((Game::kScreenWidth - titleWidth) / 2, kTitleY, titleText, kTextColor, kEdgeColor, TextDraw::FontType::Title);

	// 基礎点(クリア/クリアならず): ラベルはデフォルト、数値+ptはBell MT(Scoreサイズ)
	const char* baseLabel = m_isClear ? "クリア" : "クリアならず";
	DrawScoreLine(kLineStartY, baseLabel, m_baseScore, false, TextDraw::FontType::Default, TextDraw::FontType::Score);

	if (m_resultPhase >= kPhaseHpScore)
	{
		// 残り体力: ラベルはデフォルト、数値+ptはBell MT(Scoreサイズ)
		DrawScoreLine(kLineStartY + kLineHeight, "残り体力", m_hpScore, true, TextDraw::FontType::Default, TextDraw::FontType::Score);
	}

	if (m_resultPhase >= kPhaseBulletScore)
	{
		// 残弾数: ラベルはデフォルト、数値+ptはBell MT(Scoreサイズ)
		DrawScoreLine(kLineStartY + kLineHeight * 2, "残弾数", m_bulletScore, true, TextDraw::FontType::Default, TextDraw::FontType::Score);
	}

	if (m_resultPhase >= kPhaseTotalScore)
	{
		int totalY = kLineStartY + kLineHeight * 3 + kTotalExtraGap;
		// 合計: ラベルはデフォルト、数値+ptはBell MT(Scoreサイズ)
		DrawScoreLine(totalY, "合計", m_totalScore, false, TextDraw::FontType::Default, TextDraw::FontType::Score);

		if ((m_blinkFrame / 30) % 2 == 0)
		{
			const char* pressText = "PRESS A BUTTON";
			int pressWidth = TextDraw::GetTextWidth(pressText);
			TextDraw::DrawOutlinedText((Game::kScreenWidth - pressWidth) / 2, totalY + 80, pressText, kTextColor, kEdgeColor);
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