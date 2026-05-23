#include"DxLib.h"
#include "EndScene.h"
#include"Config.h"
#include"SceneUtility.h"
#include"Input.h"
#include"Sound.h"
#include"Font.h"
#include"Movie.h"
#include"Goal.h"
#include"TitleScene.h"
#include"PlayScene.h"

EndScene::EndScene(std::shared_ptr<GameContext> context,const float&timeScale)
{
	ctx = context;
	time_scale = timeScale;
	select_scene = GameScene::END;
	is_press_button = false;
	select_end_menu = static_cast<unsigned int>(End_Menu::END_TITLE);
	stickY = ctx->input->getLeftStick().y;
	goal = std::make_shared<Goal>();
}

EndScene::~EndScene()
{
}

void EndScene::Init()
{
	select_scene = GameScene::END;
	is_press_button = false;
	select_end_menu = static_cast<unsigned int>(End_Menu::END_TITLE);
}

void EndScene::Update()
{
	ctx->input->SetEnabled(true);

	stickY = ctx->input->getLeftStick().y;
	SceneUtility::UpdateMenuSelection(is_press_button, ctx->input, select_end_menu, sizeof(EndMenuItems) / sizeof(MenuItem), ctx->sound);

	if (ctx->input->IsTrigger(Command::RETURN) || ctx->input->IsTrigger(Command::BtnB)) {
		ctx->sound->PlaySE(SoundEffect::DECIDE);
		End_Menu selected = static_cast<End_Menu>(select_end_menu);
		if (selected == End_Menu::END_TITLE) {
			select_scene = GameScene::TITLE;
		}
		else if (selected == End_Menu::END_REPLAY) {
			select_scene = GameScene::PLAY;
		}
	}
}

void EndScene::Draw()
{
	SceneUtility::DrawDemoPlayMovie(ctx->movie);
	goal->getIsGoal() ?
		ctx->font->DrawFormatChar(200, 200, "Game Clear\n", static_cast<int>(FontSize::font1_Size_80), GetColor(255, 127, 80)) :
		ctx->font->DrawFormatChar(200, 200, "Game Over\n", static_cast<int>(FontSize::font1_Size_80), GetColor(0, 191, 255));
	SceneUtility::DrawMenuItems(EndMenuItems, EndMenuCount, select_end_menu, ctx->font);
}

std::unique_ptr<ISceneBase> EndScene::GetNextScene()
{
	if(select_scene==GameScene::TITLE)return std::make_unique<TitleScene>(ctx,time_scale);
	if (select_scene == GameScene::PLAY)return std::make_unique<PlayScene>(ctx,time_scale);
	return nullptr;
	
}
