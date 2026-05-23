#pragma once
#include<memory>

enum class GameScene :int {
	TITLE,
	OPTION,
	PLAY,
	END
};


class ISceneBase
{
public:
	virtual ~ISceneBase() {}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual std::unique_ptr<ISceneBase>GetNextScene() = 0;

};
