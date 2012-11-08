/*-----------------------------------------------------------------------------------------------
	名前	PPGameSound.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameSound.h"
#include "QBSound.h"
#include <stdlib.h>

static bool soundEnable = true;

bool PPGame_GetSoundEnable()
{
	return soundEnable;
}

void PPGame_SetSoundEnable(bool enable)
{
	soundEnable = enable;
}

int PPGameMML_Play(const char* mml,int track)
{
	if (QBSound::sharedSound() == NULL) return -1;
	return QBSound::sharedSound()->play(mml,track);
}

int PPGameMML_Stop(int track)
{
	if (QBSound::sharedSound() == NULL) return -1;
	return QBSound::sharedSound()->stop(track);
}

int PPGameMML_IsPlaying(int track)
{
	if (QBSound::sharedSound() == NULL) return 0;
	return QBSound::sharedSound()->isPlaying(track);
}

int PPGameMML_SetVolume(int track,float volume)
{
	if (QBSound::sharedSound() == NULL) return -1;
	return QBSound::sharedSound()->setVolume(track,volume);
}

int PPGameMML_SetMasterVolume(float volume)
{
	if (QBSound::sharedSound() == NULL) return -1;
	return QBSound::sharedSound()->setMasterVolume(volume);
}

float PPGameMML_GetMasterVolume()
{
	if (QBSound::sharedSound() == NULL) return 0;
	return QBSound::sharedSound()->getMasterVolume();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
