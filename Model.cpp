#include"DxLib.h"
#include "Model.h"

//https://dxlib.xsrv.jp/function/dxfunc_3d_model_1.html

Model::Model(const char* FileName)
	:rot_angle(0.0f)
{
	model_handle = MV1LoadModel(FileName);
	int material_num = MV1GetMaterialNum(model_handle);
	for (int i = 0; i < material_num; i++) {
		MV1SetMaterialDifColor(model_handle, i, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

Model::~Model()
{
	MV1DeleteModel(model_handle);
}




MATRIX Model::getFramePos(int frame_index)
{
	MATRIX matrix = MV1GetFrameLocalWorldMatrix(model_handle, frame_index);
	return matrix;
}

VECTOR Model::getLocalFramePos(int frame_index)
{
	int color = GetColor(255, 255, 255);

	//printfDx("frameNum: %d\n", MV1GetFrameNum(model_handle));
	//printfDx("FrameName:%s\n", MV1GetFrameName(model_handle, frame_index));
	//MV1DrawFrame(model_handle, frame_index);
	//指定のフレームの座標を取得する
	VECTOR frame_pos = MV1GetFramePosition(model_handle, frame_index);

	return frame_pos;
}

int Model::getTextureNum()
{
	int num = MV1GetTextureNum(model_handle);
	return num;
}

int Model::SearchFrame(const char* frame_name)
{
	int serch_frame_index=MV1SearchFrame(model_handle,frame_name);
	if (serch_frame_index == -1) {
		printfDx("Error\n");
	}
	if (serch_frame_index == -2) {
		printfDx("No serch_frame_index found for frame:%s \n",frame_name);
	}

	//printfDx("FrameName:%s\n", MV1GetFrameName(model_handle, serch_frame_index));
	return serch_frame_index;
}

void Model::Init()
{
	//モデル全体の参照用メッシュを構築
	MV1SetupReferenceMesh(model_handle,-1,TRUE);
	//参照用メッシュ情報の取得
	polygonlist = MV1GetReferenceMesh(model_handle,-1,TRUE);
	//モデル全体のコリジョン情報を構築
	MV1SetupCollInfo(model_handle,-1,8,8,8);
}

void Model::DrawModel()
{
	//描画
	MV1DrawModel(model_handle);
}

void Model::setModelMatrix(VECTOR pos, MATRIX rotate)
{
	MATRIX matrix = MMult(rotate, MGetTranslate(pos));
	//テクスチャのフィルタリングモードを変更（描画負荷軽量化）
	MV1SetTextureSampleFilterMode(model_handle, 0, DX_DRAWMODE_NEAREST);
	MV1SetMatrix(model_handle, matrix);
}

void Model::setFrameLocalMatrix(int frame_index)
{
	MATRIX matrix = MV1GetFrameLocalMatrix(model_handle, frame_index);
	rot_angle = 0.036f;
	VECTOR center = VGet(matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);
	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	if (rot_angle > DX_PI_F * 2.0f) {
		rot_angle = 0.0f;
	}
	MATRIX rot = MGetRotZ(-rot_angle);
	matrix = MMult(matrix, rot);
	matrix.m[3][0] = center.x;
	matrix.m[3][1] = center.y;
	matrix.m[3][2] = center.z;
	MV1SetFrameUserLocalMatrix(model_handle, frame_index, matrix);

}

void Model::resetFrameLocalMatrix(int frame_index)
{
	MV1ResetFrameUserLocalMatrix(model_handle, frame_index);
}