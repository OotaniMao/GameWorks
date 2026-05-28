#include "Model.h"


Model::Model(const char* fileName)
	:rot_angle(0.0f)
{
	model_handle = MV1LoadModel(fileName);
	int material_num = MV1GetMaterialNum(model_handle);
	for (int i = 0; i < material_num; i++) {
		MV1SetMaterialDifColor(model_handle, i, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

Model::~Model()
{
	MV1DeleteModel(model_handle);
}

MATRIX Model::getFramePos(int frameIndex)
{
	return MV1GetFrameLocalWorldMatrix(model_handle, frameIndex);
}

VECTOR Model::getLocalFramePos(int frameIndex)
{
	//指定のフレームの座標を取得する
	return MV1GetFramePosition(model_handle, frameIndex);
}

int Model::getTextureNum()
{
	return MV1GetTextureNum(model_handle);
}

int Model::SearchFrame(const char* frameName)
{
	int serch_frame_index=MV1SearchFrame(model_handle,frameName);
	if (serch_frame_index == -1) {
		printfDx("Error\n");
	}
	if (serch_frame_index == -2) {
		printfDx("No serch_frame_index found for frame:%s \n",frameName);
	}
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

void Model::SetModelMatrix(VECTOR pos, MATRIX rotate)
{
	MATRIX matrix = MMult(rotate, MGetTranslate(pos));
	//テクスチャのフィルタリングモードを変更（描画負荷軽量化）
	MV1SetTextureSampleFilterMode(model_handle, 0, DX_DRAWMODE_NEAREST);
	MV1SetMatrix(model_handle, matrix);
}

void Model::SetFrameLocalMatrix(int frameIndex)
{
	MATRIX matrix = MV1GetFrameLocalMatrix(model_handle, frameIndex);
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
	MV1SetFrameUserLocalMatrix(model_handle, frameIndex, matrix);

}

void Model::ResetFrameLocalMatrix(int frameIndex)
{
	MV1ResetFrameUserLocalMatrix(model_handle, frameIndex);
}