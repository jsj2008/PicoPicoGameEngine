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

void PPGameBlend::set(int type)
{
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

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
