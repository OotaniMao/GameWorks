#include"DxLib.h"
#include<memory>
#include "Goal.h"
#include"Collision.h"
#include"Player.h"
//#define DEBUG

namespace {
	const VECTOR INITIAL_GOAL_POS = VGet(-7368.0f, 0.0f, 13947.0f);
	const float ARROW_DISTANCE = 150.0f;
	const float ARROW_SIZE = 150.0f;
	const float ARROW_HEIGHT = 150.0f;
	const float EPSILON = 0.0001f;//ゼロ除算防止用
}


Goal::Goal()
	:is_goal(false),
	graph_handle(-1)
{
	sphere.radius = GOAL_RADIUS;
	graph_handle = LoadGraph("graph/arrow_red.png");

	//頂点の基本情報(色・法線・UV)を初期化
	for (auto& vert : vertices) {
		vert.norm = VGet(0.0f, 1.0f, 0.0f);
		vert.dif = GetColorU8(255, 255, 255, 255);
	}

	vertices[0].u = 0.0f; vertices[0].v = 0.0f;
	vertices[1].u = 1.0f; vertices[1].v = 0.0f;
	vertices[2].u = 0.0f; vertices[2].v = 1.0f;
	vertices[3].u = 1.0f; vertices[3].v = 1.0f;


	InitIndex();
	InitPos();
}

Goal::~Goal()
{
	if (graph_handle != -1)DeleteGraph(graph_handle);
}

void Goal::InitIndex()
{
	indeces = {0,1,2,3,2,1};
}

void  Goal::InitPos()
{
	sphere.pos = INITIAL_GOAL_POS;
}

void Goal::Init()
{
	InitPos();
	is_goal = false;
}

void Goal::Update(const Player& player)
{
	//衝突判定
	static Collision coll;
	is_goal = coll.CapsuleToSphere(player.getCollCapsule(), sphere);

	//ゴール方向へのベクトル計算
	VECTOR to_goal = VSub(sphere.pos, player.getPos());

	//playerがgoalに重なりすぎている場合はデフォルト方向
	if (VSize(to_goal) > EPSILON)to_goal = VNorm(to_goal);
	else to_goal = VGet(0.0f,0.0f,1.0f);

	//画像の配置座標（playerの前方の空中）計算
	VECTOR arrow_pos = VAdd(player.getPos(), VScale(to_goal,ARROW_DISTANCE));
	arrow_pos.y = ARROW_HEIGHT;
	
	//頂点計算(ビルボード＋回転)
	UpdateSquareVertex(arrow_pos, ARROW_SIZE,to_goal);


#ifdef DEBUG
	printfDx("goal_vec:(%0.1f,%0.1f,%0.1f)\n", to_goal.x, to_goal.y, to_goal.z);
#endif // DEBUG

}

void Goal::UpdateSquareVertex(const VECTOR& center,float size,VECTOR to_goal)
{
	float h = size * 0.5f;
	//ビルボードの基本形状
	const VECTOR local_pos[4] = {
		VGet(-h, h, 0.0f),
		VGet( h, h, 0.0f),
		VGet(-h,-h, 0.0f),
		VGet( h,-h, 0.0f)
	};

	//ビルボード行列を取得(ビュー行列の回転成分を反転させたもの)
	MATRIX inv_view = MTranspose(GetCameraViewMatrix());
	//スクリーン上での向き(ゴール方向)ベクトルを計算
	VECTOR camera_space_dir = VTransformSR(to_goal,GetCameraViewMatrix());
	//スクリーン上での右と上の成分から、2D的な回転角を出す
	float z_angle = atan2f(camera_space_dir.y,camera_space_dir.x);
	//矢印を回して(z軸回転)から、カメラ(ビルボード回転)に向ける
	MATRIX rot_z = MGetRotZ(z_angle);
	MATRIX final_rot = MMult(rot_z, inv_view);


	for (int i = 0; i < VERTEX_COUNT; i++) {
		vertices[i].pos = VAdd(center,VTransformSR(local_pos[i],final_rot));
	}
}

void Goal::Draw()
{
	unsigned int color = is_goal ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
	//ゴールの球体描画
	DrawSphere3D(sphere.pos,sphere.radius,16,color,color, false);
	//矢印の描画
	DrawPolygonIndexed3D(vertices.data(), VERTEX_COUNT, indeces.data(), INDEX_COUNT / 3, graph_handle, TRUE);

#ifdef DEBUG
	printfDx("Goal Reached: %s\n", is_goal?"TRUE":"FALSE");
#endif // DEBUG
}

