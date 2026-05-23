#pragma once
#include"DxLib.h"
#include<map>

const class Enemy;
//const class Effect;

enum class Graph
{
	Heart=0,
};

class Graffiti
{
private:
	VECTOR pos;//‰æ‘œ‚ğ•`‰æ‚·‚éÀ•W
	float cx, cy;//•`‰æ‚·‚é‰æ‘œ‚Ì’†SÀ•W
	float size;
	float angle;
	std::map<Graph, int>graph_handle;
	bool trans;
public:
	Graffiti();
	~Graffiti();

	void Load(const Graph&graph_state,const char*file_name);
	void Draw(const Graph& graph_state);
	void Update(const Enemy& enemy/*,const Effect& effect*/);
};
