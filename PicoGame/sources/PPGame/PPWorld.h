/*-----------------------------------------------------------------------------------------------
	名前	PPWorld.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPWorld_h
#define picopico_PPWorld_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdarg.h>

#include "PPStep.h"
#include "PPGameGeometry.h"
//#include "PPAnimation.h"
#include "PPReadError.h"

class PPFont;
class PPObject;

#define PPFONT_DEFAULT "default"
#define PPFONT_DEFAULT2 "default2"
#define PPFONT_HALF "half"
#define PPFONT_MINI "mini"

enum {
	PP_D_PAD    = (1<<0),
	PP_A_BUTTON = (1<<1),
	PP_B_BUTTON = (1<<2),
};

enum {
	PP_FIXED       = 0x0000,
	PP_CENTER_X    = 0x0100,
	PP_CENTER_Y    = 0x0200,
	PP_AUTO_LAYOUT = 0x0400,
};

unsigned int PPGetCharBytesUTF8(const char* string);

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameSprite;

class PPWorld : public PPStep {
public:
//	static PPWorld* sharedWorld(PPWorld* object=NULL);

	PPWorld();
	virtual ~PPWorld();

	virtual unsigned long getKey();

	virtual PPObject* getObject(const char* name=NULL,int index=-1) {
		return NULL;
	}
	virtual bool doCommand(const char* name=NULL,int value=-1) {
		return false;
	}
	virtual int getValue(const char* key,int defaultValue=0) {
		return defaultValue;
	}
	virtual void setValue(const char* key,int value) {
	}

	virtual PPPoint layout(PPSize size,PPPoint deltaPos=PPPointZero,unsigned long flag=PP_FIXED,PPRect vp=PPRectZero) {
		if (vp.width == 0 || vp.height == 0) {
			vp = viewPort();
		}
		PPPoint r = deltaPos;
		if (flag & PP_CENTER_X) {
			r.x = vp.x+(vp.width - size.width)/2+r.x;
		} else {
			if (r.x < 0) {
				r.x = vp.x+vp.width-(size.width-r.x-1);
			} else {
				r.x = vp.x+r.x;
			}
		}
		if (flag & PP_CENTER_Y) {
			r.y = vp.y+(vp.height - size.height)/2+r.y;
		} else {
			if (r.y < 0) {
				r.y = vp.y+vp.height-(size.height-r.y-1);
			} else {
				r.y = vp.y+r.y;
			}
		}
		return r;
	}

//	virtual PPObject* startObject(const char* name=NULL);
//	virtual PPObject* newObject(const char* name=NULL);
	
	virtual void resetOT();
	virtual void addOT(PPObject* object);
	virtual void drawOT();
	virtual int depthOT() {return 100;}
	
	PPObject** ot;

	enum {
		BLEND_NONE,
		BLEND_LIGHT,
		BLEND_BLACKMASK,
		BLEND_FLASH,
		BLEND_RED,
		BLEND_NOMASK,
		BLEND_COLOR,
		BLEND_EDGESMOOTH,
	};

	virtual void locate(float x,float y) {
		locate(PPPoint(x,y));
	}
	virtual PPPoint locate(PPPoint pos=PPPointZero)=0;
	virtual int print(const char* format,...)=0;
	virtual void print(const char* format,va_list args)=0;
	virtual void color(PPColor color)=0;
	virtual PPColor color()=0;
	virtual void blend(unsigned short blendType)=0;
	virtual PPPoint scale(PPPoint scale=PPPointZero)=0;
	virtual void flip(int type=0)=0;
	virtual void rotate(float rotate=0,float cx=0,float cy=0)=0;
//	virtual unsigned char alpha(int alpha=-1)=0;
	virtual void fill(PPRect r)=0;
	virtual void line(PPPoint p1,PPPoint p2)=0;
	virtual unsigned char* getData(const char* key,int* dataSize)=0;
	virtual void setData(const char* key,unsigned char* data,int dataSize)=0;
	virtual void enable(unsigned long flags)=0;
	virtual void disable(unsigned long flags)=0;
	virtual int touchCount()=0;											//タッチ位置の取得//
	virtual PPPoint touchPosition(int i)=0;								//タッチ位置(座標)//
	virtual PPSize winSize()=0;
	virtual bool play(const char* mml,int ch=0)=0;
//	virtual void viewPort(PPRect rect)=0;
	virtual void put(PPPoint p,int gid,int group,int texture)=0;
	virtual PPRect viewPort()=0;
	virtual void font(const char* name=NULL)=0;
	virtual int length(const char* format,...)=0;
	virtual int length(const char* format,va_list args)=0;
	//virtual void setTexture(int fontID,unsigned char* pixel,int width,int height,int bytesPerRow)=0;
	//virtual void setTileSize(int texture,PPSize size)=0;
	//virtual PPSize getTileSize(int texture)=0;
	virtual void reloadData() {}

//	static void setDensity(float v) {density = v;}
//	static float density;
	
	virtual void drawPattern(PPPoint pos,unsigned short gid,void* userdata)=0;

	PPPoint scale_value;
	PPPoint rotate_center;
	float scale_factor;
//	float scale_x_value;
//	float scale_y_value;

	void* userdata;
	PPGameSprite* projector;
	
private:
	virtual void allocOT();
};

typedef void (PPWorld::*PP_DRAW_SELECTOR)(PPPoint pos,unsigned short gid,void* userdata);

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
