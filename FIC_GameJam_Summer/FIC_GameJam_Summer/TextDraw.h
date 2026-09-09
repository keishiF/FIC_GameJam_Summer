#pragma once

// 文字描画をフォント差し替え・縁取り対応にするための共通ユーティリティ
//
// 使い方:
//   1. プログラム開始時に一度だけ TextDraw::InitFonts() を呼ぶ(Application::Init内で呼び出し済み)
//      Bell MTとHG教科書体は共にWindows標準搭載フォントのため、ファイル読み込みは不要
//   2. 文字を描画したい箇所で、DrawString/DrawFormatStringの代わりに
//      TextDraw::DrawOutlinedText / TextDraw::DrawFormatOutlinedText を使う(自動で縁取りが付く)
//   3. FontType引数で使うフォントを指定できる(省略時はDxLib標準フォント)
//
// 注意: 関数名に "DrawText" を使わないこと。WinUser.hのDrawTextマクロと衝突しリンクエラーになる。
namespace TextDraw
{
	enum class FontType
	{
		Default,	// DxLib標準フォント(指定なしの場合)
		Title,		// タイトル・見出し用(Bell MT)
		Ui,			// ゲーム内UI用(HG教科書体)
	};

	// 起動時に一度だけ呼び出し、Title/Uiフォントを作成する
	void InitFonts();

	// 指定タイプのフォントハンドルを取得する(-1ならデフォルトフォント扱い)
	int GetFontHandle(FontType type);

	// 指定フォントで文字列を描画した際の横幅を取得する(中央揃え・右揃えの計算用)
	int GetTextWidth(const char* text, FontType type = FontType::Default);

	// 縁取り付きで文字列を描画する
	void DrawOutlinedText(int x, int y, const char* text, unsigned int color, unsigned int edgeColor = 0x000000, FontType type = FontType::Default);

	// 縁取り付きでフォーマット文字列を描画する
	void DrawFormatOutlinedText(int x, int y, unsigned int color, unsigned int edgeColor, FontType type, const char* format, ...);

	// 読み込んだフォントの後始末(プログラム終了時に呼ぶ、任意)
	void Terminate();
}