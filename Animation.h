#pragma once
#include <map>
#include<deque>
#include"Model.h"


//アニメーション状態番号順
enum class AnimState :int {
	NONE = -1,		//なし
	IDLE = 0,		//待機
	JUMP = 1,		//ジャンプ 
	ATTACK = 2,		//攻撃
	KICK = 3,		//蹴る
	RUN = 4,		//走る
	SPECIAL_ATTACK = 5,	//必殺技
	DOWN = 6,
	FALL=7,
	AIRDASH=8,
	CARTWHEEL=9,
	SKATE=10,//滑る
	SLASH=11,//剣で切る
	IMPACT=12,//衝撃
};

typedef struct Anim {
	int handle=-1;
	bool is_loop=false;
	float play_speed=1.0f;
}Anim;

class Animation :public Model
{
private:
	
	const int animIndex = 0;
	const float add_time = 0.6f;
	const float add_blend_rate = 0.08f;
	const float MAX_BLEND_RATE = 1.0f;
	const float MIN_BLEND_RATE = 0.0f;

	std::map<int, Anim>anim_handle;	//アニメーションのハンドル
	std::deque<int>attach_index;
	std::deque<bool>attach_is_loop;
	std::deque<float>attach_play_speed;
	float total_time;
	float play_time;
	bool is_play_end;//アニメーションの再生が終了したかどうか

	bool is_blend_end;
	float blend_rate;

	AnimState current_anim_state;
	AnimState previous_anim_state;


	//アニメーションの読み込み
	void Load(AnimState _animState, const char* fileName, bool isLoop, float play_speed);

	//アニメーションをアタッチ
	void Attach(const int& _currentState);

	void SetTotalTime();

	//アニメーションのデタッチ
	void Detach();

	//アニメーションを再生する
	void PlayAnim(const bool& _is_blend);
	void BlendAnim(bool _is_blend);


public:
	//コンストラクタ
	Animation(const char* fileName);
	~Animation();

	//ゲッター
	bool getIsPlayEnd()const { return is_play_end; }
	bool getIsBlendEnd()const { return is_blend_end; }
	int getIsAttachSize()const { return static_cast<int>(attach_index.size()); }
	AnimState getCurrentAnimState()const { return current_anim_state; }
	int getModelHandle()const { return model_handle; }

	void Init();

	void SetIdleState();

	void SetOpacityRate(float opacity_rate);


	void Update(const int&_currentState,const int& _previousState, const bool& _is_blend);

	
	//アタッチされているアニメーションのフレーム位置を取得
	void GetAttachAnimFramePos(int frameIndex);
	
};
