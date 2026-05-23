#include"DxLib.h"
#include "SceneUtility.h"
#include"Config.h"
#include"Input.h"
#include"Sound.h"
#include"Font.h"
#include"Movie.h"

void SceneUtility::UpdateMenuSelection(bool& is_press_button, const std::shared_ptr<IInput> key, int& select_index, int max_count, std::shared_ptr<Sound> sound)
{
	float stickY = key->getLeftStick().y;
	bool up = (stickY > Config::StickThreshold) || key->IsTrigger(Command::BtnUP) || key->IsTrigger(Command::UP);
	bool down = (stickY < -Config::StickThreshold) || key->IsTrigger(Command::BtnDOWN) || key->IsTrigger(Command::DOWN);

	if ((up || down) && !is_press_button) {
		if (up)select_index = (select_index - 1 + max_count) % max_count;
		if (down)select_index = (select_index + 1 + max_count) % max_count;
		is_press_button = true;
		sound->PlaySE(SoundEffect::SELECT);
	}
	//入力なし
	else if (std::abs(stickY) <= Config::StickThreshold) {
		is_press_button = false;
	}
}

void SceneUtility::DrawMenuItems(const MenuItem* items, int count, int selected_index, std::shared_ptr<Font> font)
{
	for (int i = 0; i < count; i++) {
		bool is_active = (selected_index == i);
		font->DrawFormatChar(
			Config::MenuLeftX,
			Config::TitleTextY + items[i].y_offset,
			is_active ? items[i].active_txt : items[i].nomal_txt,
			static_cast<int>(FontSize::GAGA_Size_30),
			is_active ? Config::ColorOrange : Config::ColorWhite
		);
	}
}

void SceneUtility::DrawDemoPlayMovie(std::shared_ptr<Movie> movie)
{
	int black = GetColor(0, 0, 0);
	movie->Update(static_cast<int>(MovieState::M_TITLE));
	movie->Play();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DrawBox(0, 0, 960, 540, black, true);
	//ブレンドモードを通常状態に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
