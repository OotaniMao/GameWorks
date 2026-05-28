#include"DxLib.h"
#include"SceneUtility.h"
#include"Config.h"
#include"Input.h"
#include"Sound.h"
#include"Font.h"
#include"Movie.h"

void SceneUtility::UpdateMenuSelection(bool& isPressButton, const std::shared_ptr<IInput> key, int& selectIndex, int maxCount, std::shared_ptr<Sound> sound)
{
	float stickY = key->getLeftStick().y;
	bool up = (stickY > Config::STICK_THRESHOLD) || key->IsTrigger(Command::BtnUP) || key->IsTrigger(Command::UP);
	bool down = (stickY < -Config::STICK_THRESHOLD) || key->IsTrigger(Command::BtnDOWN) || key->IsTrigger(Command::DOWN);

	if ((up || down) && !isPressButton) {
		if (up)selectIndex = (selectIndex - 1 + maxCount) % maxCount;
		if (down)selectIndex = (selectIndex + 1 + maxCount) % maxCount;
		isPressButton = true;
		sound->PlaySE(SoundEffect::SELECT);
	}
	//入力なし
	else if (std::abs(stickY) <= Config::STICK_THRESHOLD) {
		isPressButton = false;
	}
}

void SceneUtility::DrawMenuItems(const MenuItem* items, int count, int selectedIndex, std::shared_ptr<Font> font)
{
	for (int i = 0; i < count; i++) {
		bool is_active = (selectedIndex == i);
		font->DrawFormatChar(
			Config::MENU_LEFT_X,
			Config::TITLE_TEXT_Y + items[i].y_offset,
			is_active ? items[i].active_txt : items[i].nomal_txt,
			static_cast<int>(FontSize::GAGA_Size_30),
			is_active ? Config::COLOR_ORANGE : Config::COLOR_WHITE
		);
	}
}

void SceneUtility::DrawDemoPlayMovie(std::shared_ptr<Movie> movie)
{
	int black = GetColor(0, 0, 0);
	movie->Update(static_cast<int>(MovieState::TITLE));
	movie->Play();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DrawBox(0, 0, 960, 540, black, true);
	//ブレンドモードを通常状態に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
