#pragma once
#include<memory>
#include"SceneBase.h"

class Sound;
class IInput;
class Font;
class Movie;
class FpsController;
struct GameContext;int;

class GameMain
{
public:
	GameMain();
	~GameMain();

	void Init();
	void Run();
	void Terminal();

private:
	std::unique_ptr<ISceneBase>current_scene;

	std::shared_ptr<GameContext>context;
	
	float time_scale;
};
