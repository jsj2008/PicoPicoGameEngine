//
//  PPTTFont-Def.h
//  PicoGame
//
//  Created by 山口 博光 on 2012/11/09.
//
//

#ifndef PicoGame_PPTTFont_Def_h
#define PicoGame_PPTTFont_Def_h

#include <math.h>
//#include "PPGameUtil.h"
#include <freetype/ftglyph.h>
#ifdef __COCOS2DX__
#include "Cocos2dxWrapper.h"
#endif
#include "PPGameUtil.h"
#include "PPGameSprite.h"

#include <freetype/ft2build.h>
#include FT_FREETYPE_H

typedef struct _FTFONT {
	FT_Library library;
	FT_Face face;
	int type;
} FTFONT;

#endif
