/*-----------------------------------------------------------------------------------------------
	名前	PPGameButton.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameButton.h"


short PPGame_CharDeltaX(PPGameImage* patternMaster,int group,int pat)
{
	return patternMaster[group].pat[pat*9+5];
}

short PPGame_CharDeltaY(PPGameImage* patternMaster,int group,int pat)
{
	return patternMaster[group].pat[pat*9+6];
}

short PPGame_CharDeltaW(PPGameImage* patternMaster,int group,int pat)
{
	return patternMaster[group].pat[pat*9+3];
}

short PPGame_CharDeltaH(PPGameImage* patternMaster,int group,int pat)
{
	return patternMaster[group].pat[pat*9+4];
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
