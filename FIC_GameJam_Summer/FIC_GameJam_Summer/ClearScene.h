#pragma once
#include "SceneBase.h"

class ClearScene final : public SceneBase
{
public:
	// hp: クリア時の残りHP, remainingBullets: クリア時の残弾数, totalBullets: そのステージの弾数上限
	ClearScene(SceneController& controller, int hp, int remainingBullets, int totalBullets);
	~ClearScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_hp;
	int m_remainingBullets;
	int m_totalBullets;

	int m_hpScore;
	int m_bulletScore;
	int m_totalScore;

	int m_resultPhase;	// 0:クリア表示 1:HPスコア表示 2:弾スコア表示 3:合計スコア表示
	int m_phaseTimer;	// 各フェーズの経過フレーム数(自動送り用)

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(ClearScene::*)();
	using DrawFunc_t = void(ClearScene::*)();

	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	// 通常時更新処理(結果表示の進行を管理)
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();

	// スコアを計算する
	void CalcScore();
};