#include"DxLib.h"
#include"Map.h"
#include"Player.h"
#include"Enemy.h"
#include "Collision.h"
//#define DEBUG

Collision::Collision()
	:floor(),
	wall(),
	hit_polygon_dim(),
	poly(),
	is_hit_floor(false)
{
}

Collision::~Collision()
{
}

bool Collision::CapsuleToSphere(const Capsule& capsule, const Sphere& sphere)
{
	bool is_hit = false;
	VECTOR dis = VSub(capsule.pos, sphere.pos);
	VECTOR dis2 = VSub(capsule.pos2, sphere.pos);
	if (VSize(dis) > VSize(dis2))dis = dis2;
	Segment segment = { capsule.pos, capsule.pos2 };
	VECTOR tmp = R_Math::CalcDistancePointToLineSegment(sphere.pos, segment);

	if (VSize(dis) > VSize(tmp))dis = tmp;
	if (VSize(dis) <= capsule.radius + sphere.radius) {
		is_hit = true;
	}
	else
	{
		is_hit = false;
	}

	return is_hit;
}

VECTOR Collision::DistanceCapsuleToCapsule(const Capsule& capsule, const Capsule& capsule2)
{
	return R_Math::CalcSegmentToSegment(capsule.GetAxis(), capsule2.GetAxis());
}

bool Collision::CapsuleToCapsule(const Capsule& capsule, const Capsule& capsule2)
{
	bool is_hit = false;
	float size = VSquareSize(DistanceCapsuleToCapsule(capsule, capsule2));
	float radius_sum = capsule.radius + capsule2.radius;
	return size <= (radius_sum* radius_sum);
}

VECTOR Collision::ExtrusionCapsuleToCapsule(const Capsule& capsule, const Capsule& capsule2)
{
	VECTOR distance;						//capsule同士の距離
	VECTOR velocity = VGet(0.0f, 0.0f, 0.0f);	//移動量
	float put;								//マイナスだったら重なってる
	float e = 1;							//反発係数
	float dis;								//ボール一つの押し戻し量
	//2つの円の中心座標の距離を求める
	distance = DistanceCapsuleToCapsule(capsule, capsule2);
	//重なっているかどうか(負の場合重なっている)
	put = VSize(distance) - capsule2.radius - capsule.radius;
	put = round(put);	//四捨五入
	//重なっていたら押し戻す
	if (put < 0)
	{
		//どれくらい押し戻すか
		dis = put * e;
		//正規化
		if (VSize(distance) != 0) distance = VNorm(distance);
		//どれくらい押し戻すか
		velocity = VScale(distance, -dis);
	}
	return velocity;
}

VECTOR Collision::CapsuleSlider(const Capsule& capsule,const Capsule&capsule2,VECTOR pos)
{
	float EPS = 5.0f;
	VECTOR velocity;
	VECTOR closest_point;
	float move_speed = 5.0f;
	Segment seg = { capsule2.pos,capsule2.pos2 };
	//現在地からカプセル軸（線分）への垂線の足（最短地点）を求める
	closest_point = R_Math::CalcOrthogonalProjection(pos, seg);
	//軸上の地点から終点へ向かうベクトルを算出
	velocity = VSub(seg.end, closest_point);
	//軸に沿った移動量を決定
	velocity = R_Math::SetMagnitude(velocity, move_speed);
	//軸上の地点から移動量分だけ進めた座標を暫定的な移動先とする
	velocity = VAdd(closest_point,velocity);
	//計算した移動先がほぼカプセルの終点と同じ座標かどうか判定
	if (R_Math::IsAlmostEqual(velocity.x, capsule2.pos2.x, EPS) && R_Math::IsAlmostEqual(velocity.y, capsule2.pos2.y, EPS) && R_Math::IsAlmostEqual(velocity.z, capsule2.pos2.z, EPS)) {
		VECTOR vel = VSub(seg.end, seg.start);
		if (VSize(vel) != 0)vel = VNorm(vel);
		//終点に到達した際、互いの半径分＋αだけさらに先へ押し出す
		velocity = VAdd(velocity, VScale(vel, capsule.radius+capsule2.radius+10.0f));
	}
	return velocity;
}

