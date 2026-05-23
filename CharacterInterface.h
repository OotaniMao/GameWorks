#pragma once
#include"DxLib.h"
#include"R_Math.h"


enum class State :int {
	S_NONE = -1,	//なし
	S_IDLE,		//待機
	S_JUMP,		//ジャンプ 
	S_ATTACK,	//殴る
	S_KICK,		//蹴る
	S_RUN,		//走る
	S_SPECIAL_ATTACK,	//必殺技
	S_DOWN,		//ダウン
	S_AIRDASH,	//エアダッシュ
	S_CARTWHEEL,	//側転
	S_SKATE,//滑る
	S_IMPACT,//攻撃をくらったとき
};

class ICharacter
{
public:
	virtual ~ICharacter(){}
	virtual VECTOR getPos()const = 0;
	virtual Capsule getCollCapsule()const = 0;
	virtual float getSphereRadius()const = 0;
	virtual VECTOR  getRightLegPos()const = 0;
	virtual State getCurrentState() const = 0;
	virtual bool getIsPlayAnimEnd()const = 0;
	virtual bool getIsAlive()const = 0;
	virtual Capsule getBatonCollCapsule() const = 0;
	virtual bool getIsSpecialRangeHit() const = 0;
};
