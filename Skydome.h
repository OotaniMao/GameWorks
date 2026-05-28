#pragma once
#include"Model.h"

const class Camera;

class Skydome :public Model
{
private:
	VECTOR pos;
	float scale_size;
public:
	Skydome();
	virtual ~Skydome();

	void Update(std::shared_ptr<ICamera>camera);
	void Draw();

};
