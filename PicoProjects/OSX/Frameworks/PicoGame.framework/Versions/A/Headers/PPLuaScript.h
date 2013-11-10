/*-----------------------------------------------------------------------------------------------
	名前	PPLuaScript.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPLuaScript_h__
#define __PPLuaScript_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
//#include "PPScriptFlag.h"
#include "PPGameTexture.h"
#include <string>

#ifdef __LUAJIT__
#include <lua.hpp>
#else
extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
}
#endif

#define USERMETA
#define PPGAMEINSTNACE "__ppgame_ins__"

#define LUA_GETINT(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	lua_pushinteger(L,value);\
	return 1;\
}

#define LUA_SETINT(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	value = atoi(s->args(0));\
	return 0;\
}

#define LUA_GETBOOL(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	lua_pushboolean(L,value);\
	return 1;\
}

#define LUA_SETBOOL(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	value = atoi(s->args(0));\
	return 0;\
}

#define LUA_GETNUMBER(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	lua_pushnumber(L,value);\
	return 1;\
}

#define LUA_SETNUMBER(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	value = atof(s->args(0));\
	return 0;\
}

#define LUA_GETSTRING(name,class,value) \
static int name(lua_State* L)\
{\
	PPLuaScript* s = PPLuaScript::sharedScript(L);\
	class* m = (class*)s->userdata;\
	lua_pushstring(L,value);\
	return 1;\
}

//#define PPLuaScript_sharedScript(L) (PPLuaScript::SharedScript(this->world(),L))

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaArg {
	enum {
		MAX_ARG = 16
	};

public:
	PPLuaArg(PPWorld* world) : argCount(0),errorMessage("")/*,flags(NULL)*/,errorLine(-1),timeoutCount(0) {
		target=world;
	}

	PPWorld* world() {
		return target;
	}

	static PPWorld* World(lua_State* L);

	static void* UserData(lua_State* L);
	
	static PPLuaArg* ErrorTarget(lua_State* L) {
		return (PPLuaArg*)World(L)->userdata;
	}
	
