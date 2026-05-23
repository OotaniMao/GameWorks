#pragma once

class FpsController
{
private:


	const int TARGET_FPS = 60;  // 固定するFPS（ここでは60FPSに設定）
	const int FRAME_TIME = 1000 / TARGET_FPS;  // 1フレームの時間（ミリ秒）
	
	double current_time;
	double previous_time;
	double fps_check_time;
	float fps_counter;

	double delta_time;
	float fps;
public:
	FpsController();
	~FpsController();

	double getDeltaTime()const { return delta_time; }
	float getFps()const { return fps; }

	void DrawDebug();
	void Update();


};
