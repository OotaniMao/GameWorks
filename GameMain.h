#pragma once
#include<memory>
#include"SceneBase.h"

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
