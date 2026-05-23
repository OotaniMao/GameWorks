#include"DxLib.h"
#include "TitleScene.h"
#include"PlayScene.h"
#include"OptionScene.h"
#include"Config.h"
#include"SceneUtility.h"
#include"Input.h"
#include"Sound.h"
#include"Font.h"
#include"Movie.h"

TitleScene::TitleScene(std::shared_ptr<GameContext> context,const float& timeScale)
{
	time_scale = timeScale;
	ctx = context;
	select_menu = static_cast<unsigned int>(TitleMenu::MENU_START);
	is_press_button = false;
	//stickY = ctx->input->getLeftStick().y;
	select_scene = GameScene::TITLE;
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	select_menu = static_cast<unsigned int>(TitleMenu::MENU_START);
	is_press_button = false;
	select_scene = GameScene::TITLE;
}

void TitleScene::Update()
{
	stickY = ctx->input->getLeftStick().y;
#ifdef DEBUG
	printfDx("stickY:%f\n", stickY);

#endif // DEBUG
	SceneUtility::UpdateMenuSelection(is_press_button, ctx->input, select_menu, sizeof(TitleMenuItems) / sizeof(MenuItem), ctx->sound);

	if (ctx->input->IsTrigger(Command::RETURN) || ctx->input->IsTrigger(Command::BtnB)) {
		ctx->sound->PlaySE(SoundEffect::DECIDE);
		TitleMenu selected = static_cast<TitleMenu>(select_menu);

		if (selected == TitleMenu::MENU_START) {
			select_scene = GameScene::PLAY;
		}
		else if (selected == TitleMenu::MENU_OPTION) {
			select_scene = GameScene::OPTION;
		}
	}
}

void TitleScene::Draw()
{
	SceneUtility::DrawDemoPlayMovie(ctx->movie);
	ctx->font->DrawFormatChar(100, 200, "Street Graffiti\n", static_cast<int>(FontSize::font1_Size_80), Config::ColorWhite);
	SceneUtility::DrawMenuItems(TitleMenuItems, TitleMenuCount, select_menu, ctx->font);
}

std::unique_ptr<ISceneBase> TitleScene::GetNextScene()
{
	if(select_scene==GameScene::PLAY)return std::make_unique<PlayScene>(ctx,time_scale);
	if (select_scene == GameScene::OPTION)return std::make_unique<OptionScene>(ctx,time_scale);
	return nullptr;
}

