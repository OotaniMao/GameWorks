#pragma once
#include"R_Math.h"


enum class State :int {
	NONE = -1,	//なし
	IDLE,		//待機
	JUMP,		//ジャンプ 
	ATTACK,	//殴る
	KICK,		//蹴る
	RUN,		//走る
	SPECIAL_ATTACK,	//必殺技
	DOWN,		//ダウン
	AIRDASH,	//エアダッシュ
	CARTWHEEL,	//側転
	SKATE,//滑る
	IMPACT,//攻撃をくらったとき
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
