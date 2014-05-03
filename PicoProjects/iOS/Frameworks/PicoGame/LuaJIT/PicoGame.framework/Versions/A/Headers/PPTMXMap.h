/*-----------------------------------------------------------------------------------------------
	名前	PPTMXMap.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPTMXMap_h__
#define __PPTMXMap_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
#include <string>
#include <vector>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;

class PPTMXTileSet {
public:
	PPTMXTileSet();
	virtual ~PPTMXTileSet();
	
	int firstgid;
	std::string name;
	int tilewidth;
	int tileheight;
	
	int width;
	int height;
	std::string source;
};

class PPTMXChip {
public:
	PPTMXChip() {
		reset();
	}
	void reset() {
		tile = 0;
		flag = 0;
		color = PPColor::white();
	}
	unsigned int tile;
	unsigned short flag;
	PPColor color;
};

class PPTMXLayer {
public:
	PPTMXLayer();
	virtual ~PPTMXLayer();
	
	std::string name;
	PPSize mapSize;
	PPPoint origin;
//	int width;
//	int height;
	
	PPTMXChip* chip;
	
//	unsigned int* map;
//	unsigned char* att;
	
	bool visible;
	
	std::string encoding;
	std::string compression;
	
	unsigned int getGID(int x,int y) {
		if (chip) {
			//if (mapSize.width > 0 && mapSize.height > 0)
			{
				while (x < 0) x += mapSize.width;
				while (y < 0) y += mapSize.height;
				while (x >= mapSize.width) x -= mapSize.width;
				while (y >= mapSize.height) y -= mapSize.height;
				if (x >= 0 && x < mapSize.width && y >= 0 && y < mapSize.height) {
					return chip[(int)(x+y*mapSize.width)].tile;
				}
			}
		}
		return 0;
	}
	
	void setGID(int x,int y,unsigned int gid) {
		if (chip) {
			//if (mapSize.width > 0 && mapSize.height > 0)
			{
				while (x < 0) x += mapSize.width;
				while (y < 0) y += mapSize.height;
				while (x >= mapSize.width) x -= mapSize.width;
				while (y >= mapSize.height) y -= mapSize.height;
				if (x >= 0 && x < mapSize.width && y >= 0 && y < mapSize.height) {
					chip[(int)(x+y*mapSize.width)].tile = gid;
				}
			}
		}
	}
	
	PPTMXChip getAttribute(int x,int y) {
		if (chip) {
			//if (mapSize.width > 0 && mapSize.height > 0)
			{
				while (x < 0) x += mapSize.width;
				while (y < 0) y += mapSize.height;
				while (x >= mapSize.width) x -= mapSize.width;
				while (y >= mapSize.height) y -= mapSize.height;
				if (x >= 0 && x < mapSize.width && y >= 0 && y < mapSize.height) {
					return chip[(int)(x+y*mapSize.width)];
				}
			}
		}
		return PPTMXChip();
	}
	
	void setAttribute(int x,int y,PPTMXChip attr) {
		if (chip) {
			//if (mapSize.width > 0 && mapSize.height > 0)
			{
				while (x < 0) x += mapSize.width;
				while (y < 0) y += mapSize.height;
				while (x >= mapSize.width) x -= mapSize.width;
				while (y >= mapSize.height) y -= mapSize.height;
				if (x >= 0 && x < mapSize.width && y >= 0 && y < mapSize.height) {
					chip[(int)(x+y*mapSize.width)].flag = attr.flag;
					chip[(int)(x+y*mapSize.width)].color = attr.color;
				}
			}
		}
	}
	
	void show() {
		visible = true;
	}
	void hide() {
		visible = false;
	}
	
	void resize(int _width,int _height) {
		PPTMXChip* c = (PPTMXChip*)calloc(1,_width*_height*sizeof(PPTMXChip));
		if (chip) {
			for (int y=0;y<mapSize.height;y++) {
				for (int x=0;x<mapSize.width;x++) {
					if (x < _width && y < _height) {
						c[x+y*_width] = chip[(int)(x+y*mapSize.width)];
					}
				}
			}
			free(chip);
		}
		chip = c;
		mapSize.width = _width;
		mapSize.height = _height;
	}

	PPPoint pos;

	virtual void draw(PPObject* obj,PPRect drawArea);
	//virtual void draw(PPObject* obj,PPWorld* target,PP_DRAW_SELECTOR selector);

private:
	void preCalc(PPObject* obj,int x,int y,int index,PPPoint* outpos);
};

class PPTMXProperty {
public:
	PPTMXProperty();
	virtual ~PPTMXProperty();
	
	std::string name;
	std::string value;
};

class PPTMXObject {
public:
	PPTMXObject();
	virtual ~PPTMXObject();
	
	std::string name;
	std::string type;
	int x;
	int y;
	int width;
	int height;
	unsigned int gid;
	
	std::vector<PPTMXProperty*> property;
};

class PPTMXObjectGroup {
public:
	PPTMXObjectGroup();
	virtual ~PPTMXObjectGroup();
	
	std::string name;
	int width;
	int height;

	std::vector<PPTMXObject*> object;

	PPTMXObject* findObject(const char* name,int* index) {
		for (int i=0;i<object.size();i++) {
			if (object[i]) {
				if (object[i]->name == name) {
					*index = i;
					return object[i];
				}
			}
		}
		return NULL;
	}
};

class PPTMXMap : public PPObject {
public:
	PPTMXMap(PPWorld* world);
	virtual ~PPTMXMap();
	
	//virtual void start(const char* name);
	
	virtual void stepIdle();

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

	static std::string className;
	static PPObject* registClass(PPLuaScript* script,const char* name=NULL,const char* superclass=NULL);
	static PPObject* registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass=NULL);
	
	virtual bool load(const char* tmxpath);
	
//	virtual void init();
	virtual void reset();
	
//	virtual void resize(PPSize size);
	
	virtual void print(const char* str);
	virtual void moveCursor(int dx,int dy);
	
	virtual void tset(unsigned int gid,PPPoint pos);
	virtual unsigned int tget(PPPoint pos);
	virtual void aset(PPTMXChip gid,PPPoint pos);
	virtual PPTMXChip aget(PPPoint pos);

	virtual void line(PPPoint pos1,PPPoint pos2,int tid);
	virtual void box(PPRect rect,int tid);
	virtual void fill(PPRect rect,int tid);
	virtual void paint(PPPoint pos,int tid,int borderid);
	virtual void paint(PPPoint pos,int tid);
	virtual void circle(PPPoint pos,float r,int tid,float start,float end);
	virtual void scroll(PPPoint move);
	virtual void copy(PPRect rect,PPPoint pos);

	virtual void drawSelf(PPPoint _pos);

	virtual PPSize size();
	virtual PPSize realSize();

	int curLayer;
	
	std::string currentStr;

	std::vector<PPTMXTileSet*> tileset;
	std::vector<PPTMXLayer*> layer;
	std::vector<PPTMXObjectGroup*> objectgroup;
	
	std::string version;
	std::string orientation;
//	PPSize mapSize;
//	int width;
//	int height;
	int tilewidth;
	int tileheight;

	PPRect drawArea;
	
	PPPoint editPos;
	PPTMXChip editAttr;
//	PPColor editColor;
//	int editTid;
	
	PPTMXObject* _curobject;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
