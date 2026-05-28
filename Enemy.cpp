#include"Config.h"
#include"Animation.h"
#include"Collision.h"
#include"Baton.h"
#include "Enemy.h"
//#define DEBUG

namespace EnemyConfig {
	const float EPSILON = 0.0001f;
	static constexpr float DOWN_SINK_SPEED = 0.30f;
	static constexpr float FADE_OUT_SPEED = 0.005f;
	static constexpr float CAPSULE_HEIGHT = 100.0f;
	static constexpr float STOP_DISTANCE_MERGIN = 3.0f;
	static constexpr float WARP_DISTANCE_THRESHOLD = 1500.0f;
	static constexpr float WARP_FORWARD_OFFSET = 1000.0f;
	const float FLOOR_MIN_Y = 5.0f;
	const float KNOCKBACK_FRICTION = 0.8f;//吹き飛ばしの減衰率
	const float STOOP_THRESHOLD = 0.01f;//停止とみなす速度しきい値
	const int WHITE = GetColor(255, 255, 255);
	const int RED = GetColor(255, 0, 0);
	const float ATTACK_INTERVAL = 60.0f;//再攻撃までの待ち時間（フレーム単位）
}

Enemy::Enemy()
	:move_speed(10.0f),
	head_pos(VGet(0.0f, 0.0f, 0.0f)),
	model_rotate_angle(MGetRotY(0)),
	capsule_radius(30.0f),
	is_blend_anim(false),
	right_hand_pos(VGet(0.0f, 0.0f, 0.0f))
{
	//インスタンス生成
	animation = std::make_shared<Animation>("model/enemy/swat.mv1");
	baton = std::make_shared<Baton>();

	//ボーン（フレーム）の検索
	head_index = animation->SearchFrame("mixamorig:Head");
	right_hand_index = animation->SearchFrame("mixamorig:RightHandMiddle1");

	Init(VGet(-830.0f, 5.0f, -7910.0f));
}

Enemy::~Enemy()
{
}

Capsule Enemy::getBatonCollCapsule() const
{
	return baton->getCollCapsule();
}

bool Enemy::getIsPlayAnimEnd() const
{
	return animation->getIsPlayEnd();
}

void Enemy::Init(VECTOR startPos)
{
	pos = startPos;
	next_pos = VGet(0.0f, 0.0f, 0.0f); // 前回の移動速度をクリア
	impact_dir = VGet(0.0f, 0.0f, 0.0f);
	model_rotate_angle = MGetRotY(0);
	forward = VGet(0.0f, 0.0f, 1.0f);

	//フラグ類の初期化
	is_alive = true;
	is_finished = false;
	is_hit = false;
	is_touch_ground = false;
	is_attack = false;
	is_move = false;
	is_special_range_hit = false;

	//アニメーション・状態の初期化
	current_anim_state = AnimState::IDLE;
	prev_anim_state = AnimState::IDLE;
	current_state = State::IDLE;
	prev_state = State::IDLE;

	opacity = 1.0f;                             // 透明度を戻す
	animation->SetOpacityRate(opacity);         // アニメーションクラスに反映
	animation->SetIdleState();

	MV1SetPosition(animation->getModelHandle(), pos); // モデルの座標を即座に更新
	UpdateFramePosition();
	coll_cap = { head_pos,pos,capsule_radius };
	coll_cap_floor = { VAdd(pos,VGet(0.0f,EnemyConfig::CAPSULE_HEIGHT,0.0f)),pos,capsule_radius };

}

