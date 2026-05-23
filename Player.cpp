#include"DxLib.h"
#include"Config.h"
#include"Model.h"
#include"Animation.h"
#include"Collision.h"
#include"InputInterface.h"
#include"Camera.h"
#include"Gauge.h"
#include"AfterImage.h"
#include"Player.h"
#include"Movie.h"
#include"math.h"
//#define DEBUG

namespace PlayerConfig {
	static const VECTOR INITIAL_POS = VGet(-930, 5.0f, -7910.0f);//初期位置
	static constexpr float GRAVITY_LIMIT = -20.0f;//重力加速度の限界値
	static constexpr float MAX_HP = 150.0f;//最大体力値
	static constexpr float MIN_HP = 25.0f;//最小体力値
	static constexpr float JUDGE_STILL_SPEED_THRESHOLD = 0.1f;//静止速度判定の閾値
	static constexpr float AFTERIMAGE_LIGHT_OFFSET = 30.0f;//残像ライト位置のオフセット
	static const VECTOR UI_POS_OFFSET = VGet(0.0f,200.0f,0.0f);//入力促しのUIオフセット
	static constexpr float STICK_INPUT_THRESHOLD = 0.5f;//スティック入力の閾値
	static constexpr float RAIL_RADIUS = 20.0f;//レールの半径
	static constexpr float RAIL_POS = -1150.0f;//レールの座標
	static constexpr int ALPHA = 80;//ブレンドモードのアルファ値
	static constexpr int SPECIAL_MOVE_COLOR = 0x78B478;//必殺技の時の画面の色
	static constexpr float DAMAGE = 1.0f;//被ダメージ量
	static constexpr float FPS = 60.0f;//FPS換算係数
	static constexpr float FALL_DAMAGE_HEIGHT = -50.0f;//落下ダメージを受ける高さ
	static constexpr float FALL_DAMAGE_AMOUNT = 3.0f;//落下時のダメージ量
}
Player::Player(std::shared_ptr<Collision>& coll,std::shared_ptr<Gauge>&sp_moive_gauge,std::shared_ptr<IInput>&input_ptr, std::shared_ptr<ICamera>& camera_ptr)
	:move_speed(10.0f),
	sphere_radius(20),
	forward(VGet(0.0f,0.0f,1.0f)),
	is_blend_anim(true)
{
	square.pos = { 0.0f,130.0f };
	square.size = { 100.0f,20.0f };

	//インスタンス生成
	after_image = std::make_shared<Afterimage>();
	hp_gauge = std::make_shared<Gauge>(PlayerConfig::MAX_HP, PlayerConfig::MAX_HP, square);
	animation = std::make_shared<Animation>("model/player/playerModel3.mv1");
	special_move_gauge = sp_moive_gauge;
	collision = coll;
	input = input_ptr;
	camera = camera_ptr;

	//ボーン（フレーム）の検索
	frames.right_hand = animation->SearchFrame("mixamorig:RightHandMiddle1");
	frames.right_leg= animation->SearchFrame("mixamorig:RightFoot");
	frames.hips= animation->SearchFrame("mixamorig:Hips");
	frames.head = animation->SearchFrame("mixamorig:Head");

	Init();
}

bool Player::getIsPlayAnimEnd() const 
{
	bool tmp = animation->getIsPlayEnd();
	return tmp;
}

bool Player::getIsHpMin() const
{
	if (hp_gauge->getCurrentNum() <= PlayerConfig::MIN_HP) {
		return true;
	}
	return false;
}

float Player::getHPNum() const
{
	return hp_gauge->getCurrentNum();
}

void Player::Init() 
{
	//ステータスの初期化
	status.pos = PlayerConfig::INITIAL_POS;
	status.current_state = State::S_IDLE;
	status.is_touch_ground = false;
	status.velocity = VGet(0.0f, 0.0f, 0.0f);

	//アニメーション・状態の初期化
	prev_state=State::S_NONE;
	current_anim_state = AnimState::IDLE;
	prev_anim_state = AnimState::NONE;
	current_movie_state = MovieState::M_NONE;
	animation->SetIdleState();

	//フラグ類の初期化
	is_hit = false;
	is_move = false;
	is_play_special_move = false;
	has_used_air_dash = false;
	jump_power = 0.0f;
	air_dash_power = 0.0f;
	rotate_angle = 0.0f;

	//ゲージの初期化
	hp_gauge->Init(PlayerConfig::MAX_HP);

	//初期座標を基にした計算
	model_rotate_angle = MGetRotVec2(status.pos, VGet(0.0f, 0.0f, 0.0f));
	UpdateFramePosition();
}