//	static PPLuaScript* sharedScript(lua_State* L) {
//		script->userdata = NULL;
//		if (L) script->callScriptFunc(L);
//		return script;
//	}
//	
//	static PPLuaScript* sharedScript() {
//		script->userdata = NULL;
//		return script;
//	}

	static void* DeleteObject(lua_State* L) {
		lua_getmetatable(L,-1);
		lua_pushstring(L,PPGAMEINSTNACE);
		lua_rawget(L,-2);
		return lua_touserdata(L,-1);
	}

	static void newObject(lua_State* L,const char* class_name,void* userdata,lua_CFunction gc) {
		lua_createtable(L,0,1);
		lua_createtable(L,0,2);
		if (gc) {
			lua_pushcfunction(L,gc);
			lua_setfield(L,-2,"__gc");
		}
		lua_getglobal(L,class_name);
		if (!lua_isnil(L,-1)) {
			lua_setfield(L,-2,"__index");
		}
		lua_pushlightuserdata(L,userdata);
		lua_setfield(L,-2,PPGAMEINSTNACE);
		lua_pushstring(L,class_name);
		lua_setfield(L,-2,class_name);
		lua_setmetatable(L,-2);
	}
	
	PPPoint getPoint(lua_State* L,int index,float x=0,float y=0) {
		int n=index;
		if (isTable(L,n)) {
			return PPPoint(tableNumber(L,n,1,"x",x),tableNumber(L,n,2,"y",y));
		}
		return PPPoint(number(n+0),number(n+1));
	}

	PPSize getSize(lua_State* L,int index,float width=0,float height=0) {
		int n=index;
		if (isTable(L,n)) {
			return PPSize(tableNumber(L,n,1,"width",width),tableNumber(L,n,2,"height",height));
		}
		return PPSize(number(n+0,width),number(n+1,height));
	}
	
	PPRect getRect(lua_State* L,int index,PPRect rectZero=PPRectZero) {
		int n=index;
		if (isTable(L,n)) {
			return PPRect(tableNumber(L,n,1,"x",rectZero.x),tableNumber(L,n,2,"y",rectZero.y),tableNumber(L,n,3,"width",rectZero.width),tableNumber(L,n,4,"height",rectZero.height));
		}
		return PPRect(number(n+0),number(n+1),number(n+2),number(n+3));
	}
	
	PPColor getColor(lua_State* L,int index,PPColor def=PPColor(0,0,0,255)) {
		int n=index;
		if (isTable(L,n)) {
			return PPColor(tableInteger(L,n,1,"r",def.r),tableInteger(L,n,2,"g",def.g),tableInteger(L,n,3,"b",def.b),tableInteger(L,n,4,"a",def.a));
		}
		return PPColor(integer(n+0,def.r),integer(n+1,def.g),integer(n+2,def.b),integer(n+3,def.a));
	}

	virtual lua_Number number(int index,lua_Number def=0) {
		if (index < argCount) {
			return _number[index];
		}
		return def;
	}
	virtual lua_Integer integer(int index,lua_Integer def=0) {
		if (index < argCount) {
			return _integer[index];
		}
		return def;
	}
	virtual bool boolean(int index,bool def=false) {
		if (index < argCount) {
			return _boolean[index];
		}
		return def;
	}

	bool isTable(lua_State* L,int argn);
	bool isString(lua_State* L,int argn);
	bool isBoolean(lua_State* L,int argn);
	bool isNumber(lua_State* L,int argn);
	bool tableMember(lua_State* L,int argn,const char* field);
	bool tableBoolean(lua_State* L,int argn,const char* field,bool def=0);
	lua_Number tableNumber(lua_State* L,int argn,const char* field,lua_Number def=0);
	lua_Number tableNumber(lua_State* L,int argn,int index,const char* field,lua_Number def=0);
	lua_Integer tableInteger(lua_State* L,int argn,const char* field,lua_Integer def=0);
	lua_Integer tableInteger(lua_State* L,int argn,int index,const char* field,lua_Integer def=0);
	const char * tableString(lua_State* L,int argn,const char* field,const char * def="");
	PPColor tableColor(lua_State* L,int argn,const char* field,PPColor def=PPColor::white());

	lua_Number tableTableNumber(lua_State* L,int argn,const char* field,const char* field2,lua_Number def=0);

	lua_Number tableFieldNumber(lua_State* L,int argindex,const char* table,const char* field,int index,lua_Number def=0)
	{
		int top=lua_gettop(L);
		lua_getfield(L,argindex+2,table);
		if (lua_istable(L,-1)) {
			lua_getfield(L, -1, field);
			if (!lua_isnil(L,-1)) {
				def = luaL_optnumber(L, -1, def);
			} else {
				lua_rawgeti(L,-2,index);
				if (!lua_isnil(L,-1)) {
					def = luaL_optnumber(L, -1, def);
				}
			}
		}
		lua_settop(L,top);
		return def;
	}

	void* init(lua_State* L);
	void* initarg(lua_State* L);

	void resetWord();
	void addWord(const char* str);

	int argCount;

	void* userdata;
	void* userData(lua_State* L) {
		if (userdata) return userdata;
		if (lua_type(L,1) == LUA_TTABLE) {
			lua_getmetatable(L,1);
			lua_getfield(L,-1,PPGAMEINSTNACE);
			userdata = lua_touserdata(L,-1);
		}
		return userdata;
	}

	std::string _str[MAX_ARG];

	virtual int returnPoint(lua_State* L,PPPoint p) {
		lua_createtable(L,0,2);
		lua_pushnumber(L,p.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,p.y);
		lua_setfield(L,-2,"y");
		lua_getglobal(L,"pppoint_mt");
		lua_setmetatable(L,-2);
		return 1;
	}
	
	virtual void pushPoint(lua_State* L,PPPoint p) {
		returnPoint(L,p);
	}

	virtual int returnSize(lua_State* L,PPSize s) {
		lua_createtable(L,0,4);
		lua_pushnumber(L,0);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,0);
		lua_setfield(L,-2,"y");
		lua_pushnumber(L,s.width);
		lua_setfield(L,-2,"width");
		lua_pushnumber(L,s.height);
		lua_setfield(L,-2,"height");
		lua_pushboolean(L,0);
		lua_setfield(L,-2,"hit");
		lua_getglobal(L,"pprect_mt");
		lua_setmetatable(L,-2);
		return 1;
	}
	
	virtual void pushSize(lua_State* L,PPSize s) {
		returnSize(L,s);
	}

	virtual int returnRect(lua_State* L,PPRect r) {
		lua_createtable(L,0,5);
		lua_pushnumber(L,r.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,r.y);
		lua_setfield(L,-2,"y");
		lua_pushnumber(L,r.width);
		lua_setfield(L,-2,"width");
		lua_pushnumber(L,r.height);
		lua_setfield(L,-2,"height");
		lua_pushboolean(L,0);
		lua_setfield(L,-2,"hit");
		lua_getglobal(L,"pprect_mt");
		lua_setmetatable(L,-2);
		return 1;
	}

	virtual void pushRect(lua_State* L,PPRect r) {
		returnRect(L,r);
	}

	virtual int returnColor(lua_State* L,PPColor c) {
		lua_createtable(L, 0, 4);
		lua_pushnumber(L,c.r);
		lua_setfield(L, -2, "r");
		lua_pushnumber(L,c.g);
		lua_setfield(L, -2, "g");
		lua_pushnumber(L,c.b);
		lua_setfield(L, -2, "b");
		lua_pushnumber(L,c.a);
		lua_setfield(L, -2, "a");
		return 1;
	}

	virtual void pushColor(lua_State* L,PPColor c) {
		returnColor(L,c);
	}

	virtual const char* word(int line);
	virtual const char* args(int index);
	
	virtual PPGameTextureOption getTextureOption(lua_State* L,int index,PPGameTextureOption option);
	
	std::string errorMessage;
	int errorLine;
	std::string errorSrc;
	int timeoutCount;

	void resetTimeout();
	
