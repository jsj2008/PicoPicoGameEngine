/*-----------------------------------------------------------------------------------------------
	名前	PPMap.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPMap_h
#define picopico_PPMap_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
#include "PPAttributeMap.h"
#include "PPGameMap.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPMap : public PPObject {
public:
	PPMap(PPWorld* world) : PPObject(world),mapdata(NULL),attr(NULL),curLayer(0) {
#ifdef _OBJMEM_DEBUG_
    objname="PPMap";
    printf("alloc %s\n",objname);
    fflush(stdout);
#endif
  }
	PPMap(PPWorld* world,const char* name,const char* attribute=NULL) : PPObject(world),mapdata(NULL),attr(NULL) {
		start(name,attribute);
#ifdef _OBJMEM_DEBUG_
    objname="PPMap";
    printf("alloc %s\n",objname);
    fflush(stdout);
#endif
	}
	virtual ~PPMap() {
		if (mapdata) delete mapdata;
		if (attr) delete attr;
	}

	PPGameMap* mapdata;
	PPAttributeMap* attr;

	void start(const char* name,const char* attribute=NULL) {
		PPObject::start();
		if (mapdata) delete mapdata;
		mapdata = new PPGameMap(name);
		if (attr) delete attr;
		attr = NULL;
		if (attribute) {
			attr = new PPAttributeMap(attribute);
		}
		curLayer = 0;
		drawArea = PPRect(0,0,mapdata->map[0].width,mapdata->map[0].height);
	}

	PPRect rect(const char* name) {
		return mapdata->rect(name);
	}
	
	PPPoint position(const char* name) {
		PPRect r = mapdata->rect(name);
		return PPPoint(r.x,r.y);
	}
	
	PPPoint delta(const char* name) {
		PPGameMapEvent* e = mapdata->event(name);
		if (e) {
			return PPPoint(e->dx,e->dy);
		}
		return PPPointZero;
	}
	
	bool attributeCheck(PPPoint pos,int attrIndex,int layer=0) {
		if (attr) {
			return attr->attributeCheck(mapdata->map[layer].getGID(pos.x,pos.y),attrIndex);
		}
		return false;
	}
	
	bool attributeCheck(PPPoint pos,const char* attrName,int layer=0) {
		return attributeCheck(pos,attributeIndex(attrName),layer);
	}

	int attributeIndex(const char* attrName) {
		if (attr == NULL) return -1;
		return attr->index(attrName);
	}

	void put(PPPoint pos,PPMap* src,PPRect rect,int layer=0) {
		mapdata->map[layer].draw(pos,&src->mapdata->map[layer],rect);
	}

	virtual void drawSelf(PPPoint _pos) {
		mapdata->map[0].pos = pos*(-1)+_pos;
		mapdata->map[1].pos = pos*(-1)+_pos;
		mapdata->map[2].pos = pos*(-1)+_pos;
		mapdata->map[0].draw(this,drawArea);
		mapdata->map[1].draw(this,drawArea);
		mapdata->map[2].draw(this,drawArea);
	}
	
	const char* textureName() {
		return mapdata->texture;
	}

	virtual PPSize size();
	
	int curLayer;
	PPRect drawArea;

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