void Player::DrawModel()
{
	//モデルの描画
	if (VSize(forward) != 0)forward = VNorm(forward);
#ifdef DEBUG
	int color = GetColor(255, 255, 255);
	DrawLine3D(status.pos, VAdd(status.pos,VScale(forward,50)), color);
#endif // DEBUG
	forward = VTransform(VGet(0.0f, 0.0f, -1.0f), model_rotate_angle);
	ChangeBlendMode();
	animation->setModelMatrix(status.pos, model_rotate_angle);
	animation->DrawModel();
}

void Player::Update(float timeScale,const std::vector<std::shared_ptr<ICharacter>>& enemies,
	float deltaTime,const int& mapModelHandle)
{
	prev_anim_state = current_anim_state;
	prev_state = status.current_state;
	UpdateFramePosition();
	
#ifdef DEBUG
	int white = GetColor(255, 255, 255);
	DrawCapsule3D(coll_cap.pos, coll_cap.pos2, coll_cap.radius, 16, white, white, false);
	DrawCapsule3D(coll_cap_floor.pos, coll_cap_floor.pos2, coll_cap_floor.radius, 16, GetColor(255,0,0), GetColor(255, 0, 0), false);

#endif // DEBUG

	status.velocity = VGet(0.0f, status.velocity.y, 0.0f);

	HandleInput(camera->getIsSpecialMoveEnd(),camera->getInputKeyNum(), enemies, deltaTime);

	//入力による移動方向(XZ平面)の決定
	if (status.current_state == State::S_RUN || status.current_state == State::S_JUMP) {
		Move(camera->getCameraToTargetDir(), timeScale);
	}
	
	ApplyPhysics(deltaTime);

	UpdateFramePosition();

	ResolveCollision(mapModelHandle,enemies);

	UpdateGroundStatus();

	bool any_enemy_in_range = false;
	for (const auto& enemy_ptr : enemies) {
		if (enemy_ptr == nullptr || !enemy_ptr->getIsAlive())continue;
		if (enemy_ptr->getIsSpecialRangeHit())any_enemy_in_range = true;
	}
	if (any_enemy_in_range && status.current_state != State::S_SPECIAL_ATTACK&&special_move_gauge->getIsMaxNum())
	{
		input->DrawRightTrigger(VAdd(status.pos, PlayerConfig::UI_POS_OFFSET));
	}
	JudgeSpecialMove(camera->getIsSpecialMoveEnd(), camera->getInputKeyNum(), any_enemy_in_range);
#ifdef DEBUG
	
	printfDx("pos:(%0.2f,%0.2f,%0.2f)\n", status.pos.x, status.pos.y, status.pos.z);
	printfDx("velocity:(%0.2f,%0.2f,%0.2f)\n", status.velocity.x, status.velocity.y, status.velocity.z);
#endif // DEBUG

	//アニメーションの再生
	animation->Update(static_cast<unsigned int>(current_anim_state), static_cast<unsigned int>(prev_anim_state), is_blend_anim);

	UpdateHpGaugeColor();
	
	
	//残像の描画
	after_image->Update(Set_Afterimage_Model(animation->getModelHandle()),Set_Afterimage_Light(status.pos,VAdd(status.pos,VGet(30.0f,0.0f,0.0f))));
	after_image->Draw();
	after_image->Delete();
}

void Player::UpdateFramePosition()
{
	right_leg_pos = animation->getLocalFramePos(frames.right_leg);//右足の座標を渡す
	head_pos = animation->getLocalFramePos(frames.head);
	
	right_hand_pos = animation->getLocalFramePos(frames.right_hand);//右手元の座標を渡す
	hips_pos = animation->getLocalFramePos(frames.hips);

	coll_cap = { head_pos,right_leg_pos,sphere_radius };
	coll_cap_floor = { head_pos,status.pos,sphere_radius };
}

void Player::JudgeSpecialMove(const bool& cameraIsSpecialMoveEnd,const int&cameraInputKeyNum, const bool& is_special_range_hit)
{
	//tabキーを押下すると必殺技に移行
	if ((input->IsTrigger(Command::TAB) || input->IsTrigger(Command::RT)) && is_special_range_hit&&special_move_gauge->getIsMaxNum()) {
		if (status.current_state != State::S_SPECIAL_ATTACK) {
			status.current_state = State::S_SPECIAL_ATTACK;
			current_anim_state = AnimState::IDLE;
			input->SetEnabled(false);
			status.velocity = VGet(0.0f,0.0f,0.0f);
		}
	}

	if ( status.current_state == State::S_SPECIAL_ATTACK) {
		if (!cameraIsSpecialMoveEnd) {

			if (cameraInputKeyNum == 1)current_movie_state = MovieState::M_SPECIAL_CAMERA;
			if (cameraInputKeyNum == 2)current_movie_state = MovieState::M_SPECIAL_CAMERA2;
			if (cameraInputKeyNum == 3)current_movie_state = MovieState::M_SPECIAL_CAMERA3;
			if (cameraInputKeyNum == 4)current_movie_state = MovieState::M_SPECIAL_CAMERA4;
		}
		else {
			if (current_anim_state != AnimState::SPECIAL_ATTACK) {
				current_movie_state = MovieState::M_SPECIAL_MOVE;
				current_anim_state = AnimState::SPECIAL_ATTACK;
				is_play_special_move = true;
			}

			if (animation->getIsPlayEnd()) {
				printfDx("special move end detected\n");
				is_play_special_move = false;
				status.current_state = State::S_IDLE;
				current_anim_state = AnimState::IDLE;
				current_movie_state = MovieState::M_NONE;
				input->SetEnabled(true);
				special_move_gauge->Init(0.0f);
			}
		}
	}
	else {
		is_play_special_move = false;
		current_movie_state = MovieState::M_NONE;
	}
}


