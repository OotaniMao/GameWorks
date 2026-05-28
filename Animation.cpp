#include"DxLib.h"
#include"Model.h"
#include"Animation.h"

namespace AnimConfig{
	const float DEFAULT_SPEED = 1.0f;
	const int ANIM_INDEX = 0;
	const float ADD_TIME = 0.6f;
	const float ADD_BLEND_RATE = 0.08f;
	const float MAX_BLEND_RATE = 1.0f;
	const float MIN_BLEND_RATE = 0.0f;
}

Animation::Animation(const char* fileName)
	:Model(fileName),
	play_time(0.0f),
	total_time(0.0f),
	is_play_end(false),
	attach_index(),
	attach_is_loop(),
	attach_play_speed(),
	is_blend_end(false),
	blend_rate(0.0f),
	current_anim_state(AnimState::IDLE),
	previous_anim_state(AnimState::NONE)
{
	Init();
}

Animation::~Animation()
{
}


void Animation::Load(AnimState animState, const char* fileName,bool isLoop,float playSpeed)
{
	//アニメーションの読み込み
	anim_handle[static_cast<int>(animState)] = { MV1LoadModel(fileName),isLoop,playSpeed };
}

void Animation::Init()
{
	Load(AnimState::IDLE,			"animation/Standing.mv1",		true,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::JUMP,			"animation/Jumping_Up.mv1",		false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::RUN,			"animation/standardRun.mv1",	true,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::ATTACK,			"animation/attack_01.mv1",		false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::SPECIAL_ATTACK, "animation/SpecialAttack.mv1",	false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::KICK,			"animation/Kick.mv1",			false,	2.4f);
	Load(AnimState::DOWN,			"animation/stunned.mv1",		false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::FALL,			"animation/Falling.mv1",		false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::AIRDASH,		"animation/air_dash.mv1",		false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::CARTWHEEL,		"animation/cartwheel.mv1",		false,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::SKATE,			"animation/Skate.mv1",			true,	AnimConfig::DEFAULT_SPEED);
	Load(AnimState::SLASH,			"animation/SwordSlash.mv1",		false,	2.0f);
	Load(AnimState::IMPACT,			"animation/Impact.mv1",			false,	0.3f);
}

void Animation::SetIdleState()
{
	Attach(static_cast<unsigned int>(AnimState::IDLE));
}

void Animation::Update(const int&currentState,const int&previousState,const bool&isBlend)
{
	current_anim_state = static_cast<AnimState>(currentState);
	previous_anim_state = static_cast<AnimState>(previousState);

	if (previous_anim_state != current_anim_state) Attach(static_cast<unsigned int>(current_anim_state));
	if (getIsBlendEnd() && getIsAttachSize() >= 2)Detach();
	SetTotalTime();
	PlayAnim(isBlend);
}

void Animation::Attach(const int& currentState)
{
	is_play_end = false;
	auto itr = anim_handle.find(currentState);
	if (itr == anim_handle.end() || itr->second.handle == -1) {
		printfDx("No animation handle found for state: %d\n", currentState);
		return;
	}
	attach_index.push_back(MV1AttachAnim(model_handle, AnimConfig::ANIM_INDEX, itr->second.handle, TRUE));
	attach_is_loop.push_back(itr->second.is_loop);
	attach_play_speed.push_back(itr->second.play_speed);

	play_time = 0.0f;
}

void Animation::SetTotalTime()
{
	//アタッチしたアニメーションの総合時間の取得
	if (attach_index.size() >= 2)total_time = MV1GetAttachAnimTotalTime(model_handle, attach_index[1]);
	else total_time = MV1GetAttachAnimTotalTime(model_handle, attach_index.front());

}

void Animation::Detach()
{
	if (!attach_index.empty()) {
		MV1DetachAnim(model_handle, attach_index.front());
		attach_index.pop_front();
		attach_is_loop.pop_front();
		attach_play_speed.pop_front();
	}
	else {
		printfDx("No animation to detach\n");
	}

}

void Animation::PlayAnim(const bool& isBlend)
{

	if(attach_index.size()>=2)BlendAnim(isBlend);

	//時間を加算
	play_time += AnimConfig::ADD_TIME*attach_play_speed.front();

	//もし再生時間が総合時間を越えた時
	if (play_time >= total_time) {
		if (attach_is_loop.front()) {
			play_time = 0.0f;
		}
		else {
			play_time = total_time;
			is_play_end = true;
		}
	}
	//アタッチしたアニメーションの再生時間を設定
	if(!attach_index.empty())MV1SetAttachAnimTime(model_handle, attach_index.front(), play_time);
}

void Animation::GetAttachAnimFramePos(int framIndex)
{
	VECTOR frame_pos = MV1GetFramePosition(model_handle, framIndex);
}

void Animation::BlendAnim(bool isBlend)
{
	if (attach_index.size() < 2)return;
	if (attach_index.size() >= 3)Detach();
	isBlend ? blend_rate += AnimConfig::ADD_BLEND_RATE : blend_rate = AnimConfig::MAX_BLEND_RATE;
	if (blend_rate > AnimConfig::MAX_BLEND_RATE)blend_rate = AnimConfig::MAX_BLEND_RATE;

	int previous = attach_index.front();
	int current = attach_index[1];

	MV1SetAttachAnimBlendRate(model_handle,previous, AnimConfig::MAX_BLEND_RATE-blend_rate);
	MV1SetAttachAnimBlendRate(model_handle,current,blend_rate);

	if (blend_rate >= AnimConfig::MAX_BLEND_RATE) {
		MV1SetAttachAnimBlendRate(model_handle,previous, AnimConfig::MIN_BLEND_RATE);
		blend_rate = AnimConfig::MIN_BLEND_RATE;
		Detach();
	}
}

void Animation::SetOpacityRate(float opacity_rate)
{
	MV1SetOpacityRate(model_handle,opacity_rate);
}
