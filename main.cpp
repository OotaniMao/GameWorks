#include<memory>
#include"DxLib.h"
#include"Config.h"
#include"FpsController.h"
#include"Time.h"
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
#include<time.h>
#include"Graffiti.h"
#include"Goal.h"
#include"Baton.h"
//#define DEBUG



enum class GameScene :int{
	TITLE,
	OPTION,
	PLAY,	
	END
};

enum class TitleMenu :int{
	MENU_START,
	MENU_OPTION,
	//MENU_EXIT,
	MENU_MAX
};

enum class OptionMenu :int {
	OPTION_BGM,
	OPTION_SE,
	OPTION_BUCK,
	OPTION_MAX
};

enum class End_Menu :int {
	END_REPLAY,
	END_TITLE,
	//END_EXIT,
	END_MAX
};

struct MenuItem {
	const char* nomal_txt;
	const char* active_txt;
	int y_offset;
};

const MenuItem TitleMenuItems [] = {
	{"ゲームスタート","ゲームスタートトト",0},
	{"オプション","オプションンン",40},
	/*{"ヤメル","ヤメルルル",80}*/
};

const MenuItem OptionMenuItems[] = {
	{"BGM:","BGM:",0},
	{"SE:","SE:",40},
	{"モドル","モドルルル",150}
};

const MenuItem EndMenuItems[] = {
	{"リプレイ","リプレイイイ",0},
	{"タイトル","タイトルルル",40}
};

const int TitleMenuCount = sizeof(TitleMenuItems) / sizeof(TitleMenuItems[0]);
const int OptionMenuCount = sizeof(OptionMenuItems) / sizeof(OptionMenuItems[0]);
const int EndMenuCount = sizeof(EndMenuItems) / sizeof(EndMenuItems[0]);

GameScene select_scene = GameScene::TITLE;
int select_menu = static_cast<unsigned int>(TitleMenu::MENU_START);
int select_option_menu = static_cast<unsigned int> (OptionMenu::OPTION_BUCK);
int select_end_menu = static_cast<unsigned int>(End_Menu::END_TITLE);

