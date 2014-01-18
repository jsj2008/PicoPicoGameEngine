/*-----------------------------------------------------------------------------------------------
	名前	QBGame.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBGame.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <list>
#include <cstdio>
#include <locale>
#include <sstream>

#include "PPGameSound.h"
#include "PPGameMap.h"
#include "PPGameUtil.h"
#include "PPGameTexture.h"
#define _PG_GLOBAL_
#include "PPImageFontData.h"
#include "PPImageFontLayout.h"
#include "QBSound.h"

#include "PPFont.h"
#include "PPNormalFont.h"
#include "PPImageFont.h"
#include "PPTTFont.h"

#ifndef NO_COCOSDENSHION
#include <CocosDenshion/SimpleAudioEngine.h>
#endif

#ifdef __LUAJIT__
#include <lua.hpp>
#else
#include <lua/lstate.h>
#endif

#if TARGET_OS_IPHONE
#include "PPTextToSpeech.h"
#endif

//enum {
//	QBCHAR_FONT
//};

static PPGameMap* gameMap=NULL;

static int funcPPHitCheck(lua_State *L);

enum {
  WORK_MEM_HIT1=0,
  WORK_MEM_HIT2,
};

static unsigned char colorTable[][4] = {
	{0x00,0x00,0x00,0xff},	//0
	{0x00,0x00,0xa0,0xff},	//1
	{0x00,0xa0,0x00,0xff},	//2
	{0x00,0xa0,0xa0,0xff},	//3
	{0xa0,0x00,0x00,0xff},	//4
	{0xa0,0x00,0xa0,0xff},	//5
	{0xa0,0xa0,0x00,0xff},	//6
	{0xa0,0xa0,0xa0,0xff},	//7
	{0x40,0x40,0x40,0xff},	//8
	{0x00,0x00,0xff,0xff},	//9
	{0x00,0xff,0x00,0xff},	//10
	{0x00,0xff,0xff,0xff},	//11
	{0xff,0x00,0x00,0xff},	//12
	{0xff,0x00,0xff,0xff},	//13
	{0xff,0xff,0x00,0xff},	//14
	{0xff,0xff,0xff,0xff},	//15
	{0x00,0x00,0x00,0xff},	//16
	{0x00,0x00,0xff,0xff},	//17
	{0x00,0xff,0x00,0xff},	//18
	{0x00,0xff,0xff,0xff},	//19
	{0xff,0x00,0x00,0xff},	//20
	{0xff,0x00,0xff,0xff},	//21
	{0xff,0x80,0x00,0xff},	//22
	{0x80,0x80,0x80,0xff},	//23
	{0x40,0x40,0x40,0xff},	//24
	{0x40,0x40,0xff,0xff},	//25
	{0x40,0xff,0x40,0xff},	//26
	{0x40,0xff,0xff,0xff},	//27
	{0xff,0x40,0x40,0xff},	//28
	{0xff,0x40,0xff,0xff},	//29
	{0xff,0xff,0x40,0xff},	//30
	{0xff,0xff,0xff,0xff},	//31
	{0x00,0x20,0xff,0xff},	//32
	{0x00,0x40,0xff,0xff},	//33
	{0x00,  96,0xff,0xff},	//34
	{0x00,0x80,0xff,0xff},	//35
	{0x00, 160,0xff,0xff},	//36
	{0x00, 192,0xff,0xff},	//37
	{0x00, 224,0xff,0xff}	//38
};

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

unsigned long QBGame::blinkCounter=0;

//static int QBRand()
//{
//	return rand();
//}

QBGame::QBGame() : __tarray(NULL),__normalFont(NULL),__halfFont(NULL),__miniFont(NULL),systemTextureID(0)
{
	curFont = NULL;
	later3GS = true;
	colorIndex = 15;
	chooseBGM = 0;
	playBGM = 0;
	__touchCount = 0;
	keyRepeatTimer = 0;
#ifdef _ANDROID
	openManual = false;
	showAd = true;
	showMarket = false;
#endif
	
	for (int i=0;i<QBGAME_MAX_COLORTABLE;i++) {
		colortable[i][0] = 0;
		colortable[i][1] = 0;
		colortable[i][2] = 0;
		colortable[i][3] = 0;
	}
	for (int i=0;i<sizeof(colorTable)/(sizeof(unsigned char)*4);i++) {
		colortable[i][0] = colorTable[i][0];
		colortable[i][1] = colorTable[i][1];
		colortable[i][2] = colorTable[i][2];
		colortable[i][3] = colorTable[i][3];
	}
	
	prekey = 0;
	key = 0;
	
	scale_value = PPPoint(1.0,1.0);
	rotate_value = 0.0;
	poly.blend = PPGameBlend::None();
	rotate_center.x = 0;
	rotate_center.y = 0;
	
	gameMap = NULL;
	
	keyConfig = false;
	
	screenWidth = 320;
	screenHeight = 480;
	
	hdVersion = false;
	stdVersion = false;
	fontWidth = 16;

	curTexture = 0;

	drawAdStep = 0;
	__anyFlags = 0;//QBGAME_CONTROLLER_ENABLE_ARROW | QBGAME_CONTROLLER_ENABLE_A | QBGAME_CONTROLLER_ENABLE_B;
	
	fontLoaded = false;
	lineWrap = false;
  pauseButton = false;

	poly.reset();
	
	QBSound* snd = QBSound::sharedSound();
	if (snd) snd->Reset();

  for (int i=0;i<10;i++) {
    workmem[i]=NULL;
    workmemsize[i]=0;
  }
}

QBGame::~QBGame()
{
	if (gameMap) delete gameMap;
	gameMap = NULL;
	if (__tarray) delete __tarray;
	if (__normalFont) delete __normalFont;
	if (__halfFont) delete __halfFont;
	if (__miniFont) delete __miniFont;
#ifndef NO_COCOSDENSHION
	CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
#endif
  for (int i=0;i<10;i++) {
    if (workmem[i]) {
      free(workmem[i]);
    }
    workmem[i]=NULL;
    workmemsize[i]=0;
  }
}

PPTextureArray* QBGame::texture()
{
	if (__tarray == NULL) {
		__tarray = new PPTextureArray();
	}
	return __tarray;
}

bool QBGame::KeyWait(unsigned long mask,bool t)
{
	if (t == false) {
		if ((!(prekey & mask)) && (key & mask)) {
			return true;
		}
	} else {
		if ((!(key & mask)) && (prekey & mask)) {
			return true;
		}
	}
	return false;
}

bool QBGame::Key(unsigned long mask)
{
	if ((key & mask)) {
		return true;
	}
	return false;
}

int QBGame::gameIdle(unsigned long ikey)
{
	if (!fontLoaded) {

		PPGameTexture* tex = new PPGameTexture();
    tex->name = "picopicodefaultfont.png";
		tex->group = PPImageFont_group;
		tex->manager = projector->textureManager;
    tex->embedpixel = EMBEDDED_PPIMAGE_FONT;
    tex->embedpixel_len = EMBEDDED_PPIMAGE_FONT_len;
		tex->loadTexture();
		fontTexture = projector->textureManager->setTexture(tex);

		__normalFont = new PPNormalFont(this,16,16);
		__normalFont->display(this, (PP_DRAW_SELECTOR)&QBGame::putNormalFont);
		__normalFont->setName("default");
		__normalFont->poly._texture = fontTexture;
		__halfFont = new PPNormalFont(this,8,16);
		__halfFont->display(this, (PP_DRAW_SELECTOR)&QBGame::putHalfFont);
		__halfFont->setName("half");
		__halfFont->poly._texture = fontTexture;
		__miniFont = new PPNormalFont(this,8,8);
		__miniFont->display(this, (PP_DRAW_SELECTOR)&QBGame::putMiniFont);
		__miniFont->setName("mini");
		__miniFont->poly._texture = fontTexture;
		fontLoaded = true;
	}
	
	key = ikey;
  
  if (key & PAD_Pause) {
    pauseButton = 1 - pauseButton;
  }
	projector->st.windowSize = PPSize(screenWidth,screenHeight);
	projector->st.viewPort = PPRect(0,0,screenWidth,screenHeight);
	projector->st.offset2 = PPPointZero;
	projector->unlockoffscreen();
	Idle();
	prekey = key;
	return 0;
}

int QBGame::Idle()
{
	fps.idle();
	PPStep::idle();
	return 0;
}

void QBGame::Clear()
{
	projector->Clear();
}

unsigned char* QBGame::GetData(const char* key,int* dataSize)
{
	return PPGame_GetData(key,dataSize);
}

void QBGame::SetData(const char* key,unsigned char* data,int dataSize)
{
	PPGame_SetData(key,data,dataSize);
}

unsigned char* QBGame::GetRGBColor(int color,unsigned char* r,unsigned char* g,unsigned char* b,unsigned char* a)
{
	unsigned char* c = &colortable[color][0];
	*r = c[0];
	*g = c[1];
	*b = c[2];
	*a = c[3];
	return c;
}

int QBGame::SetRGBColor(int color,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	if (color >= 0 && color < QBGAME_MAX_COLORTABLE) {
		unsigned char* c = &colortable[color][0];
		c[0] = r;
		c[1] = g;
		c[2] = b;
		c[3] = a;
	}
	return 0;
}

void QBGame::Color(int incolor)
{
	colorIndex = incolor;
}

void QBGame::Locate(float x,float y)
{
	locatex = x*16;
	locatey = y*16;
}

void QBGame::Offset(float x,float y)
{
	locatex = x;
	locatey = y;
}

int QBGame::rawLength(const char* str)
{
	float x,y;
	x = 0;
	y = 0;
	if (curFont) {
#ifndef NO_TTFONT
		if (curFont->ttfont) {
			PPGameBlend o = poly.blend;
      poly.blend = PPGameBlend::EdgeSmooth();
			for (int n=0;str[n]!=0;) {
				int len;
				char s[5]={0};
				ConvertCharUTF8toUTF32(&str[n],&len);
				for (int i=0;i<len;i++) s[i] = str[n+i];
				x += calcLength(curFont,s);
				n += getCharBytesUTF8(&str[n]);
			}
			poly.blend = o;
		} else {
			x = curFont->length(str);
		}
#else
		x = curFont->length(str);
#endif
	} else {
		x = __normalFont->length(str);
	}
	return x;
}

int QBGame::__length(const char* format,va_list args)
{
	int r=0;
	va_list argsdest;
	va_copy(argsdest, args);
	int len = vsnprintf(NULL,0,format,argsdest)+1;
	va_end(argsdest);
	if (len > 0) {
		char* str = (char*)calloc(1,len);
		if (str) {
			vsnprintf((char*)str,len,format,args);
			r = rawLength(str);
			free(str);
		}
	}
	return r;
}

void QBGame::__print(const char* format,va_list args)
{
	va_list argsdest;
	va_copy(argsdest, args);
	int len = vsnprintf(NULL,0,format,argsdest)+1;
	va_end(argsdest);
	if (len > 0) {
		char* str = (char*)calloc(1,len);
		if (str) {
			vsnprintf((char*)str,len,format,args);
			rawPrint(str);
			free(str);
		}
	}
}

void QBGame::Print(const char* format,...)
{
	va_list args;
	va_start(args,format);
	__print(format,args);
	va_end(args);
}

void QBGame::Print2(const char* format,...)
{
	va_list args;
	va_start(args,format);
	PPFont* oldFont = curFont;
	curFont = __normalFont;
	__print(format,args);
	curFont = oldFont;
	va_end(args);
}

void QBGame::Play(const char* mml,int ch)
{
	if (PPGame_GetSoundEnable()) PPGameMML_Play(mml,ch);
}

void QBGame::Stop(int ch)
{
	if (PPGame_GetSoundEnable()) PPGameMML_Stop(ch);
}

bool QBGame::IsPlaying(int ch)
{
	if (PPGame_GetSoundEnable()) return false;
	return PPGameMML_IsPlaying(ch);
}

void QBGame::Center(float cx,float cy)
{
	rotate_center.x = cx;
	rotate_center.y = cy;
}

void QBGame::origin(PPPoint origin)
{
	projector->st.origin = origin;
}

void QBGame::Scale(PPPoint scale,float cx,float cy)
{
	scale_value = scale;
	rotate_center.x = cx;
	rotate_center.y = cy;
}

void QBGame::Scale(PPPoint scale)
{
	scale_value = scale;
}

void QBGame::Rotate(float rotate,float cx,float cy)
{
	rotate_value = rotate*2*M_PI/360;
	rotate_center.x = cx;
	rotate_center.y = cy;
}

void QBGame::Rotate(float rotate)
{
	rotate_value = rotate*2*M_PI/360;
}

PPSize QBGame::PatternSize(int textureid,int group)
{
	return projector->textureManager->patternSize(textureid,group);
}

void QBGame::rawPrint(const char* str)
{
  if (str == NULL) return;
	PPPoint p;
	PPPoint oldscale = scale_value;
	p.x = locatex;
	p.y = locatey;
	if (curFont) {
#ifndef NO_TTFONT
		if (curFont->ttfont) {
			p.x = locatex;//*scale_value.x;
			p.y = locatey;//*scale_value.y;
			PPGameBlend o = poly.blend;
			poly.blend = PPGameBlend::EdgeSmooth();
			for (int n=0;str[n]!=0;) {
				int len;
				char s[5]={0};
				ConvertCharUTF8toUTF32(&str[n],&len);
				for (int i=0;i<len;i++) s[i] = str[n+i];
				int width = calcLength(curFont,s);

				if (lineWrap) {
					PPRect r = viewPort();
					if (r.width <= p.x+width/**scale_value.x*/) {
						p.x = 0;//locatex;
						p.y += curFont->height()/**scale_value.y*/;
					}
				}

				p.x += putFont(curFont,p.x,p.y,s);
				n += getCharBytesUTF8(&str[n]);
			}
			poly.blend = o;
		} else {
			p.x = curFont->print(p.x,p.y,str,lineWrap);
			p = curFont->cur;
		}
