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
	Model(const char* fileName);
	//デストラクタ
	virtual ~Model();

	MATRIX getFramePos(int frameIndex);
	VECTOR getLocalFramePos(int frameIndex);
	int getTextureNum();
	int SearchFrame(const char*frameName);

	void Init();
	void DrawModel();	//モデルの描画
	void SetModelMatrix(VECTOR pos, MATRIX rotate);
	void SetFrameLocalMatrix(int frameIndex);
	void ResetFrameLocalMatrix(int frameIndex);

};
