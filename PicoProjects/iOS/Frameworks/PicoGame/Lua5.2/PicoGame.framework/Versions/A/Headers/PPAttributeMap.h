/*-----------------------------------------------------------------------------------------------
	名前	PPAttributeMap.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPAttributeMap_h
#define picopico_PPAttributeMap_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <string>
#include "PPGameMap.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPAttributeMap {
public:
	PPAttributeMap(const char* name) {
		maxgid = 0;
		attmax = 0;
		PPGameMap pattern(name);
		PPGameMapData* map = &pattern.map[0];
		attmax = map->eventNum;
		attrlist = new std::string*[map->eventNum];
		for (int i=0;i<map->eventNum;i++) {
			attrlist[i] = new std::string(map->event[i].name);
		}
		for (int y=0;y<map->height;y++) {
			for (int x=0;x<map->width;x++) {
				if (maxgid < pattern.getGID(x,y)) maxgid = pattern.getGID(x,y);
			}
		}
		attrmap = new unsigned char*[attmax];
		for (int i=0;i<attmax;i++) {
			attrmap[i] = new unsigned char[(maxgid+7)/8];
			for (int j=0;j<(maxgid+7)/8;j++) {
				attrmap[i][j] = 0;
			}
		}
		for (int j=0;j<attmax;j++) {
			for (int i=0;i<map->eventNum;i++) {
				PPGameMapEvent* e = &map->event[i];
				if (attrlist[j]->compare(e->name) == 0) {
					for (int y=e->y;y<e->y+e->h;y++) {
						for (int x=e->x;x<e->x+e->w;x++) {
							unsigned short gid = pattern.getGID(x,y);
							if (gid > 0) {
								attrmap[j][gid/8] |= (1<<(gid%8));
							}
						}
					}
				}
			}
		}
	}
	virtual ~PPAttributeMap() {
		if (attrmap) {
			for (int i=0;i<attmax;i++) {
				delete attrmap[i];
			}
			delete attrmap;
		}
		if (attrlist) {
			for (int i=0;i<attmax;i++) {
				delete attrlist[i];
			}
			delete attrlist;
		}
	}
		
	unsigned short maxgid;
	int attmax;
	unsigned char** attrmap;
	std::string** attrlist;
	
	int index(const char* name) {
		for (int i=0;i<attmax;i++) {
			if (attrlist[i]->compare(name) == 0) {
				return i;
			}
		}
		return -1;
	}

	bool attributeCheck(int gid,const char* name) {
		bool r = false;
		if (gid > 0) {
			int i=index(name);
			if (i >= 0) {
				r = ((attrmap[i][gid/8] & (1<<(gid%8)))!=0);
			}
		}
		return r;
	}

	bool attributeCheck(int gid,int index) {
		if (gid > 0 && index >= 0) {
			return ((attrmap[index][gid/8] & (1<<(gid%8)))!=0);
		}
		return false;
	}
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