#else
		p.x = curFont->print(p.x,p.y,str,lineWrap);
		p = curFont->cur;
#endif
	} else
  if (__normalFont) {
		p.x = __normalFont->print(p.x*scale_value.x,p.y*scale_value.y,str,lineWrap);
		p.x /= scale_value.x;
	}
	scale_value = oldscale;
	printlocate.x = p.x;
	printlocate.y = p.y;
}

#ifndef NO_TTFONT
int QBGame::putFont(PPFont* basefont,float x,float y,const char* str)
{
  PPGameBlend blend_value=poly.blend;
	PPTTFont* font=basefont->ttfont;
	float px,py;
	px = x;//basefont->localScale.x;
	py = y;//basefont->localScale.y;
	PPTTFontTile* tile = font->image(str);
	if (tile == NULL) return 0;
	PPTTFontImage* base = font->base;
	unsigned char r,g,b,a;
	if (font->isUpdated()) {
		projector->textureManager->setTexture(font->texture,base->pixel,base->width,base->height,base->bytePerRow,font->option);
		font->updated();
	}
	GetRGBColor(colorIndex,&r,&g,&b,&a);
	unsigned short splite_flags=poly.flags;
	{
		PPGamePoly poly;

		poly.origin = rotate_center;//+x;
		poly.rotate = rotate_value;
		poly.color.r = r;
		poly.color.g = g;
		poly.color.b = b;
		poly.color.a = a;
		poly.flags = splite_flags;
		poly.blend = blend_value;
		poly._texture = font->texture;
		poly.texTileSize.width = font->tileWidth();
		poly.texTileSize.height = font->tileHeight();
		poly.scale.x = scale_value.x;
		poly.scale.y = scale_value.y;

		unsigned long os = projector->st.flags;
		projector->st.flags |= PPGameState::UseLocalScale;
		projector->st.localScale = basefont->localScale;

		float tx,ty;
		px += tile->bearingX*curFont->localScale.x;
		ty = py;
		for (int y=0;y<tile->gheight();y++) {
			tx = px;
			for (int x=0;x<tile->gwidth();x++) {
				int t = tile->tile[x+y*tile->gwidth()];
				if (t!=0) {
					poly.sprite(tx,ty,t,0);
          //if (projector->textureManager->checkBind(font->texture)) 
          if (poly._texture>=0) {
            projector->DrawPoly(&poly);
          }
					tx += font->tileWidth();//*scale().x;
				}
			}
			ty += font->tileHeight();
		}

		projector->st.flags = os;
	}

	return (tile->advanceX+tile->bearingX)*curFont->localScale.x;//*scale().x;
}

int QBGame::calcLength(PPFont* basefont,const char* str)
{
	PPTTFont* font=basefont->ttfont;
	int px,py;
	px = 0;
	py = 0;
	PPTTFontTile* tile = font->length(str);
	{
		int tx,ty;
		px += tile->bearingX*curFont->localScale.x;
		ty = py;
		for (int y=0;y<tile->gheight();y++) {
			tx = px;
			for (int x=0;x<tile->gwidth();x++) {
				int t = tile->tile[x+y*tile->gwidth()];
				if (t!=0) {
					tx += font->tileWidth();
				}
			}
			ty += font->tileHeight();
		}
	}
	return (tile->advanceX+tile->bearingX)*curFont->localScale.x;
}
#endif

void QBGame::putNormalFont(PPPoint pos,unsigned short gid)
{
	unsigned char r,g,b,a;
	GetRGBColor(colorIndex,&r,&g,&b,&a);
	unsigned long os = projector->st.flags;
	projector->st.flags |= PPGameState::UseLocalScale;
	projector->st.localScale = PPPoint(2,2);
	PutFont(pos.x,pos.y,gid,r,g,b,a);
	projector->st.flags = os;
}

void QBGame::putHalfFont(PPPoint pos,unsigned short gid)
{
	unsigned char r,g,b,a;
	GetRGBColor(colorIndex,&r,&g,&b,&a);
	unsigned long os = projector->st.flags;
	projector->st.flags |= PPGameState::UseLocalScale;
	projector->st.localScale = PPPoint(1,2);
	PutFont(pos.x,pos.y,gid,r,g,b,a);
	projector->st.flags = os;
}

void QBGame::putMiniFont(PPPoint pos,unsigned short gid)
{
	unsigned char r,g,b,a;
	GetRGBColor(colorIndex,&r,&g,&b,&a);
	PutFont(pos.x,pos.y,gid,r,g,b,a);
}

void QBGame::Put(float x,float y,int pat,int group,int image,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	if (image < 0) return;
	poly.origin = rotate_center;//+x;
	poly.rotate = rotate_value;
	poly.color.r = r;
	poly.color.g = g;
	poly.color.b = b;
	poly.color.a = a;
//	poly.blend = blend_value;
	poly.sprite(x,y,pat,group);
	poly._texture = image;

	poly.scale = scale_value;
	projector->DrawPoly(&poly);
}

void QBGame::PutFont(float x,float y,int pat,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	poly.origin = rotate_center;//+x;
	poly.rotate = rotate_value;
	poly.color.r = r;
	poly.color.g = g;
	poly.color.b = b;
	poly.color.a = a;
	poly.sprite(x,y,pat,PG_FONT);
	poly.initTexture(fontTexture);
	poly.scale = scale_value;
	projector->DrawPoly(&poly);
}

void QBGame::Line(float x1,float y1,float x2,float y2,int color)
{
	poly.origin = rotate_center;//+x1;
	poly.rotate = rotate_value;
	poly.color.r = colortable[color][0];
	poly.color.g = colortable[color][1];
	poly.color.b = colortable[color][2];
	poly.color.a = colortable[color][3];
	poly.line(x1,y1,x2,y2);
	poly.scale = scale_value;
	projector->DrawPoly(&poly);
}

void QBGame::Fill(float x1,float y1,float x2,float y2,int color)
{
	poly.origin = rotate_center;//+x1;
	poly.rotate = rotate_value;
	poly.color.r = colortable[color][0];
	poly.color.g = colortable[color][1];
	poly.color.b = colortable[color][2];
	poly.color.a = colortable[color][3];
	poly.fill(x1,y1,x1+x2,y1+y2);
	poly.scale.x = scale_value.x;
	poly.scale.y = scale_value.y;
	projector->DrawPoly(&poly);
}

void QBGame::Box(float x1,float y1,float x2,float y2,int color)
{
	poly.origin = rotate_center;//+x1;
	poly.rotate = rotate_value;
	poly.color.r = colortable[color][0];
	poly.color.g = colortable[color][1];
	poly.color.b = colortable[color][2];
	poly.color.a = colortable[color][3];
	poly.scale.x = scale_value.x;
	poly.scale.y = scale_value.y;
	poly.box(x1,y1,x1+x2,y1+y2);
	projector->DrawPoly(&poly);
}

void QBGame::StartPoint()
{
  projector->StartPoint();
}

void QBGame::SetPoint(float x,float y)
{
  projector->SetPoint(x,y,colortable[colorIndex]);
}

void QBGame::ClosePoint(int type)
{
  projector->ClosePoint(type);
}

//bool QBGame::LoadMap(const char* mapname)
//{
//	if (gameMap) delete gameMap;
//	gameMap = new PPGameMap(PPGameResourcePath(mapname));
//	if (gameMap) {
//		map[0] = &gameMap->map[0];
//		map[1] = &gameMap->map[1];
//		map[2] = &gameMap->map[2];
//		return true;
//	}
//	return false;
//}

void* QBGame::LoadData(const char* name,unsigned long* dataSize)
{
	const char* path = PPGameResourcePath(name);
	FILE* fp = fopen(path,"rb");
	if (fp) {
		fseek(fp,0,SEEK_END);
		unsigned long size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		unsigned char* str = (unsigned char*)malloc(size);
		fread(str,size,1,fp);
		fclose(fp);
		*dataSize = size;
		return str;
	}
	return NULL;
}

int QBGame::SaveData(const char* name,void* data,unsigned long dataSize)
{
	const char* path = PPGameResourcePath(name);
	FILE* fp = fopen(path,"wb");
	if (fp) {
		fwrite(data,1,dataSize,fp);
		fclose(fp);
		return 0;
	}
	return -1;
}

void QBGame::RepeatKey(int firstDelay,int secondDelay)
{
	if (key == prekey && key != 0) {
		keyRepeatTimer ++;
		if (keyRepeatTimer > firstDelay) {
			keyRepeatTimer = secondDelay;
			prekey = 0;
		}
	}
	if (key == 0) {
		keyRepeatTimer = 0;
	}
}

unsigned long QBGame::GetKey()
{
	return key;
}

unsigned long QBGame::GetPreKey()
{
  return prekey;
}

void QBGame::SetKey(unsigned long ikey)
{
	key = ikey;
}

PPGameTextureInfo* QBGame::TextureInfo()
{
	return texture()->info();
}

void QBGame::DrawAd()
{
#ifdef __PICOPICO_AD__
	switch (WindowLayout()) {
	case QBGAME_CONTROLLER_TYPE_HORIZONTAL:
	case QBGAME_CONTROLLER_TYPE_H_LEFT:
	case QBGAME_CONTROLLER_TYPE_H_RIGHT:
		{
			int x,y,w,h;
			w = 72;
			h = 64;
			x = 16;
			y = screenHeight-h-16;

			Fill(x,y,w,h,0);
			Box(x,y,w,h,7);
			switch (drawAdStep) {
			case 0:
				if (TouchCount() == 0) {
					drawAdStep ++;
				}
				break;
			case 1:
				if (TouchCount() > 0) {
					int tx = TouchXPosition(0);
					int ty = TouchYPosition(0);
					if (x < tx && y < ty && tx < x+w && ty < y+h) {
#ifdef _ANDROID
						showMarket = true;
#endif
					}
					drawAdStep = 0;
				}
				break;
			}
		}
		break;
	default:
		Fill(0,screenHeight-36,screenWidth,36,0);
		Scale(2.0);
		Scale(1.0);
		Box(0,screenHeight-36,screenWidth,36,7);
		Offset(16+32+16,screenHeight-32+8);
		Color(15);
		Print("PicoPicoGames");
		switch (drawAdStep) {
		case 0:
			if (TouchCount() == 0) {
				drawAdStep ++;
			}
			break;
		case 1:
			if (TouchCount() > 0) {
				if (TouchYPosition(0) > screenHeight-32) {
#ifdef _ANDROID
					showMarket = true;
#endif
				}
				drawAdStep = 0;
			}
			break;
		}
		break;
	}
#endif
}

