#include"DxLib.h"
#include "Map.h"
#include"Player.h"

/*
ポリゴンの描画の参考
https://qiita.com/0reo07/items/f2424069412b97a02848
*/


Map::Map()
	:Model("model/map/map.mv1")
	, pos(VGet(0.0f, 0.0f, 0.0f))
{
	MV1SetScale(model_handle, VGet(0.1f, 0.1f, 0.1f));
	Init();
}

Map::~Map()
{
	
}

void Map::Draw()
{
	MV1DrawModel(model_handle);
}

void Map::DrawMesh()
{
	for (int i = 0; i < polygonlist.PolygonNum; i++) {
		//ポリゴンを形成する三頂点を使用してワイヤーフレームを描画する
		DrawLine3D(
			polygonlist.Vertexs[polygonlist.Polygons[i].VIndex[0]].Position,
			polygonlist.Vertexs[polygonlist.Polygons[i].VIndex[1]].Position,
			GetColor(255,255,0));
		DrawLine3D(
			polygonlist.Vertexs[polygonlist.Polygons[i].VIndex[1]].Position,
			polygonlist.Vertexs[polygonlist.Polygons[i].VIndex[2]].Position,
			GetColor(255, 255, 0));
		DrawLine3D(
			polygonlist.Vertexs[polygonlist.Polygons[i].VIndex[2]].Position,
			polygonlist.Vertexs[polygonlist.Polygons[i].VIndex[0]].Position,
			GetColor(255, 255, 0));
	}
}

float Map::GetFloorHeight(VECTOR target_pos) const
{
	//ターゲット座標の少し上から下まで十分に線分を引く
	VECTOR start = VAdd(target_pos,VGet(0.0f,500.0f,0.0f));
	VECTOR end = VAdd(target_pos,VGet(0.0f,-1000.0f,0.0f));
	//マップモデルと線分の当たり判定
	MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(model_handle,-1,start,end);

	//当たった位置のy座標を返す
	if (hit.HitFlag == true) {
		return hit.HitPosition.y;
	}

	return 0.0f;//何処にも当たらなかった場合
}

bool Map::CheckHitWall(const Sphere&sphere) const
{
	MV1_COLL_RESULT_POLY_DIM hit = MV1CollCheck_Sphere(model_handle, -1, sphere.pos, sphere.radius);
	return (hit.HitNum==0?true:false);
}
