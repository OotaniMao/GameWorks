#include"DxLib.h"
#include"InputInterface.h"
#include"Player.h"
#include"Collision.h"
#include"Camera.h"
//#define DEBUG


Camera::Camera(std::shared_ptr<IInput>&inputKey)
	:camera_pos(VGet(0.0f, 200.0f, -350.0f))
	, target_pos(VGet(0.0f, 100.0f, 0.0f))
	, move_speed(10.0f)
	, memory_camera_pos(camera_pos)
	, camera_to_target_dir(VGet(0.0f, 0.0f, 0.0f))
	, rot_arbitary_axis(VGet(20.0f, 0.0f, 0.0f))
	, current_camera_pos(VAdd(VGet(-930, 0.0f, -7910.0f), camera_pos))
	, current_target_pos(VAdd(VGet(-930, 0.0f, -7910.0f), target_pos))
	, current_state(CameraState::NORMAL)
	, is_special_move_end(false)
	, end_count(0)
	, end_time(0),
	input_key_num(0)
	
{
	input = inputKey;
	SetCameraNearFar(10.0f, 35000.0f);
	for (auto i = 0; i < INPUT_KEY_NUM; i++) {
		is_input_key[i] = false;
	}
	LoadDivGraphF("graph/number_black.png", INPUT_KEY_NUM, 3, 2, 80, 74.5f, number_graph_handle);
	LoadDivGraphF("graph/number_red.png", INPUT_KEY_NUM, 3, 2, 80, 74.5f, number_graph_handle_red);
}

Camera::~Camera()
{
}

void Camera::Init()
{
	camera_pos = VGet(0.0f, 200.0f, -350.0f);
	target_pos = VGet(0.0f, 100.0f, 0.0f);
	memory_camera_pos = camera_pos;
	camera_to_target_dir = VGet(0.0f, 0.0f, 0.0f);
	rot_arbitary_axis = VGet(20.0f, 0.0f, 0.0f);
	current_camera_pos = VAdd(VGet(-930, 0.0f, -7910.0f), camera_pos);
	current_target_pos = VAdd(VGet(-930, 0.0f, -7910.0f), target_pos);
	current_state = CameraState::NORMAL;
	is_special_move_end = false;
	end_count = 0;
	end_time = 0;
	input_key_num = 0;
}

void Camera::Update(const Player& player, float timeScale)
{
	//DxLibのカメラとエフェクシアのカメラを同期させる
	//Effekseer_Sync3DSetting();
	camera_to_target_dir = VSub(target_pos, camera_pos);

	switch (current_state)
	{
	case CameraState::NORMAL:
		input_key_num = 0;
		camera_pos = memory_camera_pos;
		NormalMove(timeScale,player.getCurrentState());
		memory_camera_pos = camera_pos;
		break;
	case CameraState::SPECIAL:
		if (is_special_move_end) {
			for (auto i = 0; i < 5; i++) {
				is_input_key[i] = false;
			}
			current_state = CameraState::NORMAL;
		}
		else {
			SpecialMove();
		}
		break;
	default:
		NormalMove(timeScale,player.getCurrentState());
		break;
	}
	static Collision collision;
	if (player.getCurrentState() == State::CARTWHEEL) {
		current_target_pos = R_Math::Lerp(current_target_pos, VAdd(player.getHipsPos(), VSub(target_pos, VGet(0.0f, 100.0f, 0.0f))), 0.2f);
		current_camera_pos = R_Math::Lerp(current_camera_pos, VAdd(player.getHipsPos(), VSub(camera_pos, VGet(0.0f, 100.0f, 0.0f))), 0.08f);
	}
	else {
		current_target_pos = R_Math::Lerp(current_target_pos, VAdd(player.getPos(), target_pos), 0.2f);
		current_camera_pos = R_Math::Lerp(current_camera_pos, VAdd(player.getPos(), camera_pos), 0.08f);
	}
	//カメラに反映
	SetCameraPositionAndTarget_UpVecY(current_camera_pos, current_target_pos);
}

