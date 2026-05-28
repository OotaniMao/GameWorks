#include"DxLib.h"
#include "FpsController.h"
//#define DEBUG

FpsController::FpsController()
	:current_time(0.0f),
	fps_counter(0.0f),
	delta_time(0.000001f),//É}ÉCÉNÉçïb
	fps(0.0f),
	previous_time(static_cast<double>(GetNowHiPerformanceCount())),
	fps_check_time(static_cast<double>(GetNowHiPerformanceCount()))
{
}

FpsController::~FpsController()
{
}

void FpsController::DrawDebug()
{
#ifdef DEBUG
	printfDx("fps:%0.2f\n",fps);
	//printfDx("deltaTime:%0.2f\n", delta_time);

#endif // DEBUG

}

void FpsController::Update()
{
	
	current_time = static_cast<double>(GetNowHiPerformanceCount());
	delta_time = (current_time - previous_time)/1000000.0f;//ïb
	previous_time = current_time;//éûä‘ï€ë∂
	
	if (delta_time > 0.03f)delta_time = 0.03f;

	//fpsä÷åWÇÃèàóù
	fps_counter++;
	if (current_time - fps_check_time > 1000000.0f) {
		fps = fps_counter;
		fps_counter = 0;
		fps_check_time = current_time;
	}
	//fpså≈íË
	const double frame_time_ms = FRAME_TIME;
	double process_time_ms = (GetNowHiPerformanceCount()-current_time)/1000.0f;
	if (process_time_ms < frame_time_ms) {
		WaitTimer(static_cast<int>(frame_time_ms-process_time_ms));
	}
}
