#pragma once
#include<memory>
#include<vector>
#include"R_Math.h"
#include"CharacterInterface.h"

class Animation;
class Collision;
//class Effect;
class Baton;

enum class State :int;
enum class AnimState:int;


class Enemy:public ICharacter
{
private:
	const float FLOOR_MIN_Y = 5.0f;
	const float KNOCKBACK_FRICTION = 0.8f;//吹き飛ばしの減衰率
	const float STOOP_THRESHOLD = 0.01f;//停止とみなす速度しきい値
	const int WHITE = GetColor(255, 255, 255);
	const int RED = GetColor(255, 0, 0);

	std::shared_ptr<Animation>animation;	//アニメーション
	std::shared_ptr<Baton>baton;			//警棒
	
	VECTOR pos;								//モデルの位置
	VECTOR next_pos;						//フレームの移動増分
	VECTOR forward;							//モデルの前方ベクトル
	VECTOR impact_dir;						//吹き飛ばしベクトル（慣性用）
	MATRIX model_rotate_angle;				//モデルの回転行列
	float move_speed;						//移動速度
	

	State current_state;					//現在の状態
	State prev_state;						//ひとつ前の状態
	AnimState current_anim_state;			//現在の状態
	AnimState prev_anim_state;				//ひとつ前の状態

	Capsule coll_cap;						//自身の当たり判定用カプセル
	Capsule coll_cap_floor;
	float capsule_radius;					//カプセルの半径
	VECTOR head_pos;						//ボーンから取得した頭の位置
	VECTOR right_hand_pos;					//ボーンから取得した右手の位置
	int head_index;							//頭のフレーム番号
	int right_hand_index;					//右手のフレーム番号

	float opacity;							//モデルの透明度  消滅演出用
	bool is_hit;							//playerと接触したかどうか
	bool is_alive;							//生存するかどうか
	bool is_attack;							//攻撃可能かどうか
	bool is_blend_anim;						//アニメーションをブレンドするかどうか
	bool is_move;							//移動したかどうか
	bool is_touch_ground;					//接地しているかどうか
	bool is_special_range_hit;				//必殺技の範囲内に入ったかどうか
	bool is_finished;						//削除していい状態かどうか

	bool is_active = false;

	float attack_cool_time = 0.0f;
	const float ATTACK_INTERVAL = 60.0f;//再攻撃までの待ち時間（フレーム単位）

public:
	Enemy();
	~Enemy();

	//ゲッター
	VECTOR getPos()const override{ return pos; }
	Capsule getCollCapsule()const override{ return coll_cap; }
	State getCurrentState()const override { return current_state; }
	Capsule getBatonCollCapsule()const;
	bool getIsAlive()const { return is_alive; }
	bool getIsSpecialRangeHit()const { return is_special_range_hit; }
	bool getIsFinished()const { return is_finished; }

	float getSphereRadius()	const override { return 0; }
	VECTOR getRightLegPos()const override { return VGet(0,0,0); }
	bool getIsPlayAnimEnd()const override;

	void SetIsActive(bool flag) { is_active = flag; }
	bool getIsActive()const { return is_active; }

	/// @brief 初期化
	void Init(VECTOR start_pos);
	/// @brief エネミーの状態制御
	void Update(const ICharacter& target,  Collision& collision, const float &deltaTime/*,const Effect&effect*/, const int& mapModelHandle);
	/// @brief 描画
	void Draw();
	void IsHitColl(const VECTOR playerPos, const Capsule& cap1);
	/// @brief 押し出し処理
	void Extrusion(const Capsule& capsule, const Capsule& capsule2, Collision& coll);
private:
	//状態遷移の判断
	void UpdateStateTransitions(const ICharacter&target);
	//吹き飛ばし処理
	void ProcessImpactState();
	//ダウン・消滅演出
	void UpdateDownState();
	//ボーン座標の取得
	void UpdateFramePosition();
	//モデルの向きの更新
	void UpdateRotation();
	//重力・接地処理
	void ApplyPhysics(float delta_time);

	void ApplyFinalPosition(const int& mapModelHandle, Collision& collision);
	/// @brief 移動制御
	void Move(const VECTOR& playerPos, const float& playerCollCapsuleRadius);
	/// @brief 衝突判定処理を行うかどうか
	void JudgeCollision(const Capsule& playerCapsule, Collision& coll);
	/// @brief 走り状態かどうか
	void JudgeRun();
	/// @brief　攻撃状態かどうか
	void JudgeAttack();

};
