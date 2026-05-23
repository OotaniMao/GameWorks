#include"DxLib.h"
#include "Movie.h"

/*
	簡易クロマキー処理のサンプル

	・動画を一度スクリーンに描画
	・GraphBlendBlt を使って RGBA を再構成
	・緑成分を反転してアルファに使用することで
	  「緑っぽい部分を透明にする」疑似クロマキー

	※ 注意
	この方法は「G成分のみ」を基準にしているため、
	黄色・黄緑・白など緑を含む色も透明になってしまう
*/

Movie::Movie()
	:width(960),
	height(540),
	current_state(MovieState::M_NONE),
	total_frame(0),
	is_play_end(false)
{
	//動画の初期化
	Init();
	
	//クロマキー用スクリーンの作成
	screen = MakeScreen(width, height, TRUE);
	out_screen= MakeScreen(width, height, TRUE);
	
}

Movie::~Movie()
{
	for (auto& m : movie_handle) {
		if (m.second.handle != -1) {
			//動画ハンドルの解放
			DeleteGraph(m.second.handle);
		}
	}
	// スクリーン解放
	if (screen != -1) DeleteGraph(screen);
	if (out_screen != -1) DeleteGraph(out_screen);
}

void Movie::Init()
{
	Load(static_cast<int>(MovieState::M_SPECIAL_MOVE), "movie/Smoke5.mp4", false, true);
	Load(static_cast<int>(MovieState::M_SPECIAL_CAMERA), "movie/Smoke3.mp4", false, true);
	Load(static_cast<int>(MovieState::M_SPECIAL_CAMERA2), "movie/Ink3.mp4", false, true);
	Load(static_cast<int>(MovieState::M_SPECIAL_CAMERA3), "movie/Ink2.mp4", false, true);
	Load(static_cast<int>(MovieState::M_SPECIAL_CAMERA4), "movie/Smoke2.mp4", false, true);
	Load(static_cast<int>(MovieState::M_TITLE), "movie/TitleMovie.mp4", true, false);
}

void Movie::Load(const int& movie_state, const char* file_name, bool is_movie_loop, bool is_chroma)
{
	movie_handle[movie_state] = { LoadGraph(file_name),is_movie_loop,is_chroma };
}

void Movie::PlayMovie(MovieState movie_state)
{
	auto& elem = movie_handle[movie_state];
	PlayMovieToGraph(elem.handle,elem.is_loop? DX_PLAYTYPE_LOOP:DX_PLAYTYPE_NORMAL, FALSE);
}

void Movie::Setting(const MovieState&curr_state)
{
	auto itr = movie_handle.find(curr_state);
	if (itr == movie_handle.end() || itr->second.handle == -1) {
		printfDx("No movie handle found for state:%d\n",(int)curr_state);
		return;
	}

	//再生リストをクリアして現在の状態を追加
	play_index.clear();
	play_index.push_back(curr_state);
	int current_frame = TellMovieToGraphToFrame(itr->second.handle);
	//動画を先頭フレームに移動
	SeekMovieToGraphToFrame(itr->second.handle,0);
	current_frame = TellMovieToGraphToFrame(itr->second.handle);
	//再生開始
	PlayMovie(curr_state);
	current_frame = TellMovieToGraphToFrame(itr->second.handle);
	//フレーム数を取得
	is_play_end = false;
}


void Movie::Play()
{
	if (play_index.empty())return;

	auto state = play_index.front();
	auto& elem = movie_handle[state];

	if (elem.is_chromakey) {
		//クロマキー処理あり
		ChromaKeying();
		DrawExtendGraph(0, 0, width, height, out_screen, TRUE);
	}
	else {
		//通常描画
		DrawExtendGraph(0, 0, width, height, movie_handle[state].handle, TRUE);
	}
}

void Movie::Update(const int&curr_state)
{
	previous_state = current_state;
	current_state = static_cast<MovieState>(curr_state);
	if (current_state == MovieState::M_NONE)return;
	if (current_state != previous_state) {
		Setting(current_state);
	}
	Play();
}

void Movie::ChromaKeying()
{
	//動画をスクリーンに描画
	SetUseSetDrawScreenSettingReset(FALSE);
	SetDrawScreen(screen);
	ClearDrawScreen();
	DrawExtendGraph(0, 0, width, height, movie_handle[current_state].handle, TRUE);
	SetDrawScreen(DX_SCREEN_BACK);
	SetUseSetDrawScreenSettingReset(TRUE);

	//RGBAを再構成して疑似クロマキー処理
	GraphBlendBlt(
		screen,		//転送元グラフィック
		screen,		//マスク（今回は同じもの）
		out_screen, //出力先グラフィック
		255,		//不透明度　DX_GRAPH_BLEND_RGBA_SELECT_MIXの時は関係ない
		DX_GRAPH_BLEND_RGBA_SELECT_MIX,
		DX_RGBA_SELECT_SRC_R,		// 出力:R	元のR成分
		DX_RGBA_SELECT_SRC_R,		// 出力:G	元のR成分
		DX_RGBA_SELECT_SRC_B,		// 出力:B	元のB成分
		DX_RGBA_SELECT_SRC_INV_G	// 出力:アルファ　元のG成分の反転
	);
}
