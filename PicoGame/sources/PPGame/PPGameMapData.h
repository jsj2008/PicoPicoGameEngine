/*-----------------------------------------------------------------------------------------------
	名前	PPGameMapData.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PPGameMapEvent.h"
#include "PPGameGeometry.h"
#include "PPWorld.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class QBGame;
class PPGameMapData;

class PPGameMapData {
public:
	PPGameMapData();
	virtual ~PPGameMapData();
	bool decompress(const void* bytes);
	void init();
	void freedata();
	unsigned short pat(int x,int y);
	void setPat(int x,int y,unsigned short pat);
	
	unsigned short* map;
	unsigned short width;
	unsigned short height;
	
	PPPoint pos;
	
	PPGameMapEvent* event;
	int eventNum;
	
	void draw(PPObject* object,PPWorld* target,PP_DRAW_SELECTOR selector);
	void draw(PPObject* object,PPRect drawArea);

	unsigned short getGID(int x,int y) {
		if (x >= 0 && x < width && y >= 0 && y < height) {
			return map[x+y*width];
		}
		return 0;
	}
	void setGID(int x,int y,unsigned short gid) {
		if (x >= 0 && x < width && y >= 0 && y < height) {
			map[x+y*width] = gid;
		}
	}

	void draw(PPPoint pos,PPGameMapData* src,PPRect rect) {
		for (int y=rect.y;y<rect.y+rect.height;y++) {
			for (int x=rect.x;x<rect.x+rect.width;x++) {
				setGID(x-rect.x+pos.x,y-rect.y+pos.y,src->getGID(x,y));
			}
		}
	}
	
};

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
