#pragma once
#include "Model.h"
#include"R_Math.h"

class Baton :public Model
{
private:
	VECTOR pos;
	MATRIX rotate_angle;
	MATRIX rotate;
	MATRIX model_scale;
	VECTOR forward;
	Capsule coll_cap;
	int fold_index;
	int top_index;
public:
	Baton();
	virtual ~Baton();

	Capsule getCollCapsule()const { return coll_cap; }

	void Draw();
	void Update(const VECTOR& handPos, const VECTOR& enemyRotAngle);
};
