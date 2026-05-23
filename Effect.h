#pragma once
#include<map>

const class EnemyManager;
const class Player;

enum class EffectState :int {
	HIT = 0,
};


class Effect
{
private:
	std::map<EffectState, int>resource_handle;
	int playing_effect_handle;//エフェクト用ハンドル
	int play_count;
	bool is_alive;
	VECTOR pos;
	bool is_playing;
public:
	Effect();
	~Effect();

	bool getIsAlive()const { return is_alive; }
	bool getIsPlaying()const { return is_playing; }

	void Init();
	void Load(const EffectState&effectstate,const char* filename);
	void Update(const EnemyManager&enemy,const Player&player);
	void Play();

};
