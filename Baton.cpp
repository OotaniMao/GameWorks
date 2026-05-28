#include "Baton.h"
#include"math.h"
//#define DEBUG


Baton::Baton()
	:Model("model/enemy/baton.mv1"),
	pos(VGet(0.0f,0.0f,0.0f)),
	rotate_angle(MGetRotX(0)),
	model_scale(MGetScale(VGet(0.1f, 0.1f, 0.1f))),
	rotate(),
	forward(VGet(0.0f,0.0f,1.0f)),
	coll_cap(),
	fold_index(-1)
{
	fold_index = SearchFrame("folding bit.001");
	top_index = SearchFrame("folding bit");
}

Baton::~Baton()
{
}

void Baton::Draw()
{
	DrawModel();
#ifdef DEBUG
	DrawCapsule3D(coll_cap.pos,coll_cap.pos2,coll_cap.radius,16,GetColor(255,255,255),GetColor(255,255,255),FALSE);

#endif // DEBUG

}

void Baton::Update(const VECTOR &handPos, const VECTOR& enemyRotAngle)
{
	rotate = MGetRotY(atan2f(enemyRotAngle.x, enemyRotAngle.z));
	if (VSize(forward) != 0)forward = VNorm(forward);
	forward = VTransform(VGet(0.0f, 0.0f, 1.0f), rotate);
	rotate_angle = MGetRotX(DX_PI_F/4.0f);

	VECTOR current_fold_pos = getLocalFramePos(fold_index);
	VECTOR offset = VSub(current_fold_pos,pos);
	pos = handPos;
	pos = VSub(pos,offset);

	MATRIX matrix = MGetIdent();	//単位行列の取得
	matrix = MMult(matrix, model_scale);
	matrix = MMult(matrix, rotate_angle);
	matrix = MMult(matrix, MGetRotX(DX_PI_F));
	matrix = MMult(matrix, rotate);

	SetModelMatrix(pos, matrix);

	VECTOR update_fold_pos = getLocalFramePos(fold_index);
	//capsule座標の更新
	coll_cap = { pos,update_fold_pos , 5.0f };
}
