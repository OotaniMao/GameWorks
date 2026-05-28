#pragma once
#include<memory>
#include"SceneBase.h"
#include"SceneUtility.h"

enum class OptionMenu :int {
	BGM,
	SE,
	BUCK,
	MAX
};

const MenuItem OptionMenuItems[] = {
	{"BGM:","BGM:",0},
	{"SE:","SE:",40},
	{"ƒ‚ƒhƒ‹","ƒ‚ƒhƒ‹ƒ‹ƒ‹",150}
};
const int OptionMenuCount = sizeof(OptionMenuItems) / sizeof(OptionMenuItems[0]);

enum class MoveDirection :int;
class Movie;
class Font;
class IInput;
class Sound;

class OptionScene:public ISceneBase
{
public:
	OptionScene(std::shared_ptr<GameContext> context,const float& timeScale);
	~OptionScene();

	void Init()override;
	void Update()override;
	void Draw()override;

	std::unique_ptr<ISceneBase>getNextScene()override;
private:
	int select_option_menu;
	MoveDirection dir;
	GameScene select_scene;
	bool is_press_button;
	std::shared_ptr<GameContext>ctx;
	float time_scale;
	
};
