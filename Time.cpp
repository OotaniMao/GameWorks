#include"DxLib.h"
#include"Time.h"
#include"Player.h"
#include"Font.h"
//#define DEBUG

const int MAX_TIME = 100;

namespace TimeConfig
{
	float clock_w = 30.0f;
	float clock_h = 30.0f;
	float clock_cx = 395.0f;
	float clock_cy = 45.0f;

}


Time::Time()
	:pos()
	,pos2()
	,prev_time(0)
	,start_time(0)
	,count(0)
	,max_time(MAX_TIME)
	,brink_count(0)
	,time_limit(MAX_TIME)
{
	Init();
	handle_bar = LoadGraph("graph/time_bar.png");
	handle_clock = LoadGraph("graph/time.png");
};
void Time::Init()
{
	
	pos = VGet(100.0f, 20.0f, 0.0f);
	pos2 = VGet(800.0f, 50.0f, 0.0f);
	dis = pos2.x - pos.x;
	dis = dis / max_time;
}

void Time::BlinkGraph(VECTOR pos,int graphHandle,bool transFlag)
{
	brink_count++;
	if (brink_count >= 60) {
		brink_count = 0;
	}
	if (brink_count <= 30) {
		DrawGraph(pos.x,pos.y, graphHandle, transFlag);
	}
}


void Time::Start()
{
	current_time = GetNowCount();
	start_time = GetNowCount();
	max_time = MAX_TIME;
}

void Time::Count(float &timeScale,const Player&player)
{
	if (player.getCurrentState()==State::S_SPECIAL_ATTACK) {
		start_time = GetNowCount() - current_time;
		return;
	}
	current_time = GetNowCount() - start_time;
	time_limit = max_time - ( current_time / 1000.0f);

	static int last_check_time = 0;
	int current_second = static_cast<int>(current_time / 1000.0f);
	if (current_second >= 10&&current_second%10==0&&current_second!=last_check_time) {
		timeScale += 0.001f;
		last_check_time = current_second;
	}
	if (time_limit <= 0.0f)time_limit = 0.0f;
}

void Time::Draw(Font font)
{
	pos = VGet(350.0f, 20.0f, 0.0f);
	pos2 = VGet(550.0f, 70.0f, 0.0f);
	
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
	float width = pos2.x - pos.x;
	float height = pos2.y - pos.y;
	float center_x = pos.x + width / 2.0f;
	float center_y = pos.y + height / 2.0f;

	//点滅（アルファ値）の適用
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,alpha);

	//バ－の描画
	DrawExtendGraph(center_x-(width/2.0f)*scale,center_y-(height/2.0f)*scale,center_x+(width/2.0f)*scale,center_y+(height/2.0f)*scale,handle_bar,TRUE);
	
	//設定のリセット
	SetDrawBright(255,255,255);

	//時計アイコンと数値の描画
	DrawExtendGraph(TimeConfig::clock_cx-(TimeConfig::clock_w/2.0f)*scale, TimeConfig::clock_cy-(TimeConfig::clock_h/2.0f)*scale,
		TimeConfig::clock_cx+(TimeConfig::clock_w/2.0f)*scale, TimeConfig::clock_cy+(TimeConfig::clock_h/2.0f)*scale, handle_clock, TRUE);

	float text_x = 420.0f;
	float text_y = pos.y;

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
	DrawBoxAA(pos.x, pos.y, pos2.x, pos2.y, GetColor(220, 240, 250), TRUE);
	DrawBoxAA(pos.x, pos.y, pos.x + (dis * time_limit), pos2.y, GetColor(0, 233, 149), TRUE);

	font.DrawFormatChar(400, pos.y, "ジカン", static_cast<int>(FontSize::GAGA_Size_30), GetColor(255, 255, 255));
	font.DrawFormatNum(500, pos.y, time_limit, static_cast<int>(FontSize::font1_Size_30), GetColor(255, 255, 255));

	DrawLineAA(pos.x, pos.y, pos2.x, pos.y, GetColor(137, 137, 137));
	DrawLineAA(pos.x, pos2.y, pos2.x, pos2.y, GetColor(137, 137, 137));
	DrawLineAA(pos2.x, pos2.y, pos2.x, pos.y, GetColor(137, 137, 137));
	DrawLineAA(pos.x, pos.y, pos.x, pos2.y, GetColor(137, 137, 137));

	if (pos2.y <= 0)
	{
		pos2.y = 0;
	}
}
