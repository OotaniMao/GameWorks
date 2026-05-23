#pragma once
#include"DxLib.h"

enum class CameraState {
	NORMAL = 0,//’Êí
	SPECIAL = 1,//•KE‹Z
};

class Player;

class ICamera
{
public:
	
	virtual ~ICamera() {}

	virtual VECTOR getCurrentCameraPos()const = 0;
	virtual VECTOR getCameraToTargetDir()const = 0;
	virtual bool getIsSpecialMoveEnd()const = 0;
	virtual CameraState getCameraState()const = 0;
	virtual int getInputKeyNum()const = 0;

	/// @brief ‰Šú‰»
	virtual void Init() = 0;
	/// @brief ƒJƒƒ‰‚Ì‹““®§Œä
	virtual void Update(const Player& player, float timeScale) = 0;
	virtual void DrawUI()=0;
private:

};
