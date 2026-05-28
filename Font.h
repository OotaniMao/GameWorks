#pragma once
#include<map>

enum class FontSize :int{
	GAGA_Size_30,
	GAGA_Size_150,
	font1_Size_30,
	font1_Size_50,
	font1_Size_80,
	font1_Size_180
};

class Font
{
private:
	std::map<int, int>handle;
public:
	Font();
	~Font();

	void Init();
	void DrawFormatChar(int x, int y, const char*string, int fontSize, int color);
	void DrawFormatRank(int x, int y, char string, int fontSize, int color);
	void DrawFormatNum(int x, int y, const float num, int fontSize, int color);
	void Delete();
};