void Enemy::Draw()
{
	baton->Draw();
	animation->SetModelMatrix(pos, model_rotate_angle);
	animation->DrawModel();
}
void Enemy::IsHitColl(const VECTOR playerPos, const Capsule& cap1)
{
	static Collision coll;

	Capsule capsule;
	Sphere sphere;
	capsule = { playerPos,VAdd(playerPos,VGet(0,100,0)) };
	sphere = { cap1.pos2,200.0f };
	is_special_range_hit = coll.CapsuleToSphere(capsule, sphere);
}
void Enemy::Update(const ICharacter& target, Collision& collision, const float& deltaTime, const int& mapModelHandle)
{
	if (is_finished) return;
	prev_state = current_state;
	prev_anim_state = current_anim_state;

	UpdateFramePosition();
	is_blend_anim = true;
	next_pos = VGet(0.0f, next_pos.y, 0.0f);
	is_move = false;

	//既に消滅フラグが立っている場合は以降の処理をスキップ
	if (is_finished)return;

	//ダウン状態時は専用の更新処理を行い、他の行動をスキップ
	if (current_state == State::DOWN) {
		UpdateDownState();
		return;
	}

	//被弾衝撃状態でなければ移動処理を実行
	if (current_state != State::IMPACT) {
		Move(target.getPos(),target.getCollCapsule().radius);
	}


	//自身の衝突判定カプセルを更新し、プレイヤーとの接触を判定
	coll_cap = { head_pos,pos,capsule_radius };
	coll_cap_floor = { VAdd(pos,VGet(0.0f,EnemyConfig::CAPSULE_HEIGHT,0.0f)),pos,capsule_radius };
	is_hit = collision.CapsuleToCapsule(target.getCollCapsule(), coll_cap);

	UpdateStateTransitions(target);

	//被弾衝撃状態の物理挙動を処理
	if (current_state == State::IMPACT) {
		ProcessImpactState();
	}

	ApplyPhysics(deltaTime);
	UpdateFramePosition();

	ApplyFinalPosition(mapModelHandle, collision);
	JudgeCollision(target.getCollCapsule(), collision);
	//アニメーションを更新
	animation->Update(static_cast<unsigned int>(current_anim_state), static_cast<unsigned int>(prev_anim_state), is_blend_anim);

	UpdateRotation();

	//バトンの位置と向きを更新
	baton->Update(right_hand_pos, forward);
#ifdef DEBUG
	DrawCapsule3D(coll_cap.pos, coll_cap.pos2, coll_cap.radius, 16, WHITE, WHITE, false);
	DrawCapsule3D(coll_cap_floor.pos, coll_cap_floor.pos2, coll_cap_floor.radius, 16, RED, RED, false);
	DrawLine3D(pos, VAdd(pos, forward), RED);
	printfDx("en_pos(%.1f,%.1f,%.1f)\n", pos.x, pos.y, pos.z);
	//printfDx("current_state:%d\n",current_state);

#endif // DEBUG
}

//ダウン状態の更新：アニメーション終了後の消滅（落下とフェードアウト）
void Enemy::UpdateDownState()
{
	animation->Update(static_cast<unsigned int>(current_anim_state), static_cast<unsigned int>(prev_anim_state), is_blend_anim);
	if (animation->getIsPlayEnd()) {
		pos.y -= EnemyConfig::DOWN_SINK_SPEED;
		opacity -= EnemyConfig::FADE_OUT_SPEED;
		if (opacity < 0.0f)opacity = 0.0f;
		animation->SetOpacityRate(opacity);
		//一定値以下まで落下または透明化したら消滅フラグをセット
		if (pos.y < -EnemyConfig::CAPSULE_HEIGHT || opacity <= 0.0f) {
			is_finished = true;
		}
	}
	//バトンの位置と向きを更新
	baton->Update(right_hand_pos, forward);
}

//特定のボーン（頭部・右手）のワールド座標を更新
void Enemy::UpdateFramePosition()
{
	head_pos = animation->getLocalFramePos(head_index);
	right_hand_pos = animation->getLocalFramePos(right_hand_index);
}

