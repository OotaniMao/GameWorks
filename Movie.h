#pragma once
#include<map>
#include<deque>

enum MovieState :int {
	NONE=-1,
	SPECIAL_MOVE=0,
	SPECIAL_CAMERA=1,
	SPECIAL_CAMERA_2=2,
	SPECIAL_CAMERA_3=3,
	SPECIAL_CAMERA_4=4,
	TITLE = 5,
};

typedef struct MovieElement{
	int handle;//動画ハンドル
	bool is_loop;
	bool is_chromakey;//クロマキーを使用するかどうか　true:する　false:しない
}MovieElement;

class Movie
{
private:
	std::map<int, MovieElement> movie_handle;
	std::deque<MovieState>play_index;
	std::deque<bool>play_is_loop;
	int total_frame;
	bool is_play_end;

	int screen;//動画を描画するための一時スクリーン（アルファあり）
	int out_screen;//クロマキー処理後の結果を書き込むスクリーン
	int width, height;//スクリーンの縦横サイズ
	
	MovieState current_state;
	MovieState previous_state;
	
public:
	Movie();//コンストラクタ
	~Movie();//デストラクタ

	void Init();
	void Load(const int& movieState,const char*fileName,bool isMovieLoop,bool isChroma);
	void PlayMovie(MovieState movieState);
	void Setting(const MovieState& currState);
	/// @brief 動画の再生
	void Play();
	/// @brief 更新処理
	void Update(const int& currState);
	/// @brief 疑似クロマキー
	/// @brief 必ずPlayの前に置くこと
	/// @brief 今回は黄色だけを抽出
	void ChromaKeying();
};
