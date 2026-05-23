#pragma once

const class Font;
const class Player;

class Time
{
private:
	float prevTime, startTime;
	float currentTime;
	float count;
	float maxTime;
	int BrinkCount;
	float dis;
	float time_limit;
	VECTOR Pos;
	VECTOR Pos2;

	int handle_bar;
	int handle_clock;
public:
	Time();

	float GetTimeCount()const { return prevTime; }
	float GetTimeLimit()const { return time_limit; }

	void Init();

	void BlinkGraph();
	void Start();
	void Count(float &timeScale, const Player& player);
	void Draw(Font font);
	void DrawTimeLimitBar(Font font);
};

