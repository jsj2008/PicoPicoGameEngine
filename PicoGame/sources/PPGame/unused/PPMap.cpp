/*-----------------------------------------------------------------------------------------------
	名前	PPMap.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPMap.h"
#include "PPLuaScript.h"

PPSize PPMap::size()
{
	return PPSize(mapdata->map[0].width,mapdata->map[0].height)*tileSize();
}

static int funcLoad(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 1) {
		m->start(s->args(0),s->args(1));
	} else {
		m->start(s->args(0));
	}
	return 0;
}

static int funcEventPos(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	PPPoint d = m->delta(s->args(0));
	PPPoint p = m->position(s->args(0));
	p.x = p.x*m->tileSize().width+d.x*m->tileSize().width/32;
	p.y = p.y*m->tileSize().height+d.y*m->tileSize().height/32;
	lua_pushnumber(L,p.x);
	lua_pushnumber(L,p.y);
	return 2;
}

static int funcEventSize(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	PPRect t = m->rect(s->args(0));
	lua_pushnumber(L,t.width);
	lua_pushnumber(L,t.height);
	return 2;
}

static int funcGID(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	float x = atof(s->args(0))/m->tileSize().width;
	float y = atof(s->args(1))/m->tileSize().height;
	if (s->argCount > 2) {
		int gid = atoi(s->args(2));
		m->mapdata->setGID(x,y,gid,m->curLayer);
	}
	lua_pushinteger(L,m->mapdata->getGID(x,y,m->curLayer));
	return 1;
}

static int funcLayer(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->curLayer = atoi(s->args(0))-1;
		if (m->curLayer < 0) m->curLayer = 0;
		if (m->curLayer > 2) m->curLayer = 2;
	}
	lua_pushinteger(L,m->curLayer+1);
	return 1;
}

static int funcCheck(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	float x = atof(s->args(0))/m->tileSize().width;
	float y = atof(s->args(1))/m->tileSize().height;
	const char* attr = s->args(2);
	lua_pushboolean(L,m->attributeCheck(PPPoint(x,y),attr,m->curLayer));
	return 1;
}

static int funcMapSize(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	PPSize t = PPSizeZero;
	if (m->mapdata) {
		if (m->mapdata->map) {
			t = PPSize(m->mapdata->map[0].width,m->mapdata->map[0].height);
		}
	}
	return s->returnSize(L,t);
}

static int funcMapArea(lua_State* L)
{
	PPMap* m = (PPMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPMap* m = (PPMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
//	PPSize t = PPSizeZero;
	if (s->argCount > 0 && s->isTable(L,0)) {
		m->drawArea = s->getRect(L,0);
	} else
	if (s->argCount > 3) {
		m->drawArea = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
	}
	return s->returnRect(L,m->drawArea);
}

void PPMap::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
//	PPObject::openLibrary(s,name);
	s->openModule(name,this,0,superclass);
		s->addCommand("start",funcLoad);
		s->addCommand("load",funcLoad);
		s->addCommand("eventPos",funcEventPos);
		s->addCommand("eventSize",funcEventSize);
		s->addCommand("gid",funcGID);
		s->addCommand("layer",funcLayer);
		s->addCommand("mapSize",funcMapSize);
//		s->addCommand("setGid",funcSetGID);
		s->addCommand("check",funcCheck);
		s->addCommand("area",funcMapArea);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
