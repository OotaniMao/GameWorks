#include"DxLib.h"
//#include"EffekseerForDXLib.h"
#include"Effect.h"
#include"Enemy.h"
#include"EnemyManager.h"
#include"Player.h"


/*https://ameblo.jp/546kw/entry-12617906090.html
エフェクシアのリファレンス
*/

Effect::Effect()
	:playing_effect_handle(-1)
	,play_count(0)
	,pos(VGet(-830.0f, 0.0f, -7910.0f))
	,is_alive(false)
	,is_playing(false)
{
	Init();
	
}

Effect::~Effect() {

}

void Effect::Init()
{
	Load(EffectState::HIT, "effect/hit.efk");
}

void Effect::Load(const EffectState& effectstate, const char* filename)
{
	/*int tmp = LoadEffekseerEffect(filename, 30.0f);
	if (tmp == -1) {
		printfDx("Failed to load file:%s\n",filename);
	}
	else {
		resource_handle[effectstate] = tmp;
	}*/
}

void Effect::Update(const EnemyManager&enemy,const Player&player)
{
	for (const auto& enemy_ptr : enemy.getEnemies()) {
		pos = VAdd(enemy_ptr->getPos(), VGet(0, 100, 0));
		/*if (player.getCurrentState() == State::S_SPECIAL_ATTACK && player.getIsPlayAnimEnd())
		{
			playing_effect_handle = PlayEffekseer3DEffect(resource_handle[EffectState::HIT]);

		}
		if (CheckHitKey(KEY_INPUT_RETURN)) {
			playing_effect_handle = PlayEffekseer3DEffect(resource_handle[EffectState::HIT]);
		}
		else {
			is_alive = false;
		}*/
		Play();
	}
	
	
}

void Effect::Play()
{
	/*SetPosPlayingEffekseer3DEffect(playing_effect_handle, pos.x, pos.y, pos.z);
	UpdateEffekseer3D();
	DrawEffekseer3D();
	is_playing=IsEffekseer3DEffectPlaying(playing_effect_handle);*/
	//is_alive = true;
}