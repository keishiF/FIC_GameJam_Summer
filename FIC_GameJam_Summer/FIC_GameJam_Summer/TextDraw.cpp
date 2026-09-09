#include "TextDraw.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <DxLib.h>

namespace
{
	// フォントサイズ・太さ(ここを書き換えれば見た目を調整できる)
	constexpr int kTitleFontSize = 72;
	constexpr int kTitleFontThick = 6;

	constexpr int kUiFontSize = 28;
	constexpr int kUiFontThick = 4;

	// [0]:Default(未使用、常に-1) [1]:Title [2]:Ui
	int s_fontHandles[3] = { -1, -1, -1 };

	int GetIndex(TextDraw::FontType type)
	{
		return static_cast<int>(type);
	}
}

namespace TextDraw
{
	void InitFonts()
	{
		s_fontHandles[GetIndex(FontType::Title)] = CreateFontToHandle("Bell MT", kTitleFontSize, kTitleFontThick, DX_FONTTYPE_ANTIALIASING);
		s_fontHandles[GetIndex(FontType::Ui)] = CreateFontToHandle("HG教科書体", kUiFontSize, kUiFontThick, DX_FONTTYPE_ANTIALIASING);

		// 生成に失敗した場合(フォント未インストールなど)は-1のままとなり、DxLib標準フォントに自動的にフォールバックする
	}

	int GetFontHandle(FontType type)
	{
		return s_fontHandles[GetIndex(type)];
	}

	int GetTextWidth(const char* text, FontType type)
	{
		int handle = GetFontHandle(type);

		if (handle != -1)
		{
			return GetDrawStringWidthToHandle(text, static_cast<int>(strlen(text)), handle);
		}

		return GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	}

	void DrawOutlinedText(int x, int y, const char* text, unsigned int color, unsigned int edgeColor, FontType type)
	{
		int handle = GetFontHandle(type);

		// 縁取り: 周囲8方向に1pxずらして縁取り色で描画してから、中央に本来の色で描画する
		constexpr int kOffsets[8][2] = {
			{-1,-1}, {0,-1}, {1,-1},
			{-1, 0},         {1, 0},
			{-1, 1}, {0, 1}, {1, 1},
		};

		for (const auto& offset : kOffsets)
		{
			if (handle != -1)
			{
				DrawStringToHandle(x + offset[0], y + offset[1], text, edgeColor, handle);
			}
			else
			{
				DrawString(x + offset[0], y + offset[1], text, edgeColor);
			}
		}

		if (handle != -1)
		{
			DrawStringToHandle(x, y, text, color, handle);
		}
		else
		{
			DrawString(x, y, text, color);
		}
	}

	void DrawFormatOutlinedText(int x, int y, unsigned int color, unsigned int edgeColor, FontType type, const char* format, ...)
	{
		char buffer[256];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		DrawOutlinedText(x, y, buffer, color, edgeColor, type);
	}

	void Terminate()
	{
		for (int& handle : s_fontHandles)
		{
			if (handle != -1)
			{
				DeleteFontToHandle(handle);
				handle = -1;
			}
		}
	}
}