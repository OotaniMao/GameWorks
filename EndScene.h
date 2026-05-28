#pragma once
#include<memory>
#include"SceneBase.h"
#include"SceneUtility.h"


enum class End_Menu :int {
	END_REPLAY,
	END_TITLE,
	END_MAX
};


const MenuItem EndMenuItems[] = {
	{"リプレイ","リプレイイイ",0},
	{"タイトル","タイトルルル",40}
};

const int EndMenuCount = sizeof(EndMenuItems) / sizeof(EndMenuItems[0]);

class IInput;
class Movie;
class Sound;
class Goal;
class Font;

class EndScene:public ISceneBase
{
public:
	EndScene(std::shared_ptr<GameContext>context,const float& timeScale);
	~EndScene();

	void Init()override;
	void Update()override;
	void Draw()override;

	std::unique_ptr<ISceneBase>getNextScene()override;
private:
	std::shared_ptr<GameContext>ctx;
	std::shared_ptr<Goal>goal;

	GameScene select_scene;
	bool is_press_button;
	int select_end_menu;
	float stickY;
	float time_scale;
};

