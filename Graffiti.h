#pragma once
#include"DxLib.h"
#include<map>

class Enemy;

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

	void Load(const Graph&graphState,const char*fileName);
	void Draw(const Graph& graphState);
	void Update(const Enemy& enemy);
};
