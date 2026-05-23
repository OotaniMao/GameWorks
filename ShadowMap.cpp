#include"DxLib.h"
#include"Player.h"
#include"Map.h"
#include"EnemyManager.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap()
	:stage_shadow_map_handle(-1)//シャドウマップの作成
	,chara_shadow_map_handle(-1)
{
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::Init()
{
	stage_shadow_map_handle = MakeShadowMap(2048, 2048);
	chara_shadow_map_handle = MakeShadowMap(1024,1024);
}

void ShadowMap::DrawSetUP_Stage(Map& map)
{
	//ライトの向きを設定
	SetShadowMapLightDirection(stage_shadow_map_handle, VGet(1.0f, -1.0f, 0.0f));
	//シャドウマップに描画する範囲（AABB）
	SetShadowMapDrawArea(stage_shadow_map_handle,
		 VGet(-10000.0f, -1.0f, -10000.0f),
		VGet(10000.0f, 500.0f, 10000.0f)
	);

	ShadowMap_DrawSetup(stage_shadow_map_handle);
	map.Draw();
	ShadowMap_DrawEnd();
}

void ShadowMap::DrawSetUP_Chara(Player& player, EnemyManager& enemy)
{
	VECTOR center = player.getPos();
	SetShadowMapLightDirection(chara_shadow_map_handle, VGet(1.0f, -1.0f, 0.0f));
	SetShadowMapDrawArea(chara_shadow_map_handle,
		VAdd(center, VGet(-500.0f, -1.0f, -500.0f)),
		VAdd(center, VGet(500.0f, 500.0f, 500.0f))
	);
	

	ShadowMap_DrawSetup(chara_shadow_map_handle);
	
	enemy.Draw();
	player.DrawModel();
	ShadowMap_DrawEnd();
}

void ShadowMap::SetUse(Player& player, EnemyManager& enemy, Map& map)
{
	SetUseShadowMap(0, stage_shadow_map_handle);
	SetUseShadowMap(1, chara_shadow_map_handle);
	map.Draw();
	enemy.Draw();
	
	player.DrawModel();

	SetUseShadowMap(0,-1);
	SetUseShadowMap(1, -1);
}

void ShadowMap::Delete()
{
	if (stage_shadow_map_handle != -1) {
		DeleteShadowMap(stage_shadow_map_handle);
		stage_shadow_map_handle = -1;
	}
	if (chara_shadow_map_handle != -1) {
		DeleteShadowMap(chara_shadow_map_handle);
		chara_shadow_map_handle = -1;
	}
}