void Player::JudgeRun()
{
	if ((input->IsPress(Command::W) || input->IsPress(Command::A) || input->IsPress(Command::S) || input->IsPress(Command::D) ||
		std::abs(input->getLeftStick().x) > PlayerConfig::STICK_INPUT_THRESHOLD || std::abs(input->getLeftStick().y) > PlayerConfig::STICK_INPUT_THRESHOLD))
	{
		if (status.current_state==State::S_IDLE) {
			status.current_state = State::S_RUN;
			current_anim_state = AnimState::RUN;
		}
	}
}

void Player::JudgeKick()
{
	//左シフト入力時kickに移行
	if (input->IsTrigger(Command::LSHIFT)|| input->IsTrigger(Command::BtnB)) {
		if (status.current_state != State::S_JUMP && status.current_state != State::S_AIRDASH && status.current_state != State::S_ATTACK&&
			status.current_state!=State::S_CARTWHEEL) {
			status.current_state = State::S_KICK;
			current_anim_state = AnimState::KICK;
		}
	}
	if (status.current_state == State::S_KICK && animation->getIsPlayEnd()) {
		status.current_state = State::S_IDLE;
		current_anim_state = AnimState::IDLE;
	}
}

void Player::JudgeRailCollision()
{
	Capsule capsule = coll_cap;
	Capsule capsule2 = { VGet(-1150.0f, 200.0f, -7750.0f), VGet(-1950.0f,200.0f, -6610.0f) ,(float)20 };
	bool hit = collision->CapsuleToCapsule(capsule, capsule2);
	if (hit) {
		status.pos= collision->CapsuleSlider(capsule, capsule2, status.pos);
		status.current_state = State::S_SKATE;
		current_anim_state = AnimState::SKATE;
	}
}

void Player::ResolveCollision(const int& mapModelHandle, const std::vector<std::shared_ptr<ICharacter>>& enemies)
{
	StageCollResult result = collision->StageToCapsule(coll_cap_floor, mapModelHandle);

	//最終結果をplayerに反映
	if (result.is_grounded && getVelocity().y <= 0.0f) {
		setIsTouchGround(true);
		setPos(VGet(result.current_pos.x, result.floor_y, result.current_pos.z));
	}
	else if (result.is_grounded) {
		setIsTouchGround(false);
	}
	else {
		setIsTouchGround(false);
		setPos(result.current_pos);//壁に押し出された後の座標を適用
	}
	JudgeRailCollision();
	for (const auto& enemy_ptr : enemies) {
		if (enemy_ptr == nullptr || !enemy_ptr->getIsAlive())continue;
		if (enemy_ptr->getCurrentState() == State::S_ATTACK) {
			this->JudgeAttackCollision(enemy_ptr->getBatonCollCapsule());
		}
	}
}

void Player::UpdateGroundStatus()
{
	if (!status.is_touch_ground)return;
	if (status.current_state == State::S_SPECIAL_ATTACK || is_play_special_move) return;

	if (status.current_state == State::S_JUMP) {
		status.current_state = State::S_IDLE;
	}
	bool is_action = (
		status.current_state == State::S_ATTACK ||
		status.current_state == State::S_KICK ||
		status.current_state == State::S_CARTWHEEL);

	if (!is_action) {
		float speed_xz = VSize(VGet(status.velocity.x, 0.0f, status.velocity.z));
		if (speed_xz < 0.1f) {
			status.current_state = State::S_IDLE;
			current_anim_state = AnimState::IDLE;
		}
		else {
			status.current_state = State::S_RUN;
			current_anim_state = AnimState::RUN;
		}
	}
}

