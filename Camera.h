#pragma once
#include <vector>
#include<memory>
#include"CameraInterface.h"

class Player;
class IInput;
const int INPUT_KEY_NUM = 5;

enum class State :int;


class Camera:public ICamera
{
private:
	struct Node {
		VECTOR pos;
		int data;
	};
	//必殺技のカメラ位置の設定
	const VECTOR special_camera_positions[INPUT_KEY_NUM] = {
	VGet(150.0f, 130.0f, -150.0f),
	VGet(-150.0f, -70.0f, -150.0f),
	VGet(-150.0f, 130.0f, -150.0f),
	VGet(150.0f, -70.0f, -150.0f),
	VGet(100.0f, 80.0f, -150.0f)
	};

	const VECTOR special_move_ui[INPUT_KEY_NUM] = {
	VGet(430, 250, 0.0f),
	VGet(420, 50, 0.0f),
	VGet(680, 250, 0.0f),
	VGet(430, 450, 0.0f),
	VGet(200, 240, 0.0f)
	};
	//カメラ切り替え時に使うキー
	const int keys[INPUT_KEY_NUM] = {
		KEY_INPUT_1, KEY_INPUT_2, KEY_INPUT_3, KEY_INPUT_4, KEY_INPUT_5
	};

	VECTOR camera_pos;	//カメラの座標位置
	VECTOR target_pos;	//注視点の座標位置
	float move_speed;	//移動速度
	VECTOR memory_camera_pos;//カメラの座標を一時的に記憶しておく
	VECTOR camera_to_target_dir;//カメラから注視点への距離ベクトル
	VECTOR rot_arbitary_axis;//任意軸（縦回転用）
	VECTOR current_camera_pos;//現在のカメラ座標
	VECTOR current_target_pos;//現在の注視点座標

	CameraState current_state;//現在のカメラの状態

	bool is_input_key[INPUT_KEY_NUM];//必殺技用キーが押下されたかどうか
	bool is_special_move_end;//必殺技のcamera移動が終わったかどうか
	int end_count;
	int input_key_num;
	int number_graph_handle[INPUT_KEY_NUM];
	int number_graph_handle_red[INPUT_KEY_NUM];
	std::vector <Node> list;
	int end_time;

	std::shared_ptr<IInput>input;
public:
	Camera(std::shared_ptr<IInput>& input_key);	//コンストラクタ
	~Camera();	//デストラクタ

	//ゲッター
	VECTOR getCurrentCameraPos()const override{ return current_camera_pos; }
	VECTOR getCameraToTargetDir()const override { return camera_to_target_dir; }
	bool getIsSpecialMoveEnd()const override { return is_special_move_end; }
	CameraState getCameraState()const override { return current_state; }
	int getInputKeyNum()const override { return input_key_num; }

	/// @brief 初期化
	void Init()override;
	/// @brief カメラの挙動制御
	void Update(const Player& player, float timeScale)override;

	void DrawUI();
private:
	/// @brief 通常カメラ
	void NormalMove(float timeScale,const State& player_state);
	/// @brief 必殺技用カメラ
	void SpecialMove();
	void SetSpecialMoveCamera(int intputKeyNum);
	void DrawLine();

};
