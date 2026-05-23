#include"DxLib.h"
#include "OptionScene.h"
#include"Config.h"
#include"SceneUtility.h"
#include"Input.h"
#include"Sound.h"
#include"Font.h"
#include"Movie.h"
#include"TitleScene.h"

OptionScene::OptionScene(std::shared_ptr<GameContext> context,const float& timeScale)
{
	ctx = context;
	time_scale = timeScale;
	select_scene = GameScene::OPTION;
	is_press_button = false;
	select_option_menu = static_cast<unsigned int> (OptionMenu::OPTION_BUCK);
	dir = MoveDirection::NONE;
}

OptionScene::~OptionScene()
{
}

void OptionScene::Init()
{
	select_scene = GameScene::OPTION;
	is_press_button = false;
	select_option_menu = static_cast<unsigned int> (OptionMenu::OPTION_BUCK);
	dir = MoveDirection::NONE;
}

void OptionScene::Update()
{
	dir = ctx->input->getNavigationInput();
#ifdef DEBUG
	printfDx("stick_X:%f\nstick_Y:%f\n", stickX, stickY);
#endif // DEBUG
	if (dir == MoveDirection::NONE) {
		is_press_button = false;
	}

	if (!is_press_button && dir != MoveDirection::NONE)
	{
		is_press_button = true;

		switch (dir)
		{
		case MoveDirection::UP:
		case MoveDirection::DOWN:
			SceneUtility::UpdateMenuSelection(is_press_button, ctx->input, select_option_menu, sizeof(OptionMenuItems) / sizeof(MenuItem), ctx->sound);
			break;
		case MoveDirection::LEFT:
			if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_SE)) ctx->sound->SetSEVolume(ctx->sound->getSeVolume() - 5);
			if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_BGM))ctx->sound->SetBGMVolume(ctx->sound->getBgmVolume() - 5);
			break;
		case MoveDirection::RIGHT:
			if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_SE)) ctx->sound->SetSEVolume(ctx->sound->getSeVolume() + 5);
			if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_BGM)) ctx->sound->SetBGMVolume(ctx->sound->getBgmVolume() + 5);
			break;
		}
	}

	if (ctx->input->IsTrigger(Command::RETURN) || ctx->input->IsTrigger(Command::BtnB)) {
		ctx->sound->PlaySE(SoundEffect::DECIDE);
		OptionMenu selected = static_cast<OptionMenu>(select_option_menu);
		if (selected == OptionMenu::OPTION_BUCK) {
			select_scene = GameScene::TITLE;
		}
	}
}

void OptionScene::Draw()
{
	SceneUtility::DrawDemoPlayMovie(ctx->movie);
	ctx->font->DrawFormatChar(200, 100, "ƒIƒvƒVƒ‡ƒ“\n", static_cast<int>(FontSize::GAGA_Size_150), Config::ColorWhite);
	for (int i = 0; i < OptionMenuCount; i++) {
		bool is_active = (select_option_menu == i);
		unsigned int color = is_active ? Config::ColorOrange : Config::ColorWhite;
		const char* text = is_active ? OptionMenuItems[i].active_txt : OptionMenuItems[i].nomal_txt;
		int font_size = i == static_cast<int>(OptionMenu::OPTION_BUCK) ? static_cast<int>(FontSize::GAGA_Size_30) : static_cast<int>(FontSize::font1_Size_30);
		ctx->font->DrawFormatChar(
			Config::MenuLeftX,
			300 + OptionMenuItems[i].y_offset,
			text,
			font_size,
			color
		);
	}
	ctx->font->DrawFormatNum(350, 300, static_cast<float>(ctx->sound->getBgmVolume()), static_cast<unsigned int>(FontSize::font1_Size_30), Config::ColorWhite);

	ctx->font->DrawFormatNum(350, 330, static_cast<float>(ctx->sound->getSeVolume()), static_cast<unsigned int>(FontSize::font1_Size_30), Config::ColorWhite);
}

std::unique_ptr<ISceneBase> OptionScene::GetNextScene()
{
	if(select_scene==GameScene::TITLE)	return std::make_unique<TitleScene>(ctx,time_scale);
	return nullptr;

}
