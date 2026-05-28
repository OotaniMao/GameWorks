#pragma once
#include<memory>


struct MenuItem {
	const char* nomal_txt;
	const char* active_txt;
	int y_offset;
};

class IInput;
class Sound;
class Font;
class Movie;
class FpsController;

struct GameContext {
	std::shared_ptr<IInput> input;
	std::shared_ptr<Movie> movie;
	std::shared_ptr<Sound> sound;
	std::shared_ptr<Font> font;
	std::shared_ptr<FpsController>fps_controller;
};

class SceneUtility
{
public:
	static void UpdateMenuSelection(bool& isPressButton, const std::shared_ptr<IInput>key, int& selectIndex, int maxCount, std::shared_ptr<Sound>sound);
	static void DrawMenuItems(const MenuItem* items, int count, int selectedIndex, std::shared_ptr<Font> font);
	static void DrawDemoPlayMovie(std::shared_ptr<Movie>movie);
};
