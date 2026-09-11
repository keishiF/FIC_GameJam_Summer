#include "Application.h"
#include "game.h"
#include "Input.h"
#include "SceneController.h"
#include "TextDraw.h"
#include "AudioManager.h"
#include <cassert>
#include <DxLib.h>

Application& Application::GetInstance()
{
	// TODO: return ステートメントをここに挿入します
	static Application app;
	return app;
}

bool Application::Init()
{
	// フルスクリーンではなく、ウインドウモードで開くようにする
	ChangeWindowMode(Game::kDefaultWindowMode);
	// ウィンドウ名設定
	SetMainWindowText(Game::kTitleText);
	// 画面のサイズを変更する
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// タイトル・UI用フォントを準備する
	TextDraw::InitFonts();

	// 乱数の種を現在時刻で初期化する(これが無いと毎回同じ乱数列になる)
	SRand(static_cast<int>(GetNowCount()));

	// オーディオを初期化
	AudioManager::GetInstance().Init();

	return true;
}

void Application::Run()
{
	m_frame = 0;

	// メモリの確保
	SceneController sceneController;

	// ゲームループ
	while (ProcessMessage() == 0)	// Windowsが行う処理を待つ必要がある
	{
		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		// 画面全体をクリアする
		ClearDrawScreen();

		// ここにゲームの処理を書く
		// パッドの入力更新処理
		Input::GetInstance().Update();

		sceneController.Update();
		sceneController.Draw();

		// 画面の切り替わりを待つ必要がある
		ScreenFlip();	// 1/60秒経過するまで待つ
		++m_frame;

		// FPS(Frame Per Second)60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}

		// もしＥＳＣキーが押されていたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
	}
}

void Application::Terminate()
{
	TextDraw::Terminate();
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}