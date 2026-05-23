#pragma once

namespace Config {
	constexpr int ScreenWidth = 960;
	constexpr int ScreenHeight = 540;
	constexpr int ColorBit = 16;

	constexpr float StickThreshold = 0.5f;

	const unsigned int ColorWhite = 0xFFFFFF;
	const unsigned int ColorOrange = 0xED6D35;
	const unsigned int ColorBlack = 0x000000;
	const unsigned int ColorRunMsg = 0xEA5549;

	constexpr int MenuLeftX = 200;
	constexpr int TitleTextY = 400;
	constexpr int MenuItemStep = 30;

	constexpr float MaxSpecialGauge = 15.0f;
	constexpr float GaugeIncreaseRate = MaxSpecialGauge / (60.0f * 15.0f);
	constexpr int   FadeOutSpeed = 2;

	static constexpr float GRAVITY = 5.0f;
}