void QBGame::font(const char* name)
{
	if (name == NULL) {
		curFont = __normalFont;
	} else {
		curFont = projector->textureManager->fontManager->getFont(name);
	}
}

#pragma mark -

#include "PPLuaScript.h"

static int funcPut(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		PPPoint scale;
		PPPoint p;
		PPColor c= m->color();
		PPColor nc = c;
		bool localScale=false;
		int n=0;
		if (s->isTable(L,n)) {
			PPRect r;
			r = s->getRect(L,n,PPRect(0,0,-1,-1));
			p.x = r.x;
			p.y = r.y;
			if (!(r.width < 0 || r.height < 0)) {
				localScale = true;
				scale = PPPoint(r.width/m->poly.texTileSize.width,r.height/m->poly.texTileSize.height);
			}
		} else {
			p = s->getPoint(L,n);
			n++;
		}
		n++;
		int d = (int)s->integer(n)-1;
		if (d < 0) return 0;
		n ++;
		int g = 0;//atoi(s->args(3));
		if (n < s->argCount) {
			if (s->isNumber(L,n)) {
				m->curTexture = (int)s->integer(n)-1;
				nc=s->getColor(L,n+1,nc);
			} else
			if (s->isTable(L,n)) {
				m->curTexture = (int)s->tableInteger(L,n,"index",0)-1;		//Texture
				nc=s->getColor(L,n+1,nc);
			} else {
				PPGameTextureOption option;
				if (n+1 < s->argCount && s->isTable(L,n+1)) {
					option = s->getTextureOption(L,n+1,option);//boolean(n+1);
					nc=s->getColor(L,n+2,nc);
				}
				m->curTexture = m->projector->textureManager->loadTexture(s->args(n),option);

				if (PPReadError()) {
					PPReadErrorReset();
					return luaL_error(L,"texture file read error '%s'",s->args(n));
				}
			}
		} else {
			m->curTexture = m->projector->textureManager->defaultTexture;
		}
		if (localScale) {
			unsigned long os = m->projector->st.flags;
			m->projector->st.flags |= PPGameState::UseLocalScale;
			m->projector->st.localScale = scale;
			m->Put(p.x,p.y,d,g,m->curTexture,nc.r,nc.g,nc.b,nc.a);
			m->projector->st.flags = os;
		} else {
			m->Put(p.x,p.y,d,g,m->curTexture,nc.r,nc.g,nc.b,nc.a);
		}
	}
	return 0;
}

static int funcLocate(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPPoint pos;
	if (s->argCount > 0) {
		pos = PPPoint(m->locatex,m->locatey);
		if (s->isTable(L,0)) {
			pos = PPPoint(s->tableNumber(L,0,1,"x",pos.x),s->tableNumber(L,0,2,"y",pos.y));
		} else
		if (s->argCount > 1) {
			pos = PPPoint(s->number(0),s->number(1));
		}
		m->locate(pos);
	}
	pos = PPPoint(m->locatex,m->locatey);
	return s->returnPoint(L,pos);
}

static int funcMove(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPPoint pos;
	if (s->argCount > 0) {
		pos = PPPoint(m->locatex,m->locatey);//m->locate();
		if (s->isTable(L,0)) {
			pos = PPPoint(pos.x+s->tableNumber(L,0,1,"x"),pos.y+s->tableNumber(L,0,2,"y"));
		} else
		if (s->argCount > 1) {
			pos = PPPoint(pos.x+s->number(0),pos.y+s->number(1));
		}
		m->locate(pos);
	}
	pos = PPPoint(m->locatex,m->locatey);
	return s->returnPoint(L,pos);
}

static int funcPrint(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		PPColor c= m->color();
		PPColor nc = c;
		nc = s->getColor(L,1,nc);
		m->color(nc);
		PPGamePoly p = m->poly;	
		m->rawPrint(s->args(0));
		m->poly = p;
		m->color(c);
	}
	return s->returnPoint(L,m->printlocate);
}

static int funcColor(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPColor color = m->color();
	if (s->argCount > 0) {
		color = s->getColor(L,0,color);
		m->color(color);
		return 0;
	}
	color = m->color();
	lua_createtable(L, 0, 4);
	lua_pushinteger(L,color.r);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L,color.g);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L,color.b);
	lua_setfield(L, -2, "b");
	lua_pushinteger(L,color.a);
	lua_setfield(L, -2, "a");
	return 1;
}

static int funcBlend(lua_State *L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}

	if (s->argCount > 0) {
		m->poly.blend.blend = lua_toboolean(L,2);
    if (s->argCount > 1) {
      m->poly.blend.blendSrc = (int)lua_tointeger(L,3);
      if (s->argCount > 2) {
        m->poly.blend.blendDst = (int)lua_tointeger(L,4);
      }
    }
    return 0;
  }

  lua_pushboolean(L,m->poly.blend.blend);
	lua_pushnumber(L,m->poly.blend.blendSrc);
	lua_pushnumber(L,m->poly.blend.blendDst);
	return 3;
}

static int funcFog(lua_State *L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}

	if (s->argCount > 0) {
		m->poly.blend.fog = lua_toboolean(L,2);
    if (s->argCount > 1) {
      if (s->isTable(L,1)) {
        m->poly.blend.fogColor.r = s->tableInteger(L,1,1,"r",m->poly.color.r);
        m->poly.blend.fogColor.g = s->tableInteger(L,1,2,"g",m->poly.color.g);
        m->poly.blend.fogColor.b = s->tableInteger(L,1,3,"b",m->poly.color.b);
        m->poly.blend.fogColor.a = s->tableInteger(L,1,4,"a",m->poly.color.a);
      } else {
        m->poly.blend.fogColor.r = s->integer(1);
        if (s->argCount > 1) m->poly.blend.fogColor.g = s->integer(2);
        if (s->argCount > 2) m->poly.blend.fogColor.b = s->integer(3);
        if (s->argCount > 3) m->poly.blend.fogColor.a = s->integer(4);
      }
    }
		return 0;
	}
  
  lua_pushboolean(L,m->poly.blend.fog);
  
	lua_createtable(L, 0, 4);
	lua_pushinteger(L,m->poly.blend.fogColor.r);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L,m->poly.blend.fogColor.g);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L,m->poly.blend.fogColor.b);
	lua_setfield(L, -2, "b");
	lua_pushinteger(L,m->poly.blend.fogColor.a);
	lua_setfield(L, -2, "a");

  return 2;
}

static int funcScale(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->scale_value.x = s->tableNumber(L,0,1,"x",m->scale_value.x);
			m->scale_value.y = s->tableNumber(L,0,2,"y",m->scale_value.y);
		} else {
			if (s->argCount == 1) {
				m->scale_value.x = s->number(0);
				m->scale_value.y = s->number(0);
			} else {
				m->scale_value.x = s->number(0);
				m->scale_value.y = s->number(1);
			}
		}
		return 0;
	}
	return s->returnPoint(L,m->scale_value);
}

static int funcFlip(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	unsigned short type = 0;
	if (s->argCount > 0) {
		if (s->boolean(0)) {
			type |= PPFLIP_H;
		}
		if (s->argCount > 1) {
			if (s->boolean(1)) {
				type |= PPFLIP_V;
			}
		}
		if (s->argCount > 2) {
			type |= ((s->integer(2)<<2) & PPFLIP_RMASK);
		}
		m->poly.flags = type;
		return 0;
	}
	lua_pushboolean(L,(m->poly.flags & PPFLIP_H)!=0);
	lua_pushboolean(L,(m->poly.flags & PPFLIP_V)!=0);
	lua_pushinteger(L,(m->poly.flags & PPFLIP_RMASK)>>2);
	return 3;
}

static int funcRotate(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		m->rotate_value = s->number(0);//atof(s->args(0));
		while (m->rotate_value < 0) {
			m->rotate_value += 2*M_PI;
		}
		while (m->rotate_value > 2*M_PI) {
			m->rotate_value -= 2*M_PI;
		}
    return 0;
	}
	lua_pushnumber(L,m->rotate_value);
	return 1;
}

static int funcLineWrap(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		m->lineWrap = s->boolean(0);
    return 0;
	}
	lua_pushboolean(L,m->lineWrap);
	return 1;
}

static int funcOrigin(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->rotate_center.x = s->tableNumber(L,0,1,"x",m->rotate_center.x);
			m->rotate_center.y = s->tableNumber(L,0,2,"y",m->rotate_center.y);
		} else {
			if (s->argCount == 1) {
				m->rotate_center.x = s->number(0);
				m->rotate_center.y = s->number(0);
			} else {
				m->rotate_center.x = s->number(0);
				m->rotate_center.y = s->number(1);
			}
		}
		return 0;
	}
	return s->returnPoint(L,m->rotate_center);
}

static int funcAlpha(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		PPColor c = m->color();
		c.a = s->integer(0);
		m->color(c);
	}
	{
		PPColor c = m->color();
		lua_pushinteger(L,c.a);
	}
	return 1;
}

static int funcFill(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPRect r = PPRect(0,0,0,0);
	PPColor c= m->color();
	PPColor nc = c;
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			r = s->getRect(L,0);
			nc = s->getColor(L,1,nc);
		} else {
			if (s->argCount > 3) {
				r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
				nc = s->getColor(L,4,nc);
			} else
			if (s->argCount > 2) {
				r = PPRect(s->number(0),s->number(1),s->number(2),s->number(2));
			} else {
				r = PPRect(s->number(0),s->number(1),1,1);
			}
		}
	}
	m->color(nc);
	m->fill(r);
	m->color(c);
	return 0;
}

static int funcBox(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPRect r = PPRect(0,0,0,0);
	PPColor c= m->color();
	PPColor nc = c;
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			r = s->getRect(L,0);
			nc = s->getColor(L,1,nc);
		} else {
			if (s->argCount > 3) {
				r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
				nc = s->getColor(L,4,nc);
			} else
			if (s->argCount > 2) {
				r = PPRect(s->number(0),s->number(1),s->number(2),s->number(2));
			} else {
				r = PPRect(s->number(0),s->number(1),1,1);
			}
		}
	}
	m->color(nc);
	m->box(r);
	m->color(c);
	return 0;
}

static int funcLine(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPPoint p1,p2;
	PPColor c= m->color();
	PPColor nc = c;
#if 1
	int paramError=0;
	while (true) {
		if (s->argCount > 0) {
			int n=0;
			if (s->isTable(L,0)) {
				p1 = s->getPoint(L,0);
				n++;
			} else {
				if (s->argCount > n+1) {
					p1 = PPPoint(s->number(0),s->number(1));
					n+=2;
				} else {
					paramError=n+2;
					break;
				}
			}
			if (s->argCount > n) {
				if (s->isTable(L,n)) {
					p2 = s->getPoint(L,n);
					n++;
				} else {
					if (s->argCount > n+1) {
						p2 = PPPoint(s->number(n),s->number(n+1));
						n+=2;
					} else {
						paramError=n+2;
						break;
					}
				}
			} else {
				paramError=n+1;
				break;
			}
			if (s->argCount > n) {
				nc = s->getColor(L,n,nc);
			}
			m->color(nc);
			m->line(p1,p2);
			m->color(c);
		} else {
			paramError=1;
			break;
		}
		break;
	}
	if (paramError>0) {
		return luaL_argerror(L,paramError+1,"no value");
	}
#else
//	if (s->argCount > 3) {
//		nc = s->getColor(L,4,nc);
//		m->color(nc);
//		m->line(PPPoint(s->number(0),s->number(1)),PPPoint(s->number(2),s->number(3)));
//		m->color(c);
//	} else {
//		return luaL_argerror(L,2,"no value");
//	}
#endif
	return 0;
}

