#pragma once
#include<cmath>

struct Vec2 {
	float x;
	float y;
};

struct Square {
	Vec2 pos;
	Vec2 size;
};


/// @brief 線分
/// @param start 始点
/// @param end 終点
struct Segment {
	VECTOR start;
	VECTOR end;

	Segment(VECTOR s=VGet(0,0,0),VECTOR e=VGet(0,0,0)):start(s),end(e){}

	VECTOR GetDirection()const { return VSub(end,start); }
	float GetLength()const { return VSize(GetDirection()); }
};


/// @brief 球
///　@param pos 中心座標
///　@param radius 半径
struct Sphere {
	VECTOR pos;
	float radius;

	Sphere(VECTOR p=VGet(0,0,0),float r=0.0f):pos(p),radius(r){}
};

/// @brief カプセル
/// @param pos 座標１
/// @param pos2 座標２
/// @param radius 半径
struct Capsule {
	VECTOR pos;
	VECTOR pos2;
	float radius;

	Capsule(VECTOR p1 = VGet(0, 0, 0), VECTOR p2 = VGet(0, 0, 0), float r = 0.0f)
		:pos(p1), pos2(p2), radius(r) {}

	Segment GetAxis()const { return Segment(pos, pos2); }
};



namespace R_Math {
	/// @brief 最小値と最大値の間に収める
	/// @param num クランプしたい数字
	/// @return クランプされた数字
	inline float Clamp(float min, float max, float num)
	{
		if (num < min)return min;
		if (num > max) return max;
		return num;
	}

	/// @brief 大体同じかどうか
	/// @return true:同じ　false:異なる
	inline bool IsAlmostEqual(float a, float b, float epsilon=0.001f)
	{
		return std::abs(a - b) < epsilon;
	}
	/// @brief 線形補間
	/// @param start 始点
	/// @param end 終点
	/// @param t 補間係数(0～1の間)
	inline VECTOR Lerp(const VECTOR& start, const VECTOR& end, float t)
	{
		return VAdd(VScale(VSub(end, start), t), start);
	}
	/// @brief ベクトルの大きさを任意の値に変更する
	inline VECTOR SetMagnitude(const VECTOR& vec, float size)
	{
		float length = VSize(vec);
		if (length <= 0.001f)return VGet(0,0,0);
		return VScale(vec, size/length);
	}

	/// @brief 線分と点の最近接点の計算
	/// @return 線分と点の最短距離
	inline VECTOR CalcDistancePointToLineSegment(const VECTOR& point, const Segment& seg)
	{
		VECTOR seg_vec = seg.GetDirection();							//線分ベクトル（終点ー始点）
		float denominator = VDot(seg_vec, seg_vec);						//分母=線分の長さ^2
		if (denominator == 0.0f)return VSub(seg.start, point);			//線分が点だった場合点との距離を返す
		float inner_product = VDot(seg_vec, VSub(point, seg.start));	//内積
		float t = inner_product / denominator;							//射影比
		//最近接点は線分の始点側にある
		if (t < 0.0f) {
			return VSub(seg.start, point);
		}
		//最近接点は線分の終点側にある
		else if (t > 1.0f) {
			return VSub(seg.end, point);
		}
		else {
			//最近接点は線分上に存在する
			VECTOR orthographic = VScale(seg_vec, t);					//正射影ベクトル（始点から最近接点）
			VECTOR orthgonal_pos = VAdd(seg.start, orthographic);		//射影点座標
			return VSub(orthgonal_pos, point);
		}
	}
	/// @brief 射影点の座標を求める計算(直線上)
	/// @param point 点
	/// @param seg 線分
	/// @return 射影点座標
	inline VECTOR CalcOrthogonalProjection(const VECTOR& point, const Segment& seg)
	{
		VECTOR line_segment = VSub(seg.end, seg.start);						//線分ベクトル（終点ー始点）
		float denominator = VDot(line_segment, line_segment);				//分母=線分の長さ^2
		if (denominator == 0.0f)return VSub(seg.start, point);				//線分が点だった場合点との距離を返す
		float inner_product = VDot(line_segment, VSub(point, seg.start));	//内積
		float t = inner_product / denominator;								//射影比
		//最近接点は線分上に存在する
		VECTOR orthographic = VScale(line_segment, t);						//正射影ベクトル（始点から最近接点）
		VECTOR orthgonal_pos = VAdd(seg.start, orthographic);				//射影点座標

		return orthgonal_pos;
	}
	/// @brief 線分と線分の最短距離の計算
	/// @return  線分と線分の最短距離方向　線分2→線分1
	inline VECTOR CalcSegmentToSegment(const Segment& segment, const Segment& segment2)
	{
		VECTOR segA =segment.GetDirection();				//線分１の方向ベクトル
		VECTOR segB = segment2.GetDirection();				//線分２の方向ベクトル
		VECTOR segC = VSub(segment.start, segment2.start);	//segment2.start→segment.startのベクトル

		float dot_segA = VDot(segA, segA);	//線分１の距離の2乗
		float dot_segB = VDot(segB, segB);	//線分２の距離の2乗
		float dot_segAB = VDot(segA, segB);	//線分１と線分２の方向ベクトルの内積
		float dot_segAC = VDot(segA, segC);	//線分１と線分３の方向ベクトルの内積
		float dot_segBC = VDot(segB, segC);	//線分２と線分３の方向ベクトルの内積

		float denominator = dot_segA * dot_segB - dot_segAB * dot_segAB;	//2線分の方向ベクトルから導かれる分母
		float s, t;	//ベクトル係数(0<=s,t<=1)
		float MIN = 0.0f;
		float MAX = 1.0f;
		float EPS = 1.0e-7f;
		//2つの線分がほぼ平行なとき
		if (denominator <= EPS) {
			s = MIN;//線分１の始点を基準とする
			dot_segB > 0.0f ? t = dot_segBC / dot_segB : t = MIN;//0除算回避
		}
		else {//平行でない時
			//クレイマーの公式より
			s = (dot_segAB * dot_segBC - dot_segB * dot_segAC) / denominator;//線分1上の最近接点係数
			t = (dot_segA * dot_segBC - dot_segAB * dot_segAC) / denominator;//線分2上の最近接点係数
		}

		//ベクトル係数を[0,1]の範囲にそろえる（線分上に限定する）
		s = Clamp(MIN, MAX, s);
		t = Clamp(MIN, MAX, t);

		//ベクトル係数に対する各線分上の最近接点を求める
		VECTOR p1 = VAdd(segment.start, VScale(segA, s));//線分１上の最近接点
		VECTOR p2 = VAdd(segment2.start, VScale(segB, t));//線分２上の最近接点
		//最近接点同士を結ぶベクトル（最短距離方向）
		return VSub(p1, p2);
	}

	/// @brief ベクトルに沿って滑らせる
	/// @param vec  移動させたいオブジェクトのベクトル
	/// @param vec2 滑らせたいベクトル
	inline VECTOR SlideVector(VECTOR vec, VECTOR vec2)
	{
		float dot = VDot(vec, vec2);
		if (dot < 0)return VSub(vec, VScale(vec2, dot));
		return vec;
	}
}