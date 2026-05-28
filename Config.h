#pragma once

namespace Config {
	constexpr int SCREEN_WIDTH = 960;
	constexpr int SCREEN_HEIGHT = 540;
	constexpr int COLOR_BIT = 16;

	constexpr float STICK_THRESHOLD = 0.5f;

	const unsigned int COLOR_WHITE = 0xFFFFFF;
	const unsigned int COLOR_ORANGE = 0xED6D35;
	const unsigned int COLOR_BLACK = 0x000000;
	const unsigned int COLOR_RUN_MESSAGE = 0xEA5549;

	constexpr int MENU_LEFT_X = 200;
	constexpr int TITLE_TEXT_Y = 400;
	constexpr int MENU_ITEM_STEP = 30;

	constexpr float MAX_SPECIAL_GAUGE = 15.0f;
	constexpr float GAUGE_INCREASE_RATE = MAX_SPECIAL_GAUGE / (60.0f * 15.0f);
	constexpr int   FADE_OUT_SPEED = 2;

	static constexpr float GRAVITY = 5.0f;
}