static int funcCircle(lua_State* L)
{
  int top=lua_gettop(L);
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);

  int type=QBLINE_LOOP_TAG;
  PPPoint center;
  PPPoint radius(1,1);
  float st=0;
  float ed=360;
	PPColor c= m->color();
	PPColor nc = c;
  int n=PPLuaArg::getPPPoint(L,2,center);
  if (n>=2) {
    int n=3;
    if (!lua_istable(L,2)) {
      n++;
    }
    PPLuaArg::getPPPoint(L,n,radius);
    if (!lua_istable(L,n)) {
      radius.y=radius.x;
    }
    n++;
    if (top >= n) {
      nc = s->getColor(L,n-2);
      if (!s->isTable(L,n-2)) {
        n +=3;
      }
      n++;
      if (top >= n) {
        st = lua_tonumber(L,n);
        n++;
        if (top >= n) {
          ed = lua_tonumber(L,n);
        }
      }
    }
  }

  m->color(nc);
  m->StartPoint();

	while (st < 0) {
		st += 360;
		ed += 360;
	}
	while (st >= 360) {
		st -= 360;
		ed -= 360;
	}
  
  if (st > 0 || ed < 360) type=QBLINE_STRIP_TAG;
  
  st = st*M_PI*2/360;
  ed = ed*M_PI*2/360;

  {
    float k_segments = 16.0f;
    float r=radius.x;
    if (radius.y>r) r=radius.y;
    k_segments = floorf(r/4);
    if (k_segments < 16) k_segments=16;
    if (k_segments > 100) k_segments=100;
    const float k_increment = 2.0f * M_PI / k_segments;
    float theta = st;
    float px,py;
    for (int i = 0; i < k_segments; ++i)
    {
      px=radius.x*cosf(theta)+center.x;
      py=radius.y*sinf(theta)+center.y;
      m->SetPoint(px,py);
      if (theta > ed) break;
      theta += k_increment;
      if (theta > ed) theta = ed;
    }
  }

  m->ClosePoint(type);
  m->color(c);

  return 0;
}

static int funcTriangle(lua_State* L,int type)
{
  int top=lua_gettop(L);
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);

	PPColor c= m->color();
	PPColor nc = c;

  if (top>1) {

    float v[2];
    int vn=0;
    {
      if (lua_istable(L,top)) {
        nc = s->getColor(L,3-2,nc);
      }

      m->color(nc);
      m->StartPoint();
      
      vn=0;
      
      //for (int i=2;i<=top;i++) 
      {
        int i=2;
        if (lua_istable(L,i)) {
          vn=0;
          lua_getfield(L,i,"x");
          if (!lua_isnil(L,-1)) {
            v[vn++] = lua_tonumber(L,-1);
          }
          lua_pop(L,1);
          lua_getfield(L,i,"y");
          if (!lua_isnil(L,-1)) {
            v[vn++] = lua_tonumber(L,-1);
          }
          lua_pop(L,1);

#ifdef __LUAJIT__
          int n=(int)lua_objlen(L,i);
#else
          int n=(int)lua_rawlen(L,i);
#endif
        
          for (int j=1;j<=n;j++) {
            
            lua_rawgeti(L,i,j);
            
            if (lua_istable(L,-1)) {
              vn=0;
              lua_getfield(L,-1,"x");
              if (!lua_isnil(L,-1)) {
                v[vn++] = lua_tonumber(L,-1);
              }
              lua_pop(L,1);
              lua_getfield(L,-1,"y");
              if (!lua_isnil(L,-1)) {
                v[vn++] = lua_tonumber(L,-1);
              }
              lua_pop(L,1);
              if (vn < 2) {
                lua_rawgeti(L,-1,1);
                if (!lua_isnil(L,-1)) {
                  v[vn++] = lua_tonumber(L,-1);
                }
                lua_pop(L,1);
                lua_rawgeti(L,-1,2);
                if (!lua_isnil(L,-1)) {
                  v[vn++] = lua_tonumber(L,-1);
                }
                lua_pop(L,1);
              }
              if (vn>=2) {
                m->SetPoint(v[0],v[1]);
                vn=0;
              }
            } else {
              v[vn++] = lua_tonumber(L,-1);
              if (vn>=2) {
                m->SetPoint(v[0],v[1]);
                vn=0;
              }
            }

            lua_pop(L,1);
          }

        } else {
          v[vn++] = lua_tonumber(L,i);
        }
        
        if (vn>=2) {
          m->SetPoint(v[0],v[1]);
          vn=0;
        }
      }

      m->ClosePoint(type);
      m->color(c);
    }
  }
  return 0;
}

static int funcTriangleFan(lua_State* L)
{
  return funcTriangle(L,QBTRIANGLE_FAN_TAG);
}

static int funcTriangleStrip(lua_State* L)
{
  return funcTriangle(L,QBTRIANGLE_STRIP_TAG);
}

static int funcTouch(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
#if 1
	lua_createtable(L,m->touchCount(),0);
	if (m->touchCount() > 0) {
		for (int i=0;i<m->touchCount();i++) {
			s->returnPoint(L,PPPoint(m->TouchXPosition(i),m->TouchYPosition(i)));
			lua_rawseti(L,-2,i+1);
		}
	}
#else
	if (s->argCount > 0) {
		int i=s->integer(0)-1;//atoi(s->args(0)-1);
		if (i >= 0 && i < m->touchCount()) {
			return s->returnPoint(L,PPPoint(m->TouchXPosition(i),m->TouchYPosition(i)));
		} else {
			return luaL_argerror(L,2,lua_pushfstring(L,"invalid index %d",i+1));
		}
	} else
	if (m->touchCount() > 0) {
		lua_createtable(L,m->touchCount(),0);
		return s->returnPoint(L,PPPoint(m->TouchXPosition(0),m->TouchYPosition(0)));
	}
	lua_createtable(L,0,0);
#endif
	return 1;
}

static int funcFontWithName(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount < 1) {
		return luaL_argerror(L,2+0,"value expected");
	}
	if (s->argCount < 2) {
		return luaL_argerror(L,2+1,"value expected");
	}
	PPFont* font;
	if (s->argCount > 2) {
		font = new PPFont(m,s->args(0),atoi(s->args(2)));
	} else {
		font = new PPFont(m,s->args(0));
	}
	if (PPReadError()) {
		PPReadErrorReset();
		return luaL_error(L,"font file read error '%s'",s->args(0));
	}
	font->setName(s->args(1));
	m->curFont=font;
	return 0;
}

static int funcFontWithTexture(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 2) {
		PPImageFont* font;
		int texture = -1;
		if (lua_isstring(L,2)) {
			PPGameTextureOption option;
			texture = m->projector->textureManager->loadTexture(s->args(0),option);
			if (PPReadError()) {
				PPReadErrorReset();
				return luaL_error(L,"texture file read error '%s'",s->args(0));
			}
		} else
		if (s->isTable(L,0)) {
			texture = (int)s->tableInteger(L,0,"index",0)-1;		//Texture
		} else {
			texture = (int)s->integer(0)-1;
		}
		if (s->isTable(L,2)) {
			int top=lua_gettop(L);
#ifdef __LUAJIT__
			int t= (int)lua_objlen(L,2+2);
#else
			int t= (int)lua_rawlen(L,2+2);
#endif
			if (t > 0) {
				float n[4]={0};
				for (int i=0;i<t&&i<4;i++) {
					lua_rawgeti(L,2+2,i+1);
					n[i] = lua_tonumber(L,-1);
				}
				if (t == 1) {
					n[1] = n[0];
					n[2] = 1;
					n[3] = 1;
				}
				if (t == 2) {
					n[2] = 1;
					n[3] = 1;
				}
				if (t == 3) {
					n[3] = n[2];
				}
				if (n[0] > 0 && n[1] > 0) {
					font = new PPImageFont(m,texture,n[0],n[1]);
					font->localScale.x = n[2];
					font->localScale.y = n[3];
					font->setName(s->args(1));
					//m->font(s->args(1));
					m->curFont=font;
				}
			}
			lua_settop(L,top);
		} else
		if (s->argCount > 3) {
			if (s->integer(2) > 0 && s->integer(3) > 0) {
				font = new PPImageFont(m,texture,(int)s->integer(2),(int)s->integer(3));
				if (s->argCount > 5) {
					font->localScale.x = s->number(4);
					font->localScale.y = s->number(5);
				} else
				if (s->argCount > 4) {
					font->localScale.x = s->number(4);
					font->localScale.y = s->number(4);
				}
				font->setName(s->args(1));
				//m->font(s->args(1));
				m->curFont=font;
			}
		}
	}
	return 0;
}

static int funcFontScale(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPFont* font=NULL;
	if (m->curFont) {
		font=m->curFont;
	} else {
		font=m->__normalFont;
	}
	if (font) {
		if (s->argCount > 0) {
			PPPoint pos;
			if (s->isTable(L,0)) {
				pos = PPPoint(s->tableNumber(L,0,1,"x",pos.x),s->tableNumber(L,0,2,"y",pos.y));
			} else
			if (s->argCount > 1) {
				pos = PPPoint(s->number(0),s->number(1));
			}
			font->localScale=pos;
		} else {
			s->pushPoint(L,font->localScale);
			return 1;
		}
	}
	return 0;
}

static int funcFont(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isNumber(L,0)) {
			int index = (int)lua_tointeger(L,2);
			if (index > 0 && index <= m->projector->textureManager->fontManager->registCount()) {
				PPFont** f = m->projector->textureManager->fontManager->table();
				if (f) {
					int n=1;
					for (int i=0;i<m->projector->textureManager->fontManager->tableCount();i++) {
						if (f[i]) {
							if (n==index) {
								m->curFont=f[i];
								break;
							}
							n++;
						}
					}
				}
			}
		} else {
			m->font(s->args(0));
		}
	}
	if (m->curFont) {
		lua_pushstring(L,m->curFont->name());
	} else {
		lua_pushstring(L,"");
	}
	return 1;
}

static int funcFontHeight(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->curFont) {
		lua_pushnumber(L,m->curFont->height());
	} else {
		lua_pushnumber(L,m->__normalFont->height());
	}
	return 1;
}

static int funcFontLength(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		lua_pushinteger(L,m->rawLength(s->args(0)));
	} else {
		lua_pushinteger(L,0);
	}
	return 1;
}

static int funcFontRect(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPRect r;
	if (m->curFont) {
		r = PPRect(0,0,0,m->curFont->height());
	} else {
		r = PPRect(0,0,0,m->__normalFont->height());
	}
	if (s->argCount > 0) {
		r.width = m->rawLength(s->args(0));
	}
	return s->returnRect(L,r);
}

static int funcFontTexture(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPFont* font;
	if (m->curFont == NULL) {
		font = m->__normalFont;
	} else {
		font = m->curFont;
	}
	m->projector->textureManager->pushTextureInfo(L,font->poly._texture);
	return 1;
}

static int funcFontNames(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaScript::World(L);
	lua_createtable(L,m->projector->textureManager->fontManager->registCount(),0);
	PPFont** f = m->projector->textureManager->fontManager->table();
	if (f) {
		int n=1;
		for (int i=0;i<m->projector->textureManager->fontManager->tableCount();i++) {
			if (f[i]) {
				lua_pushstring(L,f[i]->name());
				lua_rawseti(L,-2,n);
				n++;
			}
		}
	}
	return 1;
}

static int funcFontDelete(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPFont* font=NULL;
	if (m->curFont == NULL) {
		font = m->__normalFont;
	} else {
		font = m->curFont;
	}
	if (font) {
		if (font != m->__normalFont
		 && font != m->__halfFont
		 && font != m->__miniFont) {
		 	delete font;
			m->curFont = NULL;
		}
	}
	return 0;
}

