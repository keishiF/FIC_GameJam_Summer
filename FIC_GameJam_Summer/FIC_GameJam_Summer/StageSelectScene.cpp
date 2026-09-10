#include "game.h"
#include "GameScene.h"
#include "Input.h"
#include "SceneController.h"
#include "StageSelectScene.h"
#include "TextDraw.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;
	constexpr int kStageNum = 3;	// ステージ4を削除したため4→3に変更
	constexpr int kStageBoxSize = 100;
	constexpr int kStageBoxMargin = 60;
	constexpr int kCursorColor = 0xff0000;

	constexpr int kPlayerSize = 40;
	constexpr float kMoveLerpRate = 0.3f;
	constexpr float kPlayerAboveGap = 70.0f;	// ステージボックスの上端からどれだけ離して自機を飛ばすか

	// タイトル文字の表示位置(ここを書き換えれば配置を調整できる)
	constexpr int kTitleY = 60;

	// 操作方法ヒント
	constexpr int kHintIconSize = 50;		// アイコンの表示サイズ
	constexpr int kHintIconGap = 12;		// アイコンと文字の間隔
	constexpr int kHintGroupGap = 50;		// 「移動」と「決定」の間隔
	constexpr int kHintMarginBottom = 40;	// 画面下端からの余白

	// ステージアイコンの画像パス(ステージごとの見た目を表す画像。差し替え可能)
	const char* const kStageIconPaths[kStageNum] =
	{
		"Data/StageIcon1.png",
		"Data/StageIcon2.png",
		"Data/StageIcon3.png",
	};
}

StageSelectScene::StageSelectScene(SceneController& controller) :
	SceneBase(controller),
	m_bgHandle(-1),
	m_playerHandle(-1),
	m_stickIconHandle(-1),
	m_aButtonIconHandle(-1),
	m_playerX(0.0f),
	m_playerY((Game::kScreenHeight - kStageBoxSize) / 2.0f - kPlayerAboveGap),
	m_selectStage(0),
	m_facingRight(true),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&StageSelectScene::FadeInUpdate),
	m_draw(&StageSelectScene::FadeDraw)
{
	m_bgHandle = LoadGraph("Data/StageTitlebg.png");
	assert(m_bgHandle > 0);

	m_playerHandle = LoadGraph("Data/Player.png");
	assert(m_playerHandle > 0);

	// 操作方法ヒント用アイコン(左スティック/Aボタン)
	m_stickIconHandle = LoadGraph("Data/Icon_MoveStick.png");
	assert(m_stickIconHandle > 0);

	m_aButtonIconHandle = LoadGraph("Data/Icon_ButtonA.png");
	assert(m_aButtonIconHandle > 0);

	// 各ステージの表示アイコン(1〜3の数字の代わりに表示する画像)
	m_stageIconHandle.resize(kStageNum);
	for (int i = 0; i < kStageNum; ++i)
	{
		m_stageIconHandle[i] = LoadGraph(kStageIconPaths[i]);
		assert(m_stageIconHandle[i] > 0);
	}

	m_playerX = GetStageCenterX(m_selectStage);
}

StageSelectScene::~StageSelectScene()
{}

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
		// 選択したステージ番号(1始まり)を渡してGameSceneへ遷移
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller, m_selectStage + 1));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

float StageSelectScene::GetStageCenterX(int stageIndex) const
{
	int totalWidth = kStageBoxSize * kStageNum + kStageBoxMargin * (kStageNum - 1);
	int startX = (Game::kScreenWidth - totalWidth) / 2;
	int boxX = startX + stageIndex * (kStageBoxSize + kStageBoxMargin);

	return static_cast<float>(boxX + kStageBoxSize / 2);
}

void StageSelectScene::UpdatePlayerPosition()
{
	float targetX = GetStageCenterX(m_selectStage);

	// 目標位置へ少しずつ近づける(線形補間)
	m_playerX += (targetX - m_playerX) * kMoveLerpRate;

	// ほぼ到達したらぴったり合わせる(誤差の蓄積防止)
	if (std::abs(targetX - m_playerX) < 0.5f)
	{
		m_playerX = targetX;
	}
}

