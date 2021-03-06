/*-----------------------------------------------------------------------------------------------
	名前	PPGameSound.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

int PPGameMML_Play(const char* mml,int track);
int PPGameMML_Stop(int track);
int PPGameMML_IsPlaying(int track);
int PPGameMML_SetVolume(int track,float volume=1.0);
int PPGameMML_SetMasterVolume(float volume=1.0);
float PPGameMML_GetMasterVolume();

bool PPGame_GetSoundEnable();
void PPGame_SetSoundEnable(bool enable);

#ifdef __cplusplus
};
#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
