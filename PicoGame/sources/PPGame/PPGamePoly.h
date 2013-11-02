/*-----------------------------------------------------------------------------------------------
	名前	PPGamePoly.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGAMEPOLY__
#define __PPGAMEPOLY__

#pragma once

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameGeometry.h"
#include <stdlib.h>

#define __TILE_SIZE__

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameBlend {
public:
  PPGameBlend();
  PPGameBlend(bool b,int src,int dst,bool f,PPColor col);

	bool blend;
	int blendSrc;
	int blendDst;
	bool fog;
	PPColor fogColor;
	
	void set(int type);

	bool operator == (PPGameBlend& b) {
		if (blend != b.blend) return false;
		if (blendSrc != b.blendSrc) return false;
		if (blendDst != b.blendDst) return false;
		if (fog != b.fog) return false;
		if (fogColor != b.fogColor) return false;
		return true;
	};
  
  static PPGameBlend None();
	static PPGameBlend Light();
	static PPGameBlend BlackMask();
	static PPGameBlend Flash();
	static PPGameBlend Red();
	static PPGameBlend NoMask();
	static PPGameBlend Color();
	static PPGameBlend EdgeSmooth();
};

class PPGamePoly {
public:
	PPGamePoly() : flags(0) {
		init();
	}

	int _texture;
	PPPoint pos;
	PPPoint size;
	PPPoint origin;
	PPPoint scale;
	int pat;
	int group;

	PPGameBlend blend;
	
	unsigned short flags;
	float rotate;
	PPColor color;

#ifdef __TILE_SIZE__
	PPPoint texOffset;
	PPSize texTileSize;
	PPSize texTileStride;
	PPPoint tilePosition(int pat,int tw);
//	PPSize tileSize();
#endif

	void init();
	void reset();
	void initTexture(int texture);
	void sprite(float _x,float _y,int _pat,int _group=0);

	void line(float x1,float y1,float x2,float y2);
	void fill(float x1,float y1,float x2,float y2);
	void box(float x1,float y1,float x2,float y2);
	
	void setBlend(int type);
};

class PPRegion {
public:
	PPRegion() {
		o = PPPointZero;
		step = 0;
		type = 0;
		count = 0;
		orgpoints = NULL;
	}
	virtual ~PPRegion() {};
	
	void update(PPPoint* org,int pointNum=0);
	void update();
	int init(PPPoint* org,int pointNum,float x=0,float y=0);

	int hitLine(PPRegion* region);
	bool hitCheck(PPRegion* region);
	bool hitCheck(PPPoint* point);
	
	int hitPoints(PPRegion* region,PPPoint* outPos,int maxPos);
	
	int poly(PPGamePoly* poly,PPPoint* offset=NULL,unsigned char r=255,unsigned char g=255,unsigned char b=255);
	int line(PPGamePoly* poly,PPPoint* offset=NULL,unsigned char r=255,unsigned char g=255,unsigned char b=255);

	float calcLength(PPPoint* point);

	PPPoint o;
	PPPoint n;
	PPPoint cross;

	void calcBoundingBox();
	PPPoint min;
	PPPoint max;
	
	PPPoint* points;
	PPPoint* normals;
	PPPoint* orgpoints;
	
	int pointNum;

	int type;
	int step;
	int count;
	
	bool hitCheckTarget;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
