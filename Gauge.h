#pragma once
#include"R_Math.h"

class Gauge
{
private:
	float max_num;			//最大数
	float current_num;		//現在の値
	int graph_handle_frame;		//ゲージの画像ハンドル
	int graph_handle_bar;		//ゲージの土台画像ハンドル
	Square square;

public:
	Gauge(float max_hp, float current_hp, const Square& square_ptr);
	~Gauge();

	float getCurrentNum()const { return current_num; }
	bool getIsMaxNum()const { return max_num==current_num?true:false; }

	/// @brief 初期化
	void Init(float max_hp);

	void DrawGauge_3d(const VECTOR& pos, const int& red = 255, const int& green = 255, const int& blue = 255);
	void DrawGauge_2d(const int& red = 255, const int& green = 255, const int& blue = 255);
	/// @brief 値を減らす
	/// @param  num　減らす数
	void SubNum(float num);
	void Update(float charge_speed=0.0f);
private:
	float CalculateRate();
	float CalculateRightPosition(const float& referencePointX, const float& width);
	void DrawDebug();
};
