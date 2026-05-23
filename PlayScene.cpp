#include"DxLib.h"
#include"PlayScene.h"
#include<memory>
#include<time.h>
#include"Config.h"
#include"SceneUtility.h"
#include"FpsController.h"
#include"Time.h"
#include"EndScene.h"
#include"Sound.h"
#include"Font.h"
#include"Camera.h"
#include"InputInterface.h"
#include"Input.h"
#include"Skydome.h"
#include"Map.h"
#include"Enemy.h"
#include"EnemyManager.h"
#include"Player.h"
#include"ShadowMap.h"
#include"Collision.h"
#include"Gauge.h"
#include"Movie.h"
#include"Graffiti.h"
#include"Goal.h"
//#define DEBUG


PlayScene::PlayScene(std::shared_ptr<GameContext> context, const float& timeScale)
{
	ctx = context;
	time = std::make_shared<Time>();
	skydome = std::make_shared<Skydome>();
	map = std::make_shared<Map>();
	enemies = std::make_shared<EnemyManager>();
	collision = std::make_shared<Collision>();
	graffiti = std::make_shared<Graffiti>();
	shadowMap = std::make_shared<ShadowMap>();
	goal = std::make_shared<Goal>();

	Square square;
	square.pos = { 600.0f ,450.0f };
	square.size = { 300.0f ,80.0f };

	camera = std::make_shared<Camera>(ctx->input);
	special_move_gauge = std::make_shared<Gauge>(15.0f,0.0f,square);//必殺技ゲージ
	player = std::make_shared<Player>(collision, special_move_gauge, ctx->input, camera);
	charge_speed = 0.0f;
	time_scale = timeScale;
	handle_commands = LoadGraph("graph/commands.png");
	alpha = 255;
	select_scene = GameScene::PLAY;
	time->Start();
}

PlayScene::~PlayScene()
{
}

void PlayScene::Init()
{
}

void PlayScene::Update()
{
	enemies->Update(*player, *collision, static_cast<float>(ctx->fps_controller->getDeltaTime())/*, *effect*/, map->getModelHandle());
	player->Update(time_scale, enemies->getEnemies(), static_cast<float>(ctx->fps_controller->getDeltaTime()), map->getModelHandle());
	goal->Update(*player);

	skydome->Update(camera);
	//graffiti->Update(*enemy, *effect);
	time->Count(time_scale, *player);
	charge_speed = player->getCurrentState() != State::S_SPECIAL_ATTACK ? 15.0f / (60.0f * 15.0f) : 0.0f;
	special_move_gauge->Update(charge_speed);

	//カメラのアップデート
	camera->Update(*player, time_scale);
	//effect->Update(*enemies,*player);

	if (time->GetTimeLimit() <= 0.0f || goal->getIsGoal() || player->getIsHpMin()) {
		select_scene = GameScene::END;
	}
}

void PlayScene::Draw()
{
	skydome->Draw();
	shadowMap->DrawSetUP_Chara(*player, *enemies);
	shadowMap->SetUse(*player, *enemies, *map);
	goal->Draw();
	time->Draw(*ctx->font);

	if (alpha > 0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		alpha -= 2;
		ctx->font->DrawFormatChar(
			static_cast<int>(Config::ScreenWidth / 3.0f),
			static_cast<int>(Config::ScreenHeight / 3.0f),
			"Run!!\n", static_cast<int>(FontSize::font1_Size_180), Config::ColorRunMsg);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}

	if (special_move_gauge->getIsMaxNum()) {
		//中身の描画
		special_move_gauge->DrawGauge_2d(255, 255, 100);
	}
	else {
		special_move_gauge->DrawGauge_2d(255, 255, 255);
	}

	//操作方法描画
	DrawExtendGraph(10, 10, 230, 300, handle_commands, TRUE);
	ctx->movie->Update(player->getCurrentMovieState());
	camera->DrawUI();
}

std::unique_ptr<ISceneBase> PlayScene::GetNextScene()
{
	if(select_scene==GameScene::END) return std::make_unique<EndScene>(ctx,time_scale);
	return nullptr;
   
}
