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

#define PPGAMEINSTNACE "__ppgame_ins__"

#define PPUserDataAssert(t) if (!(t)) {return luaL_error(L,lua_tostring(L,-1));}

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaArg {

public:

	PPLuaArg(PPWorld* world) : Lua(NULL),argCount(0),errorMessage(""),errorLine(-1),timeoutCount(0) {
		target=world;
	}

	PPWorld* world() {
		return target;
	}

	static PPWorld* World(lua_State* L);

	static void* UserData(lua_State* L,std::string &classname,bool nullcheck=true);
  static void* UserData(lua_State* L,int idx,std::string &classname,bool nullcheck=true);
	static void* UserData(lua_State* L,const char* classname,bool nullcheck=true);
  static void* UserData(lua_State* L,int idx,const char* classname,bool nullcheck=true);
	
	static PPLuaArg* ErrorTarget(lua_State* L) {
		return (PPLuaArg*)World(L)->userdata;
	}
	
	static void* DeleteObject(lua_State* L) {
#ifdef _OBJMEM_DEBUG_
printf("DeleteObject %d\n",lua_gettop(L));
fflush(stdout);
printf("%s\n",lua_typename(L,-1));
fflush(stdout);
#endif
#ifdef  __LUAJIT__
    void** obj=(void**)lua_topointer(L,-1);
    void* userdata = *obj;
#else
    int top=lua_gettop(L);
		lua_getmetatable(L,-1);
		lua_pushstring(L,PPGAMEINSTNACE);
		lua_rawget(L,-2);
		void* userdata = lua_touserdata(L,-1);
    lua_settop(L,top);
#endif
    return userdata;
	}

	static void newObject(lua_State* L,const char* class_name,void* userdata,lua_CFunction gc);
	static void setSetter(lua_State* L,int idx,lua_CFunction funcSetter);
	static void setGetter(lua_State* L,int idx,lua_CFunction funcGetter);
	
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
      return lua_tonumber(Lua,index+argShift);
		}
		return def;
	}
	virtual lua_Integer integer(int index,lua_Integer def=0) {
		if (index < argCount) {
      return lua_tointeger(Lua,index+argShift);
		}
		return def;
	}
	virtual bool boolean(int index,bool def=false) {
		if (index < argCount) {
      return lua_toboolean(Lua,index+argShift);
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

	void init(lua_State* L);
	void initarg(lua_State* L);

	int argCount;
  int argShift;

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
//		lua_pushboolean(L,0);
//		lua_setfield(L,-2,"hit");
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
//		lua_pushboolean(L,0);
//		lua_setfield(L,-2,"hit");
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

	virtual PPGameTextureOption getTextureOption(lua_State* L,int index,PPGameTextureOption option);
	
	std::string errorMessage;
	int errorLine;
	std::string errorPath;
	std::string errorName;
	std::string errorReason;
	int timeoutCount;

	void resetTimeout();
	
	virtual const char* word(int line);
	virtual const char* args(int index);

  static int setterReadOnlyError(lua_State* L,const char* name);

  static int getPPPoint(lua_State *L,int idx,PPPoint &p);
  static int getPPSize(lua_State *L,int idx,float &w,float &h);
  static int getPPRect(lua_State *L,int idx,PPRect &r);
  
private:
	PPWorld* target;
  lua_State* Lua;
};

class PPLuaScript : public PPLuaArg {
public:
	
	PPLuaScript(PPWorld* world);
	virtual ~PPLuaScript();

	virtual void idle();
	
	virtual bool addSearchPath(const char* path);

	virtual bool load(const char* scriptfile);
	virtual void dumpStack(lua_State* L);
	
	virtual void openModule(std::string name,void* userdata=NULL,lua_CFunction gc=NULL,const char* superclass=NULL);
	virtual void openModule(const char* name,void* userdata=NULL,lua_CFunction gc=NULL,const char* superclass=NULL);
	virtual void addNumberValue(const char* name,float value);
	virtual void addIntegerValue(const char* name,int value);
	virtual void addBoolValue(const char* name,bool value);
	virtual void addStringValue(const char* name,const char* str);
	virtual void addCommand(const char* name,lua_CFunction func);
	virtual void addMetaTable(const char* name,lua_CFunction func);
  virtual void addAccessor(lua_CFunction getter,lua_CFunction setter);
  virtual void makeReadOnlyMetatable();
  virtual void setupGeometryCommand();
	virtual void addYieldCommand(const char* name,lua_CFunction func) {
		addCommand(name,func);
	}
	virtual void closeModule();
	virtual void makeObjectTable(PPObject** object,int num,const char* valuename,const char* classname);
	
	virtual int getErrorLine();
	
	virtual bool isFunction(const char* functon_name);
	
	virtual void startProcess();
	virtual bool doProcess();
	virtual bool checkProcess();
	
	bool execString(const char* script);
	void addLuaLoader(lua_CFunction func);

	lua_State* L;
	lua_State* coL;
	
	std::string* _module;

	bool alive;

	void drawDisplayList(const char* ppgraph);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