//関数プロトタイプ宣言
void UpdateMenuSelection(bool& is_press_button, const std::shared_ptr<IInput>key, int& select_index, int max_count,std::shared_ptr<Sound>sound);
void DrawMenuItems(const MenuItem* items, int count, int selected_index, std::shared_ptr<Font> font);
void DrawDemoPlayMovie(std::shared_ptr<Movie>movie);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
	// ウインドウモードで起動
	ChangeWindowMode(true);
	SetGraphMode(Config::ScreenWidth, Config::ScreenHeight,Config::ColorBit);
	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	/*if (Effekseer_Init(8000) == -1) {
		DxLib_End();
		return -1;
	}*/
	SetDrawScreen(DX_SCREEN_BACK);
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	SetUseLighting(TRUE);
	SetLightDirection(VGet(1.0f, -1.0f, 0.0f));
	SetLightAmbColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	//宣言＆初期化
	std::shared_ptr<FpsController>	fpsController = std::make_shared<FpsController>();
	std::shared_ptr<Time>			time = std::make_shared<Time>();
	std::shared_ptr<Font>			font = std::make_shared<Font>();
	std::shared_ptr<Sound>			sound = std::make_shared<Sound>();
	std::shared_ptr<Skydome>		skydome = std::make_shared<Skydome>();
	std::shared_ptr<Map>			map = std::make_shared<Map>();
	std::shared_ptr<EnemyManager>	enemies = std::make_shared<EnemyManager>();
	std::shared_ptr<Collision>		collision = std::make_shared<Collision>();
	//std::shared_ptr<Effect>			effect = std::make_shared<Effect>();
	std::shared_ptr<Graffiti>		graffiti = std::make_shared<Graffiti>();
	std::shared_ptr<IInput>			input = std::make_shared<Input>();
	std::shared_ptr<ShadowMap>		shadowMap = std::make_shared<ShadowMap>();
	std::shared_ptr<Movie>			movie = std::make_shared<Movie>();
	std::shared_ptr<Goal>			goal = std::make_shared<Goal>();

	std::shared_ptr<ICamera>			camera = std::make_shared<Camera>(input);
	Square square;
	square.pos = { 600.0f ,450.0f };
	square.size = { 300.0f ,80.0f };

	std::shared_ptr<Gauge>			special_move_gauge = std::make_shared<Gauge>(15.0f,square);//必殺技ゲージ
	std::shared_ptr<Player>			player = std::make_shared<Player>(collision, special_move_gauge,input,camera);

	shadowMap->Init();
	shadowMap->DrawSetUP_Stage(*map);
	float timeScale = 1.0f;
	bool is_exit=false;
	bool is_press_button = false;
	float stickX = input->getLeftStick().x;
	float stickY = input->getLeftStick().y;
	float charge_speed=0.0f;
	int handle_commands = LoadGraph("graph/commands.png");
	int alpha = 255;
	MoveDirection dir;
	auto ResetGame = [&]() {
		alpha = 255;
		player->Init();
		camera->Init();
		special_move_gauge->Init(0.0f);
		select_scene = GameScene::PLAY;
		enemies->Init();
		time->Start();
		goal->Init();
		};

	// メインループ
	while (ProcessMessage() == 0 && !is_exit)
	{
		// メインウインドウ用の描画
		ClearDrawScreen();
		clsDx();
		sound->DrawDebug();
		input->Update();
		sound->PlayBGM(SoundBGM::TITLE);

		if (input->IsPress(Command::LSHIFT) && input->IsPress(Command::RETURN))is_exit = true;
		switch (select_scene)
		{
		case GameScene::TITLE:

			DrawDemoPlayMovie(movie);
			stickY = input->getLeftStick().y;
#ifdef DEBUG
			printfDx("stickY:%f\n", stickY);

#endif // DEBUG
			UpdateMenuSelection(is_press_button, input, select_menu, sizeof(TitleMenuItems) / sizeof(MenuItem),sound);

			font->DrawFormatChar(100, 200, "Street Graffiti\n", static_cast<int>(FontSize::font1_Size_80), Config::ColorWhite);
			DrawMenuItems(TitleMenuItems, TitleMenuCount, select_menu, font);

			if (input->IsTrigger(Command::RETURN)||input->IsTrigger(Command::BtnB)) {
				sound->PlaySE(SoundEffect::DECIDE);
				TitleMenu selected = static_cast<TitleMenu>(select_menu);

				if (selected==TitleMenu::MENU_START) {
					ResetGame();
				}
				else if (selected==TitleMenu::MENU_OPTION) {
					select_scene = GameScene::OPTION;
				}
			}
			break;
		case GameScene::OPTION:

			DrawDemoPlayMovie(movie);
			dir = input->getNavigationInput();
#ifdef DEBUG
			printfDx("stick_X:%f\nstick_Y:%f\n",stickX,stickY);
#endif // DEBUG
			if (dir == MoveDirection::NONE) {
				is_press_button = false;
			}

			if (!is_press_button && dir != MoveDirection::NONE)
			{
				is_press_button = true;

				switch (dir)
				{
				case MoveDirection::UP:
				case MoveDirection::DOWN:
					UpdateMenuSelection(is_press_button, input, select_option_menu, sizeof(OptionMenuItems) / sizeof(MenuItem), sound);
					break;
				case MoveDirection::LEFT:
					if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_SE)) sound->SetSEVolume(sound->getSeVolume() - 5);
					if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_BGM))sound->SetBGMVolume(sound->getBgmVolume() - 5);
					break;
				case MoveDirection::RIGHT:
					if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_SE)) sound->SetSEVolume(sound->getSeVolume() + 5);
					if (select_option_menu == static_cast<unsigned int>(OptionMenu::OPTION_BGM)) sound->SetBGMVolume(sound->getBgmVolume() + 5);
					break;
				}
			}

			font->DrawFormatChar(200, 100, "オプション\n", static_cast<int>(FontSize::GAGA_Size_150), Config::ColorWhite);
			
			for (int i = 0; i < OptionMenuCount; i++) {
				bool is_active = (select_option_menu== i);
				unsigned int color = is_active ? Config::ColorOrange : Config::ColorWhite;
				const char* text = is_active ? OptionMenuItems[i].active_txt : OptionMenuItems[i].nomal_txt;
				int font_size = i == static_cast<int>(OptionMenu::OPTION_BUCK) ? static_cast<int>(FontSize::GAGA_Size_30) : static_cast<int>(FontSize::font1_Size_30);
				font->DrawFormatChar(
					Config::MenuLeftX,
					300 + OptionMenuItems[i].y_offset,
					text,
					font_size,
					color
				);
			}

			font->DrawFormatNum(350, 300, static_cast<float>(sound->getBgmVolume()), static_cast<unsigned int>(FontSize::font1_Size_30), Config::ColorWhite);

			font->DrawFormatNum(350, 330, static_cast<float>(sound->getSeVolume()), static_cast<unsigned int>(FontSize::font1_Size_30), Config::ColorWhite);

			if (input->IsTrigger(Command::RETURN)||input->IsTrigger(Command::BtnB)) {
				sound->PlaySE(SoundEffect::DECIDE);
				OptionMenu selected = static_cast<OptionMenu>(select_option_menu);
				if (selected == OptionMenu::OPTION_BUCK) {
					select_scene = GameScene::TITLE;
				}
			}
			break;
		case GameScene::PLAY:
			
			enemies->Update(*player, *collision, static_cast<float>(fpsController->getDeltaTime())/*, *effect*/, map->getModelHandle());
			player->Update(timeScale,enemies->getEnemies(), static_cast<float>(fpsController->getDeltaTime()), map->getModelHandle());
			goal->Update(*player);

			skydome->Update(camera);
			skydome->Draw();
			shadowMap->DrawSetUP_Chara(*player, *enemies);
			shadowMap->SetUse(*player, *enemies, *map);
			goal->Draw();
			//graffiti->Update(*enemy, *effect);
			time->Count(timeScale,*player);
			time->Draw(*font);
			if (alpha > 0) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				alpha -= 2;
				font->DrawFormatChar(
					static_cast<int>(Config::ScreenWidth / 3.0f),
					static_cast<int>(Config::ScreenHeight / 3.0f),
					"Run!!\n",static_cast<int>(FontSize::font1_Size_180),Config::ColorRunMsg);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}
			charge_speed = player->getCurrentState() != State::S_SPECIAL_ATTACK ? 15.0f / (60.0f * 15.0f) : 0.0f;
			special_move_gauge->Update(charge_speed);
			
			
			if (special_move_gauge->getIsMaxNum()) {
				//中身の描画
				special_move_gauge->DrawGauge_2d(255, 255, 100);
			}
			else {
				special_move_gauge->DrawGauge_2d(255, 255, 255);
			}

			
			//操作方法描画
			DrawExtendGraph(10,10,230,300,handle_commands,TRUE);
			movie->Update(player->getCurrentMovieState());
			//カメラのアップデート
			camera->Update(*player, timeScale);
			//effect->Update(*enemies,*player);

			if (time->GetTimeLimit() <= 0.0f||goal->getIsGoal()||player->getIsHpMin()) {
				select_scene = GameScene::END;
			}
			break;
		case GameScene::END:
			input->SetEnabled(true);
			DrawDemoPlayMovie(movie);

			stickY = input->getLeftStick().y;
			UpdateMenuSelection(is_press_button, input, select_end_menu, sizeof(EndMenuItems) / sizeof(MenuItem),sound);
			goal->getIsGoal() ?
				font->DrawFormatChar(200, 200, "Game Clear\n", static_cast<int>(FontSize::font1_Size_80), GetColor(255,127,80)) :
				font->DrawFormatChar(200, 200, "Game Over\n", static_cast<int>(FontSize::font1_Size_80), GetColor(0, 191, 255));
			DrawMenuItems(EndMenuItems, EndMenuCount, select_end_menu, font);

			if (input->IsTrigger(Command::RETURN)||input->IsTrigger(Command::BtnB)) {
				sound->PlaySE(SoundEffect::DECIDE);
				End_Menu selected = static_cast<End_Menu>(select_end_menu);
				if (selected == End_Menu::END_TITLE) {
					select_scene = GameScene::TITLE;
				}
				else if (selected == End_Menu::END_REPLAY) {
					ResetGame();
				}
				
			}
			break;
		}

