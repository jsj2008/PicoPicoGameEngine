/*-----------------------------------------------------------------------------------------------
	名前	PPGameMap.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGAME_MAP__
#define __PPGAME_MAP__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameMapData.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameMap {
public:
	PPGameMap(const char* path);
	virtual ~PPGameMap();

	PPGameMapData map[3];
	
	unsigned short getGID(int x,int y,int layer=0) {
		if (layer >= 0 && layer < 3) {
			PPGameMapData* l = &map[layer];
			if (x >= 0 && x < l->width && y >= 0 && y < l->height) {
				return l->map[x+y*l->width];
			}
		}
		return 0;
	}
	void setGID(int x,int y,unsigned short gid,int layer=0) {
		if (layer >= 0 && layer < 3) {
			PPGameMapData* l = &map[layer];
			if (x >= 0 && x < l->width && y >= 0 && y < l->height) {
				l->map[x+y*l->width] = gid;
			}
		}
	}
	
	PPPoint position(const char* name) {
		PPGameMapEvent* e = event(name);
		if (e == NULL) return PPPointZero;
		return PPPoint(e->x,e->y);
	}

	PPRect rect(const char* name) {
		PPGameMapEvent* e = event(name);
		if (e == NULL) return PPRect(0,0,0,0);
		return PPRect(e->x,e->y,e->w,e->h);
	}
	
	PPGameMapEvent* event(const char* name) {
		for (int i=0;i<map[0].eventNum;i++) {
			if (strcmp(map[0].event[i].name,name) == 0) return &map[0].event[i];
		}
		return NULL;
	}
	
	char* texture;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
