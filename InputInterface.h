#pragma once
#include"DxLib.h"
#include<memory>

//キーの種類を定義
enum class Command {
	UP, DOWN, LEFT, RIGHT,
	W, A, S, D,
	RETURN, SPACE, TAB, LSHIFT,
	BtnA, BtnB, BtnX, BtnY, RT,
	BtnUP, BtnDOWN, BtnRIGHT, BtnLEFT,
	MAX//要素数
};

typedef struct Stick {
	float x = 0.0f;
	float y = 0.0f;
}Stick;

enum class MoveDirection {
	NONE, UP, DOWN, LEFT, RIGHT
};

class Input;

class IInput
{
public:
	virtual ~IInput(){}

	//長押しの判定
	virtual bool IsPress(Command cmd)const = 0;
	//押した瞬間の判定
	virtual bool IsTrigger(Command cmd)const = 0;

	virtual Stick getLeftStick()const = 0;
	virtual Stick getRightStick()const = 0;

	virtual MoveDirection getNavigationInput() = 0;

	virtual void SetEnabled(bool enabled) = 0;

	virtual void Update() = 0;
	virtual void DrawTab(const VECTOR& pos)const = 0;
	virtual void DrawRightTrigger(const VECTOR& pos)const = 0;
private:

};