void QBGame::openFontLibrary(PPLuaScript* script,const char* name)
{
	script->openModule(name,this);
#ifndef NO_TTFONT
		script->addCommand("loadTTF",funcFontWithName);
#endif
		script->addCommand("loadTexture",funcFontWithTexture);
		script->addCommand("set",funcFont);
		script->addCommand("scale",funcFontScale);
		script->addCommand("height",funcFontHeight);
		script->addCommand("width",funcFontLength);
		script->addCommand("size",funcFontRect);
		script->addCommand("texture",funcFontTexture);
		script->addCommand("names",funcFontNames);
		script->addCommand("delete",funcFontDelete);
	script->closeModule();
}

static int funcLayout(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	bool centerx = false;
	bool centery = false;
	PPRect r = PPRect(0,0,0,0);
	PPRect v = m->viewPort();
	if (s->argCount > 0 && lua_type(L,2)==LUA_TSTRING) {
		if (m->curFont) {
			r = PPRect(0,0,0,m->curFont->height());
		} else {
			r = PPRect(0,0,0,m->__normalFont->height());
		}
		if (s->argCount > 0) {
			r.width = m->rawLength(s->args(0));
		}
		centerx = s->boolean(1);
		centery = s->boolean(2);
		if (s->argCount > 2) {
			v = s->getRect(L,3);
		}
	} else
	if (s->argCount > 1 && s->isTable(L,0)) {
		r = s->getRect(L,0);
		centerx = s->boolean(1);
		centery = s->boolean(2);
		if (s->argCount > 2) {
			v = s->getRect(L,3);
		}
	} else
	if (s->argCount > 4) {
		r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
		centerx = s->boolean(4);
		centery = s->boolean(5);
		if (s->argCount > 5) {
			v = s->getRect(L,6);
		}
	}
	int flag = 0;
	if (centerx) flag |= PP_CENTER_X;
	if (centery) flag |= PP_CENTER_Y;
	PPPoint p = m->layout(r.size(),r.pos(),flag,v);
	return s->returnRect(L,PPRect(p.x,p.y,r.width,r.height));
}

static int funcUpdate(lua_State* L)
{
#ifdef __LUAJIT__
  return lua_yield(L,0);
#else
	if (!(L->nny > 0))  {
		return lua_yield(L,0);
	}
#endif
	return 0;
}

static int funcClear(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	m->Clear();
	return 0;
}

static int funcRemove(lua_State* L)
{
	int top=lua_gettop(L);
	if (top >= 2) {
		luaL_checktype(L,1,LUA_TTABLE);
		luaL_checktype(L,2,LUA_TTABLE);
		
		lua_getfield(L,2,"prev");
		lua_getfield(L,2,"next");
		if (lua_istable(L,-2)) {
			lua_setfield(L,-2,"next");
		}
		
		lua_getfield(L,2,"next");
		lua_getfield(L,2,"prev");
		if (lua_istable(L,-2)) {
			lua_setfield(L,-2,"prev");
		}
		
		lua_getfield(L,1,"top");
		if (lua_rawequal(L,-1,2)) {
			lua_getfield(L,2,"next");
			lua_setfield(L,1,"top");
		}
		lua_getfield(L,1,"last");
		if (lua_rawequal(L,-1,2)) {
			lua_getfield(L,2,"prev");
			lua_setfield(L,1,"last");
		}
		
		lua_pushnil(L);
		lua_setfield(L,2,"prev");
		lua_pushnil(L);
		lua_setfield(L,2,"next");
		
		lua_settop(L,top);
	}
	return 0;
}

static int funcAppend(lua_State* L)
{
	int top=lua_gettop(L);
	if (top >= 2) {
		luaL_checktype(L,1,LUA_TTABLE);
		luaL_checktype(L,2,LUA_TTABLE);
		
		funcRemove(L);
		
		lua_getfield(L,1,"top");
		if (!lua_istable(L,-1)) {
			lua_pushvalue(L,2);
			lua_setfield(L,1,"top");
		}
		
		lua_getfield(L,1,"last");
		lua_setfield(L,2,"prev");

		lua_getfield(L,1,"last");
		if (lua_istable(L,-1)) {
			lua_pushvalue(L,2);
			lua_setfield(L,-2,"next");
		}

		lua_pushvalue(L,2);
		lua_setfield(L,1,"last");
	}
	return 0;
}

static int funcMousePosition(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	return s->returnPoint(L,m->mouseLocation);
}

static int funcKeyCommon(lua_State* L,unsigned long mask)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
  bool r=false;
  unsigned long b=m->GetKey()&mask;
  if (lua_gettop(L)>0) {
    unsigned long p=m->GetPreKey()&mask;
    bool t=lua_toboolean(L,1);
    if (t) {
      if (b!=0 && p==0) {
        r = true;
      }
    } else {
      if (b==0 && p!=0) {
        r = true;
      }
    }
  } else {
    r = (b!=0);
  }
	lua_pushboolean(L,r);
	return 1;
}

static int funcGetKeyUp(lua_State* L)
{
  funcKeyCommon(L,PAD_UP);
	return 1;
}

static int funcGetKeyDown(lua_State* L)
{
  funcKeyCommon(L,PAD_DOWN);
	return 1;
}

static int funcGetKeyLeft(lua_State* L)
{
  funcKeyCommon(L,PAD_LEFT);
	return 1;
}

static int funcGetKeyRight(lua_State* L)
{
  funcKeyCommon(L,PAD_RIGHT);
	return 1;
}

static int funcGetKeyA(lua_State* L)
{
  funcKeyCommon(L,PAD_A);
	return 1;
}

static int funcGetKeyB(lua_State* L)
{
  funcKeyCommon(L,PAD_B);
	return 1;
}

static int funcGetKeyC(lua_State* L)
{
  funcKeyCommon(L,PAD_C);
	return 1;
}

static int funcGetKeyD(lua_State* L)
{
  funcKeyCommon(L,PAD_D);
	return 1;
}

static int funcGetKeyE(lua_State* L)
{
  funcKeyCommon(L,PAD_E);
	return 1;
}

static int funcGetKeyF(lua_State* L)
{
  funcKeyCommon(L,PAD_F);
	return 1;
}

static int funcGetKeyG(lua_State* L)
{
  funcKeyCommon(L,PAD_G);
	return 1;
}

static int funcGetKeyH(lua_State* L)
{
  funcKeyCommon(L,PAD_H);
	return 1;
}

static int funcGetKeyX(lua_State* L)
{
  funcKeyCommon(L,PAD_X);
	return 1;
}

static int funcGetKeyY(lua_State* L)
{
  funcKeyCommon(L,PAD_Y);
	return 1;
}

static int funcGetKeyL(lua_State* L)
{
  funcKeyCommon(L,PAD_L);
	return 1;
}

static int funcGetKeyR(lua_State* L)
{
  funcKeyCommon(L,PAD_R);
	return 1;
}

static int funcGetKeyStartDiscovery(lua_State* L)
{
	PPGameControllerStartDiscoverty();
	return 0;
}

static int funcGetKeyStopDiscovery(lua_State* L)
{
	PPGameControllerStopDiscoverty();
	return 0;
}

static int functionGetKeyControllers(lua_State* L)
{
	lua_pushinteger(L,PPGameControllerCount());
	return 1;
}

static int functionGetKeyControllersInfo(lua_State* L)
{
  if (lua_gettop(L) <= 0) return 0;
  int index = (int)lua_tointeger(L,1)-1;
  PPGameControllerInfo(L,index);
  return 1;
}

static int functionGetKeyControllersPlayerIndex(lua_State* L)
{
  if (lua_gettop(L) <= 0) return 0;
  int index = (int)lua_tointeger(L,1)-1;
  if (index >= 0 && index < PPGameControllerCount()) {
    if (lua_gettop(L) > 1) {
      int playerIndex = (int)lua_tointeger(L,2);
      PPGameControllerSetPlayerIndex(index,playerIndex);
      return 0;
    }
    lua_pushinteger(L,PPGameControllerGetPlayerIndex(index));
    return 1;
  }
  return 0;
}

static int funcGetKeySetUP(lua_State* L)
{
  funcKeyCommon(L,PAD_SetUP);
	return 1;
}

static int funcGetKeyStart(lua_State* L)
{
  funcKeyCommon(L,PAD_Start);
	return 1;
}

static int funcGetKeyPause(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
  if (lua_gettop(L)>0) {
    m->pauseButton = lua_toboolean(L,1);
    return 0;
  }
	lua_pushboolean(L,m->pauseButton);
	return 1;
}

//static int funcGetDensity(lua_State* L)
//{
//	QBGame* m = (QBGame*)PPLuaArg::World(L);
//	lua_pushnumber(L,m->scale_factor);
//	return 1;
//}

static int funcStep(lua_State* L)
{
	if (lua_gettop(L)>1) {
		lua_setglobal(L,"___scriptIdleFunc");
	}
	return 0;
}

static int funcPlatform(lua_State* L)
{
#ifdef _ANDROID
	lua_pushstring(L,"android");
#else
#ifdef WIN32
	lua_pushstring(L,"windows");
#elif TARGET_OS_IPHONE
	lua_pushstring(L,"ios");
#else
	lua_pushstring(L,"mac");
#endif
#endif
	return 1;
}

static int funcFPS(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	lua_pushnumber(L,m->fps.frameRate());
	return 1;
}

static int funcDeltaTime(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	lua_pushnumber(L,m->fps.m_fDeltaTime);
	return 1;
}

static int funcMasterVolume(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		if (snd) snd->setMasterVolume(s->number(0));
		return 0;
	}
  if (snd) {
    lua_pushnumber(L,snd->getMasterVolume());
  } else {
    lua_pushnumber(L,0);
  }
	return 1;
}

static int funcSetDefaultNumber(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 1) {
		bool sync=true;
		if (s->argCount > 2) {
      if (!lua_isnil(L,4)) {
        sync=s->boolean(2);
      }
		}
		PPSetNumber(s->args(0),s->number(1),sync);
	}
	return 0;
}

static int funcGetDefaultNumber(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	float value = 0;
	if (s->argCount > 0) {
		if (s->argCount > 1) {
			value = s->number(1);
		}
		value = PPGetNumber(s->args(0),value);
	}
	lua_pushnumber(L,value);
	return 1;
}

static int funcSetDefaultInteger(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 1) {
		bool sync=true;
		if (s->argCount > 2) {
      if (!lua_isnil(L,4)) {
        sync=s->boolean(2);
      }
		}
		PPSetInteger(s->args(0),(int)s->integer(1),sync);
	}
	return 0;
}

static int funcGetDefaultInteger(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	int value = 0;
	if (s->argCount > 0) {
		if (s->argCount > 1) {
			value = (int)s->integer(1);
		}
		value = PPGetInteger(s->args(0),value);
	}
	lua_pushinteger(L,value);
	return 1;
}

static int funcSetDefaultString(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 1) {
		bool sync=true;
		if (s->argCount > 2) {
      if (!lua_isnil(L,4)) {
        sync=s->boolean(2);
      }
		}
		PPSetString(s->args(0),s->args(1),sync);
	}
	return 0;
}

static int funcGetDefaultString(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	std::string value = "";
	if (s->argCount > 0) {
		if (s->argCount > 1) {
			value = s->args(1);
		}
		value = PPGetString(s->args(0),value);
	}
	lua_pushstring(L,value.c_str());
	return 1;
}

//static int funcDump(lua_State* L)
//{
//	return 0;
//}

static int funcTile(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
    lua_getfield(L,2,"size");
    if (lua_istable(L,-1)) {
      m->poly.texTileSize = s->getSize(L,lua_gettop(L)-2);
    }
    lua_getfield(L,2,"stride");
    if (lua_istable(L,-1)) {
      PPPoint p=s->getPoint(L,lua_gettop(L)-2);
      m->poly.texTileStride = PPSize(p.x,p.y);
    }
    lua_getfield(L,2,"offset");
    if (lua_istable(L,-1)) {
      m->poly.texOffset = s->getPoint(L,lua_gettop(L)-2);
    }
		return 0;
	}

	lua_createtable(L, 0, 3);
	
	lua_createtable(L, 0, 4);
	lua_pushnumber(L,0);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,0);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L,m->poly.texTileSize.width);
	lua_setfield(L, -2, "width");
	lua_pushnumber(L,m->poly.texTileSize.height);
	lua_setfield(L, -2, "height");
  lua_getglobal(L,"pprect_mt");
  lua_setmetatable(L,-2);
	lua_setfield(L, -2, "size");
	
	lua_createtable(L, 0, 2);
	lua_pushnumber(L,m->poly.texTileStride.width);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,m->poly.texTileStride.height);
	lua_setfield(L, -2, "y");
  lua_getglobal(L,"pppoint_mt");
  lua_setmetatable(L,-2);
	lua_setfield(L, -2, "stride");
	
	lua_createtable(L, 0, 2);
	lua_pushnumber(L,m->poly.texOffset.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,m->poly.texOffset.y);
	lua_setfield(L, -2, "y");
  lua_getglobal(L,"pppoint_mt");
  lua_setmetatable(L,-2);
	lua_setfield(L, -2, "offset");

	return 1;
}

