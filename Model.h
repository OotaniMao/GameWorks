#pragma once
#include"DxLib.h"

class Model
{
protected:
	int model_handle;	//モデルハンドル
	float rot_angle;
	MV1_REF_POLYGONLIST polygonlist;//格納先変数

public:
	//コンストラクタ
	Model(const char* FileName);
	//デストラクタ
	virtual ~Model();

	MATRIX getFramePos(int frame_index);
	VECTOR getLocalFramePos(int frame_index);
	int getTextureNum();
	int SearchFrame(const char*frame_name);

	void Init();
	void DrawModel();	//モデルの描画
	void setModelMatrix(VECTOR pos, MATRIX rotate);
	void setFrameLocalMatrix(int frame_index);
	void resetFrameLocalMatrix(int frame_index);

};