void Camera::NormalMove(float timeScale,const State&playerState)
{
	//回転角度用行列
	MATRIX matrix;
	float angle = (DX_PI_F / 180)*1.5f * timeScale;
	float highestY = 400.0f;
	float lowestY = 100.0f;

	float stickX = input->getRightStick().x;
	float stickY = input->getRightStick().y;

	//左に回転
	if (input->IsPress(Command::LEFT)|| stickX> 0.5f ) {
		matrix = MGetRotY(angle);
		camera_pos = VTransform(camera_pos, matrix);
		rot_arbitary_axis = VTransform(rot_arbitary_axis, matrix);
	}
	//右に回転
	if (input->IsPress(Command::RIGHT) || stickX < -0.5f) {
		matrix = MGetRotY(angle);
		matrix = MInverse(matrix);
		camera_pos = VTransform(camera_pos, matrix);
		rot_arbitary_axis = VTransform(rot_arbitary_axis, matrix);
	}
	//上に回転
	if ((input->IsPress(Command::UP) ||  stickY>0.5f) && camera_pos.y < highestY) {
		matrix = MGetRotAxis(rot_arbitary_axis, angle);
		camera_pos = VTransform(camera_pos, matrix);
	}
	//下に回転
	if ((input->IsPress(Command::DOWN)||stickY<-0.5f)&& camera_pos.y > lowestY) {
		matrix = MGetRotAxis(rot_arbitary_axis, angle);
		matrix = MInverse(matrix);
		camera_pos = VTransform(camera_pos, matrix);
	}

	if (playerState==State::SPECIAL_ATTACK) {
		current_state = CameraState::SPECIAL;
	}
	else {
		current_state = CameraState::NORMAL;
		is_special_move_end = false;
	}

	if (camera_pos.y >= highestY)camera_pos.y = highestY;
	else if (camera_pos.y < lowestY) camera_pos.y = lowestY;
}	

void Camera::SpecialMove()
{
	float stickX = input->getLeftStick().x;
	float stickY = input->getLeftStick().y;
#ifdef DEBUG
	printfDx("stickX:%0.2f stickY:%0.2f\n", stickX, stickY);

#endif // DEBUG
	
	//カメラの位置を変更
	if (stickX > 0.5f && !is_input_key[2]) SetSpecialMoveCamera(input_key_num = 2);
	if (stickX < -0.5f && !is_input_key[4])SetSpecialMoveCamera(input_key_num = 4);
	if (stickY > 0.5f && !is_input_key[1]) SetSpecialMoveCamera(input_key_num = 1);
	if (stickY < -0.5f && !is_input_key[3])SetSpecialMoveCamera(input_key_num = 3);
	if (!is_input_key[0])SetSpecialMoveCamera(0);
	end_count = 0;
	//すべてのボタンが押された時終了
	for (auto i = 0; i < 5; i++) {
		if (is_input_key[i])end_count++;
	}
	int curr_time = GetNowCount();
	if (end_count >= 5&&end_time==0.0f) {
		end_time = curr_time;
	}
	//printfDx("end_time:%0.2f\n",curr_time-end_time);
	if (curr_time - end_time >= 400.0f && end_time != 0.0f) {
		list.clear();
		is_special_move_end = true;
		end_time =0;
	}
}

void Camera::SetSpecialMoveCamera(int intputKeyNum)
{
	camera_pos = special_camera_positions[intputKeyNum];
	is_input_key[intputKeyNum] = true;
	list.push_back({ special_move_ui[intputKeyNum], intputKeyNum });
}

void Camera::DrawLine()
{
	if (list.empty())return;
	for (int i=0;i<list.size()-1;i++)
	{
		DrawLineAA(list[i].pos.x, list[i].pos.y, list[i+1].pos.x, list[i+1].pos.y, GetColor(255, 0, 0), 5);
	}
}

void Camera::DrawUI()
{
	if (current_state != CameraState::SPECIAL)return;
	if (is_special_move_end) {
		for (auto i = 0; i < 5; i++) {
			is_input_key[i] = false;
		}
		current_state = CameraState::NORMAL;
		return;
	}
	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);

	for (int i = 0; i < INPUT_KEY_NUM; i++) {
		//入力キーの促し
		DrawGraph(static_cast<int>(special_move_ui[i].x), static_cast<int>(special_move_ui[i].y), is_input_key[i] ? number_graph_handle_red[i] : number_graph_handle[i], true);
		//カメラの位置を変更
		if (CheckHitKey(keys[i]) && !is_input_key[i]) {
			SetSpecialMoveCamera(input_key_num = i);
		}
	}
	DrawLine();

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
}
