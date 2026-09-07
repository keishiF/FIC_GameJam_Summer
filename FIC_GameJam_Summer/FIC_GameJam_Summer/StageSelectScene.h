#pragma once
#include "SceneBase.h"

class StageSelectScene final : public SceneBase
{
public:
	explicit StageSelectScene(SceneController& controller);
	~StageSelectScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_playerHandle;
	float m_playerX;	// 見た目の現在位置(補間で少しずつ動く)
	float m_playerY;

	int m_selectStage;	// 選択中のステージ番号(0始まり)
	bool m_facingRight;	// プレイヤーの向き(true:右向き, false:左向き)

	float m_fadeFrame;
	int m_blinkFrame;

	using UpdateFunc_t = void(StageSelectScene::*)();
	using DrawFunc_t = void(StageSelectScene::*)();

	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	// 通常時更新処理
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();

	// 選択中のステージ番号からその中心X座標を計算する
	float GetStageCenterX(int stageIndex) const;

	// プレイヤーの見た目位置を選択中ステージへ近づける
	void UpdatePlayerPosition();
};