void StageSelectScene::NormalDraw()
{
	DrawGraph(0, 0, m_bgHandle, true);

	// 中央上に「STAGE SELECT」をTitleフォントで表示
	const char* titleText = "STAGE SELECT";
	int titleWidth = TextDraw::GetTextWidth(titleText, TextDraw::FontType::Title);
	TextDraw::DrawOutlinedText((Game::kScreenWidth - titleWidth) / 2, kTitleY, titleText, 0xffffff, 0x000000, TextDraw::FontType::Title);

	int totalWidth = kStageBoxSize * kStageNum + kStageBoxMargin * (kStageNum - 1);
	int startX = (Game::kScreenWidth - totalWidth) / 2;
	int y = (Game::kScreenHeight - kStageBoxSize) / 2;

	for (int i = 0; i < kStageNum; ++i)
	{
		int x = startX + i * (kStageBoxSize + kStageBoxMargin);

		//DrawBox(x, y, x + kStageBoxSize, y + kStageBoxSize, 0xffffff, false);

		// 番号の代わりにステージアイコン画像をボックス中央に表示
		int iconW = 0;
		int iconH = 0;
		GetGraphSize(m_stageIconHandle[i], &iconW, &iconH);
		int iconX = x + (kStageBoxSize - iconW) / 2;
		int iconY = y + (kStageBoxSize - iconH) / 2;
		DrawGraph(iconX, iconY, m_stageIconHandle[i], true);

		// 選択中のステージを強調表示
		if (i == m_selectStage && (m_blinkFrame / 10) % 2 == 0)
		{
			//DrawBox(x - 5, y - 5, x + kStageBoxSize + 5, y + kStageBoxSize + 5, kCursorColor, false);
		}
	}

	// プレイヤーキャラクター描画(中心座標基準)。ステージボックスの上を横移動する。
	int drawX = static_cast<int>(m_playerX - kPlayerSize / 2.0f);
	int drawY = static_cast<int>(m_playerY - kPlayerSize / 2.0f);

	if (m_facingRight)
	{
		DrawGraph(drawX, drawY, m_playerHandle, true);
	}
	else
	{
		// 左右反転して描画
		DrawTurnGraph(drawX, drawY, m_playerHandle, true);
	}

	// 操作方法ヒント(左下:移動、右下:決定)
	DrawControlHints();

#ifdef _DEBUG
	//DrawString(0, 0, "Stage Select Scene", 0xffffff);
#endif
}

void StageSelectScene::DrawControlHints()
{
	// ============================================================
	// 操作方法ヒント
	//
	//        [スティック] 移動     [A] 決定
	//
	// 画面中央下にまとめて表示
	// ============================================================

	constexpr int iconSize = kHintIconSize;

	const char* moveText = "移動";
	const char* okText = "決定";

	// ------------------------------------------------------------
	// 文字の幅を取得
	// ------------------------------------------------------------

	int moveTextWidth =
		GetDrawStringWidth(moveText, static_cast<int>(strlen(moveText)));

	int okTextWidth =
		GetDrawStringWidth(okText, static_cast<int>(strlen(okText)));

	// ------------------------------------------------------------
	// 各グループの幅
	//
	// [画像] + 隙間 + [文字]
	// ------------------------------------------------------------

	int moveGroupWidth =
		iconSize + kHintIconGap + moveTextWidth;

	int okGroupWidth =
		iconSize + kHintIconGap + okTextWidth;

	// ------------------------------------------------------------
	// 全体の幅
	// ------------------------------------------------------------

	int totalWidth =
		moveGroupWidth +
		kHintGroupGap +
		okGroupWidth;

	// 画面中央に配置するための開始X座標
	int startX =
		(Game::kScreenWidth - totalWidth) / 2;

	// ------------------------------------------------------------
	// Y座標
	// ------------------------------------------------------------

	int hintY =
		Game::kScreenHeight -
		kHintMarginBottom -
		iconSize;

	// ============================================================
	// 左側
	// [左スティック] 移動
	// ============================================================

	int stickX = startX;

	// 1254x1254の画像を50x50に縮小して描画
	DrawExtendGraph(
		stickX,
		hintY,
		stickX + iconSize,
		hintY + iconSize,
		m_stickIconHandle,
		true
	);

	// 「移動」
	int moveTextX =
		stickX +
		iconSize +
		kHintIconGap;

	int moveTextY =
		hintY +
		(iconSize - 16) / 2;

	DrawFormatString(
		moveTextX,
		moveTextY,
		0xffffff,
		"%s",
		moveText
	);

	// ============================================================
	// 右側
	// [Aボタン] 決定
	// ============================================================

	int okGroupX =
		startX +
		moveGroupWidth +
		kHintGroupGap;

	int aX = okGroupX;

	// Aボタン画像も50x50に統一
	DrawExtendGraph(
		aX,
		hintY,
		aX + iconSize,
		hintY + iconSize,
		m_aButtonIconHandle,
		true
	);

	// 「決定」
	int okTextX =
		aX +
		iconSize +
		kHintIconGap;

	int okTextY =
		hintY +
		(iconSize - 16) / 2;

	DrawFormatString(
		okTextX,
		okTextY,
		0xffffff,
		"%s",
		okText
	);
}

void StageSelectScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}