#pragma once
#include "SceneBase.h"
#include <vector>

class StageSelectScene final : public SceneBase
{
public:
	explicit StageSelectScene(SceneController& controller);
	~StageSelectScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_bgHandle;
	int m_playerHandle;

	int m_stickIconHandle;       // 「移動」ヒント用アイコン(左スティック)
	int m_aButtonIconHandle;     // 「決定」ヒント用アイコン(Aボタン)
	std::vector<int> m_stageIconHandle;  // 各ステージのアイコン画像(要素数 = ステージ数)

	float m_playerX;	// 見た目の現在位置
	float m_playerY;

	int m_selectStage;	// 選択中のステージ番号
	bool m_facingRight;	// プレイヤーの向き

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

	void DrawControlHints();
};