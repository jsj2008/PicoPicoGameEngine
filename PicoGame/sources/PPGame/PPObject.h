/*-----------------------------------------------------------------------------------------------
	名前	PPObject.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPObject_h
#define picopico_PPObject_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPWorld.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "PPGamePoly.h"
#include "PPAnimation.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPObject;
//class PPArray;
class QBGame;
class PPAnimationData;
class PPLuaScript;

#ifdef __LUAJIT__
#include <lua.hpp>
#else
extern "C" {
#include <lua/lua.h>
}
#endif

struct PPObjectHit {
  PPObjectHit() {
    index = 0;
    mask = 0;
    type = 0;
    pos = PPPointZero;
    center = PPPointZero;
    rect = PPRectZero;
    length = 0;
  }
	int index;
	int mask;
	unsigned long type;
	PPPoint pos;
	PPPoint center;
	PPRect rect;
	lua_Number length;
};

class PPObject : public PPStep {
public:
//	friend class PPArray;
	friend class PPWorld;
#ifdef _OBJMEM_DEBUG_
  const char* objname;
#endif

private:
	PPObject* __otnext;
  
#if 1
private:
	PPObject* __parent;
	PPObject* __next;
	PPObject* __prev;
	PPObject* __child;
	uint32_t __tag;
	
public:
	void setTag(uint32_t tag) {
		__tag = tag;
	}
	uint32_t tag() {
		return __tag;
	}
	PPObject* parent() {
		return __parent;
	}
	PPObject* child() {
		return __child;
	}
	PPObject* sibling() {
		return __next;
	}
	void addChild(PPObject* object) {
		if (__child) {
			__child->__prev = object;
		}
		object->__parent = this;
		object->__prev = NULL;
		object->__next = __child;
		__child = object;
	}
	void removeFromParent() {
		if (__prev) {
			__prev->__next = __next;
		}
		if (__next) {
			__next->__prev = __prev;
		}
		__next = NULL;
		__prev = NULL;
	}
	PPObject* removeChild(uint32_t tag) {
		PPObject* o = getChild(tag);
		if (o) {
			o->removeFromParent();
		}
		return o;
	}
	PPObject* getChild(uint32_t tag) {
		PPObject* c = __child;
		while (c) {
			if (c->__tag == tag) return c;
			c = c->__next;
		}
		return NULL;
	}
#endif

public:
	PPObject(PPWorld* world);
	virtual ~PPObject();
	
	virtual void start();
	
	virtual bool isAlive();
	virtual void setAlive(bool flag);
	virtual void kill();
	virtual bool isVisible();
	virtual void setVisible(bool flag);
	virtual bool isTouch();
	virtual void setTouch(bool flag);
	
	virtual void draw(PPPoint pos=PPPointZero);
	virtual void drawSelf(PPPoint pos);
	virtual void drawEndSelf(PPPoint pos);
	
	virtual PPRect bounds() {
		PPRect r;
		r.setSize(size());
		r.setPos(autoLayout(pos));
		return r;
	}
	
	void setWorld(PPWorld* world) {
		target = world;
	}
	
	PPWorld* world() {
		return target;
	}
	
	virtual void idle(bool _touch=true);

	virtual void touchDown(PPPoint pos) {
	}

	virtual void touchDrag(PPPoint pos) {
	}

	virtual void touchUp(PPPoint pos) {
	}

	virtual void resetTouchStep() {
		touchStep = 0;
	}
  
  PPObjectHit hit;

protected:
	PPPoint touchPos;
	int touchStep;
	
	virtual bool tapCheck() {
		for (int i=0;i<world()->touchCount();i++) {
			PPRect b = bounds();
			PPPoint p = convertToScreen();
			b.x = p.x;
			b.y = p.y;
			if (world()->touchPosition(i).hitCheck(b)) {
				touchPos = world()->touchPosition(i);
				return true;
			}
		}
		return false;
	}

	virtual bool touchIdle(bool touch=true);
		
	virtual void touchStep1() {
		if (tapCheck()) {
			touchDown(touchPos);
			touchStep = 1;
		}
	}

	virtual void touchStep2() {
		if (world()->touchCount() == 0) {
			touchUp(touchPos);
			touchStep = 0;
		} else {
			PPPoint pos = touchNear(touchPos);
			if (pos != touchPos) {
				touchPos = pos;
				touchDrag(touchPos);
			}
		}
	}
	
public:
	virtual void __directTouchUp(PPPoint pos) {
		//bool t = false;
		{
			PPRect b = bounds();
			PPPoint p = convertToScreen();
			b.x = p.x;
			b.y = p.y;
			if (pos.hitCheck(b)) {
				touchDown(pos);
				touchUp(pos);
				//t = true;
			}
		}
		//if (!t) 
		{
			PPObject* c = child();
			while (c) {
				c->__directTouchUp(pos);
				c = c->sibling();
			}
		}
	}

public:

	float __hit_x;
	float __hit_y;
	float __hit_w;
	float __hit_h;

	virtual void hitArea(float x,float y,float w,float h) {
		__hit_x = x;
		__hit_y = y;
		__hit_w = w;
		__hit_h = h;
	}

	virtual float hitcx() {
		return __hit_x+__hit_w/2;
	}
	
	virtual float hitcy() {
		return __hit_y+__hit_h/2;
	}

	virtual bool hitCheck(PPObject* o) {
		if (__hit_w != 0 && __hit_h != 0) {
			if (o->__hit_w != 0 && o->__hit_h != 0) {
				float x1 = convertToScreen().x+__hit_x;
				float y1 = convertToScreen().y+__hit_y;
				float x2 = o->convertToScreen().x+o->__hit_x;
				float y2 = o->convertToScreen().y+o->__hit_y;
				float dx = x2-x1;
				float dy = y2-y1;
				if (dx > -o->__hit_w && dx < __hit_w) {
					if (dy > -o->__hit_h && dy < __hit_h) {
						return true;
					}
				}
			}
		}
		return false;
	}
	

public:
	virtual void init(PPWorld* world);
	void display(PPWorld* target,PP_DRAW_SELECTOR selector,PPAnimationData* anime);
	void display(PPWorld* target,PP_DRAW_SELECTOR selector);

	void setAnime(const char* anime,const char* name);
	void setAnimeSize(const char* anime,const char* name);
	void setAnimeSize(PPSize size);
	PPSize animeSize();
	void setAnimeDuration(float speed=60);
	void startAnime(int loop=-1);
	void setAnimeFrame(float frame=0);
	void resetAnime();

	int kind;
	PPPoint pos;
	PPAnimation animation;
	int depth;
	
	lua_Number animationTime;
	//PPPoint __rotateCenter;
	
	PPGamePoly poly;
	PPPoint origin;
	
	virtual PPSize tileSize();
	
	PPPoint convertToScreen(PPPoint pos=PPPointZero);
	PPPoint convertToLocal(PPPoint pos);
	
	PPPoint dragPos;
	int dragStep;
	
public:
	virtual float length(const char* format,...) {
		va_list args;
		va_start(args,format);
		return world()->length(format,args);
	}
	virtual void color(PPColor color) {
		world()->color(color);
	}
	virtual PPColor color() {
		return world()->color();
	}
	virtual void blend(PPGameBlend blendType) {
		return world()->blend(blendType);
	}
	virtual PPPoint scale(PPPoint scale=PPPointZero) {
		return world()->scale(scale);
	}
	virtual void rotate(float rotate,float cx=0,float cy=0) {
		world()->rotate(rotate,cx,cy);
	}
	virtual void flip(int type=0) {
		world()->flip(type);
	}
//	virtual unsigned char alpha(int alpha=-1) {
//		return world()->alpha(alpha);
//	}
	virtual void fill(PPRect r) {
		world()->fill(r);
	}
	virtual void line(PPPoint p1,PPPoint p2) {
		world()->line(p1,p2);
	}
	virtual void put(PPPoint p,int gid,int group,int texture) {
		world()->put(p,gid,group,texture);
	}
	virtual int touchCount() {
		return world()->touchCount();
	}
	virtual PPPoint touchPosition(int i) {
		return world()->touchPosition(i);
	}
	virtual PPSize winSize() {
		return world()->winSize();
	}
	virtual void font(const char* name=NULL) {
		return world()->font(name);
	}
	virtual PPPoint layout(PPSize size,PPPoint deltaPos=PPPointZero,unsigned long flag=PP_FIXED,PPRect viewPort=PPRectZero) {
		return world()->layout(size,deltaPos,flag,viewPort);
	}
	virtual PPPoint autoLayout(PPPoint _pos);
	virtual void setLayout(unsigned long flag=0);
	virtual unsigned long layout();
	virtual PPSize size();
	virtual PPSize realSize();

	virtual PPRect viewPort() {
		return world()->viewPort();
	}
	virtual void hide() {
		setVisible(false);
	}
	virtual void show() {
		setVisible(true);
	}

	virtual PPPoint center() {
		PPSize s = size()/2;
		PPPoint c = autoLayout(pos);
		c.x += s.width;
		c.y += s.height;
		return c;
	}

	PPPoint touchNear(PPPoint pos) {
		PPPoint rpos = pos;
		float tlength = 0;
		for (int i=0;i<world()->touchCount();i++) {
			float len = world()->touchPosition(i).length(pos);
			if (i==0 || len < tlength) {
				tlength = len;
				rpos = world()->touchPosition(i);
			}
		}
		return rpos;
	}
	
	static void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

	static std::string className;
	static PPObject* registClass(PPLuaScript* script,const char* name=NULL,const char* superclass=NULL);
	static PPObject* registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass=NULL);
	
	int index;

	void pushTextureInfo(lua_State* L,int tex);

protected:
	unsigned long flags;
	void dispWindow(PPRect rect,PPWorld* target,PP_DRAW_SELECTOR selector);
	void dispPattern(PPPoint point,PPWorld* target,PP_DRAW_SELECTOR selector);
	PPWorld* target;
	PP_DRAW_SELECTOR selector;
	
	PPRect viewPortRect;
	PPRect oldViewPort;

	virtual bool isViewPort();
	virtual void setViewPort(bool flag);
	virtual PPRect _viewPortRect();
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
