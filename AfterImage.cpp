#include "DxLib.h"
#include <memory>
#include <vector>
#include "Afterimage.h"

Afterimage::Afterimage()
{
	Init();
}

Afterimage::~Afterimage()
{
	Delete();
}

void Afterimage::Init()
{
	model.clear();
	light.clear();
	timeCount_M = 0.0f;
	timeCount_L = 0.0f;
	isModel = false;
	isLight = false;
	isBoth = false;
}

void Afterimage::SetModel(Afterimage_model NewModel)
{
	// ˆê’èŠÔ‚Å’Ç‰Á
	++timeCount_M;
	if(model.size() == 0)
	{
		model.push_back(NewModel);
	}
	else if(timeCount_M > span_M)
	{
		model.push_back(NewModel);
		timeCount_M = 0.0f;
	}
}

void Afterimage::SetLight(Afterimage_light NewLight)
{

	// ˆê’èŠÔ‚Å’Ç‰Á
	timeCount_L += CountSpeed;
	if(light.size() == 0)
	{
		light.push_back(NewLight);
	}
	else if(timeCount_L >= span_L)
	{
		light.push_back(NewLight);
		timeCount_L = 0.0f;
	}

	// ˆê’èŠÔ‚ª—§‚Á‚Ä‚¢‚È‚¢‚Íí‚ÉÅŒã‚Ì—v‘f‚ğŒ»İ’n‚É‚·‚é
	if(light.size() > 0)
	{
		light.back() = NewLight;
	}
}

void Afterimage::Update(Afterimage_model Model, Afterimage_light Light)
{
	if(isModel || isBoth)
	{
		SetModel(Model);
	}
	if(isLight || isBoth)
	{
		SetLight(Light);
	}

	if (CheckHitKey(KEY_INPUT_F3))
	{
		isModel = true;
	}
	if (CheckHitKey(KEY_INPUT_F4))
	{
		isLight = true;
	}
	if (CheckHitKey(KEY_INPUT_F6))
	{
		isBoth = true;
	}
	if (CheckHitKey(KEY_INPUT_F7))
	{
		isModel = false; isLight = false; isBoth = false;
	}
}

void Afterimage::Delete()
{

	if(model.size() > modelmax)
	{
		model.erase(model.begin());
	}
	if(light.size() > lightmax)
	{
		light.erase(light.begin());
	}
	//@ŠÔ‚Åíœ
	if(DeleteTime_M <= deleteTimer_M)
	{
		if(model.size() > 0)
		{
			model.erase(model.begin());
		}
		deleteTimer_M = 0.0f;
	}
	else
	{
		deleteTimer_M += CountSpeed;
	}

	if (DeleteTime_L <= deleteTimer_L)
	{
		if (light.size() > 0)
		{
			light.erase(light.begin());
		}
		deleteTimer_L = 0.0f;
	}
	else
	{
		deleteTimer_L += CountSpeed;
	}
}

void Afterimage::DrawModel()
{
	float rate = 0.0f;
	for (int i = 0; i < model.size(); i++)
	{
		// “§–¾“x‚ğ—v‘f”‚É‰‚¶‚Ä‚O‚©‚ç‚P‚É•Ï‰»‚³‚¹‚é
		rate = (1.0f * (float)i / (float)model.size());
		MV1SetOpacityRate(model[i].model_handle, rate);
		// ˆÊ’u‚ğƒZƒbƒg‚µ‚Ä•`‰æ
		MV1SetMatrix(model[i].model_handle, model[i].position);
		MV1DrawModel(model[i].model_handle);
	}
	MV1SetOpacityRate(model.back().model_handle, 1.0f);
}

void Afterimage::DrawLight()
{
	// Œã‚ë‚©‚çŒ»İ‚ÌˆÊ’u‚Æ
	for(int i = 0; i < light.size(); i++)
	{
		// ÅŒã‚Ì—v‘f‚Ü‚Å—ˆ‚½‚ç”²‚¯‚é
		if (i + 1 >= light.size()) break;
		// “§–¾“x‚ğ—v‘f”‚É‰‚¶‚Ä‚O‚©‚ç‚P‚É•Ï‰»‚³‚¹‚é
		int alpha = (int)(255 * ((float)i / (float)light.size()));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		// ‰º¢
		DrawTriangle3D(light[i].end, light[i + 1].end, light[i + 1].start,
			color, TRUE);
		// ã¢
		DrawTriangle3D(light[i].start, light[i].end, light[i + 1].start,
			color, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

void Afterimage::Draw()
{
	if (model.size() != 0)
	{
		DrawModel();
	}
	if( light.size() != 0)
	{
		DrawLight();
	}
}