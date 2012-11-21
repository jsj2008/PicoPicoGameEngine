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
#include "CCFileUtils.h"
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

static unsigned getCharBytesUTF8( const char* string )
{
    const char ch = string[0];

    // マルチバイト文字かどうか
    if( ch & 0x80 ){

        // 先頭文字かどうか
        if( ch & 0x40 ){
            if( !(ch & 0x20 ) ){
                return 2;
            }
            else if( !(ch & 0x10) ){
                return 3;
            }
            else {
                return 4;
            }
        } else {
            // 不正な文字です
            return 0;
        }
    }
    else {
        return 1;
    }
}

static unsigned int ConvertCharUTF8toUTF32( const char* string, int* bytes )
{   
    if( !string || *string == '\0' ){
        if( bytes ){
            *bytes = 0;
        }
        return 0;
    }
    
    const char* p = string;
    unsigned b  = getCharBytesUTF8( string );
    unsigned ch = 0;

    
    switch( b ){
    case 1:
        ch = *p;
        break;
    case 2:
        ch |= ( p[0] & 0x1F ) << 6;
        ch |= ( p[1] & 0x3F );
        break;
    case 3:
        ch |= ( p[0] & 0x0F ) << 12;
        ch |= ( p[1] & 0x3F ) << 6;
        ch |= ( p[2] & 0x3F );
        break;
    case 4:
        ch |= ( p[0] & 0x07 ) << 18;
        ch |= ( p[1] & 0x3F ) << 12;
        ch |= ( p[2] & 0x3F ) << 6;
        ch |= ( p[3] & 0x3F );
        break;
    default:
        ch = 0;
        b  = 0;
    }

    if( bytes ){
        *bytes = b;
    }

    return ch;
}

#endif
