#pragma once
#include<memory>
#include"InputInterface.h"

class Player;

/// @brief “ü—ÍŠÇ—ƒNƒ‰ƒX(keyboard,XInput‘Î‰)
class Input:public IInput
{	
public:
	Input();
	~Input();

	//’·‰Ÿ‚µ‚Ì”»’è
	bool IsPress(Command cmd)const override{ return current[static_cast<int>(cmd)]; }
	//‰Ÿ‚µ‚½uŠÔ‚Ì”»’è
	bool IsTrigger(Command cmd)const override{ return current[static_cast<int>(cmd)] && !previous[static_cast<int>(cmd)]; }

	Stick getLeftStick()const override{ return left_stick; }
	Stick getRightStick()const override{ return right_stick; }

	MoveDirection getNavigationInput() override;

	void SetEnabled(bool enabled) override { is_enabled = enabled; }

	void Update()override;
	void DrawTab(const VECTOR& pos)const override;
	void DrawRightTrigger(const VECTOR& pos)const override;
private:
	void UpdateKeyboard();
	void UpdateXInput();
	void ResetAll();

private:
	bool current[static_cast<int>(Command::MAX)] = { false };
	bool previous[static_cast<int>(Command::MAX)] = { false };
	bool is_enabled = true;

	Stick left_stick;
	Stick right_stick;

	int handle_tab = -1;
	int handle_RT = -1;

	static constexpr int STICK_DEADZONE = 20000;
};
