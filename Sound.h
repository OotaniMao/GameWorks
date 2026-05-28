#pragma once
#include<array>


enum class SoundEffect :int{
	SELECT,
	DECIDE,
	COUNT,
};

enum class SoundBGM :int {
	TITLE,
	PLAY,
	COUNT,
};

class Sound
{
private:
	std::array<int, static_cast<int>(SoundEffect::COUNT)> se_handle{};
	std::array<int, static_cast<int>(SoundBGM::COUNT)> bgm_handle{};
	int current_bgm;
	int bgm_volume;
	int se_volume;
public:
	Sound();
	~Sound();

	Sound(const Sound&) = delete;//コピーコンストラクタ禁止
	Sound& operator=(const Sound&) = delete;//代入演算子禁止

	int getBgmVolume()const { return bgm_volume; }
	int getSeVolume()const { return se_volume; }

	void PlaySE(SoundEffect soundId);
	void PlayBGM(SoundBGM soundId);
	void SetBGMVolume(int volume);
	void SetSEVolume(int volume);
	void StopBGM();
	void DrawDebug()const;
private:
	void Init();
};
