#include "GameMain.h"
#include"FpsController.h"
#include"Input.h"
#include"Font.h"
#include"Sound.h"
#include"Movie.h"
#include"SceneUtility.h"
#include"TitleScene.h"
#include"OptionScene.h"
#include"PlayScene.h"
#include"EndScene.h"

GameMain::GameMain()
{
	context = std::make_shared<GameContext>();
	context->sound = std::make_shared<Sound>();
	context->input = std::make_shared<Input>();
	context->font = std::make_shared<Font>();
	context->movie = std::make_shared<Movie>();
	context->fps_controller = std::make_shared<FpsController>();
	time_scale = 1.0f;
	current_scene = std::make_unique<TitleScene>(context,time_scale);
}
GameMain::~GameMain()
{
}
void GameMain::Init()
{
	if (current_scene)current_scene->Init();
}

void GameMain::Run()
{
	if (!current_scene)return;
	context->input->Update();

	current_scene->Update();
	current_scene->Draw();

	std::unique_ptr<ISceneBase>next_scene = current_scene->getNextScene();

	if (next_scene != nullptr) {
		current_scene = std::move(next_scene);
		current_scene->Init();
	}
	context->fps_controller->Update();
	context->fps_controller->DrawDebug();
}

void GameMain::Terminal()
{
}
