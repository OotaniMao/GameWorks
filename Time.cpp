#include"DxLib.h"
#include"Time.h"
#include"Player.h"
#include"Font.h"
//#define DEBUG

//フォントのハンドル
//フォント名　サイズ　太さ　フォントタイプ
//int fontHandle = CreateFontToHandle("Bernard MT Condensed", 50, 2, DX_FONTTYPE_ANTIALIASING_16X16);

const int MAX_TIME = 100;

Time::Time()
	:Pos()
	,Pos2()
	,prevTime(0)
	,startTime(0)
	,count(0)
	,maxTime(MAX_TIME)
	,BrinkCount(0)
	,time_limit(MAX_TIME)
{
	Init();
	handle_bar = LoadGraph("graph/time_bar.png");
	handle_clock = LoadGraph("graph/time.png");
};
void Time::Init()
{
	
	Pos = VGet(100.0f, 20.0f, 0.0f);
	Pos2 = VGet(800.0f, 50.0f, 0.0f);
	dis = Pos2.x - Pos.x;
	dis = dis / maxTime;
}

void Time::BlinkGraph()
{
	BrinkCount++;
	if (BrinkCount >= 60) {
		BrinkCount = 0;
	}
	if (BrinkCount <= 30) {
		//DrawGraph();
	}
}


void Time::Start()
{
	currentTime = GetNowCount();
	startTime = GetNowCount();
	maxTime = MAX_TIME;
}

void Time::Count(float &timeScale,const Player&player)
{
	if (player.getCurrentState()==State::S_SPECIAL_ATTACK) {
		startTime = GetNowCount() - currentTime;
		return;
	}
	currentTime = GetNowCount() - startTime;
	time_limit = maxTime - ( currentTime / 1000.0f);

	static int last_check_time = 0;
	int current_second = static_cast<int>(currentTime / 1000.0f);
	if (current_second >= 10&&current_second%10==0&&current_second!=last_check_time) {
		timeScale += 0.001f;
		last_check_time = current_second;
		//currentTime = GetNowCount();
	}
	if (time_limit <= 0.0f)time_limit = 0.0f;
}

void Time::Draw(Font font)
{
	Pos = VGet(350.0f, 20.0f, 0.0f);
	Pos2 = VGet(550.0f, 70.0f, 0.0f);
	
	float scale = 1.0f;//デフォルトの拡大率
	int alpha = 255;//デフォルトの透明度

	//残り5秒以下の時だけアニメーション計算を実行
	if (time_limit <= 5.0f) {
		//1秒周期(2π)のサイン波を生成
		float angle = 2.0f * DX_PI_F * (GetNowCount() % 1000 / 1000.0f);
		float sin_value = sinf(angle);

		//拡大率を1.0～1.1の間で変化させる
		scale = 1.0f + (sin_value + 1.0f) * 0.15f;
		//不透明度を150～255の間で変化させる
		alpha = static_cast<int>(202+(sin_value*53));

		SetDrawBright(234, 85, 73);//赤系に設定
	}
	else {
		SetDrawBright(0, 0, 0);//5秒より多いときは黒系
	}

	//中心座標を基準に拡大縮小描画を行うための計算
	float width = Pos2.x - Pos.x;
	float height = Pos2.y - Pos.y;
	float center_x = Pos.x + width / 2.0f;
	float center_y = Pos.y + height / 2.0f;

	//点滅（アルファ値）の適用
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,alpha);

	//バ－の描画
	DrawExtendGraph(center_x-(width/2.0f)*scale,center_y-(height/2.0f)*scale,center_x+(width/2.0f)*scale,center_y+(height/2.0f)*scale,handle_bar,TRUE);
	
	//設定のリセット
	SetDrawBright(255,255,255);

	float clock_w = 30.0f;
	float clock_h = 30.0f;
	float clock_cx = 395.0f;
	float clock_cy = 45.0f;

	//時計アイコンと数値の描画
	DrawExtendGraph(clock_cx-(clock_w/2.0f)*scale, clock_cy-(clock_h/2.0f)*scale, clock_cx+(clock_w/2.0f)*scale, clock_cy+(clock_h/2.0f)*scale, handle_clock, TRUE);

	float text_x = 420.0f;
	float text_y = Pos.y;

	float offset_x = (text_x - center_x) * (scale - 1.0f);
	float offset_y = (text_y - center_y) * (scale - 1.0f);

	font.DrawFormatNum(static_cast<int>(text_x+offset_x),static_cast<int>(text_y+offset_y), time_limit, static_cast<int>(FontSize::font1_Size_50), GetColor(255, 255, 255));
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


#ifdef DEBUG

	printfDx("timeLimit:%0.1f\n", time_limit);
	printfDx("prevTime:%0.2f\n",prevTime);
#endif // DEBUG

}

void Time::DrawTimeLimitBar(Font font)
{
	DrawBoxAA(Pos.x, Pos.y, Pos2.x, Pos2.y, GetColor(220, 240, 250), TRUE);
	DrawBoxAA(Pos.x, Pos.y, Pos.x + (dis * time_limit), Pos2.y, GetColor(0, 233, 149), TRUE);

	font.DrawFormatChar(400, Pos.y, "ジカン", static_cast<int>(FontSize::GAGA_Size_30), GetColor(255, 255, 255));
	font.DrawFormatNum(500, Pos.y, time_limit, static_cast<int>(FontSize::font1_Size_30), GetColor(255, 255, 255));

	DrawLineAA(Pos.x, Pos.y, Pos2.x, Pos.y, GetColor(137, 137, 137));
	DrawLineAA(Pos.x, Pos2.y, Pos2.x, Pos2.y, GetColor(137, 137, 137));
	DrawLineAA(Pos2.x, Pos2.y, Pos2.x, Pos.y, GetColor(137, 137, 137));
	DrawLineAA(Pos.x, Pos.y, Pos.x, Pos2.y, GetColor(137, 137, 137));

	if (Pos2.y <= 0)
	{
		Pos2.y = 0;
	}
}
