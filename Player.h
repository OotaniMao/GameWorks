#pragma once
#include<memory>
#include<vector>
#include"R_Math.h"
#include"CharacterInterface.h"

class Afterimage;
class ICamera;
class Animation;
class IInput;
class Collision;
class Gauge;

enum  class AnimState:int;
enum MovieState;


class Player:public ICharacter
{
public:
	
	Player(std::shared_ptr<Collision>& coll, std::shared_ptr<Gauge>& sp_moive_gauge,std::shared_ptr<IInput>& input_ptr,std::shared_ptr<ICamera>&camera_ptr);		//コンストラクタ
	~Player() = default;

	//ゲッター
	VECTOR		getPos()				const override{ return status.pos; }
	VECTOR		getForward()			const { return forward; }
	State		getCurrentState()		const override { return status.current_state; }
	MATRIX		getModelRotateAngle()	const { return model_rotate_angle; }
	VECTOR		getVelocity()			const { return status.velocity; }
	bool		getIsTouchGround()		const { return status.is_touch_ground; }

	Capsule		getCollCapsule()		const override{ return coll_cap; }
	Capsule		getCollCapsuleFloor()	const { return coll_cap_floor; }
	float		getSphereRadius()		const override{ return sphere_radius; }
	
	bool		getIsHit()				const { return is_hit; }
	bool		getIsPlaySpecialMove()	const { return is_play_special_move; }
	bool		getIsPlayAnimEnd()		const override;
	MovieState	getCurrentMovieState()	const { return current_movie_state; }
	bool		getIsHpMin()			const;
	float		getHPNum()				const;


	bool getIsAlive()const override { return false; }
	Capsule getBatonCollCapsule() const override { return { VGet(0,0,0),VGet(0,0,0),0 }; }
	bool getIsSpecialRangeHit() const override { return false; }


	//フレーム座標取得
	VECTOR		getRightHandPos()		const { return right_hand_pos; }
	VECTOR		getRightLegPos()		const override{ return right_leg_pos; }
	VECTOR		getHeadPos()			const { return head_pos; }
	VECTOR		getHipsPos()			const { return hips_pos; }

	//セッター
	void setPos(const VECTOR& _new_pos) { status.pos = _new_pos; }
	void setVelocity(const VECTOR& _new_velocity) { status.velocity = _new_velocity; }
	void setIsTouchGround(bool _ground) { status.is_touch_ground = _ground; }

	/// @brief 初期化
	void Init();
	/// @brief 描画 
	void DrawModel();
	/// @brief 状態制御 
	void Update(float timeScale,const std::vector<std::shared_ptr<ICharacter>>& enemies, float deltaTime,const int&mapModelHandle);

private:
	std::shared_ptr<Gauge>hp_gauge;//体力バー
	std::shared_ptr<Animation>animation;//モデルのアニメーション
	std::shared_ptr<Afterimage>after_image;
	std::shared_ptr<Collision>collision;
	std::shared_ptr<Gauge>special_move_gauge;
	std::shared_ptr<IInput>input;
	std::shared_ptr<ICamera>camera;

	Square square;

	struct Status {
		VECTOR pos=VGet(0.0f,0.0f,0.0f);					//player位置
		VECTOR velocity;
		State current_state=State::S_IDLE;		//現在のplayerの状態
		AnimState anim_state;
		bool is_touch_ground=false;		//接地しているかどうか
	}status;

	struct FrameIndices {
		int right_hand, right_leg, hips,head;
	}frames;

	float move_speed;			//移動速度
	float jump_power;			//ジャンプの加速度
	float air_dash_power;		//エアダッシュの加速度
	bool is_move;				//移動したかどうか

	bool is_hit;				//当たったかどうか
	bool has_used_air_dash;
	bool is_play_special_move;	//必殺技の描画を再生するか
	
	Capsule coll_cap;			//当たり判定用カプセル
	Capsule coll_cap_floor;
	float sphere_radius;		//当たり判定の半径(球)

	AnimState current_anim_state;		//現在のplayerの状態
	AnimState prev_anim_state;			//過去のplayerの状態
	State prev_state;			//過去のplayerの状態


	MATRIX model_rotate_angle;	//モデルの回転角度
	VECTOR forward;				//playerの前方方向
	float rotate_angle;			//移動時の固定回転角度
	bool is_blend_anim;			//アニメーションのブレンドをするかどうか

	MovieState current_movie_state;

	VECTOR right_hand_pos;			//右手元の座標
	VECTOR right_leg_pos;		//	右足の座標
	VECTOR hips_pos;
	VECTOR head_pos;
	
private:
	void UpdateFramePosition();

	/// @brief 必殺技関連
	void JudgeSpecialMove(const bool& cameraIsSpecialMoveEnd, const int& cameraInputKeyNum,const bool& is_special_range_hit);
	/// @brief 走るanimationへ移行するかどうか 
	void JudgeRun();
	/// @brief 蹴るanimationへ移行するかどうか 
	void JudgeKick();
	/// @brief 移動制御
	void Move(const VECTOR& cameraToTargetDir, float timeScale);
	/// @brief 必殺技時にblendモードを変更する
	void ChangeBlendMode();
	/// @brief 攻撃が当たったかどうかの処理
	void JudgeAttackCollision(const Capsule& enemy_batton_cap);
	/// @brief レールとの衝突判定処理
	void JudgeRailCollision();
	void ResolveCollision(const int&mapModelHandle,const std::vector<std::shared_ptr<ICharacter>>& enemies);
	void UpdateGroundStatus();
	void HandleInput(const bool& cameraIsSpecialMoveEnd, const int& cameraInputKeyNum,const std::vector<std::shared_ptr<ICharacter>>& enemies, float delta_time);
	void ApplyPhysics(float delta_time);

	void UpdateHpGaugeColor();
	
};
