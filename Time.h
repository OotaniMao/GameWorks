#pragma once

const class Font;
const class Player;

class Time
{
private:
	float prev_time, start_time;
	float current_time;
	float count;
	float max_time;
	int brink_count;
	float dis;
	float time_limit;
	VECTOR pos;
	VECTOR pos2;

	int handle_bar;
	int handle_clock;
public:
	Time();

	float GetTimeCount()const { return prev_time; }
	float GetTimeLimit()const { return time_limit; }

	void Init();

	void BlinkGraph(VECTOR pos, int graphHandle, bool transFlag=true);
	void Start();
	void Count(float &timeScale, const Player& player);
	void Draw(Font font);
	void DrawTimeLimitBar(Font font);
};