static int funcTileSize(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->poly.texTileSize = s->getSize(L,0);
		return 0;
	} else
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			m->poly.texTileSize.width = s->number(0);
			m->poly.texTileSize.height = s->number(0);
		} else {
			m->poly.texTileSize.width = s->number(0);
			m->poly.texTileSize.height = s->number(1);
		}
		return 0;
	}
	return s->returnSize(L,m->poly.texTileSize);
}

static int funcTileStride(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->poly.texTileStride = s->getSize(L,0);
		return 0;
	} else
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			m->poly.texTileStride.width = s->number(0);
			m->poly.texTileStride.height = s->number(0);
		} else {
			m->poly.texTileStride.width = s->number(0);
			m->poly.texTileStride.height = s->number(1);
		}
		return 0;
	}
	return s->returnPoint(L,PPPoint(m->poly.texTileStride.width,m->poly.texTileStride.height));
}

static int funcTileOffset(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->poly.texOffset = s->getPoint(L,0);
		return 0;
	} else
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			m->poly.texOffset.x = s->number(0);
			m->poly.texOffset.y = s->number(0);
		} else {
			m->poly.texOffset.x = s->number(0);
			m->poly.texOffset.y = s->number(1);
		}
		return 0;
	}
	return s->returnPoint(L,m->poly.texOffset);
}

static int funcDocumentPath(lua_State* L)
{
  lua_pushstring(L,PPGameDocumentPath(""));
  return 1;
}

void QBGame::openGameLibrary(PPLuaScript* script,const char* name)
{
	script->addCommand("pphitcheck",funcPPHitCheck);

	script->openModule(name,this);
		script->addCommand("start",funcStep);
		script->addCommand("platform",funcPlatform);
		script->addCommand("fps",funcFPS);
		script->addCommand("deltaTime",funcDeltaTime);
//		script->addCommand("accelerometer",funcAccelerometer);
//		script->addCommand("gyro",funcGyro);
//		script->addCommand("magnetometer",funcMagnetometer);
		script->addCommand("mouse",funcMousePosition);
		script->addCommand("volume",funcMasterVolume);
		script->addCommand("setNumber",funcSetDefaultNumber);
		script->addCommand("getNumber",funcGetDefaultNumber);
		script->addCommand("setInteger",funcSetDefaultInteger);
		script->addCommand("getInteger",funcGetDefaultInteger);
		script->addCommand("setString",funcSetDefaultString);
		script->addCommand("getString",funcGetDefaultString);
		script->addCommand("documentPath",funcDocumentPath);
    //script->addCommand("preferencePath",funcPreferncePath);
		//script->addCommand("regexReplace",funcRegexReplace);
	script->closeModule();
}

void QBGame::openViewLibrary(PPLuaScript* script,const char* name)
{
	script->openModule(name,this);
		script->addCommand("tileInfo",funcTile);
		script->addCommand("tileSize",funcTileSize);
		script->addCommand("tileStride",funcTileStride);
		script->addCommand("tileOffset",funcTileOffset);
		script->addCommand("put",funcPut);
		script->addCommand("position",funcLocate);
		script->addCommand("pos",funcLocate);
//		script->addCommand("pos",funcLocate);
		script->addCommand("move",funcMove);
		script->addCommand("locate",funcLocate);
		script->addCommand("print",funcPrint);
		script->addCommand("color",funcColor);
		script->addCommand("scale",funcScale);
		script->addCommand("flip",funcFlip);
		script->addCommand("rotate",funcRotate);
		script->addCommand("alpha",funcAlpha);
		script->addCommand("fill",funcFill);
		script->addCommand("box",funcBox);
		script->addCommand("line",funcLine);
		script->addCommand("circle",funcCircle);
		script->addCommand("triangleFan",funcTriangleFan);
		script->addCommand("triangleStrip",funcTriangleStrip);
//		script->addCommand("windowSize",funcWinRect);
//		script->addCommand("viewPort",funcViewPort);
		script->addCommand("layout",funcLayout);
//		script->addCommand("density",funcGetDensity);
		script->addCommand("update",funcUpdate);
		script->addCommand("clear",funcClear);
//		script->addCommand("origin",funcOrigin);
		script->addCommand("pivot",funcOrigin);
		script->addCommand("lineWrap",funcLineWrap);
		script->addCommand("append",funcAppend);
		script->addCommand("remove",funcRemove);
//		script->addCommand("deg2rad",funcDeg2Rad);
//		script->addCommand("rad2deg",funcRad2Deg);
//		script->addCommand("reset",funcReset);
//		script->addCommand("dump",funcDump);
		script->addCommand("blend",funcBlend);
		script->addCommand("shadow",funcFog);
	script->closeModule();
}

void QBGame::openTouchLibrary(PPLuaScript* script,const char* name)
{
	lua_pushcfunction(script->L,funcTouch);
	lua_setglobal(script->L,name);
}

void QBGame::openKeyLibrary(PPLuaScript* script,const char* name)
{
	script->openModule(name,this);
		script->addCommand("up",funcGetKeyUp);
		script->addCommand("down",funcGetKeyDown);
		script->addCommand("left",funcGetKeyLeft);
		script->addCommand("right",funcGetKeyRight);
		script->addCommand("a",funcGetKeyA);
		script->addCommand("b",funcGetKeyB);
		script->addCommand("c",funcGetKeyC);
		script->addCommand("d",funcGetKeyD);
		script->addCommand("e",funcGetKeyE);
		script->addCommand("f",funcGetKeyF);
		script->addCommand("g",funcGetKeyG);
		script->addCommand("h",funcGetKeyH);
		script->addCommand("x",funcGetKeyX);
		script->addCommand("y",funcGetKeyY);
		script->addCommand("leftShoulder",funcGetKeyL);
		script->addCommand("rightShoulder",funcGetKeyR);
		script->addCommand("startDiscovery",funcGetKeyStartDiscovery);
		script->addCommand("stopDiscovery",funcGetKeyStopDiscovery);
		script->addCommand("count",functionGetKeyControllers);
		script->addCommand("setup",funcGetKeySetUP);
		script->addCommand("start",funcGetKeyStart);
		script->addCommand("pause",funcGetKeyPause);
	script->closeModule();
}

static int funcLoadTexture(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		PPGameTextureOption option;
#if 1
		if (s->argCount > 1) {
			if (s->isTable(L,1)) {
				option = s->getTextureOption(L,1,option);
			}
		}
#else
		bool linear = true;
		if (s->argCount > 1) {
			linear = s->boolean(1);
		}
#endif

		int tex=m->projector->textureManager->loadTexture(s->args(0),option);
		m->projector->textureManager->pushTextureInfo(L,tex);

		if (PPReadError()) {
			PPReadErrorReset();
			return luaL_error(L,"texture file read error '%s'",s->args(0));
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int funcDefaultTexture(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			int tex = (int)s->tableInteger(L,0,"index",0);
			if (tex>0) {
				m->projector->textureManager->defaultTexture = tex-1;
			}
		}
	}
	m->projector->textureManager->pushTextureInfo(L,m->projector->textureManager->defaultTexture);
	return 1;
}

static int funcDeleteTexture(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			int tex = (int)s->tableInteger(L,0,"index",0);
			if (tex>0) {
				m->projector->textureManager->deleteTexture(tex-1);
			}
		} else
    if (lua_isstring(L,-1)) {
      m->projector->textureManager->deleteTextureWithName(lua_tostring(L,-1),m->systemTextureID);
    }
	}
	return 0;
}

static int funcTextureCount(lua_State* L)
{
	PPWorld* m = PPLuaArg::World(L);
	//PPLuaScript* s = PPLuaScript::sharedScript(L);
	lua_pushinteger(L,m->projector->textureManager->getTextureCount());
	return 1;
}

static int funcTexture(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	lua_createtable(L,m->projector->textureManager->getTextureCount(),0);
	int c=0;
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (m->projector->textureManager->getTexture(i)) {
			m->projector->textureManager->pushTextureInfo(L,i);
			lua_rawseti(L,-2,c+1);
			c++;
		}
	}
	return 1;
}

void QBGame::openTextureLibrary(PPLuaScript* s,const char* name)
{
	s->openModule(name,this);
		s->addCommand("load",funcLoadTexture);
		s->addCommand("default",funcDefaultTexture);
//		s->addCommand("make",funcEmptyTexture);
		s->addCommand("delete",funcDeleteTexture);
//		s->addCommand("tileSize",funcTileSize);
//		s->addCommand("_size",funcTextureSize);
//		s->addCommand("size",funcTextureImageSize);
		s->addCommand("count",funcTextureCount);
		s->addCommand("textures",funcTexture);
	s->closeModule();
}

#pragma mark -

static bool checkFileName(const char* s)
{
  if (s) {
    if (s[0] != 0) return true;
  }
  return false;
}

static int funcPreloadBackgroundMusic(lua_State* L)
{
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	if (lua_gettop(L) > 0) {
		if (lua_isstring(L,1)) {
#ifndef NO_COCOSDENSHION
      const char* s = PPGameDataSubPath(lua_tostring(L,1));
      if (checkFileName(s)) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(s);
      } else {
        PPReadErrorReset();
        return luaL_error(L,"music file read error '%s'",lua_tostring(L,1));
      }
#endif
		}
	}
	return 0;
}

static int funcPlayBackgroundMusic(lua_State* L)
{
	if (lua_gettop(L) > 0) {
		if (lua_isstring(L,1)) {
      bool loop=false;
      if (lua_gettop(L) > 1) {
        loop=lua_toboolean(L,2);
      }
      const char* s = PPGameDataSubPath(lua_tostring(L,1));
#ifndef NO_COCOSDENSHION
      if (checkFileName(s)) {
        bool stream=false;
        QBSound* snd = QBSound::sharedSound();
        if (snd) {
          if (snd->streamTest(s)) {
            long long looppoint=0;
            if (lua_gettop(L) > 2) {
              looppoint=lua_tointeger(L,3);
            }
            if (loop) {
              snd->streamLoopPlay(s,looppoint);
            } else {
              snd->streamPlay(s);
            }
            stream=true;
          } else {
          }
        }
        if (!stream) {
          CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(s,loop);
        }
      } else {
        PPReadErrorReset();
        return luaL_error(L,"music file read error '%s'",lua_tostring(L,1));
      }
#endif
		}
	}
	return 0;
}

static int funcStopBackgroundMusic(lua_State* L)
{
  QBSound* snd = QBSound::sharedSound();
  if (snd) {
    snd->streamStop();
  }
	if (lua_gettop(L) > 0) {
#ifndef NO_COCOSDENSHION
		CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(lua_toboolean(L,1));
#endif
	} else {
#ifndef NO_COCOSDENSHION
		CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
#endif
	}
	return 0;
}

static int funcPauseBackgroundMusic(lua_State* L)
{
#ifndef NO_COCOSDENSHION
  QBSound* snd = QBSound::sharedSound();
  if (snd) {
    snd->streamPause();
  }
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
#endif
	return 0;
}

static int funcResumeBackgroundMusic(lua_State* L)
{
#ifndef NO_COCOSDENSHION
  QBSound* snd = QBSound::sharedSound();
  if (snd) {
    snd->streamResume();
  }
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
#endif
	return 0;
}

