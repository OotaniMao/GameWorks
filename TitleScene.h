#pragma once
#include"SceneBase.h"
#include"SceneUtility.h"
#include<memory>

enum class TitleMenu :int {
	MENU_START,
	MENU_OPTION,
	//MENU_EXIT,
	MENU_MAX
};

const MenuItem TitleMenuItems[] = {
	{"ゲームスタート","ゲームスタートトト",0},
	{"オプション","オプションンン",40},
	/*{"ヤメル","ヤメルルル",80}*/
};

const int TitleMenuCount = sizeof(TitleMenuItems) / sizeof(TitleMenuItems[0]);

class Movie;
class IInput;
class Sound;
class Font;

class TitleScene:public ISceneBase
{
public:
	TitleScene(std::shared_ptr<GameContext> context, const float& timeScale);
	~TitleScene();

	void Init()override;
	void Update()override;
	void Draw()override;

	std::unique_ptr<ISceneBase>GetNextScene()override;
private:
	int select_menu;
	bool is_press_button;
	float stickY;
	std::shared_ptr<GameContext>ctx;
	
	GameScene select_scene;
	float time_scale;
};
