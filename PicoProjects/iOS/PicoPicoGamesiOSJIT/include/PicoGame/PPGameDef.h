/*-----------------------------------------------------------------------------------------------
	名前	PPGameDef.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#define QBFILL_TAG (-1)
#define QBLINE_TAG (-2)
#define QBFONT_TAG (-3)
#define QBCHAR_TAG (-4)
#define QBVIEW_TAG (-5)
#define QBBOX_TAG (-6)
#define QBLINE_LOOP_TAG (-7)
#define QBLINE_STRIP_TAG (-8)
#define QBTRIANGLE_STRIP_TAG (-9)
#define QBTRIANGLE_FAN_TAG (-10)

typedef struct _PPGameImage {
	short* pat;
	short width;
	short height;
	short chipWidth;
	short texWidth;
	short texHeight;
} PPGameImage;

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameTextureOption {
public:
	PPGameTextureOption();

	bool operator == (PPGameTextureOption& o);

	static int wrap(const char* name);
	static const char* wrap(int wrap);
	
	bool linear;
	int wrap_s;		//GL_CLAMP_TO_EDGE or GL_REPEAT or GL_CLAMP
	int wrap_t;		//GL_CLAMP_TO_EDGE or GL_REPEAT or GL_CLAMP
};

typedef struct _PPGameTextureInfo {
	char* name;
	PPGameImage* group;
	int chipsize;
	PPGameTextureOption option;
} PPGameTextureInfo;

#define PAD_UP		0x01
#define PAD_DOWN	0x02
#define PAD_LEFT	0x04
#define PAD_RIGHT	0x08
//#define PAD_ARROW	0x8000
#define PAD_A		0x0010
#define PAD_B		0x0020
#define PAD_C		0x0040
#define PAD_D		0x0080
#define PAD_E		0x0100
#define PAD_F		0x0200
#define PAD_G		0x0400
#define PAD_H		0x0800

#define PAD_X		0x0040
#define PAD_Y		0x0080
#define PAD_L		0x0100
#define PAD_R		0x0200

#define PAD_Start	0x1000
#define PAD_SetUP	0x2000
#define PAD_Pause	0x4000
#define PAD_ANY		(-1)

#ifdef _ANDROID
#define SDEXT "ogg"
#else
#ifdef WIN32
#define SDEXT "wav"
#elif TARGET_OS_IPHONE
#define SDEXT "caf"
#else
#define SDEXT "aiff"
#endif
#endif

//#ifdef _ANDROID
//#include <android/log.h>
//#define LOG_TAG "PicoPicoGames"
//#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#else
//#define LOGD(...)
//#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
