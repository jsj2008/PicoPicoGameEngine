/*-----------------------------------------------------------------------------------------------
	名前	PPGameUtil.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGAMEUTIL_H__
#define __PPGAMEUTIL_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <string>

#pragma once

#define PPGAME_KEY_SIDE_RIGHT "key_side_right"
#define PPGAME_KEY_SIDE_LEFT "key_side_left"

enum {
	QBGAME_LOCALE_OTHER,
	QBGAME_LOCALE_JAPANESE
};

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

const char* PPGameResourcePath(const char* name);
const char* PPGameDocumentPath(const char* dbfile);

int PPGetInteger(const char* key,int defaultValue);
void PPSetInteger(const char* key,int value,bool sync=true);

float PPGetNumber(const char* key,float defaultValue);
void PPSetNumber(const char* key,float value,bool sync=true);

std::string PPGetString(const char* key,const std::string & defaultValue = "");
void PPSetString(const char* key,const std::string & value,bool sync=true);

void PPSync();

unsigned char* PPGame_GetData(const char* key,int* dataSize);
void PPGame_SetData(const char* key,unsigned char* data,int dataSize);
unsigned char* PPGame_LoadPNG(const char* path,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow);
unsigned char* PPGame_DecodePNG(unsigned char* bytes,unsigned long size,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow);
const char* PPGameLoadShader(const char* name);

int PPGame_InitBGM(int no,const char* key);
int PPGame_InitBGMiPad(int no,const char* key);
void PPGame_ReleaseBGM();
void PPGame_IdleBGM(void* controller,int playBGM,bool playBGMOneTime,int chooseBGM,int x,int y,int w,int h);

int PPGame_GetLocale();
const char* PPGame_LocaleString(const char* jp,const char* en);

int PPGame_LoadAIFF(const char* name,const char* type);
int PPGame_PlayAIFF(int soundId);

void PPGame_Vibrate();
int PPGame_3GSLater();

const char* PPGameMainLua();
void PPGameSetMainLua(const char* luaname);
void PPGameSetDataPath(const char* datapath);
const char* PPGameDataPath(const char* name);

#ifdef __cplusplus
extern "C" {
#endif

int PPGame_GetSelectingBGM();
void PPGame_SetSelectingBGM(int flag);

#ifdef __cplusplus
};
#endif

#ifdef _WIN32
#define M_PI		3.14159265358979323846
#endif

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
