#include"DxLib.h"
#include"R_Math.h"
#include"Sound.h"
#define DEBUG

Sound::Sound()
	:se_volume(100),
	bgm_volume(100),
	current_bgm(-1)	
{
	Init();
}

Sound::~Sound()
{
	for(auto& se : se_handle) {
		if(se >= 0) DeleteSoundMem(se);
	}
	for(auto& bgm : bgm_handle) {
		if(bgm >= 0) DeleteSoundMem(bgm);
	}
}

void Sound::Init()
{
	se_handle.fill(-1);
	bgm_handle.fill(-1);
	se_handle[static_cast<int>(SoundEffect::SELECT)] = LoadSoundMem("sound/select_button_se.mp3");
	se_handle[static_cast<int>(SoundEffect::DECIDE)] = LoadSoundMem("sound/start_button_se.mp3");
	bgm_handle[static_cast<int>(SoundBGM::TITLE)] = LoadSoundMem("sound/bgm1.mp3");
}


void Sound::PlaySE( SoundEffect soundId)
{
	int id = static_cast<int>(soundId);
	int handle = se_handle[id];
	if (handle < 0)return;
	ChangeVolumeSoundMem(se_volume * 255 / 100, handle);
	PlaySoundMem(handle, DX_PLAYTYPE_BACK);
	
}

void Sound::PlayBGM( SoundBGM soundId)
{
	int id = static_cast<int>(soundId);
	int handle = bgm_handle[id];
	if (handle < 0)return;
	if (current_bgm == handle) return;
	StopBGM();
	current_bgm = handle;
	ChangeVolumeSoundMem(bgm_volume * 255 / 100, handle);
	PlaySoundMem(handle, DX_PLAYTYPE_LOOP);
}

void Sound::SetBGMVolume(int volume)
{
	bgm_volume = R_Math::Clamp(0, 100, volume);
	if (current_bgm >= 0)ChangeVolumeSoundMem(bgm_volume*255/100,current_bgm);
}

void Sound::SetSEVolume(int volume)
{
	se_volume = R_Math::Clamp(0, 100, volume);
}

void Sound::StopBGM()
{
	if (current_bgm >= 0) {
		StopSoundMem(current_bgm);
		current_bgm = -1;
	}
}

void Sound::DrawDebug() const
{
#ifdef DEBUG
	printfDx("se_volume=%d,dxvol=%d\n", se_volume, se_volume * 255 / 100);
#endif // DEBUG

}