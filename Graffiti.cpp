#include "Graffiti.h"
#include"DxLib.h"
#include"Enemy.h"
//#include"Effect.h"
#include "Player.h"

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

void Graffiti::Load(const Graph& graph_state, const char* file_name)
{
	graph_handle[graph_state] = LoadGraph(file_name);
}

void Graffiti::Draw(const Graph& graph_state)
{
	if (graph_handle.empty())return;
	DrawBillboard3D(pos,cx,cy,size,angle,graph_handle[graph_state], trans);
}
void Graffiti::Update(const Enemy&enemy/*,const Effect&effect*/)
{
	pos = VAdd(enemy.getPos(),VGet(0,100,0));
	if (enemy.getCurrentState() == State::S_DOWN/*&&effect.getIsAlive()*/) {
		Draw(Graph::Heart);
	}
}