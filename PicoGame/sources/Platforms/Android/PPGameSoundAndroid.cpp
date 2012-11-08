/*-----------------------------------------------------------------------------------------------
	名前	PPGameSoundAndroid.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include "QBSound.h"

extern "C" {
//	int FILLSoundBuffer(short* buffer,int size);
	void EXITSound();
	int PPGame_GetSelectingBGM();
	void PPGame_SetSelectingBGM(int flag);
	void PPGameBGM_SetCommand(int command,int value);
};

void PPGame_IdleBGM(void* controller,int playBGM,bool playBGMOneTime,int chooseBGM,int x,int y,int w,int h);

void PPGameBGM_SetCommand(int command,int value)
{
}

static int selectbgmflag = 0;

void EXITSound()
{
	QBSound::exitSound();
}

//int FILLSoundBuffer(short* buffer,int size)
//{
//	QBSound* snd = QBSound::sharedSound();
//	if (snd == NULL) {
//		snd = new QBSoundAndroid(5);
//		if (snd == NULL) return 0;
//		snd->Init();
//		snd->Reset();
////		QBSound_Start(5);
////		snd = QBSound_Instance();
////		snd->Init();
//	}
//	return	snd->fill_sound_buffer(buffer,size);
//}

int PPGame_GetSelectingBGM()
{
	return selectbgmflag;
}

void PPGame_SetSelectingBGM(int flag)
{
	selectbgmflag = flag;
}

void PPGame_IdleBGM(void* controller,int playBGM,bool playBGMOneTime,int chooseBGM,int x,int y,int w,int h)
{
	if (playBGM != 0) {
		if (playBGMOneTime) {
			PPGameBGM_SetCommand(0,playBGM);
		} else {
			PPGameBGM_SetCommand(1,playBGM);
		}
	}
	if (chooseBGM != 0) {
		PPGameBGM_SetCommand(2,chooseBGM);
	}
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
