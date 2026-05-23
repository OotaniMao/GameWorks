#pragma once
#include<memory>
#include"SceneBase.h"
#include"SceneUtility.h"

class IInput;
class Player;
class FpsController;
class Time;
class Font;
class Sound;
class Skydome;
class Map;
class EnemyManager;
class Collision;
class Graffiti;
class ShadowMap;
class Movie;
class Goal;
class ICamera;
class Gauge;


class PlayScene:public ISceneBase
{
public:
	PlayScene(std::shared_ptr<GameContext> context, const float& timeScale);
	~PlayScene();

	void Init()override;
	void Update()override;
	void Draw()override;

	std::unique_ptr<ISceneBase>GetNextScene()override;

private:
	std::shared_ptr<GameContext>ctx;
	std::shared_ptr<Time>			time ;
	std::shared_ptr<Skydome>		skydome ;
	std::shared_ptr<Map>			map ;
	std::shared_ptr<EnemyManager>	enemies ;
	std::shared_ptr<Collision>		collision ;
	std::shared_ptr<Graffiti>		graffiti ;
	std::shared_ptr<ShadowMap>		shadowMap;
	std::shared_ptr<Goal>			goal;

	std::shared_ptr<ICamera>			camera ;
	std::shared_ptr<Gauge>			special_move_gauge ;//ïKéEãZÉQÅ[ÉW
	std::shared_ptr<Player>			player;

	float time_scale;
	int handle_commands;
	float charge_speed;
	int alpha;
	GameScene select_scene;
};