private:
	lua_Number _number[MAX_ARG];
	lua_Integer _integer[MAX_ARG];
	int _boolean[MAX_ARG];
	PPWorld* target;
};

class PPLuaScript : public PPLuaArg {
public:
	enum {
		MAX_ARG = 16
	};
	
	PPLuaScript(PPWorld* world);
	virtual ~PPLuaScript();

	virtual void idle();
	
//	virtual void start();
//	virtual void startWithFlag();

	virtual bool addSearchPath(const char* path);

	virtual bool load(const char* scriptfile);
	virtual void dumpStack(lua_State* L);
	
	virtual void openModule(const char* name,void* userdata=NULL,lua_CFunction gc=NULL,const char* superclass=NULL);
	virtual void addNumberValue(const char* name,float value);
	virtual void addIntegerValue(const char* name,int value);
	virtual void addBoolValue(const char* name,bool value);
	virtual void addStringValue(const char* name,const char* str);
	virtual void addCommand(const char* name,lua_CFunction func);
	virtual void addMetaTable(const char* name,lua_CFunction func);
	virtual void addYieldCommand(const char* name,lua_CFunction func) {
		addCommand(name,func);
	}
	virtual void closeModule();
	
	virtual void makeObjectTable(PPObject** object,int num,const char* name) {
		char cmd[256];
		sprintf(cmd,"%s = {}\n",name);
		execString(cmd);
		for (int i=0;i<num;i++) {
			sprintf(cmd,"_%s%d",name,i+1);
			object[i]->openLibrary(this,cmd);
			sprintf(cmd,"%s[%d]=_%s%d\n",name,i+1,name,i+1);
			execString(cmd);
		}
		sprintf(cmd,"%s.count = %d\n",name,num);
		execString(cmd);
	}
	
	
	virtual int getErrorLine();
	
	virtual bool isFunction(const char* functon_name);
//	virtual void call(const char* functon_name);
	
	virtual void startProcess();
	virtual bool doProcess();
	virtual bool checkProcess();
	
//	int execFunction(const char* function_name);
	bool execString(const char* script);


//	void* callScriptFunc2(lua_State* L);

	void addLuaLoader(lua_CFunction func);

	lua_State* L;
	lua_State* coL;
	
	std::string* _module;

//	std::string _str[MAX_ARG];
//	lua_Number _number[MAX_ARG];
//	lua_Integer _integer[MAX_ARG];
//	int _boolean[MAX_ARG];
//	int argCount;
	
	bool alive;

//	PPScriptFlag* flags;
	
	void drawDisplayList(const char* ppgraph);	
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
