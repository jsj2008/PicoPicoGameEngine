/*-----------------------------------------------------------------------------------------------
	名前	PPUIScrollView.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPUIScrollView.h"

PPUIScrollView::PPUIScrollView(PPWorld* world) : PPObject(world)
{
	_frameSize = PPSize(100,100);
	_contentsRect = PPRect(0,0,100,100);
}

PPUIScrollView::~PPUIScrollView()
{
}

void PPUIScrollView::init()
{
}

void PPUIScrollView::start()
{
	PPObject::start();

	cpos = PPPointZero;
	dpos = PPPointZero;
	counter = 0;
	for (int i=0;i<SPEED_QUE;i++) {
		speedque[i] = PPPointZero;
	}
	
	touchDownFlag = false;
	
//	if (contentsView == NULL) {
//		contentsView = new PPNode();
//		contentsView->start();
//		addChild(contentsView);
//	}
	
	longtouch = 0;
	manualScroll = true;
	pretouchCount = 0;

	NEXT(PPUIScrollView::stepIdle);
}

void PPUIScrollView::stepIdle()
{
}

void PPUIScrollView::setFrameSize(PPSize s)
{
	_frameSize = s;
}

PPSize PPUIScrollView::frameSize()
{
	return _frameSize;
}

void PPUIScrollView::setContentsSize(PPSize s)
{
	_contentsRect = s;
}

PPSize PPUIScrollView::contentsSize()
{
	return _contentsRect.size();
}

#include "PPLuaScript.h"

static int funcFrameRect(lua_State* L)
{
	PPUIScrollView* m = (PPUIScrollView*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPUIScrollView* m = (PPUIScrollView*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}

	lua_getfield(L, 1, "x");
	m->pos.x = lua_tonumber(L, -1);
	lua_getfield(L, 1, "y");
	m->pos.y = lua_tonumber(L, -1);

	PPRect r;
	bool t = false;
	if (s->argCount > 0/* && s->isTable(L,0)*/) {
		PPRect rect = PPRect(m->pos.x,m->pos.y,m->frameSize().width,m->frameSize().height);
		r = s->getRect(L,0,rect);
		t = true;
//	} else
//	if (s->argCount > 3) {
//		r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
//		t = true;
//	} else {
//		r = m->viewPort();
//		t = true;
	}
	if (t) {
		if (m->frameSize().width != r.width || m->frameSize().height != r.height) {
			m->pos = r.pos();
			m->setFrameSize(r.size());
		} else {
			m->pos = r.pos();
		}
		lua_pushnumber(L, m->pos.x);
		lua_setfield(L, 1, "x");
		lua_pushnumber(L, m->pos.y);
		lua_setfield(L, 1, "y");
		return 0;
	}
	return s->returnRect(L,PPRect(m->pos.x,m->pos.y,m->frameSize().width,m->frameSize().height));
}

static int funcContentsRect(lua_State* L)
{
	PPUIScrollView* m = (PPUIScrollView*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPUIScrollView* m = (PPUIScrollView*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	PPRect r;
	if (s->argCount > 0) {
		m->_contentsRect = s->getRect(L,0,m->_contentsRect);
		return 0;
	}
	return s->returnRect(L,m->_contentsRect);
}

static int funcIsDiragging(lua_State* L)
{
	PPUIScrollView* m = (PPUIScrollView*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPUIScrollView* m = (PPUIScrollView*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	bool r=false;
	if (m->touchDownFlag) {
		r=true;
	}
	lua_pushboolean(L,r);
	return 1;
}

static int funcDelete(lua_State *L)
{
	delete (PPUIScrollView*)(PPLuaScript::DeleteObject(L));
	return 0;
}

static int funcNew(lua_State *L)
{
	PPWorld* world = PPLuaScript::World(L);
//	PPUIScrollView* o = (PPUIScrollView*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPUIScrollView* m = (PPUIScrollView*)s->userdata;
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPObject* o = (PPObject*)s->userdata;
	if (s->argCount > 0) {
		lua_createtable(L,(int)s->integer(0),0);
		int table = lua_gettop(L);
		for (int i=0;i<s->integer(0);i++) {
			PPUIScrollView* obj = new PPUIScrollView(world);
			obj->init();
			obj->start();
			PPLuaScript::newObject(L,PPUIScrollView::className.c_str(),obj,funcDelete);
			lua_rawseti(L,table,i+1);
		}
	} else {
		PPUIScrollView* obj = new PPUIScrollView(world);
		obj->init();
		obj->start();
		PPLuaScript::newObject(L,PPUIScrollView::className.c_str(),obj,funcDelete);
	}
	return 1;
}

std::string PPUIScrollView::className = "PPUIScrollView";

PPObject* PPUIScrollView::registClass(PPLuaScript* script,const char* name,const char* superclass)
{
	if (name) PPUIScrollView::className = name;
	return PPUIScrollView::registClass(script,PPUIScrollView::className.c_str(),new PPUIScrollView(script->world()),superclass);
}

PPObject* PPUIScrollView::registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass)
{
	obj->init(script->world());
	PPObject::registClass(script,name,obj);
	script->openModule(name,obj,funcDelete,superclass);
		script->addCommand("new",funcNew);
		script->addCommand("frameRect",funcFrameRect);
		script->addCommand("contentsRect",funcContentsRect);
		script->addCommand("isDragging",funcIsDiragging);
	script->closeModule();
	return obj;
}

void PPUIScrollView::openLibrary(PPLuaScript* script,const char* name,const char* superclass)
{
	registClass(script,name,this,superclass);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
