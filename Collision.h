#pragma once
#include"R_Math.h"
#include<vector>

class Player;
class Map;

typedef struct StageCollResult {
	float floor_y;
	VECTOR current_pos;
	bool is_grounded;
}StageCollResult;

class Collision
{
private:
	static constexpr float MIN_VERTICAL_Y = 0.2f;//垂直判定のしきい値
	const float MAX_HIT_COLL = 2048;

	MV1_COLL_RESULT_POLY_DIM hit_polygon_dim;
	bool is_hit_floor;
	
	std::vector<MV1_COLL_RESULT_POLY*> wall;//壁ポリゴンと判断されたポリゴンの構造体アドレスを保存するポインタ配列
	std::vector<MV1_COLL_RESULT_POLY* >floor;//床ポリゴンと判断されたポリゴンの構造体アドレスを保存するポインタ配列
	MV1_COLL_RESULT_POLY* poly;//ポリゴンの構造体にアクセスする用のポインタ

public:
	Collision();
	~Collision();

	bool getIsHitFloor()const { return is_hit_floor; }

	/// @brief カプセルと球の当たり判定
	/// @return 衝突判定結果　true:衝突　false:衝突なし
	bool CapsuleToSphere(const Capsule& capsule, const Sphere& sphere);
	/// @brief カプセルとカプセルの当たり判定
	/// @return 衝突判定結果　true:衝突　false:衝突なし
	bool CapsuleToCapsule(const Capsule& capsule, const Capsule& capsule2);
	/// @brief カプセルとカプセルの最短距離
	/// @return　カプセルとカプセルの最短距離 
	VECTOR DistanceCapsuleToCapsule(const Capsule& capsule, const Capsule& capsule2);
	/// @brief カプセルとカプセルの押し出し
	/// @return 押し出し量
	VECTOR ExtrusionCapsuleToCapsule(const Capsule& capsule, const Capsule& capsule2);
	/// @brief カプセルの軸上を終点に向かってスライドする座標を計算する
	/// @param capsule  操作対象（プレイヤー）のカプセル
	/// @param capsule2 対象物（ギミック）のカプセル
	/// @return VECTOR  移動後の座標
	VECTOR CapsuleSlider(const Capsule& capsule,const Capsule&capsule2,VECTOR pos);
	

	/// @brief ステージとカプセルの当たり判定処理
	StageCollResult StageToCapsule(Capsule& capsule, const int mapModelHandle);

private:
	
	float GetPolyMaxY(const MV1_COLL_RESULT_POLY* pPoly);
	VECTOR ResolveOverlap(const VECTOR& currentPos,const MV1_COLL_RESULT_POLY*pPoly,float radius);
};