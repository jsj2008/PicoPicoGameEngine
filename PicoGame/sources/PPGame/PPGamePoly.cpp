/*-----------------------------------------------------------------------------------------------
	名前	PPGamePoly.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGamePoly.h"
#include "PPGameDef.h"
#include "PPGameSprite.h"
#include <stdio.h>

PPGameBlend::PPGameBlend() : blend(false),blendSrc(GL_SRC_ALPHA),blendDst(GL_ONE_MINUS_SRC_ALPHA),fog(false)
{
}

PPGameBlend::PPGameBlend(bool b,int src,int dst,bool f,PPColor col) : blend(false),blendSrc(GL_SRC_ALPHA),blendDst(GL_ONE_MINUS_SRC_ALPHA),fog(false)
{
  blend=b;
  blendSrc=src;
  blendDst=dst;
  fog=f;
  fogColor=col;
}

void PPGameBlend::set(int type)
{
//  blend=type.blend;
//  blendSrc=type.blendSrc;
//  blendDst=type.blendDst;
//  fog=type.fog;
//  fogColor=type.fogColor;
//#if 0
	switch (type) {
	case PPGameBlend_None:
		blend = true;
		blendSrc = GL_SRC_ALPHA;
		blendDst = GL_ONE_MINUS_SRC_ALPHA;
		fog = false;
		break;
	case PPGameBlend_Light:
		blend = true;
		blendSrc = GL_SRC_ALPHA;
		blendDst = GL_ONE;
		fog = false;
		break;
	case PPGameBlend_BlackMask:
		blend = true;
		blendSrc = GL_SRC_ALPHA_SATURATE;
		blendDst = GL_ONE_MINUS_SRC_ALPHA;
		fog = false;
		break;
	case PPGameBlend_Flash:
		blend = true;
		blendSrc = GL_SRC_ALPHA;
		blendDst = GL_ONE_MINUS_SRC_ALPHA;
		fog = true;
		fogColor = PPColor::white();
		break;
	case PPGameBlend_Red:
		blend = true;
		blendSrc = GL_SRC_ALPHA;
		blendDst = GL_ONE_MINUS_SRC_ALPHA;
		fog = true;
		fogColor = PPColor::red();
		break;
	case PPGameBlend_NoMask:
		blend = false;
		fog = false;
		break;
	case PPGameBlend_Color:
		blend = true;
		blendSrc = GL_ONE;
		blendDst = GL_ONE_MINUS_SRC_ALPHA;
		fog = false;
		break;
	case PPGameBlend_EdgeSmooth:
		blend = true;
		blendSrc = GL_ONE;
		blendDst = GL_ONE_MINUS_SRC_ALPHA;
		fog = false;
		break;
	}
//#endif
}

PPGameBlend PPGameBlend::None(){
  return PPGameBlend(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,false,PPColor::white());
}

PPGameBlend PPGameBlend::Light(){
  return PPGameBlend(true,GL_SRC_ALPHA,GL_ONE,false,PPColor::white());
}

PPGameBlend PPGameBlend::BlackMask(){
  return PPGameBlend(true,GL_SRC_ALPHA_SATURATE,GL_ONE_MINUS_SRC_ALPHA,false,PPColor::white());
}

PPGameBlend PPGameBlend::Flash(){
  return PPGameBlend(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,true,PPColor::white());
}

PPGameBlend PPGameBlend::Red(){
  return PPGameBlend(false,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,true,PPColor::red());
}

PPGameBlend PPGameBlend::NoMask(){
  return PPGameBlend(true,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,false,PPColor::white());
}

PPGameBlend PPGameBlend::Color(){
  return PPGameBlend(true,GL_ONE,GL_ONE_MINUS_SRC_ALPHA,false,PPColor::white());
}

PPGameBlend PPGameBlend::EdgeSmooth(){
  return PPGameBlend(true,GL_ONE,GL_ONE_MINUS_SRC_ALPHA,false,PPColor::white());
}

#pragma mark -

void PPGamePoly::init()
{
	_texture = -1;
	reset();
}

void PPGamePoly::reset()
{
	pos = PPPointZero;
	size = PPPointZero;
	scale = PPPoint(1,1);
	origin = PPPointZero;
	color = PPColor::white();
#ifdef __TILE_SIZE__
	texOffset = PPPointZero;
	texTileSize = PPSize(32,32);
	texTileStride = PPSize(0,0);
#endif
	pat = 0;
	rotate = 0;
	flags = 0;
	blend.set(PPGameBlend_None);
}

#ifdef __TILE_SIZE__
PPPoint PPGamePoly::tilePosition(int pat,int tw)
{
	int w = texTileSize.width +texTileStride.width;
	int h = texTileSize.height+texTileStride.height;
	if (w == 0) w = 1;
	int t = (tw/w);
	if (t == 0) t = 1;
	return PPPoint((pat % t)*w+texOffset.x,(pat / t)*h+texOffset.y);
}
#endif

void PPGamePoly::initTexture(int texid)
{
	_texture = texid;
	texTileSize.width = 32;
	texTileSize.height = 32;
	texTileStride.width = 0;
	texTileStride.height = 0;
	texOffset = PPPointZero;
}

void PPGamePoly::sprite(float _x,float _y,int _pat,int _group)
{
	pos.x = _x;
	pos.y = _y;
	pat = _pat;
	group = _group;
}

void PPGamePoly::line(float x1,float y1,float x2,float y2)
{
	_texture = QBLINE_TAG;
	pos.x = x1;
	pos.y = y1;
	size.x = x2;
	size.y = y2;
}

void PPGamePoly::fill(float x1,float y1,float x2,float y2)
{
	_texture = QBFILL_TAG;
	pos.x = x1;
	pos.y = y1;
	size.x = x2;
	size.y = y2;
}

void PPGamePoly::box(float x1,float y1,float x2,float y2)
{
	_texture = QBBOX_TAG;
	pos.x = x1;
	pos.y = y1;
	size.x = x2;
	size.y = y2;
}

void PPGamePoly::setBlend(int type)
{
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
