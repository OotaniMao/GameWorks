#pragma once

class Player;
class EnemyManager;
class Map;

class ShadowMap
{
private:
	int stage_shadow_map_handle;								//ステージのシャドウマップハンドル
	int chara_shadow_map_handle;								//Characterのシャドウマップハンドル
public:
	ShadowMap();												//コンストラクタ
	~ShadowMap();												//デストラクタ

	void Init();												//シャドウマップ生成の初期化
	void DrawSetUPStage(Map&map);								//シャドウマップ描画処理(影を落とす物体)
	void DrawSetUPCharacter(Player& player, EnemyManager& enemy);	//シャドウマップ描画処理(影を落とす物体)
	void SetUse(Player& player, EnemyManager& enemy, Map& map);	//通常描画＋影を使用(影を受ける物体)
	void Delete();												//シャドウマップの削除
};
