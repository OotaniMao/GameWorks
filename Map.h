#pragma once
#include"Model.h"
#include"R_Math.h"

class Player;

class Map :public Model
{
private:
	VECTOR pos;

public:
	Map();
	virtual ~Map();

	int getModelHandle()const { return model_handle; }

	void Draw();
	void DrawMesh();
	float GetFloorHeight(VECTOR target_pos)const;
	bool CheckHitWall(const Sphere& sphere) const;

};