static int funcRewindBackgroundMusic(lua_State* L)
{
#ifndef NO_COCOSDENSHION
  QBSound* snd = QBSound::sharedSound();
  if (snd) {
    snd->streamRewind();
  }
	CocosDenshion::SimpleAudioEngine::sharedEngine()->rewindBackgroundMusic();
#endif
	return 0;
}

static int funcWillPlayBackgroundMusic(lua_State* L)
{
#ifndef NO_COCOSDENSHION
  QBSound* snd = QBSound::sharedSound();
  if (snd) {
    snd->streamWillPlay();
  }
	CocosDenshion::SimpleAudioEngine::sharedEngine()->willPlayBackgroundMusic();
#endif
	return 0;
}

static int funcIsBackgroundMusicPlaying(lua_State* L)
{
#ifndef NO_COCOSDENSHION
  QBSound* snd = QBSound::sharedSound();
  if (snd) {
    if (snd->streamIsPlaying()) {
      lua_pushboolean(L,true);
      return 1;
    }
  }
	lua_pushboolean(L,CocosDenshion::SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying());
	return 1;
#else
  return 0;
#endif
}

static int funcBackgroundMusicVolume(lua_State* L)
{
#ifndef NO_COCOSDENSHION
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	if (lua_gettop(L) > 0) {
    QBSound* snd = QBSound::sharedSound();
    if (snd) {
      snd->streamSetVolume(lua_tonumber(L,1));
    }
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(lua_tonumber(L,1));
		return 0;
	}
	lua_pushnumber(L,CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume());
	return 1;
#else
  return 0;
#endif
}

void QBGame::openAudioEngineBGM(PPLuaScript* script,const char* name)
{
#ifndef NO_COCOSDENSHION
	script->openModule(name,CocosDenshion::SimpleAudioEngine::sharedEngine());
		script->addCommand("preload",funcPreloadBackgroundMusic);
		script->addCommand("play",funcPlayBackgroundMusic);
		script->addCommand("stop",funcStopBackgroundMusic);
		script->addCommand("pause",funcPauseBackgroundMusic);
		script->addCommand("resume",funcResumeBackgroundMusic);
		script->addCommand("rewind",funcRewindBackgroundMusic);
		script->addCommand("willPlay",funcWillPlayBackgroundMusic);
		script->addCommand("isPlaying",funcIsBackgroundMusicPlaying);
		script->addCommand("volume",funcBackgroundMusicVolume);
	script->closeModule();
#endif
}

static int funcEffectsVolume(lua_State* L)
{
	//PPLuaScript* s = PPLuaScript::sharedScript(L);
	if (lua_gettop(L) > 0) {
		lua_Number val = lua_tonumber(L,1);
#ifndef NO_COCOSDENSHION
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(val);
#endif
		return 0;
	}
#ifndef NO_COCOSDENSHION
	lua_pushnumber(L,CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume());
	return 1;
#else
  return 0;
#endif
}

static int funcPlayEffect(lua_State* L)
{
#ifndef NO_COCOSDENSHION
	if (lua_gettop(L) > 0) {
		if (lua_isstring(L,1)) {

      bool loop=false;
      if (lua_gettop(L) > 1) {
        loop=lua_toboolean(L,2);
      }
      
      float pitch=1.0f;
      if (lua_gettop(L) > 2) {
        pitch=lua_tonumber(L,3);
      }
      
      float pan=0.0f;
      if (lua_gettop(L) > 3) {
        pan=lua_tonumber(L,4);
      }
      
      float gain=1.0f;
      if (lua_gettop(L) > 4) {
        gain=lua_tonumber(L,5);
      }

      bool isOgg=false;
      const char* s = PPGameDataSubPath(lua_tostring(L,1));
      QBSound* snd = QBSound::sharedSound();
      if (snd) {
        if (snd->streamTest(s)) {
          snd->playEffect(s, loop,pitch,pan,gain);
          isOgg=true;
        }
      }
      if (!isOgg) {
#ifdef __LUAJIT__
        if (checkFileName(s)) {
          int val = CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(s,loop,pitch,pan,gain);
          lua_pushnumber(L,val);
        } else {
          PPReadErrorReset();
          return luaL_error(L,"music file read error '%s'",lua_tostring(L,1));
        }
#else
        if (checkFileName(s)) {
          lua_Unsigned val = CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(s,loop,pitch,pan,gain);
          lua_pushunsigned(L,val);
        } else {
          PPReadErrorReset();
          return luaL_error(L,"music file read error '%s'",lua_tostring(L,1));
        }
#endif
      }

			return 1;
		}
	}
#endif
	return 0;
}

static int funcStopEffect(lua_State* L)
{
  if (lua_gettop(L) > 0) {
#ifndef NO_COCOSDENSHION
#ifdef __LUAJIT__
    int val=lua_tonumber(L,1);
#else
    lua_Unsigned val = lua_tounsigned(L,1);
#endif
    QBSound* snd = QBSound::sharedSound();
    if (snd) {
      snd->stopEffect(val);
    }
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopEffect(val);
#endif
  }
	return 0;
}

static int funcPreloadEffect(lua_State* L)
{
  if (lua_gettop(L) > 0) {
    const char* s = PPGameDataSubPath(lua_tostring(L,1));
    if (lua_isstring(L,1)) {
      QBSound* snd = QBSound::sharedSound();
      bool isOgg=false;
      if (snd) {
        if (snd->streamTest(s)) {
          snd->preloadEffect(s);
          isOgg=true;
        }
      }
      if (!isOgg) {
#ifndef NO_COCOSDENSHION
        if (checkFileName(s)) {
          CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(s);
        } else {
          PPReadErrorReset();
          return luaL_error(L,"sound effect file read error '%s'",lua_tostring(L,1));
        }
      }
#endif
    }
  }
	return 0;
}

static int funcUnloadEffect(lua_State* L)
{
  if (lua_gettop(L) > 0) {
    if (lua_isstring(L,1)) {
      const char* s = PPGameDataSubPath(lua_tostring(L,1));
      QBSound* snd = QBSound::sharedSound();
      if (snd) {
        snd->unloadEffect(s);
      }
#ifndef NO_COCOSDENSHION
      if (checkFileName(s)) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(s);
      } else {
        PPReadErrorReset();
        return luaL_error(L,"sound effect file read error '%s'",lua_tostring(L,1));
      }
    }
  }
#endif
	return 0;
}

void QBGame::openAudioEngineEffect(PPLuaScript* script,const char* name)
{
#ifndef NO_COCOSDENSHION
	script->openModule(name,CocosDenshion::SimpleAudioEngine::sharedEngine());
		script->addCommand("volume",funcEffectsVolume);
		script->addCommand("play",funcPlayEffect);
		script->addCommand("stop",funcStopEffect);
		script->addCommand("preload",funcPreloadEffect);
		script->addCommand("unload",funcUnloadEffect);
	script->closeModule();
#endif
}

static int funcPlay(lua_State* L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 1) {
		m->play(s->args(0),atoi(s->args(1)));
	} else
	if (s->argCount > 0) {
		m->play(s->args(0));
	}
	return 0;
}

void QBGame::openAudioEngineMML(PPLuaScript* script,const char* name)
{
	script->openModule(name,this);
		script->addCommand("play",funcPlay);
	script->closeModule();
}

static int funcFlMMLPlay(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
  if (snd) {
    if (!snd->isPlayingMML((int)s->integer(0))) {
      snd->playMML("",(int)s->integer(0));
    }
  }
	return 0;
}

static int funcFlMMLClear(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
  if (snd) {
    if (snd->isPlayingMML((int)s->integer(0))) {
      snd->stopMML((int)s->integer(0));
    }
    snd->preloadMML("",(int)s->integer(0));
  }
	return 0;
}

static int funcFlMMLPreload(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		QBSound* snd = QBSound::sharedSound();
    if (snd) {
      if (snd->isPlayingMML((int)s->integer(1))) {
        snd->stopMML((int)s->integer(1));
      }
      snd->preloadMML(s->args(0),(int)s->integer(1));
    }
	}
	return 0;
}

static int funcFlMMLPause(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
  if (snd) {
    snd->pauseMML((int)s->integer(0));
  }
	return 0;
}

static int funcFlMMLResume(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
  if (snd) {
    if (snd->isPausedMML((int)s->integer(0))) {
      snd->resumeMML((int)s->integer(0));
    }
  }
	return 0;
}

static int funcFlMMLIsPlaying(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
  if (snd) {
    lua_pushboolean(L,snd->isPlayingMML((int)s->integer(0)));
  } else {
    lua_pushboolean(L,false);
  }
	return 1;
}


static int funcFlMMLIsPaused(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
  if (snd) {
    lua_pushboolean(L,snd->isPausedMML((int)s->integer(0)));
  } else {
    lua_pushboolean(L,false);
  }
	return 1;
}

void QBGame::openAudioEngineFlMML(PPLuaScript* script,const char* name)
{
	script->openModule(name,this);
		script->addCommand("play",funcFlMMLPlay);
//		script->addCommand("clear",funcFlMMLClear);
		script->addCommand("stop",funcFlMMLClear);
		script->addCommand("pause",funcFlMMLPause);
		script->addCommand("preload",funcFlMMLPreload);
		script->addCommand("resume",funcFlMMLResume);
		script->addCommand("isPlaying",funcFlMMLIsPlaying);
		script->addCommand("isPaused",funcFlMMLIsPaused);
	script->closeModule();
}

static int funcWav9(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 3) {
		int waveNo = (int)s->integer(0);
		int intVol = (int)s->integer(1);
		bool loopFg = s->boolean(2);
		if (waveNo < 0 || waveNo >= FlMML::MOscFcDpcmData::MAX_WAVE) {
			return luaL_argerror(L,0+2,lua_pushfstring(L,"invalid wave no. %d",waveNo));
		}
		if (intVol < 0 || intVol > 127) {
			return luaL_argerror(L,1+2,lua_pushfstring(L,"invalid volume %d (0-127)",intVol));
		}
		if (snd) snd->setWav9(waveNo,intVol,loopFg,s->args(3));
	} else {
		return luaL_argerror(L,s->argCount+2,"value expected");
	}
	return 0;
}

struct Func33 { 
	bool operator()( const char a ) const { return isspace(a); } 
};

static string removeWhitespace(string str) {
	str.erase(remove_if(str.begin(),str.end(),Func33()),str.end());
	return str;
}

struct toLower : public string {
    toLower(const string& rstr) {
        {for (int iIndex = 0; iIndex < rstr.size(); iIndex++) {
            *this += tolower(rstr[iIndex], locale());
        }}
    }
};

