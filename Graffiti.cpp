#include"DxLib.h"
#include "Graffiti.h"
#include"Enemy.h"

Graffiti::Graffiti()
	:pos(VGet(0,0,0))
	,cx(0.5f)
	,cy(0.5f)
	,size(100)
	,angle(0.0f)
	,trans(true)
{
	Load(Graph::Heart,"graph/heart.png");
}

Graffiti::~Graffiti()
{
}

void Graffiti::Load(const Graph& graphState, const char* fileName)
{
	graph_handle[graphState] = LoadGraph(fileName);
}

void Graffiti::Draw(const Graph& graphState)
{
	if (graph_handle.empty())return;
	DrawBillboard3D(pos,cx,cy,size,angle,graph_handle[graphState], trans);
}
void Graffiti::Update(const Enemy&enemy)
{
	pos = VAdd(enemy.getPos(),VGet(0,100,0));
	if (enemy.getCurrentState() == State::DOWN) {
		Draw(Graph::Heart);
	}
}