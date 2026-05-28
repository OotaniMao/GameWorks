#include"DxLib.h"
#include"R_Math.h"
#include "Gauge.h"
//#define DEBUG

Gauge::Gauge(float maxHp,float currentHp, const Square& squarePtr)
:current_num(currentHp)
,max_num(maxHp)
{
	square = squarePtr;
	graph_handle_bar = LoadGraph("graph/gauge_bar.png");
	graph_handle_frame = LoadGraph("graph/gauge_frame.png");
}

Gauge::~Gauge()
{
}

void Gauge::Init(float maxHp)
{
	current_num = maxHp;
}

void Gauge::DrawGauge_3d(const VECTOR& pos,const int& red , const int& green, const int& blue )
{
	SetDrawBright(169, 171, 169);
	DrawModiBillboard3D(pos,
		square.pos.x, square.pos.y,										//左上の座標
		square.pos.x + square.size.x, square.pos.y,						//右上の座標
		square.pos.x + square.size.x, square.pos.y + square.size.y,		//右下の座標
		square.pos.x, square.pos.y + square.size.y,						//左下の座標
		graph_handle_bar, TRUE);
	//現在の右端座標
	float current_right_x = CalculateRightPosition(square.pos.x,square.size.x);

	SetDrawBright(red,green,blue);
	//画像の描画(ゲージ)
	DrawModiBillboard3D(pos,
		square.pos.x,					square.pos.y,						//左上の座標
		current_right_x,				square.pos.y,						//右上の座標
		current_right_x,				square.pos.y+ square.size.y ,		//右下の座標
		square.pos.x,					square.pos.y+ square.size.y ,		//左下の座標
		graph_handle_bar, TRUE);
	SetDrawBright(255, 255, 255);
	//画像の描画(外側)
	DrawModiBillboard3D(pos,
		square.pos.x,					square.pos.y,						//左上の座標
		square.pos.x+ square.size.x,	square.pos.y,						//右上の座標
		square.pos.x+ square.size.x,	square.pos.y + square.size.y,		//右下の座標
		square.pos.x,					square.pos.y + square.size.y,		//左下の座標
		graph_handle_frame, TRUE);
}

void Gauge::DrawGauge_2d(const int& red, const int& green, const int& blue)
{
	SetDrawBright(169, 171, 169);
	DrawExtendGraph((int)square.pos.x, (int)square.pos.y, (int)(square.pos.x + square.size.x), (int)(square.pos.y + square.size.y), graph_handle_bar, TRUE);
	//現在の右端座標
	float current_right_x =CalculateRightPosition(square.pos.x, square.size.x);
	//中身の輝度を設定
	SetDrawBright(red, green, blue);
	DrawExtendGraph((int)square.pos.x, (int)square.pos.y, (int)current_right_x, (int)(square.pos.y + square.size.y), graph_handle_bar, TRUE);
	//輝度を戻す
	SetDrawBright(255, 255, 255);
	//外枠の描画
	DrawExtendGraph((int)square.pos.x, (int)square.pos.y, (int)(square.pos.x + square.size.x), (int)(square.pos.y + square.size.y), graph_handle_frame, TRUE);
}

float Gauge::CalculateRate()
{
	float ratio = current_num / max_num;
	if (ratio > 1.0f) ratio = 1.0f;
	return ratio;
}

float Gauge::CalculateRightPosition(const float& referencePointX,const float& width )
{
	return referencePointX + (width* CalculateRate());
}

void Gauge::SubNum(float num)
{
	current_num -= num;
}

void Gauge::Update(float chargeSpeed)
{	
	//自動チャージ処理 (charge_speed が 0 より大きい場合のみ)
	if (chargeSpeed > 0.0f) {
		current_num += chargeSpeed;
		if (current_num > max_num) current_num = max_num;
	}
	DrawDebug();
}

void Gauge::DrawDebug()
{
#ifdef DEBUG
	printfDx("Y(%0.1f,%0.1f)\n", pos.y + gauge_pos.y, gauge_pos.y);
	printfDx("SpecialGauge Ratio: %0.2f\n", ratio);
	printfDx("hp:%0.1f\n", current_num);
	//デバッグ用　Zキーで体力を減らす
	if (CheckHitKey(KEY_INPUT_Z)) {
		current_num -= 0.5f;
	}
	//デバッグ用　Xキーで体力を回復
	if (CheckHitKey(KEY_INPUT_X)) {
		current_num += 0.5f;
		if (current_num > max_num)current_num = max_num;
	}
#endif // DEBUG
}