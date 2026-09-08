#include "ClearScene.h"
#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// 残HPスコア: HP1につき何点か(固定値、ここを書き換えれば調整できる)
	constexpr int kHpScorePerPoint = 100;

	// 残弾数スコア: 残弾の割合(0.0~1.0)に応じた段階ボーナス
	struct BulletScoreTier
	{
		float ratioThreshold;
		int score;
	};
	// 割合がこの値以上なら対応する点数を与える(必ず降順で並べること、ここを書き換えれば段階を調整できる)
	constexpr BulletScoreTier kBulletScoreTiers[] = {
		{ 0.8f, 1000 },
		{ 0.5f, 700 },
		{ 0.2f, 400 },
		{ 0.0f, 100 },	// 1発でも残っていれば最低保証点
	};

	// 各結果表示フェーズが自動的に切り替わるまでのフレーム数(最後のフェーズはOK待ち)
	constexpr int kPhaseDuration = 60;

	constexpr int kPhaseClearTitle = 0;
	constexpr int kPhaseHpScore = 1;
	constexpr int kPhaseBulletScore = 2;
	constexpr int kPhaseTotalScore = 3;
}

ClearScene::ClearScene(SceneController& controller, int hp, int remainingBullets, int totalBullets) :
	SceneBase(controller),
	m_hp(hp),
	m_remainingBullets(remainingBullets),
	m_totalBullets(totalBullets),
	m_hpScore(0),
	m_bulletScore(0),
	m_totalScore(0),
	m_resultPhase(kPhaseClearTitle),
	m_phaseTimer(0),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&ClearScene::FadeInUpdate),
	m_draw(&ClearScene::FadeDraw)
{
	CalcScore();
}

ClearScene::~ClearScene()
{}

void ClearScene::Update()
{
	(this->*m_update)();
}

void ClearScene::Draw()
{
	(this->*m_draw)();
}

void ClearScene::CalcScore()
{
	m_hpScore = m_hp * kHpScorePerPoint;

	float ratio = (m_totalBullets > 0) ? static_cast<float>(m_remainingBullets) / static_cast<float>(m_totalBullets) : 0.0f;

	m_bulletScore = 0;
	for (const auto& tier : kBulletScoreTiers)
	{
		if (ratio >= tier.ratioThreshold)
		{
			m_bulletScore = tier.score;
			break;
		}
	}

	// 残弾が1発も無い場合は最低保証点も与えない
	if (m_remainingBullets <= 0)
	{
		m_bulletScore = 0;
	}

	m_totalScore = m_hpScore + m_bulletScore;
}

void ClearScene::NormalUpdate()
{
	++m_blinkFrame;

	// 最終フェーズ(合計スコア表示)はOKボタンでフェードアウトへ
	if (m_resultPhase >= kPhaseTotalScore)
	{
		if (Input::GetInstance().IsPress("OK"))
		{
			m_update = &ClearScene::FadeOutUpdate;
			m_draw = &ClearScene::FadeDraw;
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

void ClearScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &ClearScene::NormalUpdate;
		m_draw = &ClearScene::NormalDraw;
	}
}

void ClearScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		// ステージセレクトに戻る
		m_controller.ChangeScene(std::make_shared<StageSelectScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void ClearScene::NormalDraw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x002040, true);

	DrawString(Game::kScreenWidth * 0.5f - 80, 100, "STAGE CLEAR", 0xffffff);

	if (m_resultPhase >= kPhaseHpScore)
	{
		DrawFormatString(Game::kScreenWidth * 0.5f - 150, 250, 0xffffff, "残りHP: %d   +%d pt", m_hp, m_hpScore);
	}

	if (m_resultPhase >= kPhaseBulletScore)
	{
		DrawFormatString(Game::kScreenWidth * 0.5f - 150, 300, 0xffffff, "残弾数: %d / %d   +%d pt", m_remainingBullets, m_totalBullets, m_bulletScore);
	}

	if (m_resultPhase >= kPhaseTotalScore)
	{
		DrawFormatString(Game::kScreenWidth * 0.5f - 150, 380, 0xffffff, "合計スコア: %d pt", m_totalScore);

		if ((m_blinkFrame / 30) % 2 == 0)
		{
			DrawString(Game::kScreenWidth * 0.5f - 90, 450, "Press OK", 0xffffff);
		}
	}
}

void ClearScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}