static int funcWav10(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 1) {
		int waveNo = (int)s->integer(0);
		if (waveNo < 0 || waveNo >= FlMML::MOscGbWaveData::MAX_WAVE) {
			return luaL_argerror(L,0+2,lua_pushfstring(L,"invalid wave no. %d",waveNo));
		}
		const char* data = s->args(1);
		if (data == NULL) {
			return luaL_argerror(L,1+2,"value expected");
		}
		{
			std::string s = std::string(data)+"00000000000000000000000000000000";
			s = removeWhitespace(s);
			s = toLower(s);
			for (int i=0;i<s.length()&&i<32;i++) {
				if (!((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f'))) {
				 	return luaL_argerror(L,1+2,lua_pushfstring(L,"invalid wave data. '%s'",data));
				}
			}
			if (snd) snd->setWav10(waveNo,s.c_str());
		}
	} else {
		return luaL_argerror(L,s->argCount+2,"value expected");
	}
	return 0;
}

static int funcWav13(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 1) {
		int waveNo = (int)s->integer(0);
		if (waveNo < 0 || waveNo >= FlMML::MOscWaveData::MAX_WAVE) {
			return luaL_argerror(L,0+2,lua_pushfstring(L,"invalid wave no. %d",waveNo));
		}
		const char* data = s->args(1);
		if (data == NULL) {
			return luaL_argerror(L,1+2,"value expected");
		}
		{
			std::string s = std::string(data);
			s = removeWhitespace(s);
			s = toLower(s);
			for (int i=0;i<s.length();i++) {
				if (!((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f'))) {
				 	return luaL_argerror(L,1+2,lua_pushfstring(L,"invalid wave data. '%s'",data));
				}
			}
			if (s.length() <= 0 || s.length() > FlMML::MOscWaveData::MAX_LENGTH) {
			 	return luaL_argerror(L,1+2,lua_pushfstring(L,"invalid wave data length %d. ",s.length()));
			}
			if (snd) snd->setWav13(waveNo,s.c_str());
		}
	} else {
		return luaL_argerror(L,s->argCount+2,"value expected");
	}
	return 0;
}

void QBGame::openAudioEngineSEMML(PPLuaScript* s,const char* name)
{
	s->openModule(name,this);
		s->addCommand("wav9",funcWav9);
		s->addCommand("wav10",funcWav10);
		s->addCommand("wav13",funcWav13);
	s->closeModule();
}

void QBGame::openGameController(PPLuaScript* s,const char* name)
{
	s->openModule(name,this,0);
		s->addCommand("startDiscovery",funcGetKeyStartDiscovery);
		s->addCommand("stopDiscovery",funcGetKeyStopDiscovery);
		s->addCommand("count",functionGetKeyControllers);
    s->addCommand("info",functionGetKeyControllersInfo);
    s->addCommand("playerIndex",functionGetKeyControllersPlayerIndex);
	s->closeModule();
}

#if TARGET_OS_IPHONE
static int funcSpeechReset(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
  bool r=false;
  if (speech) {
    r = speech->reset();
  }
  lua_pushboolean(L,r);
  return 1;
}

static int funcSpeechStop(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
  if (speech) {
    speech->stop();
  }
  return 0;
}

static int funcSpeechVoice(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
	if (lua_gettop(L) > 1) {
		if (lua_isstring(L,-1)) {
      const char* str = lua_tostring(L,-1);
      if (speech) {
        speech->voice(str);
      }
    } else
    if (lua_isnil(L,-1)) {
      if (speech) {
        speech->voice(NULL);
      }
    }
  } else
	if (lua_gettop(L) > 0) {
    if (speech) {
      const char* v = speech->getVoice();
      if (v) {
        lua_pushstring(L,v);
      } else {
        lua_pushnil(L);
      }
    } else {
      lua_pushnil(L);
    }
    return 1;
  }
  return 0;
}

static int funcSpeechRate(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
	if (lua_gettop(L) > 1) {
		if (lua_isnumber(L,-1)) {
      float rate = lua_tonumber(L,-1);
      if (speech) {
        speech->rate(rate);
      }
    }
  } else
	if (lua_gettop(L) > 0) {
    if (speech) {
      lua_pushnumber(L,speech->getRate());
    } else {
      lua_pushnumber(L,0);
    }
    return 1;
  }
  return 0;
}

static int funcSpeechPitch(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
	if (lua_gettop(L) > 1) {
		if (lua_isnumber(L,-1)) {
      float pitch = lua_tonumber(L,-1);
      if (speech) {
        speech->pitch(pitch);
      }
    }
  } else
	if (lua_gettop(L) > 0) {
    if (speech) {
      lua_pushnumber(L,speech->getPitch());
    } else {
      lua_pushnumber(L,0);
    }
    return 1;
  }
  return 0;
}

static int funcSpeechVolume(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
	if (lua_gettop(L) > 1) {
		if (lua_isnumber(L,-1)) {
      float volume = lua_tonumber(L,-1);
      if (speech) {
        speech->volume(volume);
      }
    }
  } else
	if (lua_gettop(L) > 0) {
    if (speech) {
      lua_pushnumber(L,speech->getVolume());
    } else {
      lua_pushnumber(L,0);
    }
    return 1;
  }
  return 0;
}

static int funcSpeechUtter(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
	if (lua_gettop(L) > 0) {
		if (lua_isstring(L,-1)) {
      const char* str = lua_tostring(L,-1);
      if (speech) {
        speech->utter(str);
      }
    }
  }
  return 0;
}

static int funcSpeechIsPlaying(lua_State* L)
{
  PPTextToSpeech* speech = PPTextToSpeech::sharedSpeech();
  if (speech) {
    bool play = speech->isPlaying();
    lua_pushboolean(L,play);
    return 1;
  }
  lua_pushboolean(L,false);
  return 1;
}
#else
static int funcSpeechReset(lua_State* L)
{
  lua_pushboolean(L,false);
  return 1;
}

static int funcSpeechStop(lua_State* L)
{
  return 0;
}

static int funcSpeechVoice(lua_State* L)
{
  return 0;
}

static int funcSpeechRate(lua_State* L)
{
  return 0;
}

static int funcSpeechPitch(lua_State* L)
{
  return 0;
}

static int funcSpeechVolume(lua_State* L)
{
  return 0;
}

static int funcSpeechUtter(lua_State* L)
{
  return 0;
}

static int funcSpeechIsPlaying(lua_State* L)
{
  lua_pushboolean(L,false);
  return 1;
}
#endif

void QBGame::openTextToSpeech(PPLuaScript* script,const char* name)
{
	script->openModule(name,this);
		script->addCommand("reset",funcSpeechReset);
		script->addCommand("stop",funcSpeechStop);
		script->addCommand("voice",funcSpeechVoice);
		script->addCommand("rate",funcSpeechRate);
		script->addCommand("pitch",funcSpeechPitch);
		script->addCommand("volume",funcSpeechVolume);
		script->addCommand("utter",funcSpeechUtter);
		script->addCommand("isPlaying",funcSpeechIsPlaying);
	script->closeModule();
}

void QBGame::drawPattern(PPPoint pos,unsigned short gid,void* userdata)
{
  PPObject* o = (PPObject*)userdata;
  unsigned char r,g,b,a;
  GetRGBColor(colorIndex,&r,&g,&b,&a);
  Put(pos.x*GetScale().x,pos.y*GetScale().y,gid,0,o->poly._texture,r,g,b,a);
}

void QBGame::disableIO()
{
}

static lua_Number getNumber(lua_State* L,int stack,int index,const char* field)
{
	lua_getfield(L,stack,field);
	if (lua_isnil(L,-1)) {
		lua_rawgeti(L,stack,index);
		if (lua_isnil(L,-1)) {
			return 0;
		}
	}
	return lua_tonumber(L,-1);
}

void* QBGame::workMemory(int index,size_t size)
{
  if (index>=0 && index<10) {
    if (workmem[index]==NULL) {
      workmem[index]=malloc(size);
      workmemsize[index]=size;
      return workmem[index];
    }
    if (workmemsize[index]<size) {
      workmem[index]=realloc(workmem[index],size);
      workmemsize[index]=size;
    }
    return workmem[index];
  }
  return NULL;
}

static int funcPPHitCheck(lua_State *L)
{
	QBGame* m = (QBGame*)PPLuaArg::World(L);
  if (m==NULL) return 0;
	int top=lua_gettop(L);
	if (lua_isfunction(L,3)) {
		if (top >= 2) {
      if (lua_type(L,1)!=LUA_TTABLE) return 0;
      if (lua_type(L,2)!=LUA_TTABLE) return 0;

			int n[2];
			PPObjectHit* hit[2];
#ifdef __LUAJIT__
			n[0] = (int)lua_objlen(L,1);
			n[1] = (int)lua_objlen(L,2);
#else
			n[0] = (int)lua_rawlen(L,1);
			n[1] = (int)lua_rawlen(L,2);
#endif
			lua_settop(L,top);
			
			if (n[0] > 0 && n[1] > 0) {
				hit[0] = (PPObjectHit*)m->workMemory(WORK_MEM_HIT1,n[0]*sizeof(PPObjectHit));
				hit[1] = (PPObjectHit*)m->workMemory(WORK_MEM_HIT2,n[1]*sizeof(PPObjectHit));
        
				for (int i=0;i<2;i++) {
					for (int j=0;j<n[i];j++) {
						int top=lua_gettop(L);
						lua_rawgeti(L,1+i,j+1);
            int table=lua_gettop(L);

            PPObject* m = (PPObject*)PPLuaArg::UserData(L,top+1,PPObject::className,false);
            if (m) {

              hit[i][j]=m->hit;
              hit[i][j].index = j+1;
              if (m->hit.type == 1) {
                PPRect r=hit[i][j].rect;
                hit[i][j].rect = r+m->pos;
                hit[i][j].pos = hit[i][j].rect.center();
                hit[i][j].center = PPPoint(0,0);
                hit[i][j].length=sqrt(r.width*r.width/4+r.height*r.height/4);
              } else {
                hit[i][j].pos = m->pos;
              }

            } else {

              hit[i][j].index = j+1;
              hit[i][j].mask = 0;
              hit[i][j].length = 0;
              hit[i][j].type = 0;
              hit[i][j].center = PPPoint(0,0);
              
              lua_settop(L,table);
              
              {
                lua_getfield(L,table,"hitmask");
                if (!lua_isnil(L,-1)) {
                  hit[i][j].mask = (int)lua_tointeger(L,-1);
                }
                lua_pop(L,1);
              }
              if (hit[i][j].mask != 0) {
                lua_getfield(L,table,"hitlength");
                if (!lua_isnil(L,-1)) {
                  hit[i][j].length = lua_tonumber(L,-1);
                }
                lua_pop(L,1);
              }
              if (hit[i][j].mask != 0) {
                lua_getfield(L,table,"hitcenter");
                if (lua_istable(L,-1)) {
                  int center=lua_gettop(L);
                  hit[i][j].center.x = getNumber(L,center,1,"x");
                  hit[i][j].center.y = getNumber(L,center,2,"y");
                }
                {
                  lua_getfield(L,table,"x");
                  hit[i][j].pos.x = lua_tonumber(L,-1);
                  lua_getfield(L,table,"y");
                  hit[i][j].pos.y = lua_tonumber(L,-1);
                }
                lua_settop(L,table);
              }
              if (hit[i][j].mask != 0) {
                lua_getfield(L,table,"hitpos");
                if (lua_istable(L,-1)) {
                  int center=lua_gettop(L);
                  hit[i][j].pos.x = getNumber(L,center,1,"x");
                  hit[i][j].pos.y = getNumber(L,center,2,"y");
                  lua_settop(L,table);
                }
              }
              if (hit[i][j].mask != 0) {
                lua_getfield(L,table,"hitrect");
                if (lua_istable(L,-1)) {
                  int s=lua_gettop(L);
                  PPRect r;
                  r.x = getNumber(L,s,1,"x");
                  r.y = getNumber(L,s,2,"y");
                  r.width = getNumber(L,s,3,"width");
                  r.height = getNumber(L,s,4,"height");
                  hit[i][j].rect = PPRect(hit[i][j].pos.x+r.x,hit[i][j].pos.y+r.y,r.width,r.height);
                  hit[i][j].pos.x += r.x+r.width/2;
                  hit[i][j].pos.y += r.y+r.height/2;
                  hit[i][j].length=sqrt(r.width*r.width/4+r.height*r.height/4);
                  hit[i][j].type = 1;
                }
              }
            }
						lua_settop(L,top);
					}
				}
				
				for (int i=0;i<n[0];i++) {
					PPObjectHit* a = &hit[0][i];
					if (a->mask != 0 && a->length > 0) {
						for (int j=0;j<n[1];j++) {
							PPObjectHit* b = &hit[1][j];
							if (b->mask != 0 && b->length > 0) {
								if (a->mask & b->mask) {
									bool hitcheck = false;
									if (b->type && a->type) {
										if (b->rect.hitCheck(a->rect)) {
											hitcheck = true;
										}
									} else
                  if ((b->pos+b->center).length(a->pos+a->center) < a->length+b->length) {
                    hitcheck = true;
                  }
									if (hitcheck) {
										lua_pushvalue(L,3);
										lua_rawgeti(L,1,a->index);
										lua_rawgeti(L,2,b->index);
										lua_call(L,2,0);
										lua_settop(L,top);
									}
								}
							}
						}
					}
				}
				
//				free(hit[0]);
//				free(hit[1]);
        
			}
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
