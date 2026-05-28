#include"DxLib.h"
#include"Font.h"

Font::Font()
{
	Init();
}

Font::~Font()
{
	
}

void Font::Init()
{
	AddFontResourceExA("font/GAGAGAGA-FREE.otf", FR_PRIVATE, NULL);
	AddFontResourceExA("font/Font1.otf",FR_PRIVATE,NULL);
	handle[static_cast<int>(FontSize::GAGA_Size_30)] = CreateFontToHandle("GAGAGAGA FREE", 30, 3, DX_FONTTYPE_ANTIALIASING);
	handle[static_cast<int>(FontSize::GAGA_Size_150)] = CreateFontToHandle("GAGAGAGA FREE", 150, 3, DX_FONTTYPE_ANTIALIASING);
	handle[static_cast<int>(FontSize::font1_Size_30)] = CreateFontToHandle("Searider Falcon Academy Italic", 30, 3, DX_FONTTYPE_ANTIALIASING);
	handle[static_cast<int>(FontSize::font1_Size_50)] = CreateFontToHandle("Searider Falcon Academy Italic", 50, 3, DX_FONTTYPE_ANTIALIASING);
	handle[static_cast<int>(FontSize::font1_Size_80)] = CreateFontToHandle("Searider Falcon Academy Italic", 80, 3, DX_FONTTYPE_ANTIALIASING);
	handle[static_cast<int>(FontSize::font1_Size_180)] = CreateFontToHandle("Searider Falcon Academy Italic", 180, 3, DX_FONTTYPE_ANTIALIASING);

}

void Font::DrawFormatChar(int x, int y, const char* string,int fontSize, int color)
{
	DrawFormatStringToHandle(x, y, color, handle[fontSize], string);
}

void Font::DrawFormatRank(int x, int y,  char string, int fontSize, int color)
{
	DrawFormatStringToHandle(x, y, color, handle[fontSize], "%c",string);
}

void Font::DrawFormatNum(int x, int y, const float num, int fontSize, int color)
{
	DrawFormatStringToHandle(x, y, color, handle[fontSize],"%1.0f", num);
}


void Font::Delete()
{
	handle.clear();
	RemoveFontResourceExA("", FR_PRIVATE, NULL);
}
