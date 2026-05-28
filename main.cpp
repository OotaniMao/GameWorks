#include"DxLib.h"
#include<memory>
#include"Config.h"
#include"GameMain.h"
#include"SceneUtility.h"
//#define DEBUG

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
	// ウインドウモードで起動
	ChangeWindowMode(true);
	SetGraphMode(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::COLOR_BIT);
	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	SetDrawScreen(DX_SCREEN_BACK);
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);

	SetUseLighting(TRUE);
	SetLightDirection(VGet(1.0f, -1.0f, 0.0f));
	SetLightAmbColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	//宣言＆初期化
	auto game_main = std::make_unique<GameMain>();
	bool is_exit = false;
	
	game_main->Init();
	// メインループ
	while (ProcessMessage() == 0 && !is_exit)
	{
		// メインウインドウ用の描画
		ClearDrawScreen();
		clsDx();
		
		if (!game_main)break;
		game_main->Run();

		if (CheckHitKey(KEY_INPUT_LSHIFT) && CheckHitKey(KEY_INPUT_RETURN)) is_exit = true;

		ScreenFlip();
	}
	game_main->Terminal();
	// ＤＸライブラリの後始末
	DxLib_End();

	return 0;
}