#ifdef DEBUG
		if (CheckHitKey(KEY_INPUT_F1)) timeScale += 0.01f;
		if (CheckHitKey(KEY_INPUT_F2)) timeScale -= 0.01f;
		if (timeScale <= 0) 	timeScale = 0.0f;
		else if (timeScale >= 3.0f)timeScale = 3.0f;

#endif // DEBUG

		fpsController->DrawDebug();

		ScreenFlip();
		fpsController->Update();
	}
	shadowMap->Delete();
	//Effkseer_End();
	// ＤＸライブラリの後始末
	DxLib_End();

	return 0;
}

void UpdateMenuSelection(bool& is_press_button, const std::shared_ptr<IInput>key, int& select_index, int max_count,std::shared_ptr<Sound>sound)
{
	float stickY = key->getLeftStick().y;
	bool up = (stickY > Config::StickThreshold) || key->IsTrigger(Command::BtnUP) || key->IsTrigger(Command::UP);
	bool down = (stickY < -Config::StickThreshold) || key->IsTrigger(Command::BtnDOWN) || key->IsTrigger(Command::DOWN);

	if ((up || down) && !is_press_button) {
		if (up)select_index = (select_index - 1 + max_count) % max_count;
		if (down)select_index = (select_index + 1 + max_count) % max_count;
		is_press_button = true;
		sound->PlaySE(SoundEffect::SELECT);
	}
	//入力なし
	else if (std::abs(stickY) <= Config::StickThreshold) {
		is_press_button = false;
	}
}

void DrawMenuItems(const MenuItem* items, int count, int selected_index, std::shared_ptr<Font> font)
{
	for (int i = 0; i < count; i++) {
		bool is_active = (selected_index == i);
		font->DrawFormatChar(
			Config::MenuLeftX,
			Config::TitleTextY + items[i].y_offset,
			is_active ? items[i].active_txt : items[i].nomal_txt,
			static_cast<int>(FontSize::GAGA_Size_30),
			is_active ? Config::ColorOrange : Config::ColorWhite
		);
	}
}

void DrawDemoPlayMovie(std::shared_ptr<Movie>movie)
{
	int black = GetColor(0, 0, 0);
	movie->Update(static_cast<int>(MovieState::M_TITLE));
	movie->Play();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
	DrawBox(0, 0, 960, 540, black, true);
	//ブレンドモードを通常状態に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}