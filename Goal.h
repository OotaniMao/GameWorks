#pragma once
#include"R_Math.h"
#include<array>

class Player;

class Goal
{
private:
	static constexpr int VERTEX_COUNT = 4;
	static constexpr int INDEX_COUNT = 6;

	Sphere sphere;				//ゴール判定用の球
	bool is_goal;				//ゴールしたかどうか true:した　false:してない
	int graph_handle;					//矢印用テクスチャハンドル
	std::array<VERTEX3D , VERTEX_COUNT>vertices;	//四角形用頂点
	std::array<WORD, INDEX_COUNT>indeces;	//インデックス（三角形２枚分）
public:
	Goal();
	~Goal();

	bool getIsGoal()const { return is_goal; }

	void Init();
	void Update(const Player&player);
	void Draw();

private:
	void InitPos();
	void InitIndex();
	void UpdateSquareVertex(const VECTOR& center, float size, VECTOR toGoal);
};
