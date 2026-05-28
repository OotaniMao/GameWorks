#include"DxLib.h"
#include"Config.h"
#include "InputInterface.h"
#include"Player.h"
#include "Input.h"

namespace InputConfig{
	const float STICK_MAX = 32767.0f;//スティックの値を-1.0f～1.0fに正規化
	const int XINPUT_RT_THRESHOLD = 30;//RTの押し込み判定　しきい値
	static constexpr int STICK_DEADZONE = 20000;
}

Input::Input()
	:is_enabled(true),
	handle_RT(-1),
	handle_tab(-1)
{
	handle_RT = LoadGraph("graph/key_right_trigger.jpg");
	handle_tab = LoadGraph("graph/key_tab.png");
	
	//全ての状態を初期化
	for (int i = 0; i < static_cast<int>(Command::MAX); i++) {
		current[i] = previous[i] = false;
	}
}

Input::~Input()
{
	if (handle_RT != -1)DeleteGraph(handle_RT);
	if (handle_tab != -1)DeleteGraph(handle_tab);
}

MoveDirection Input::getNavigationInput()
{
	auto stick = this->getLeftStick();
	if ((stick.y > Config::STICK_THRESHOLD) || this->IsTrigger(Command::BtnUP) || this->IsTrigger(Command::UP))return MoveDirection::UP;
	if ((stick.y < -Config::STICK_THRESHOLD) || this->IsTrigger(Command::BtnDOWN) || this->IsTrigger(Command::DOWN))return MoveDirection::DOWN;
	if ((stick.x < -Config::STICK_THRESHOLD) || this->IsTrigger(Command::BtnLEFT) || this->IsTrigger(Command::LEFT))return MoveDirection::LEFT;
	if ((stick.x > Config::STICK_THRESHOLD) || this->IsTrigger(Command::BtnRIGHT) || this->IsTrigger(Command::RIGHT))return MoveDirection::RIGHT;
	return MoveDirection::NONE;
}

void Input::Update()
{
	for (int i = 0; i < static_cast<int>(Command::MAX); i++) {
		previous[i] = current[i];
	}

	UpdateKeyboard();
	UpdateXInput();

	if (!is_enabled) {
		for (int i = 0; i < static_cast<int>(Command::MAX); i++) {
			current[i] = false;
		}
		right_stick = { 0.0f,0.0f };
	}

}

void Input::DrawTab(const VECTOR& pos) const
{
	DrawGraph3D(pos.x,pos.y,pos.z,handle_tab,TRUE);
}

void Input::DrawRightTrigger(const VECTOR& pos) const
{
	DrawGraph3D(pos.x, pos.y, pos.z, handle_RT, TRUE);
}

void Input::UpdateKeyboard()
{
	auto check = [this](Command cmd, int dx_key) {
		current[static_cast<int>(cmd)] = (CheckHitKey(dx_key) != 0);
	};

	check(Command::W,		KEY_INPUT_W);
	check(Command::A,		KEY_INPUT_A);
	check(Command::S,		KEY_INPUT_S);
	check(Command::D,		KEY_INPUT_D);
	check(Command::UP,		KEY_INPUT_UP);
	check(Command::DOWN,	KEY_INPUT_DOWN);
	check(Command::LEFT,	KEY_INPUT_LEFT);
	check(Command::RIGHT,	KEY_INPUT_RIGHT);
	check(Command::RETURN,	KEY_INPUT_RETURN);
	check(Command::SPACE,	KEY_INPUT_SPACE);
	check(Command::TAB,		KEY_INPUT_TAB);
	check(Command::LSHIFT,	KEY_INPUT_LSHIFT);
}

void Input::UpdateXInput()
{
	XINPUT_STATE input;
	GetJoypadXInputState(DX_INPUT_PAD1, &input);
	//ボタン類
	current[static_cast<int>(Command::BtnA)] = (input.Buttons[XINPUT_BUTTON_A] != 0);
	current[static_cast<int>(Command::BtnB)] = (input.Buttons[XINPUT_BUTTON_B] != 0);
	current[static_cast<int>(Command::BtnX)] = (input.Buttons[XINPUT_BUTTON_X] != 0);
	current[static_cast<int>(Command::BtnY)] = (input.Buttons[XINPUT_BUTTON_Y] != 0);
	current[static_cast<int>(Command::BtnUP)] = (input.Buttons[XINPUT_BUTTON_DPAD_UP] != 0);
	current[static_cast<int>(Command::BtnDOWN)] = (input.Buttons[XINPUT_BUTTON_DPAD_DOWN] != 0);
	current[static_cast<int>(Command::BtnRIGHT)] = (input.Buttons[XINPUT_BUTTON_DPAD_RIGHT] != 0);
	current[static_cast<int>(Command::BtnLEFT)] = (input.Buttons[XINPUT_BUTTON_DPAD_LEFT] != 0);
	current[static_cast<int>(Command::RT)] = (input.RightTrigger > InputConfig::XINPUT_RT_THRESHOLD);
	//スティックの正規化処理
	auto normalize_stick = [](short val) {
		if (std::abs(val) < InputConfig::STICK_DEADZONE)return 0.0f;
		return val / InputConfig::STICK_MAX;
	};

	left_stick = { normalize_stick(input.ThumbLX),normalize_stick(input.ThumbLY) };
	right_stick = { normalize_stick(input.ThumbRX),normalize_stick(input.ThumbRY) };
	
}

void Input::ResetAll()
{
	for (int i = 0; i < static_cast<int>(Command::MAX); i++) {
		current[i] = false;
	}
	left_stick = { 0.0f,0.0f };
	right_stick = { 0.0f,0.0f };
}

