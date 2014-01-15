/*-----------------------------------------------------------------------------------------------
	名前	PPObject.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include "PPObject.h"
#include "PPLuaScript.h"
#include "PPGameSprite.h"
#include "QBGame.h"
#include "PPAnimation.h"

#define PPOBJECT_VISIBLE		(1<<0)
#define PPOBJECT_ALIVE			(1<<1)
#define PPOBJECT_TOUCH			(1<<2)
#define PPOBJECT_TOUCH_IDLE		(1<<3)
#define PPOBJECT_FIXED    		0x0000
#define PPOBJECT_CENTER_X		0x0100
#define PPOBJECT_CENTER_Y		0x0200
#define PPOBJECT_AUTO_LAYOUT	0x0400
#define PPOBJECT_VIEW_PORT		0x0800

#ifdef _OBJMEM_DEBUG_
static int dealloc_count=0;
#endif

PPObject::PPObject(PPWorld* world) : target(world)
{
	flags = 0;
	index = 0;
	kind = 0;
	__parent = NULL;
	__next = NULL;
	__prev = NULL;
	__child = NULL;
	__otnext = NULL;
	__tag = 0;
	depth = 0;
	touchStep = 0;
	poly._texture = 0;
	selector = NULL;
	dragStep = 0;
	init(world);
#ifdef _OBJMEM_DEBUG_
  printf("alloc PPObject %d\n",dealloc_count);
  fflush(stdout);
  dealloc_count++;
#endif
}

PPObject::~PPObject()
{
	PPObject* c = __child;
	while (c) {
		PPObject* p = c->__next;
		delete c;
		c = p;
	}
#ifdef _OBJMEM_DEBUG_
  dealloc_count--;
  if (objname) {
    printf("dealloc PPObject %d,%s\n",dealloc_count,objname);
  } else {
    printf("dealloc PPObject %d\n",dealloc_count);
  }
  fflush(stdout);
#endif
}

void PPObject::init(PPWorld* world)
{
	PPStep::start();
	__parent = NULL;
	__next = NULL;
	__prev = NULL;
	__child = NULL;
	__tag = 0;
	kind = 0;
	depth = 0;
	touchStep = 0;
	__hit_x = 0;
	__hit_y = 0;
	__hit_w = 0;
	__hit_h = 0;
	flags = 0;
	setAlive(false);
	setVisible(false);
	animation.init();
	poly.init();
	origin = PPPointZero;
	if (world == NULL) {
	} else {
		display(world,(PP_DRAW_SELECTOR)&PPWorld::drawPattern);
	}
}

void PPObject::start()
{
	PPStep::start();
	__parent = NULL;
	__next = NULL;
	__prev = NULL;
	__child = NULL;
	__tag = 0;
	kind = 0;
	depth = 0;
	touchStep = 0;
	__hit_x = 0;
	__hit_y = 0;
	__hit_w = 0;
	__hit_h = 0;
	flags = 0;
	poly.reset();
	setAlive(true);
	show();
	animation.start();
}

bool PPObject::touchIdle(bool touch)
{
	if (touch) {
		setTouch(false);
    switch (touchStep) {
    case 0:
      touchStep1();
      break;
    case 1:
      touchStep2();
      break;
    }
	}
	return touch;
}

void PPObject::idle(bool _touch) {
	if (isAlive()) {
		_touch = touchIdle(_touch);
		PPStep::idle();
		PPObject* c = child();
		while (c) {
			c->idle(_touch);
			c = c->sibling();
		}
	}
}

bool PPObject::isAlive()
{
	return ((flags & PPOBJECT_ALIVE)!=0);
}

void PPObject::setAlive(bool flag)
{
	if (flag) {
		flags |= PPOBJECT_ALIVE;
	} else {
		flags &= (~PPOBJECT_ALIVE);
		touchStep = 0;
	}
	{
		PPObject* c = __child;
		while (c) {
			c->setAlive(flag);
			c = c->__next;
		}
	}
}

void PPObject::kill()
{
	setAlive(false);
}

bool PPObject::isVisible()
{
	return ((flags & PPOBJECT_VISIBLE)!=0);
}

void PPObject::setVisible(bool flag)
{
	if (flag) {
		flags |= PPOBJECT_VISIBLE;
	} else {
		flags &= (~PPOBJECT_VISIBLE);
		touchStep = 0;
	}
	{
		PPObject* c = __child;
		while (c) {
			c->setVisible(flag);
			c = c->__next;
		}
	}
}

bool PPObject::isTouch()
{
	return ((flags & PPOBJECT_TOUCH)!=0);
}

void PPObject::setTouch(bool flag)
{
	if (flag) {
		flags |= PPOBJECT_TOUCH;
	} else {
		flags &= (~PPOBJECT_TOUCH);
	}
}

void PPObject::display(PPWorld* _target,PP_DRAW_SELECTOR _selector,PPAnimationData* _anime)
{
	target = _target;
	selector = _selector;
}

void PPObject::display(PPWorld* _target,PP_DRAW_SELECTOR _selector)
{
	target = _target;
	selector = _selector;
}

void PPObject::setAnime(const char* anime,const char* name)
{
}

void PPObject::resetAnime()
{
	animation.reset();
}

void PPObject::setAnimeSize(const char* anime,const char* name)
{
}

PPSize PPObject::animeSize()
{
	return animation.size();
}

void PPObject::setAnimeSize(PPSize size)
{
	animation.setSize(size);
}

void PPObject::setAnimeDuration(float duration)
{
	animation.setDuration(duration);
}

void PPObject::startAnime(int loop)
{
	animation.loop = loop;
}

void PPObject::setAnimeFrame(float frame)
{
	animation.setFrame(frame);
}

void PPObject::draw(PPPoint _pos)
{
	if (isAlive() && isVisible()) {
		drawSelf(_pos);
		PPObject* c = child();
		while (c) {
			if (isViewPort()) {
				PPPoint t = _viewPortRect().pos();
				c->draw(_pos+pos-t);
			} else {
				c->draw(_pos+pos);
			}
			c = c->sibling();
		}
		drawEndSelf(_pos);
	}
}

void PPObject::drawSelf(PPPoint _pos)
{
	PPPoint p = autoLayout(pos);
	dispPattern(_pos+p,target,selector);
}

void PPObject::drawEndSelf(PPPoint _pos)
{
}

PPPoint PPObject::convertToScreen(PPPoint _pos)
{
	if (parent() != NULL) {
		return parent()->convertToScreen()+pos;
	}
	return autoLayout(pos+_pos);
}

PPPoint PPObject::convertToLocal(PPPoint _pos)
{
	PPPoint p = convertToScreen();
	return _pos-p;
}

PPPoint PPObject::autoLayout(PPPoint _pos)
{
	if (flags & PPOBJECT_AUTO_LAYOUT) {
		return world()->layout(size(),_pos,flags);
	}
	return _pos;
}

void PPObject::setLayout(unsigned long flag)
{
	unsigned long f = PPOBJECT_AUTO_LAYOUT | PPOBJECT_CENTER_X | PPOBJECT_CENTER_Y;
	flags &= ~f;
	flags |= flag;
}

unsigned long PPObject::layout()
{
	unsigned long f = PPOBJECT_AUTO_LAYOUT | PPOBJECT_CENTER_X | PPOBJECT_CENTER_Y;
	return flags & f;
}

PPSize PPObject::size()
{
	return animation.size()*tileSize()*poly.scale;
}


void PPObject::pushTextureInfo(lua_State* L,int tex)
{
	world()->projector->textureManager->pushTextureInfo(L,tex);
}

#pragma mark -

void PPObject::dispPattern(PPPoint point,PPWorld* target,PP_DRAW_SELECTOR selector)
{
	PPObject* obj = this;
	if (obj->animation.anime()) {
		if (target != NULL && selector != 0) {
		}
	} else {
		if (target != NULL && selector != 0) {
			PPPoint p = point;
			poly.origin = p+origin;
			poly.pos.x = p.x;
			poly.pos.y = p.y;
			poly.pat = obj->animation.key();
			poly.group = 0;
			world()->projector->DrawPoly(&poly);
		}
	}
}

void PPObject::dispWindow(PPRect rect,PPWorld* target,PP_DRAW_SELECTOR selector)
{
}

bool PPObject::isViewPort()
{
	return ((flags & PPOBJECT_VIEW_PORT)!=0);
}

void PPObject::setViewPort(bool flag)
{
	if (flag) {
		flags |= PPOBJECT_VIEW_PORT;
	} else {
		flags &= (~PPOBJECT_VIEW_PORT);
	}
}

PPRect PPObject::_viewPortRect()
{
	return PPRect(0,0,0,0);
}

PPSize PPObject::tileSize()
{
	return poly.texTileSize;
}

#pragma mark -

//static int funcInit(lua_State* L)
//{
//	PPObject* m = (PPObject*)PPLuaArg::UserData(L);
//  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
//	m->init(NULL);
//	return 0;
//}

//static int funcStart(lua_State* L)
//{
//	PPObject* m = (PPObject*)PPLuaArg::UserData(L);
//  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
//	m->start();
//	return 0;
//}

static int funcShow(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
	m->show();
	return 0;
}

static int funcHide(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
	m->hide();
	return 0;
}

static int funcIdle(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
	m->idle();
	return 0;
}

static int funcLayout(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
  PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
  if (m) {
    if (lua_gettop(L)>=4) {
      bool autolayout;
      bool centerx;
      bool centery;
      PPRect layoutarea;

      autolayout = lua_toboolean(L,2);
      centerx = lua_toboolean(L,3);
      centery = lua_toboolean(L,4);
      if (!lua_isnil(L,5)) {
        layoutarea = s->getRect(L,3);
      } else {
        layoutarea = PPRectZero;
      }
      lua_pushboolean(L,autolayout);
      lua_setfield(L,1,"autolayout");
      lua_pushboolean(L,centerx);
      lua_setfield(L,1,"centerx");
      lua_pushboolean(L,centery);
      lua_setfield(L,1,"centery");
      s->pushRect(L,layoutarea);
      lua_setfield(L,1,"layoutarea");
    }
  }
	return 0;
}

static int funcDraw(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBGame* g = (QBGame*)m->world();//s->userdata;

#if 0   //setter & getter test
	lua_getfield(L, 1, "x");
	m->pos.x = lua_tonumber(L, -1);
	lua_getfield(L, 1, "y");
	m->pos.y = lua_tonumber(L, -1);
#endif
	
	lua_getfield(L,1,"autolayout");
	if (lua_toboolean(L,-1)) {
		PPPoint op = m->pos;
		bool centerx;
		bool centery;
		PPRect layoutarea;
		lua_getfield(L,1,"centerx");
		centerx = lua_toboolean(L,-1);
		lua_getfield(L,1,"centery");
		centery = lua_toboolean(L,-1);
		lua_getfield(L,1,"layoutarea");
		layoutarea = s->getRect(L,lua_gettop(L)-2);
		if (layoutarea.width == 0 || layoutarea.height == 0) {
			layoutarea = g->viewPort();
		}
		int flag = 0;
		if (centerx) flag |= PP_CENTER_X;
		if (centery) flag |= PP_CENTER_Y;
		m->pos = g->layout(m->size(),m->pos,flag,layoutarea);
		m->draw();
		m->pos = op;
	} else {
		m->draw();
	}	
	return 0;
}

static int funcIsAlive(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	lua_pushboolean(L,m->isAlive());
	return 1;
}

static int funcIsVisible(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	lua_pushboolean(L,m->isVisible());
	return 1;
}

static int funcPosition(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->pos = PPPoint(s->tableNumber(L,0,1,"x",m->pos.x),s->tableNumber(L,0,2,"y",m->pos.y));
		} else {
			m->pos = PPPoint(s->number(0),s->number(1));
		}
#if 0   //setter & getter test
		lua_pushnumber(L, m->pos.x);
		lua_setfield(L, 1, "x");
		lua_pushnumber(L, m->pos.y);
		lua_setfield(L, 1, "y");
#endif
	} else {
#if 0   //setter & getter test
		lua_getfield(L, 1, "x");
		m->pos.x = lua_tonumber(L, -1);
		lua_getfield(L, 1, "y");
		m->pos.y = lua_tonumber(L, -1);
#endif
		return s->returnPoint(L,m->pos);
	}
	return 0;
}

static int funcColor(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->poly.color.r = s->tableInteger(L,0,1,"r",m->poly.color.r);
			m->poly.color.g = s->tableInteger(L,0,2,"g",m->poly.color.g);
			m->poly.color.b = s->tableInteger(L,0,3,"b",m->poly.color.b);
			m->poly.color.a = s->tableInteger(L,0,4,"a",m->poly.color.a);
		} else {
			m->poly.color.r = s->integer(0);
			if (s->argCount > 1) m->poly.color.g = s->integer(1);
			if (s->argCount > 2) m->poly.color.b = s->integer(2);
			if (s->argCount > 3) m->poly.color.a = s->integer(3);
		}
		return 0;
	}
	lua_createtable(L, 0, 4);
	lua_pushinteger(L,m->poly.color.r);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L,m->poly.color.g);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L,m->poly.color.b);
	lua_setfield(L, -2, "b");
	lua_pushinteger(L,m->poly.color.a);
	lua_setfield(L, -2, "a");
	return 1;
}

static int funcSize(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	return s->returnSize(L,m->size());
}

static int funcTileSize(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->poly.texTileSize = s->getSize(L,0);
		return 0;
	} else
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			m->poly.texTileSize.width = s->number(0);
			m->poly.texTileSize.height = s->number(0);
		} else {
			m->poly.texTileSize.width = s->number(0);
			m->poly.texTileSize.height = s->number(1);
		}
		return 0;
	}
	return s->returnSize(L,m->poly.texTileSize);
}

static int funcTileInfo(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->animation.setKey((int)s->tableInteger(L,0,"index",m->animation.key()+1)-1);
    lua_getfield(L,2,"size");
    if (lua_istable(L,-1)) {
      m->poly.texTileSize = s->getSize(L,lua_gettop(L)-2);
    }
    lua_getfield(L,2,"stride");
    if (lua_istable(L,-1)) {
      PPPoint p=s->getPoint(L,lua_gettop(L)-2);
      m->poly.texTileStride = PPSize(p.x,p.y);
    }
    lua_getfield(L,2,"offset");
    if (lua_istable(L,-1)) {
      m->poly.texOffset = s->getPoint(L,lua_gettop(L)-2);
    }
		return 0;
	}

	lua_createtable(L, 0, 4);
	lua_pushnumber(L,m->animation.key()+1);
	lua_setfield(L, -2, "index");
	
	lua_createtable(L, 0, 4);
	lua_pushnumber(L,0);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,0);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L,m->poly.texTileSize.width);
	lua_setfield(L, -2, "width");
	lua_pushnumber(L,m->poly.texTileSize.height);
	lua_setfield(L, -2, "height");
  lua_getglobal(L,"pprect_mt");
  lua_setmetatable(L,-2);
	lua_setfield(L, -2, "size");
	
	lua_createtable(L, 0, 2);
	lua_pushnumber(L,m->poly.texTileStride.width);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,m->poly.texTileStride.height);
	lua_setfield(L, -2, "y");
  lua_getglobal(L,"pppoint_mt");
  lua_setmetatable(L,-2);
	lua_setfield(L, -2, "stride");
	
	lua_createtable(L, 0, 2);
	lua_pushnumber(L,m->poly.texOffset.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,m->poly.texOffset.y);
	lua_setfield(L, -2, "y");
  lua_getglobal(L,"pppoint_mt");
  lua_setmetatable(L,-2);
	lua_setfield(L, -2, "offset");

	return 1;
}

static int funcTileStride(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->poly.texTileStride = s->getSize(L,0);
		return 0;
	} else
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			m->poly.texTileStride.width = s->number(0);
			m->poly.texTileStride.height = s->number(0);
		} else {
			m->poly.texTileStride.width = s->number(0);
			m->poly.texTileStride.height = s->number(1);
		}
		return 0;
	}
	return s->returnPoint(L,PPPoint(m->poly.texTileStride.width,m->poly.texTileStride.height));
}

static int funcTileOffset(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->poly.texOffset = s->getPoint(L,0);
		return 0;
	} else
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			m->poly.texOffset.x = s->number(0);
			m->poly.texOffset.y = s->number(0);
		} else {
			m->poly.texOffset.x = s->number(0);
			m->poly.texOffset.y = s->number(1);
		}
		return 0;
	}
	return s->returnPoint(L,m->poly.texOffset);
}

static PPRect calcAABB(lua_State* L,PPLuaArg* s,PPObject* m)
{
//	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBGame* g = (QBGame*)m->world();//s->userdata;
	
	int top=lua_gettop(L);

#if 0   //setter & getter test
	lua_getfield(L, 1, "x");
	m->pos.x = lua_tonumber(L, -1);
	lua_getfield(L, 1, "y");
	m->pos.y = lua_tonumber(L, -1);
#endif

	PPPoint p = m->pos;
	
	lua_getfield(L,1,"autolayout");
	if (lua_toboolean(L,-1)) {
		bool centerx;
		bool centery;
		PPRect layoutarea;
		lua_getfield(L,1,"centerx");
		centerx = lua_toboolean(L,-1);
		lua_getfield(L,1,"centery");
		centery = lua_toboolean(L,-1);
		lua_getfield(L,1,"layoutarea");
		layoutarea = s->getRect(L,lua_gettop(L)-2);
		if (layoutarea.width == 0 || layoutarea.height == 0) {
			layoutarea = g->viewPort();
		}
		int flag = 0;
		if (centerx) flag |= PP_CENTER_X;
		if (centery) flag |= PP_CENTER_Y;
		p = g->layout(m->size(),m->pos,flag,layoutarea);
	} else {
	}

	m->poly.pos = p;
  
  PPPoint org = m->poly.origin;
	m->poly.origin = p+m->origin;

	PPPoint delta=PPPointZero;
	PPPoint o[4];
	PPRect r(0,0,m->poly.texTileSize.width,m->poly.texTileSize.height);
	m->world()->projector->CalcPolyPoint(&m->poly,&r,&delta,o);

	float minx,miny,maxx,maxy;
	minx = o[0].x;
	maxx = o[0].x;
	miny = o[0].y;
	maxy = o[0].y;
	for (int i=1;i<4;i++) {
		if (minx > o[i].x) minx = o[i].x;
		if (miny > o[i].y) miny = o[i].y;
		if (maxx < o[i].x) maxx = o[i].x;
		if (maxy < o[i].y) maxy = o[i].y;
	}
	
	lua_settop(L,top);
  
  m->poly.origin = org;
	
	return PPRect(minx,miny,maxx-minx,maxy-miny);
}

static int funcAABB(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	return s->returnRect(L,calcAABB(L,s,m));
}

static int funcTexture(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isNumber(L,0)) {
			m->poly.setTexture((int)s->integer(0)-1);
		} else
		if (s->isTable(L,0)) {
			m->poly.setTexture((int)s->tableInteger(L,0,"index",0)-1);		//Texture
		} else
		if (s->isString(L,0)) {
			PPGameTextureOption option;
			if (s->argCount > 1 && s->isTable(L,1)) {
				option = s->getTextureOption(L, 1, option);//s->boolean(1);
			}
			m->poly.setTexture(m->world()->projector->textureManager->loadTexture(s->args(0),option));
			if (PPReadError()) {
				PPReadErrorReset();
				return luaL_error(L,"texture file read error '%s'",s->args(0));
			}
		} else {
		}
		return 0;
	}
	m->pushTextureInfo(L,m->poly._texture);
	return 1;
}

static int funcTileIndex(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		m->animation.setKey((int)s->integer(0)-1);
		return 0;
	}
	lua_pushinteger(L,m->animation.key()+1);
	return 1;
}

static int funcEnable(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	m->setAlive(true);
	return 0;
}

static int funcDisable(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	m->setAlive(false);
	return 0;
}

static int funcMove(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);

#if 0   //setter & getter test
	lua_getfield(L, 1, "x");
	m->pos.x = lua_tonumber(L, -1);
	lua_getfield(L, 1, "y");
	m->pos.y = lua_tonumber(L, -1);
#endif

	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		m->pos += p;
	}

#if 0   //setter & getter test
	lua_pushnumber(L, m->pos.x);
	lua_setfield(L, 1, "x");
	lua_pushnumber(L, m->pos.y);
	lua_setfield(L, 1, "y");
#endif
	
	return 0;
}

static int funcRotate(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		m->poly.rotate = s->number(0);
		while (m->poly.rotate < 0) {
			m->poly.rotate += 2*M_PI;
		}
		while (m->poly.rotate > 2*M_PI) {
			m->poly.rotate -= 2*M_PI;
		}
		return 0;
	}
	lua_pushnumber(L,m->poly.rotate);
	return 1;
}

static int funcOrigin(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->origin.x = s->tableNumber(L,0,1,"x");
			m->origin.y = s->tableNumber(L,0,2,"y");
		} else {
			if (s->argCount > 1) {
				m->origin.x = s->number(0);
				m->origin.y = s->number(1);
			} else {
				m->origin.x = s->number(0);
				m->origin.y = s->number(0);
			}
		}
		return 0;
	}
	return s->returnPoint(L,m->origin);
}

static int funcFlip(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 1) {
		m->poly.flags &= ~(PPFLIP_H | PPFLIP_V);
		if (s->boolean(0)) m->poly.flags |= PPFLIP_H;
		if (s->boolean(1)) m->poly.flags |= PPFLIP_V;
		if (s->argCount > 2) {
			m->poly.flags &= ~(PPFLIP_RMASK);
			m->poly.flags |= ((s->integer(2)<<2) & PPFLIP_RMASK);
		}
		return 0;
	}
	int p1,p2;
	p1 = 0;
	p2 = 0;
	if (m->poly.flags & PPFLIP_H) p1 = 1;
	if (m->poly.flags & PPFLIP_V) p2 = 1;
	lua_pushboolean(L,p1);
	lua_pushboolean(L,p2);
	lua_pushinteger(L,(m->poly.flags & PPFLIP_RMASK)>>2);
	return 3;
}

static int funcScale(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->poly.scale = s->getPoint(L,0,m->poly.scale.x,m->poly.scale.y);
		} else
		if (s->argCount == 1) {
			m->poly.scale = PPPoint(s->number(0),s->number(0));
		} else {
			m->poly.scale = s->getPoint(L,0,m->poly.scale.x,m->poly.scale.y);
		}
		return 0;
	}
	return s->returnPoint(L,m->poly.scale);
}

static int funcAlpha(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		m->poly.color.a = s->integer(0);//atoi(s->args(0));
		return 0;
	}
	lua_pushnumber(L,m->poly.color.a);
	return 1;
}

static int funcLoopAnime(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}

	lua_Number t = lua_tonumber(L,2);
	if (t > 0) {
		if (lua_istable(L,3)) {
			lua_getfield(L,1,"animationTime");
			m->animationTime = lua_tonumber(L,-1);

#ifdef __LUAJIT__
			int n=(int)lua_objlen(L,3);
#else
			int n=(int)lua_rawlen(L,3);
#endif
			
			if (m->animationTime<1) m->animationTime=1;
			if (m->animationTime>=n+1) m->animationTime=1;
			
			lua_rawgeti(L,3,(int)m->animationTime);
			m->animation.setKey((int)lua_tointeger(L,-1)-1);
			
			m->animationTime += (n/60.0)/t;
		}
	}
	
	lua_pushnumber(L,m->animationTime);
	lua_setfield(L,1,"animationTime");

	return 0;
}

static int funcPlayAnime(lua_State* L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
//	if (m==NULL) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
	bool endanime = false;

	lua_Number t = lua_tonumber(L,2);
	if (t > 0) {
		if (lua_istable(L,3)) {
			lua_getfield(L,1,"animationTime");
			m->animationTime = lua_tonumber(L,-1);

#ifdef __LUAJIT__
			int n=(int)lua_objlen(L,3);
#else
			int n=(int)lua_rawlen(L,3);
#endif
			
			if (m->animationTime<1) m->animationTime=1;
			if (m->animationTime>=n+1) {
				m->animationTime=n+1;
				endanime = true;
			}
			
			int t=m->animationTime;
			if (t > n) t = n;
			lua_rawgeti(L,3,t);
			m->animation.setKey((int)lua_tointeger(L,-1)-1);
			
			m->animationTime += (n/60.0)/t;
		}
	}

	lua_pushnumber(L,m->animationTime);
	lua_setfield(L,1,"animationTime");

	lua_pushboolean(L,endanime);
	return 1;
}

static int funcContain(lua_State *L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPRect aabb = calcAABB(L,s,m);
	PPPoint p = s->getPoint(L,0);
	bool r=p.hitCheck(aabb);
	lua_pushboolean(L,r);
	return 1;
}

static int funcIntersect(lua_State *L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPRect aabb = calcAABB(L,s,m);
	PPRect rect = s->getRect(L,0);
	bool r=rect.hitCheck(aabb);
	lua_pushboolean(L,r);
	return 1;
}

static lua_Number getNumber(lua_State* L,int stack,int index,const char* field)
{
	lua_getfield(L,stack,field);
	if (lua_isnil(L,-1)) {
		lua_rawgeti(L,stack,index);
		if (lua_isnil(L,-1)) {
			return 0;
		}
	}
	return lua_tonumber(L,-1);
}

static PPPoint nearPos(lua_State* L,int n,PPObject* o)
{
	PPPoint oldPos=o->dragPos+o->pos;
	PPPoint rPos=PPPointZero;
	if (n > 0) {
		PPPoint dragPos;
		float len=-1;
		int top=lua_gettop(L);
		for (int i=1;i<=n;i++) {
			lua_rawgeti(L,2,i);
			int top2=lua_gettop(L);
			dragPos.x = getNumber(L,-1,1,"x");
			lua_settop(L,top2);
			dragPos.y = getNumber(L,-1,2,"y");
			lua_settop(L,top);
			if (len<0) {
				len = oldPos.length(dragPos);
				rPos = dragPos;
			} else {
				float l = oldPos.length(dragPos);
				if (l < len) {
					len = l;
					rPos = dragPos;
				}
			}
		}
	}
	return rPos;
}

static int funcDrag(lua_State *L)
{
	int top=lua_gettop(L);
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);

  PPRect aabb = calcAABB(L,s,m);

	lua_settop(L,top);
	
	if (lua_istable(L,2)) {
#ifdef __LUAJIT__
		int n=(int)lua_objlen(L,2);
#else
		int n=(int)lua_rawlen(L,2);
#endif
		if (n > 0) {
			PPPoint dragPos;
			PPRect dragArea;
			bool useDragArea=false;
			
			if (lua_istable(L,3)) {
				dragArea = s->getRect(L,1);
				useDragArea = true;
			}
      
			switch (m->dragStep) {
			case 0:
				dragPos = nearPos(L,n,m);
				m->dragPos=dragPos-m->pos;
				m->dragStep = 1;
				break;
			case 1:
				dragPos = nearPos(L,n,m);
				if (m->pos.length(dragPos-m->dragPos) < 128) {
          PPPoint op = m->pos;
					m->pos=dragPos-m->dragPos;
					if (useDragArea) {
						PPPoint s=m->pos;
						PPPoint o=m->dragPos;
            
            float dx = op.x-aabb.x;
            float dy = op.y-aabb.y;
            
            dragArea.x += dx;
            dragArea.y += dy;

						PPRect r=dragArea;

						if (s.x<r.x) {
							o.x=o.x+s.x-r.x;s.x=r.x;
						}
						if (s.y<r.y) {
							o.y=o.y+s.y-r.y;s.y=r.y;
						}
						if (s.x>r.x+r.width-aabb.width) {
						  o.x=o.x+s.x-(r.x+r.width-aabb.width);
						  s.x=r.x+r.width-aabb.width;
						}
						if (s.y>r.y+r.height-aabb.height) {
						  o.y=o.y+s.y-(r.y+r.height-aabb.height);
						  s.y=r.y+r.height-aabb.height;
						}

						m->pos=s;
						m->dragPos = o;
					}
				}
				break;
			}

#if 0   //setter & getter test
			lua_pushnumber(L, m->pos.x);
			lua_setfield(L, 1, "x");
			lua_pushnumber(L, m->pos.y);
			lua_setfield(L, 1, "y");
#endif

		} else {
			m->dragStep = 0;
		}
	}
	
	return 0;
}

static int funcBlend(lua_State *L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);

	if (s->argCount > 0) {
		m->poly.blend.blend = lua_toboolean(L,2);
    if (s->argCount > 1) {
      m->poly.blend.blendSrc = (int)lua_tointeger(L,3);
      if (s->argCount > 2) {
        m->poly.blend.blendDst = (int)lua_tointeger(L,4);
      }
    }
    return 0;
  }

  lua_pushboolean(L,m->poly.blend.blend);
	lua_pushnumber(L,m->poly.blend.blendSrc);
	lua_pushnumber(L,m->poly.blend.blendDst);
	return 3;
}

static int funcFog(lua_State *L)
{
	PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);

	if (s->argCount > 0) {
		m->poly.blend.fog = lua_toboolean(L,2);
    if (s->argCount > 1) {
      if (s->isTable(L,1)) {
        m->poly.blend.fogColor.r = s->tableInteger(L,1,1,"r",m->poly.color.r);
        m->poly.blend.fogColor.g = s->tableInteger(L,1,2,"g",m->poly.color.g);
        m->poly.blend.fogColor.b = s->tableInteger(L,1,3,"b",m->poly.color.b);
        m->poly.blend.fogColor.a = s->tableInteger(L,1,4,"a",m->poly.color.a);
      } else {
        m->poly.blend.fogColor.r = s->integer(1);
        if (s->argCount > 1) m->poly.blend.fogColor.g = s->integer(2);
        if (s->argCount > 2) m->poly.blend.fogColor.b = s->integer(3);
        if (s->argCount > 3) m->poly.blend.fogColor.a = s->integer(4);
      }
    }
		return 0;
	}
  
  lua_pushboolean(L,m->poly.blend.fog);
  
	lua_createtable(L, 0, 4);
	lua_pushinteger(L,m->poly.blend.fogColor.r);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L,m->poly.blend.fogColor.g);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L,m->poly.blend.fogColor.b);
	lua_setfield(L, -2, "b");
	lua_pushinteger(L,m->poly.blend.fogColor.a);
	lua_setfield(L, -2, "a");

  return 2;
}

static int funcDelete(lua_State *L)
{
#ifdef _OBJMEM_DEBUG_
  printf("funcDelete\n");
  fflush(stdout);
#endif
	delete (PPObject*)(PPLuaScript::DeleteObject(L));
	return 0;
}

static int funcSetter(lua_State* L)
{
  std::string name = lua_tostring(L,2);
  if (name == "x") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (lua_type(L,3)==LUA_TNUMBER) {
      m->pos.x = lua_tonumber(L,3);
      lua_pushboolean(L,true);
      return 1;
    }
  } else
  if (name == "y") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (lua_type(L,3)==LUA_TNUMBER) {
      m->pos.y = lua_tonumber(L,3);
      lua_pushboolean(L,true);
      return 1;
    }
  } else
  if (name == "alive") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (lua_type(L,3)==LUA_TBOOLEAN) {
      m->setAlive(lua_toboolean(L,3));
      lua_pushboolean(L,true);
      return 1;
    }
  } else
  if (name == "visible") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (lua_type(L,3)==LUA_TBOOLEAN) {
      if (lua_toboolean(L,3)) {
        m->show();
      } else {
        m->hide();
      }
      lua_pushboolean(L,true);
      return 1;
    }
  }
  lua_pushboolean(L,false);
  return 1;
}

static int funcGetter(lua_State* L)
{
  std::string name = lua_tostring(L,2);
  if (name == "x") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (m) {
      lua_pushnumber(L, m->pos.x);
    } else {
      lua_pushnumber(L,0);
    }
    return 1;
  } else
  if (name == "y") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (m) {
      lua_pushnumber(L, m->pos.y);
    } else {
      lua_pushnumber(L,0);
    }
    return 1;
  } else
  if (name == "alive") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (m) {
      lua_pushboolean(L,m->isAlive());
    } else {
      lua_pushboolean(L,false);
    }
    return 1;
  } else
  if (name == "visible") {
    PPObject* m = (PPObject*)PPLuaArg::UserData(L,PPObject::className,false);
    if (m) {
      lua_pushboolean(L,m->isVisible());
    } else {
      lua_pushboolean(L,false);
    }
    return 1;
  }
  lua_pushnil(L);
  return 1;
}

static int funcNew(lua_State *L)
{
	PPWorld* world = PPLuaScript::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
  int texid=-1;
  int texid2=world->projector->textureManager->defaultTexture;
	if (s->argCount > 0) {
    if (lua_isnumber(L,1+s->argShift-1)) {
      lua_createtable(L,(int)s->integer(0),0);
      int table = lua_gettop(L);
      for (int i=0;i<s->integer(0);i++) {
        PPObject* obj = new PPObject(world);
        obj->start();
        obj->poly._texture = world->projector->textureManager->defaultTexture;
        PPLuaScript::newObject(L,PPObject::className.c_str(),obj,funcDelete);
        lua_rawseti(L,table,i+1);
      }
      return 1;
    } else
    if (lua_isstring(L,1+s->argShift-1)) {
			PPGameTextureOption option;
			if (s->argCount > 0 && s->isTable(L,0)) {
				option = s->getTextureOption(L, 0, option);//s->boolean(1);
			}
			texid = world->projector->textureManager->loadTexture(s->args(0),option);
			if (PPReadError()) {
				PPReadErrorReset();
				return luaL_error(L,"texture file read error '%s'",s->args(0));
			}
    } else
    if (lua_istable(L,1+s->argShift-1)) {
			texid2 = (int)s->tableInteger(L,-1,"index",0)-1;		//Texture
    }
	}
  PPObject* obj = new PPObject(world);
  obj->start();
  if (texid >= 0) {
    obj->poly._texture =texid;
    PPSize size = world->projector->textureManager->getImageSize(texid);
    obj->poly.texTileSize.width = size.width;
    obj->poly.texTileSize.height = size.height;
    obj->animation.setKey(0);
  } else {
    obj->poly._texture = texid2;
  }
  PPLuaScript::newObject(L,PPObject::className.c_str(),obj,funcDelete);

	return 1;
}

std::string PPObject::className="PPObject";

PPObject* PPObject::registClass(PPLuaScript* script,const char* name,const char* superclass)
{
	if (name) PPObject::className = name;
	return PPObject::registClass(script,PPObject::className.c_str(),new PPObject(script->world()),superclass);
}

PPObject* PPObject::registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass)
{
	script->openModule(name,obj,0,superclass);

    script->addAccessor(funcSetter,funcGetter);

		script->addCommand("new",funcNew);
		script->addCommand("show",funcShow);
		script->addCommand("hide",funcHide);
		script->addCommand("idle",funcIdle);
		script->addCommand("draw",funcDraw);
		script->addCommand("isAlive",funcIsAlive);
		script->addCommand("isVisible",funcIsVisible);
		script->addCommand("position",funcPosition);
		script->addCommand("pos",funcPosition);
//		script->addCommand("pos",funcPosition);
//		script->addCommand("locate",funcPosition);
		script->addCommand("aabb",funcAABB);
//		script->addCommand("worldPos",funcFrame);
		script->addCommand("size",funcSize);
		script->addCommand("layout",funcLayout);
		script->addCommand("texture",funcTexture);
		script->addCommand("enable",funcEnable);
		script->addCommand("disable",funcDisable);
//		script->addCommand("alive",funcEnable);
//		script->addCommand("kill",funcDisable);
		script->addCommand("move",funcMove);
		script->addCommand("tileInfo",funcTileInfo);
		script->addCommand("tile",funcTileIndex);
		script->addCommand("tileSize",funcTileSize);
		script->addCommand("tileStride",funcTileStride);
		script->addCommand("tileOffset",funcTileOffset);
		script->addCommand("rotate",funcRotate);
//		script->addCommand("origin",funcOrigin);
		script->addCommand("pivot",funcOrigin);
		script->addCommand("flip",funcFlip);
		script->addCommand("scale",funcScale);
		script->addCommand("alpha",funcAlpha);
		script->addCommand("color",funcColor);
		script->addCommand("loopAnime",funcLoopAnime);
		script->addCommand("playAnime",funcPlayAnime);
		script->addCommand("contain",funcContain);
		script->addCommand("intersect",funcIntersect);
		script->addCommand("drag",funcDrag);

//		script->addNumberValue("x",0);
//		script->addNumberValue("y",0);

		script->addNumberValue("animationTime",1);
		script->addNumberValue("hitlength",0);
		script->addIntegerValue("hitmask",0);
		script->addBoolValue("hit",false);
		script->addBoolValue("autolayout",false);
		script->addBoolValue("centerx",false);
		script->addBoolValue("centery",false);

    script->addCommand("blend",funcBlend);
    script->addCommand("shadow",funcFog);
//		script->addCommand("hitCheck",funcHitCheck);

//		lua_pushnumber(script->L,0);
//		lua_setfield(script->L,1,"x");
//		lua_pushnumber(script->L,0);
//		lua_setfield(script->L,1,"y");

//		script->addCommand("x",funcXPosition);
//		script->addCommand("y",funcYPosition);

//		script->addCommand("animation",funcSetAnimation);
//		script->addCommand("resetAnimation",funcResetAnimation);
//		script->addCommand("animationSize",funcSetAnimationSize);
//		script->addCommand("startAnimation",funcStartAnimation);
//		script->addCommand("stopAnimation",funcStopAnimation);
//		script->addCommand("animationLoop",funcAnimation);
//		script->addCommand("animationFrame",funcSetAnimationFrame);

	script->closeModule();
	return obj;
}

void PPObject::openLibrary(PPLuaScript* script,const char* name,const char* superclass)
{
	registClass(script,name,NULL,superclass);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