void Player::Move(const VECTOR& cameraToTargetDir,float timeScale)
{
	VECTOR input_dir = VGet(0.0f,0.0f,0.0f);
	
	input_dir.x = input->getLeftStick().x;
	input_dir.z = input->getLeftStick().y;

	//WASD方向へ移動
	if (input->IsPress(Command::W))	input_dir.z += 1.0f;	//前
	if (input->IsPress(Command::S)) 	input_dir.z -= 1.0f;	//後ろ
	if (input->IsPress(Command::D)) 	input_dir.x += 1.0f;	//右
	if (input->IsPress(Command::A))	input_dir.x -= 1.0f;	//左

	//もしWASDのいずれかが押下されていた時
	if (VSize(input_dir) > 0.0f)
	{
		VECTOR camera_dir = cameraToTargetDir;
		camera_dir.y = 0.0f;
		camera_dir = VNorm(camera_dir);
		float angle = atan2f(camera_dir.x, camera_dir.z); // Y軸回りの回転角度を取得
		
		//移動方向ベクトルの回転
		input_dir = VTransform(input_dir, MGetRotY(angle));
		input_dir.y = 0.0f;
		//移動距離を一定にする
		input_dir = VNorm(input_dir);

		float current_speed = status.is_touch_ground ? move_speed : move_speed * 0.7f;

		status.velocity = VAdd(status.velocity, VScale(input_dir, current_speed));
		//モデルの回転角度を求める
		rotate_angle = atan2f(input_dir.x, input_dir.z);
		model_rotate_angle = MGetRotY(rotate_angle + DX_PI_F);
	}
}
void Player::ChangeBlendMode()
{
	int black = 0x000000;
	//player以外のオブジェクトの明暗を低くする
	if (is_play_special_move) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, PlayerConfig::ALPHA);
		DrawBox(0, 0, Config::ScreenWidth, Config::ScreenHeight, PlayerConfig::SPECIAL_MOVE_COLOR, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawBox(0, 0, Config::ScreenWidth, 75, black, true);
		DrawBox(0, 465, Config::ScreenWidth, Config::ScreenHeight, black, true);
	}
	//ブレンドモードを通常状態に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


void Player::JudgeAttackCollision(const Capsule& enemy_batton_cap)
{
	if (status.current_state == State::S_SPECIAL_ATTACK)return;
	Capsule capsule = coll_cap;
	Capsule capsule2 = enemy_batton_cap;
	bool hit = collision->CapsuleToCapsule(capsule,capsule2);
	if (hit) {
		hp_gauge->SubNum(PlayerConfig::DAMAGE);
	}
}


void Player::HandleInput(const bool& cameraIsSpecialMoveEnd, const int& cameraInputKeyNum,
	const std::vector<std::shared_ptr<ICharacter>>& enemies,float delta_time)
{
	if (status.current_state == State::S_SPECIAL_ATTACK) {
		JudgeSpecialMove(cameraIsSpecialMoveEnd,cameraInputKeyNum, false);
		return;
	}
	bool can_trigger = false;
	for (const auto& enemy : enemies) {
		if (enemy && enemy->getIsSpecialRangeHit()) {
			can_trigger = true;
			break;
		}
	}
	JudgeSpecialMove(cameraIsSpecialMoveEnd, cameraInputKeyNum, can_trigger);
	if (status.current_state == State::S_SPECIAL_ATTACK)return;

	JudgeKick();

	if (status.current_state == State::S_ATTACK ||
		status.current_state == State::S_KICK ||
		status.current_state == State::S_CARTWHEEL)return;

	if (status.is_touch_ground) {
		JudgeRun();
	}
}

void Player::ApplyPhysics(float delta_time)
{
	float time_scale = delta_time * PlayerConfig::FPS;
	
	if (!status.is_touch_ground) {
		status.velocity.y -= Config::GRAVITY * time_scale;

		if (status.velocity.y < PlayerConfig::GRAVITY_LIMIT) {
			status.velocity.y = PlayerConfig::GRAVITY_LIMIT;
		}
	}
	else {
		if (status.velocity.y < 0.0f) status.velocity.y = -0.1f;
		has_used_air_dash = false;
	}
	if (status.pos.y + (status.velocity.y * time_scale) <= PlayerConfig::FALL_DAMAGE_HEIGHT)hp_gauge->SubNum(PlayerConfig::FALL_DAMAGE_AMOUNT);
	status.pos = VAdd(status.pos,VScale(status.velocity,time_scale));
}

void Player::UpdateHpGaugeColor()
{
	float hp = hp_gauge->getCurrentNum();
	if(hp>=PlayerConfig::MAX_HP*0.5f)hp_gauge->DrawGauge_3d(status.pos, 74, 225, 79);
	else if(hp>=PlayerConfig::MAX_HP*0.3f)hp_gauge->DrawGauge_3d( status.pos, 221, 227, 69);
	else hp_gauge->DrawGauge_3d( status.pos, 214, 65, 50);
}