//状態遷移判定：プレイヤーの行動に応じた被弾や攻撃・移動への遷移
void Enemy::UpdateStateTransitions(const ICharacter& target)
{
	static Collision coll;

	if (current_state == State::DOWN)return;
	if (current_state == State::IMPACT)return;

	//必殺技によるダウン判定：プレイヤーが必殺技かつ再生終了タイミングで範囲内にいる場合
	if (target.getCurrentState() == State::SPECIAL_ATTACK &&
		target.getIsPlayAnimEnd() && is_special_range_hit) {
		current_state = State::DOWN;
		current_anim_state = AnimState::DOWN;
		is_alive = false;
		return;
	}
	//蹴り被弾判定：プレイヤーの右足スフィアとの接触及び高さ条件のチェック
	if (target.getCurrentState() == State::KICK) {

		Sphere pl_sphere = { target.getRightLegPos(),target.getSphereRadius() };
		if (!coll.CapsuleToSphere(coll_cap, pl_sphere) || pl_sphere.pos.y <= VSize(target.getCollCapsule().GetAxis().GetDirection()) / 4)return;

		current_state = State::IMPACT;
		current_anim_state = AnimState::IMPACT;

		//ノックバック方向と初期速度の計算
		VECTOR knockback_vec = VSub(pos, target.getPos());
		knockback_vec.y = 0.5f;
		if (VSize(knockback_vec) > EnemyConfig::EPSILON)impact_dir = VNorm(knockback_vec);
		else impact_dir = VGet(0.0f, 0.0f, 0.0f);
		impact_dir = VScale(impact_dir, 80.0f);

		return;
	}

	//衝撃状態継続中は以下の自働遷移をスキップ
	if (current_state == State::IMPACT)return;


	if (current_state == State::ATTACK) {
		if (animation->getIsPlayEnd()) {
			current_state = State::IDLE;
			current_anim_state = AnimState::IDLE;
			attack_cool_time = EnemyConfig::ATTACK_INTERVAL;
		}
		return;
	}

	if (attack_cool_time > 0.0f) {
		attack_cool_time -= 1.0f;
	}


	//攻撃・走行・待機の優先順位に基づく遷移
	is_attack = getIsSpecialRangeHit();
	if (is_attack && attack_cool_time <= 0.0f) {
		current_state = State::ATTACK;
		current_anim_state = AnimState::SLASH;
	}
	else if (is_move) {
		current_anim_state = AnimState::RUN;
		current_state = State::RUN;
	}
	else {
		current_state = State::IDLE;
		current_anim_state = AnimState::IDLE;
	}

}
//被弾衝撃状態の物理挙動：ノックバック移動と摩擦による減衰
void Enemy::ProcessImpactState()
{
	//アニメーション終了時に待機状態へ復帰
	if (animation->getIsPlayEnd()) {
		current_state = State::IDLE;
		current_anim_state = AnimState::IDLE;
		impact_dir = VGet(0.0f, 0.0f, 0.0f);
		return;
	}
	next_pos = VAdd(next_pos, impact_dir);
	impact_dir = VScale(impact_dir, EnemyConfig::KNOCKBACK_FRICTION);
	//速度がしきい値以下になったら停止
	if (VSize(impact_dir) < EnemyConfig::STOOP_THRESHOLD)impact_dir = VGet(0.0f, 0.0f, 0.0f);
}
//プレイヤーを追尾する移動計算
void Enemy::Move(const VECTOR& playerPos,const float& playerCollCapsuleRadius)
{
	VECTOR moving_dir = VSub(playerPos, pos);
	moving_dir.y = 0.0f;
	//プレイヤーとの距離が一定以下（接触範囲内）なら移動停止
	if (VSize(moving_dir) <= (playerCollCapsuleRadius+ coll_cap.radius + EnemyConfig::STOP_DISTANCE_MERGIN)) {
		is_move = false;
		return;
	}

	is_move = true;
	VECTOR normarize = VGet(0.0f, 0.0f, 1.0f);
	if (VSize(moving_dir) > EnemyConfig::EPSILON)normarize = VNorm(moving_dir);
	//移動量と回転角の計算
	moving_dir = VScale(normarize, move_speed);
	next_pos = VAdd(next_pos, moving_dir);
	float rotate_angle = atan2f(next_pos.x, next_pos.z);
	model_rotate_angle = MGetRotY(rotate_angle + DX_PI_F);
}
//カプセル同士の押出処理
void Enemy::Extrusion(const Capsule& capsule, const Capsule& capsule2, Collision& coll)
{
	VECTOR velocity = coll.ExtrusionCapsuleToCapsule(capsule, capsule2);
	if (VSize(velocity) != 0)velocity = VNorm(velocity);
	velocity = VScale(velocity, 10);
	pos = VAdd(pos, velocity);
}
//攻撃状態への遷移判定（範囲チェック）
void Enemy::JudgeAttack()
{
	is_attack = getIsSpecialRangeHit();
	if (!is_attack) return;
	current_state = State::ATTACK;
	current_anim_state = AnimState::SLASH;
}
//モデルの前方方向ベクトルの更新
void Enemy::UpdateRotation()
{
	if (VSize(forward) != 0)forward = VNorm(forward);
	forward = VTransform(VGet(0.0f, 0.0f, -1.0f), model_rotate_angle);
}

//簡易的な重力演算と地面接地判定
void Enemy::ApplyPhysics(float deltaTime)
{
	float time_scale = deltaTime * 60.0f;
	if (!is_touch_ground) {
		next_pos.y -= Config::GRAVITY * time_scale;
		if (next_pos.y <= -20.0f) {
			next_pos.y = -20.0f;
		}
	}
	else if (next_pos.y < 0.0f) {
		next_pos.y = -0.1f;
	}

	if (pos.y + (next_pos.y * time_scale) <= -50.0f)is_alive = false;
	pos = VAdd(pos, VScale(next_pos, time_scale));
}

void Enemy::ApplyFinalPosition(const int& mapModelHandle, Collision& collision)
{
	coll_cap_floor.pos = VAdd(pos, VGet(0.0f, EnemyConfig::CAPSULE_HEIGHT, 0.0f));
	coll_cap_floor.pos2 = pos;
	coll_cap = { head_pos,pos,capsule_radius };

	StageCollResult result = collision.StageToCapsule(coll_cap_floor, mapModelHandle);
	//最終結果をplayerに反映
	if (result.is_grounded) {
		is_touch_ground = true;
		pos = VGet(result.current_pos.x, result.floor_y, result.current_pos.z);
	}
	else {
		is_touch_ground = false;
		pos = result.current_pos;//壁に押し出された後の座標を適用
	}
}

//プレイヤーとの衝突（重なり）判定と押し出しの実行
void Enemy::JudgeCollision(const Capsule& playerCapsule, Collision& coll)
{
	Capsule capsule = coll_cap;
	Capsule capsule2 = playerCapsule;
	bool hit = coll.CapsuleToCapsule(capsule, capsule2);
	if (hit) {
		Extrusion(capsule, capsule2, coll);
	}
}
//走行状態の遷移判定
void Enemy::JudgeRun()
{
	if (!is_move) return;
	current_anim_state = AnimState::RUN;
	current_state = State::RUN;
}
