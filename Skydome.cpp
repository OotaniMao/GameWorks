#include"DxLib.h"
#include"Camera.h"
#include"Skydome.h"

Skydome::Skydome()
	:Model("model/skydome/skydome.mv1")
	, pos(VGet(0.0f, 0.0f, 0.0f))
	, scaleSize(30.0f)
{
}

Skydome::~Skydome()
{
}

void Skydome::Update(std::shared_ptr<ICamera>camera)
{
	MATRIX matrix = MGetRotY(0);
	pos = camera->getCurrentCameraPos();
	matrix = MMult(matrix,MGetScale(VGet(scaleSize, scaleSize, scaleSize)));
	setModelMatrix(pos, matrix);
}


void Skydome::Draw()
{
	DrawModel();
}