VECTOR Collision::ResolveOverlap(const VECTOR& currentPos, const MV1_COLL_RESULT_POLY* pPoly, float radius)
{
	float dist = VDot(VSub(currentPos,pPoly->Position[0]),pPoly->Normal);
	float overlap = radius - dist;

	if (overlap > 0) {
		return VScale(pPoly->Normal,overlap+0.01f);
	}
	return VGet(0,0,0);
}

StageCollResult Collision::StageToCapsule(Capsule& capsule, const int mapModelHandle)
{
	VECTOR current_pos =capsule.pos2;
	const float RADIUS = capsule.radius;
	const float STEP_HEIGHT =  30.0f ;
	const float GROUND_NORM_THRESHOLD = 0.6f;
	const int MAX_ITERATIONS = 10;

	int model_handle = mapModelHandle;
	float capsule_height = VSize(capsule.GetAxis().GetDirection());
	//DrawLine3D(current_pos, VGet(current_pos.x, current_pos.y + STEP_HEIGHT, current_pos.z),GetColor(255,255,0));
	//壁・天井との衝突判定と押し出し
	for (int itr = 0; itr < MAX_ITERATIONS; itr++) {
		VECTOR bottom = current_pos;
		VECTOR top = VAdd(bottom, VGet(0.0f, capsule_height, 0.0f));
		hit_polygon_dim = MV1CollCheck_Capsule(model_handle, -1, top, bottom, RADIUS);

		if (hit_polygon_dim.HitNum <= 0) {
			MV1CollResultPolyDimTerminate(hit_polygon_dim);
			break;
		}

		bool adjusted = false;
		for (int i = 0; i < hit_polygon_dim.HitNum; i++) {
			auto* p_poly = &hit_polygon_dim.Dim[i];
			//床ポリゴン（法線が上向き）かつステップ高以下の場合は壁として処理しない
			if (p_poly->Normal.y > GROUND_NORM_THRESHOLD)continue;

			float poly_max_y = GetPolyMaxY(p_poly);
			if (poly_max_y <= (bottom.y + STEP_HEIGHT))continue;

			//衝突しているか再判定し、押し出し計算
			if (HitCheck_Capsule_Triangle(top, bottom, RADIUS, p_poly->Position[0], p_poly->Position[1], p_poly->Position[2])) {
				//押し出しベクトルの計算
				VECTOR push_vec = ResolveOverlap(bottom, p_poly, RADIUS);
				push_vec.y = 0.0f;
				current_pos = VAdd(current_pos,push_vec);
				//速度成分から壁方向への移動を相殺
				//player.setVelocity(SlideVector(player.getVelocity(), p_poly->Normal));
				adjusted = true;
			}
		}
		MV1CollResultPolyDimTerminate(hit_polygon_dim);
		if (!adjusted)break;//全ての衝突が解消されたら抜ける
	}

	//接地判定（レイキャスト）
	bool is_grounded = false;
	float floor_y = -99999.0f;
	
	//足元から下方向へレイを飛ばす
	VECTOR ray_start = VAdd(current_pos,VGet(0.0f,STEP_HEIGHT,0.0f));
	VECTOR ray_end = VAdd(current_pos,VGet(0.0f,-30.0f,0.0f));

	MV1_COLL_RESULT_POLY_DIM ray_hit = MV1CollCheck_LineDim(mapModelHandle,-1,ray_start,ray_end);

	for (int i = 0; i < ray_hit.HitNum; i++) {
		auto* r_poly = &ray_hit.Dim[i];
		//法線が上を向いてかつ最も高い位置にあるポリゴンを採用
		if (r_poly->Normal.y > GROUND_NORM_THRESHOLD) {
			if (r_poly->HitPosition.y > floor_y) {
				floor_y = r_poly->HitPosition.y;
				is_grounded = true;
			}
		}
	}

	
	MV1CollResultPolyDimTerminate(ray_hit);
	StageCollResult result = {};
	result.current_pos = current_pos;
	result.floor_y = floor_y;
	result.is_grounded = is_grounded;
	return result;
}

float Collision::GetPolyMaxY(const MV1_COLL_RESULT_POLY* pPoly) {
	float max_y = pPoly->Position[0].y;
	if (pPoly->Position[1].y > max_y) max_y = pPoly->Position[1].y;
	if (pPoly->Position[2].y > max_y) max_y = pPoly->Position[2].y;
	return max_y;
}
