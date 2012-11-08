/*-----------------------------------------------------------------------------------------------
	名前	PPVertualKey.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPVertualKey.h"

//PPVertualKey* PPVertualKey::instance = NULL;

PPVertualKey::PPVertualKey(PPWorld* world) : PPObject(world)
{
//	instance = this;
	fixed = false;
}

PPVertualKey::~PPVertualKey()
{
}

void PPVertualKey::start()
{
	vKeyTouch = false;
	vKeyTouch2 = false;
	fixed = false;
	touchState = 0;
	vKeyArea = PPRectZero;
	PPObject::start();
	NEXT(PPVertualKey::stepIdle);
}
	
void PPVertualKey::stepIdle()
{
	if (touchCount() > 0) {
		vKeyDelta = PPPointZero;
		if (!fixed) {
			startTouch = touchPosition(0);
		}
		NEXT(PPVertualKey::stepTouch);
	}
}

void PPVertualKey::stepTouch()
{
	if (touchCount() > 0) {
		int t=0;
		float len=-1;
		for (int i=0;i<touchCount();i++) {
			PPPoint p = touchPosition(i);
			bool enable = false;
			if (vKeyArea.width > 0 && vKeyArea.height > 0) {
				if (p.hitCheck(vKeyArea)) {
					enable = true;
				}
			} else {
				enable = true;
			}
			if (enable) {
				if (len < 0) {
					len = p.length(startTouch);
				} else {
					if (p.length(startTouch) < len) {
						len = p.length(startTouch);
						t = i;
					}
				}
			}
		}
		if (len > 16) {
			vKeyTouch = true;
		} else {
			vKeyTouch = false;
		}
		vKeyTouch2 = true;
//		vKeyTouch = true;
		vKeyDelta = touchPosition(t)-startTouch;
	} else {
		vKeyTouch = false;
		vKeyTouch2 = false;
		NEXT(PPVertualKey::stepIdle);
	}
}

#pragma mark -

#include "PPLuaScript.h"

static int funcTouch(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaScript::UserData(L);
	//PPLuaScript* s = PPLuaScript::sharedScript(L);
	//PPVertualKey* m = PPVertualKey::instance;//(PPVertualKey*)s->userdata;
	lua_pushboolean(L,m->vKeyTouch);
	return 1;
}

static int funcDelta(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPVertualKey* m = PPVertualKey::instance;//(PPVertualKey*)s->userdata;
	return s->returnPoint(L,m->vKeyDelta);
//	lua_pushnumber(L,m->vKeyDelta.x);
//	lua_pushnumber(L,m->vKeyDelta.y);
//	return 2;
}

static int funcDir(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPVertualKey* m = PPVertualKey::instance;//(PPVertualKey*)s->userdata;
	if (s->argCount > 0) {
		if (s->argCount > 1) {
			PPRect area = s->getRect(L,1);
			int d = m->calcDir((int)s->integer(0));
			PPPoint t = m->vKeyDelta+m->startTouch;
			bool b = m->vKeyTouch2;
			if (!b) {
				if (m->touchCount() > 0) {
					if (m->vKeyDelta.length() > 0 && m->vKeyDelta.length() <= 16) {
						b = true;
					}
				}
			}
			if (m->touchState == 0 && b) {
				m->touchState = -1;
				if (t.hitCheck(area)) m->touchState = 1;
			}
			
			if (!b) {
				m->touchState = 0;
			}
			
			if (m->touchState == -1) d = -1;
			lua_pushinteger(L,d);
		} else {
			lua_pushinteger(L,m->calcDir((int)s->integer(0)));
		}
	} else {
		lua_pushinteger(L,m->calcDir(4));
	}
	return 1;
}

static int funcCenter(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPVertualKey* m = PPVertualKey::instance;//(PPVertualKey*)s->userdata;
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->startTouch.x = s->tableNumber(L,0,1,"x");
			m->startTouch.y = s->tableNumber(L,0,2,"y");
		} else {
			if (s->argCount > 1) {
				m->startTouch.x = s->number(0);
				m->startTouch.y = s->number(1);
			} else {
				m->startTouch.x = s->number(0);
				m->startTouch.y = s->number(0);
			}
		}
	}
	return s->returnPoint(L,m->startTouch);
}

static int funcArea(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPVertualKey* m = PPVertualKey::instance;//(PPVertualKey*)s->userdata;
//	if (s->argCount < 2) {
//		return luaL_argerror(L,1,"invalid argument.");
//	}
	if (s->argCount > 0) {
		m->vKeyArea = s->getRect(L,0,m->vKeyArea);
	}
	return s->returnRect(L,m->vKeyArea);
}

static int funcFixed(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPVertualKey* m = PPVertualKey::instance;//(PPVertualKey*)s->userdata;
	if (s->argCount > 0) {
		m->fixed = s->boolean(0,m->fixed);
	}
	lua_pushboolean(L,m->fixed);
	return 1;
}

void PPVertualKey::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
	//PPObject::openLibrary(s,name);
	s->openModule(name,this,0,superclass);
		s->addCommand("center",funcCenter);
		s->addCommand("fixed",funcFixed);
		s->addCommand("touch",funcTouch);
		s->addCommand("delta",funcDelta);
		s->addCommand("area",funcArea);
		s->addCommand("dir",funcDir);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
