/*-----------------------------------------------------------------------------------------------
	名前	PPLuaScript.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPLuaScript.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <FlMML/FlMML_Regex.h>

#if TARGET_OS_MAC
#include <regex.h>
#else
#include "regex.h"
#endif

#ifdef __COCOS2DX__
#include "Cocos2dxLuaLoader.h"
#include "Cocos2dxWrapper.h"
#endif

#ifdef __LUAJIT__
#include <lua.hpp>
#define LUA_OK 0
#else
extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lua/ldebug.h>
}
#endif

#define PICO_GETTER "__getter"
#define PICO_SETTER "__setter"
#define PICO_SUPER "__super"
#define PICO_CLASSNAME "__classname"

static int error_handler(lua_State* L) {
  //int top=lua_gettop(L);
  //printf("top = %d\n",top);

#if 1   //lua_getinfoではうまく取れないときがあるので、エラーメッセージから取り出す方式に変更
  std::string err = lua_tostring(L,-1);

  std::string err_path = "";
  std::string err_name = "";
  std::string err_line = "-1";
  std::string err_reason = "";
  
  {
    FlMML::regex exp("^(.*):([[:digit:]]+):[[:space:]]+(.*)$");
    std::string::const_iterator start=err.begin();
    std::string::const_iterator end=err.end();
    FlMML::smatch result;
    while (FlMML::regex_search(start, end, result, exp,FlMML::regex_constants::match_not_dot_newline)) {
      err_path = result.str(1);
      err_line = result.str(2);
      err_reason = result.str(3);
      start = result[0].second;
    }
  }
  
  {
    FlMML::regex exp("((.*)/)*(.*)");
    std::string::const_iterator start=err_path.begin();
    std::string::const_iterator end=err_path.end();
    FlMML::smatch result;
    while (FlMML::regex_search(start, end, result, exp,FlMML::regex_constants::match_not_dot_newline)) {
      err_name = result.str(3);
      break;
    }
  }

  PPLuaArg* script = PPLuaArg::ErrorTarget(L);
  script->errorLine = atoi(err_line.c_str());
  script->errorMessage = err;
  script->errorPath = err_path;
  script->errorName = err_name;
  script->errorReason = err_reason;
  lua_pushstring(L,err.c_str());
#else
  std::string err;

  lua_Debug d;
  int level = 0;
  while (lua_getstack(L, level, &d)) {
    lua_getinfo(L, "Sln", &d);
    printf("##\nerorr level %d what %s line %s:%d\n##\n",level,d.what,d.short_src,d.currentline);
    if (d.what[0] != 'C') {
      PPLuaArg* script = PPLuaArg::ErrorTarget(L);
      script->errorLine = d.currentline;
      script->errorMessage = err;
      script->errorPath = d.short_src;
      lua_pushstring(L,err.c_str());
      break;
    }
    ++level;
  }
#endif

  return 1;
}

static void dumpStack(lua_State* L)
{
	int i;
	//スタックに積まれている数を取得する
	int stackSize = lua_gettop(L);
	for( i = stackSize; i >= 1; i-- ) {
			int type = lua_type(L, i);
			printf("Stack[%2d-%10s] : ", i, lua_typename(L,type) );
			
			switch( type ) {
			case LUA_TNUMBER:
					//number型
					printf("%f", lua_tonumber(L, i) );
					break;
			case LUA_TBOOLEAN:
					//boolean型
					if( lua_toboolean(L, i) ) {
							printf("true");
					}else{
							printf("false");
					}
					break;
			case LUA_TSTRING:
					//string型
					printf("%s", lua_tostring(L, i) );
					break;
			case LUA_TNIL:
					//nil
					break;
			default:
					//その他の型
					printf("%s", lua_typename(L, type));
					break;
			}
			printf("\n");
	}
	printf("\n");
}

//static int setter_error_handler(lua_State* L) {
//printf("## setter_error_handler\n");
//  return error_handler(L);
//}

static bool callsetter(lua_State* L)
{
  int top=lua_gettop(L);
  int s=top-3;
  int k=top-2;
  int v=top-1;
  int n=top;
  do {
    if (lua_isnil(L,n)) break;
    
//    lua_pushvalue(L,k);
//    lua_rawget(L,n);
//    if (lua_isnil(L,-1)) break;
    
    lua_getmetatable(L,n);
    if (lua_isnil(L,-1)) break;
    if (!lua_istable(L,-1)) break;

//    lua_pushstring(L, PICO_CLASSNAME);
//    lua_rawget(L,-2);
//    printf("callsetter class %s\n",lua_tostring(L, -1));
//    lua_pop(L,1);
    
//    printf(" -- %s\n",lua_typename(L,-1));
    
    lua_pushstring(L, PICO_SETTER);
    lua_rawget(L,-2);
    if (!lua_isnil(L,-1)) {
      lua_pushvalue(L,s);
      lua_pushvalue(L,k);
      lua_pushvalue(L,v);
      if (lua_pcall(L, 3, 1, 0)!=0) {
        return false;
      }
      if (lua_toboolean(L,-1)) {
        return true;
      }
    }
    lua_pop(L,1);

    lua_pushstring(L, PICO_SUPER);
    lua_rawget(L,-2);
    if (!lua_isnil(L,-1)) {
      lua_pushvalue(L,s);
      lua_pushvalue(L,k);
      lua_pushvalue(L,v);
      lua_pushvalue(L,-4);
      return callsetter(L);
    }
    lua_pop(L,1);
  } while (false);
  
  lua_pushboolean(L,false);
  return true;
}

static int funcSetter(lua_State* L)
{
  lua_pushvalue(L,1);
  if (!callsetter(L)) {
    return luaL_error(L,lua_tostring(L,-1));
  }
  if (lua_toboolean(L,-1)) {
    return 0;
  }
  lua_pushvalue(L,2);
  lua_pushvalue(L,3);
  lua_rawset(L,1);
  return 0;
}

static void callgetter(lua_State* L)
{
  int top=lua_gettop(L);
  int s=top-2;
  int k=top-1;
  int n=top;
  do {
    if (lua_isnil(L,n)) break;

//printf("%s\n",lua_tostring(L,k));
//printf("%s\n",lua_typename(L,1));
   
    lua_pushvalue(L,k);
    lua_rawget(L,n);
    if (!lua_isnil(L,-1)) break;
    
    lua_getmetatable(L,n);
    if (lua_isnil(L,-1)) break;
    if (!lua_istable(L,-1)) break;

//    lua_pushstring(L, PICO_CLASSNAME);
//    lua_rawget(L,-2);
//    printf("callgetter class %s\n",lua_tostring(L, -1));
//    lua_pop(L,1);
    
    lua_pushstring(L, PICO_GETTER);
    lua_rawget(L,-2);
    if (!lua_isnil(L,-1)) {
      lua_pushvalue(L,s);
      lua_pushvalue(L,k);
      if (lua_pcall(L, 2, 1, 0)!=0) {
      }
      if (!lua_isnil(L,-1)) {
        break;
      }
    }
    lua_pop(L,1);

    lua_pushstring(L, PICO_SUPER);
    lua_rawget(L,-2);
    if (!lua_isnil(L,-1)) {
      lua_pushvalue(L,s);
      lua_pushvalue(L,k);
      lua_pushvalue(L,-3);
      callgetter(L);
    }
  } while (false);
  
  return;
}

static int funcGetter(lua_State* L)
{
  lua_pushvalue(L,1);
  callgetter(L);
  return 1;
}

void PPLuaArg::init(lua_State* L)
{
  Lua = L;
  int top=lua_gettop(L);
  argCount = top;
  argShift = 1;
	if (lua_type(L,1) == LUA_TTABLE) {
		lua_getmetatable(L,1);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		if (lua_touserdata(L,-1)) {
			argShift=2;
      argCount -= (argShift-1);
		}
	}
  lua_settop(L,top);
}

void PPLuaArg::initarg(lua_State* L)
{
  Lua = L;
  argCount = lua_gettop(L);
  argShift = 1;
}

bool PPLuaArg::isTable(lua_State* L,int argn)
{
	argn += 2;
	return lua_istable(L,argn);
}

bool PPLuaArg::isString(lua_State* L,int argn)
{
	argn += 2;
	return lua_isstring(L,argn);
}

bool PPLuaArg::isBoolean(lua_State* L,int argn)
{
	argn += 2;
	return lua_isboolean(L,argn);
}

bool PPLuaArg::isNumber(lua_State* L,int argn)
{
	argn += 2;
	return lua_isnumber(L,argn);
}

bool PPLuaArg::tableMember(lua_State* L,int argn,const char* field)
{
	bool r=false;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		r = (lua_isnil(L, -1)!=true);
		lua_pop(L,1);
	}
	return r;	
}

lua_Number PPLuaArg::tableTableNumber(lua_State* L,int argn,const char* field,const char* field2,lua_Number def)
{
	lua_Number r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		if (lua_istable(L,-1)) {
			lua_getfield(L, -1, field2);
			r = luaL_optnumber(L, -1, def);
		}
		lua_pop(L,1);
	}
	return r;
}

bool PPLuaArg::tableBoolean(lua_State* L,int argn,const char* field,bool def)
{
	bool r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		if (lua_isboolean(L, -1)) {
			r = lua_toboolean(L,-1);
		}
		lua_pop(L,1);
	}
	return r;
}

lua_Number PPLuaArg::tableNumber(lua_State* L,int argn,const char* field,lua_Number def)
{
	lua_Number r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		r = luaL_optnumber(L, -1, def);
		lua_pop(L,1);
	}
	return r;
}

lua_Number PPLuaArg::tableNumber(lua_State* L,int argn,int index,const char* field,lua_Number def)
{
	lua_Number r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L,argn,field);
		if (lua_isnil(L,-1)) {
			lua_pop(L,1);
			lua_rawgeti(L,argn,index);
		}
		r = luaL_optnumber(L,-1,def);
		lua_pop(L,1);
	}
	return r;
}

lua_Integer PPLuaArg::tableInteger(lua_State* L,int argn,const char* field,lua_Integer def)
{
	lua_Integer r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		r = luaL_optint(L, -1, def);
		lua_pop(L,1);
	}
	return r;
}

lua_Integer PPLuaArg::tableInteger(lua_State* L,int argn,int index,const char* field,lua_Integer def)
{
	lua_Integer r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		if (lua_isnil(L,-1)) {
			lua_pop(L,1);
			lua_rawgeti(L,argn,index);
		}
		r = luaL_optint(L, -1, def);
		lua_pop(L,1);
	}
	return r;
}

const char * PPLuaArg::tableString(lua_State* L,int argn,const char* field,const char *def)
{
	const char * r=def;
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		r = luaL_optstring(L, -1, "");
		lua_pop(L,1);
	}
	return r;
}

PPColor PPLuaArg::tableColor(lua_State* L,int argn,const char* field,PPColor def)
{
	argn += 2;
	if (lua_istable(L,argn)) {
		lua_getfield(L, argn, field);
		if (lua_istable(L,-1)) {
      def = getColor(L,-1-2,def);
    }
		lua_pop(L,1);
  }
	return def;
}

void PPLuaScript::openModule(std::string name,void* userdata,lua_CFunction gc,const char* superclass)
{
  openModule(name.c_str(),userdata,gc,superclass);
}

void PPLuaScript::openModule(const char* name,void* userdata,lua_CFunction gc,const char* superclass)
{
	if (_module) delete _module;
	int top = lua_gettop(L);
	lua_getglobal(L,name);
	if (lua_isnil(L,-1)) {
		lua_settop(L,top);
		lua_createtable(L,0,0);
		lua_setglobal(L,name);
		lua_getglobal(L,name);
	}
	int methods = lua_gettop(L);

	luaL_newmetatable(L,name);
	int metatable = lua_gettop(L);
  if (userdata) {
    lua_pushlightuserdata(L,userdata);
    lua_setfield(L,metatable,PPGAMEINSTNACE);
  }
	if (superclass) {
		lua_getglobal(L,superclass);
		lua_setfield(L,metatable,PICO_SUPER);
	}
  lua_pushcfunction(L,funcGetter);
  lua_setfield(L,metatable,"__index");
  lua_pushcfunction(L,funcSetter);
  lua_setfield(L,metatable,"__newindex");
  lua_pushstring(L,name);
  lua_setfield(L,metatable,PICO_CLASSNAME);
	lua_setmetatable(L,methods);

	lua_settop(L,top);
	_module = new std::string(name);

//  printf(" ---- %s <= %s\n",_module->c_str(),superclass);
}

void PPLuaScript::addAccessor(lua_CFunction setter,lua_CFunction getter)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
  } else {
		lua_getglobal(L,_module->c_str());
    if (getter) {
      PPLuaScript::setGetter(L,-1,getter);
    }
    if (setter) {
      PPLuaScript::setSetter(L,-1,setter);
    }
  }
  lua_settop(L,top);
}

void PPLuaScript::makeReadOnlyMetatable()
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
  } else {
		lua_getglobal(L,_module->c_str());
    lua_getmetatable(L,-1);
    lua_createtable(L, 0, 0);
    lua_setfield(L,-2,"__metatable");
  }
  lua_settop(L,top);
}

void PPLuaScript::addIntegerValue(const char* name,int value)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushinteger(L, value);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		lua_pushstring(L,name);
		lua_pushinteger(L,value);
		lua_rawset(L,-3);
	}
  lua_settop(L,top);
}

void PPLuaScript::addNumberValue(const char* name,float value)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushinteger(L, value);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		lua_pushstring(L,name);
		lua_pushnumber(L,value);
		lua_rawset(L,-3);
	}
  lua_settop(L,top);
}

void PPLuaScript::addBoolValue(const char* name,bool value)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushboolean(L,value);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		lua_pushstring(L,name);
		lua_pushboolean(L,value);
		lua_rawset(L,-3);
	}
  lua_settop(L,top);
}

void PPLuaScript::addStringValue(const char* name,const char* str)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushstring(L,str);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		lua_pushstring(L,name);
		lua_pushstring(L,str);
		lua_rawset(L,-3);
	}
  lua_settop(L,top);
}

void PPLuaScript::addMetaTable(const char* name,lua_CFunction func)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
	} else {
		lua_getglobal(L,_module->c_str());
		lua_createtable(L,0,0);
		lua_pushcfunction(L,func);
		lua_setfield(L,-2,name);
		lua_setmetatable(L,-2);
	}
  lua_settop(L,top);
}

void PPLuaScript::addCommand(const char* name,lua_CFunction func)
{
  int top=lua_gettop(L);
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushcfunction(L,func);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		lua_pushstring(L,name);
		lua_pushcfunction(L,func);
		lua_rawset(L,-3);
	}
  lua_settop(L,top);
}

void PPLuaScript::closeModule()
{
  makeReadOnlyMetatable();
	if (_module) delete _module;
	_module = new std::string("");
}

const char* PPLuaArg::word(int line)
{
	return "";
}

const char* PPLuaArg::args(int index)
{
	if (index < argCount) {
    const char* s=lua_tostring(Lua,index+argShift);
    if (s) return s;
	}
	return "";
}

PPGameTextureOption PPLuaArg::getTextureOption(lua_State* L,int index,PPGameTextureOption option)
{
	int n=index;
	if (isTable(L,n)) {
		option.linear = tableBoolean(L,n,"linear",option.linear);
		option.wrap_s = PPGameTextureOption::wrap(tableString(L,n,"wrap_s",PPGameTextureOption::wrap(option.wrap_s)));
		option.wrap_t = PPGameTextureOption::wrap(tableString(L,n,"wrap_t",PPGameTextureOption::wrap(option.wrap_t)));
	}
	return option;
}

static lua_Number startclock;

extern "C" {
static void myCountHook(lua_State* L,lua_Debug *ar);

static lua_Number os_clock()
{
	return ((lua_Number)clock())/((lua_Number)CLOCKS_PER_SEC);
}

static void myCountHook(lua_State* L, lua_Debug *ar)
{
	PPLuaArg* target = PPLuaArg::ErrorTarget(L);
	target->timeoutCount ++;
	if (target->timeoutCount > 100000) {
		target->timeoutCount = 0;
		if (os_clock() > startclock + 3) {
			startclock = os_clock();
			std::string s = "timeout error";
			if (target) target->errorMessage = s;
			luaL_error(L,s.c_str());
		}
	}
}
};

int PPLuaScript::getErrorLine()
{
	lua_Debug ar;
	lua_getstack(L,1,&ar);
	lua_getinfo(L,"nSl",&ar);
	return ar.currentline;
}

void PPLuaArg::resetTimeout()
{
	startclock = os_clock();
	timeoutCount = 0;
}

void PPLuaArg::setSetter(lua_State* L,int idx,lua_CFunction funcSetter) {
  int top=lua_gettop(L);
  lua_getmetatable(L,idx);
  lua_pushstring(L,PICO_SETTER);
  lua_pushcfunction(L,funcSetter);
  lua_rawset(L,-3);
  lua_settop(L,top);
}

void PPLuaArg::setGetter(lua_State* L,int idx,lua_CFunction funcGetter) {
  int top=lua_gettop(L);
  lua_getmetatable(L,idx);
  lua_pushstring(L,PICO_GETTER);
  lua_pushcfunction(L,funcGetter);
  lua_rawset(L,-3);
  lua_settop(L,top);
}

int PPLuaArg::setterReadOnlyError(lua_State* L,const char* name)
{
  return luaL_error(L,"%s is read only.",name);
}

void PPLuaArg::newObject(lua_State* L,const char* class_name,void* userdata,lua_CFunction gc)
{
  int top=lua_gettop(L);
  bool err=false;
  
  lua_getglobal(L,class_name);
  if (!lua_istable(L,-1)) {
    err=true;
  } else {
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, PICO_CLASSNAME);
    if (lua_isnoneornil(L,-1)) {
      err=true;
    }
  }
  
  if (err) {
    std::string name = "invaild class name '";
    name += class_name;
    name += "'";
    luaL_error(L,name.c_str());
  }
  
  lua_settop(L,top);


  lua_createtable(L,0,1);

  lua_createtable(L,0,2);
  lua_pushlightuserdata(L,userdata);
  lua_setfield(L,-2,PPGAMEINSTNACE);

#ifdef  __LUAJIT__
  void** ptr = (void**)lua_newuserdata(L,sizeof(userdata));
  *ptr = userdata;
  lua_createtable(L,0,2);
  if (gc) {
    lua_pushcfunction(L,gc);
    lua_setfield(L,-2,"__gc");
  }
  lua_setmetatable(L,-2);
  lua_setfield(L,-2,"__systemdata__");
#else
  if (gc) {
    lua_pushcfunction(L,gc);
    lua_setfield(L,-2,"__gc");
  }
#endif

  lua_getglobal(L,class_name);
#if 0   //setter & getter test
  lua_setfield(L,-2,"__index");
#else
  lua_setfield(L,-2,PICO_SUPER);
  lua_pushcfunction(L,funcGetter);
  lua_setfield(L,-2,"__index");
  lua_pushcfunction(L,funcSetter);
  lua_setfield(L,-2,"__newindex");
  lua_pushstring(L,class_name);
  lua_setfield(L,-2,PICO_CLASSNAME);
  lua_createtable(L,0,0);
  lua_setfield(L,-2,"__metatable");
#endif

  lua_setmetatable(L,-2);

}

#pragma mark -

PPLuaScript::PPLuaScript(PPWorld* world) : PPLuaArg(world),_module(NULL)
{
	resetTimeout();
	L = luaL_newstate();
  lua_sethook(L,&myCountHook,LUA_MASKCOUNT,1000);
	coL = NULL;
#ifdef __LUAJIT__
  lua_gc(L, LUA_GCSTOP, 0);
  luaL_openlibs(L);
  lua_gc(L, LUA_GCRESTART, -1);
  luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
#else
	luaL_openlibs(L);
#endif
#ifdef __COCOS2DX__
	addLuaLoader(loader_Android);
#endif
	_module = new std::string("");
	world->userdata = this;
	int top=lua_gettop(L);
	lua_pushlightuserdata(L,world);
	lua_setglobal(L,"__PPWorld");
	lua_settop(L,top);
}

PPLuaScript::~PPLuaScript()
{
	lua_close(L);
	if (_module) delete _module;
}

bool PPLuaScript::load(const char* scriptfile)
{
	bool r = true;
	errorMessage = "";
#ifdef _ANDROID
	unsigned long size;
	char *pFileContent = (char*)ccGetFileData(scriptfile,"r",&size);
	if (pFileContent) {
    char *pCodes = new char[size + 1];
    pCodes[size] = '\0';
    memcpy(pCodes, pFileContent, size);
    delete[] pFileContent;
    r = execString(pCodes);
    delete []pCodes;
	}
#else
  lua_pushcfunction(L,error_handler);
  int nRet;
  nRet = luaL_loadfile(L,scriptfile);
	if (nRet != 0) {
    error_handler(L);
    printf("%s\n",errorMessage.c_str());
    lua_pop(L, 1);
    r = false;
  } else {
    nRet = lua_pcall(L,0,LUA_MULTRET,-2);
    //lua_gc(L, LUA_GCCOLLECT, 0);
    if (nRet != 0)
    {
      printf("%s\n",errorMessage.c_str());
      lua_pop(L, 1);
      r = false;
    }
  }
#endif
	alive = false;
	resetTimeout();
	return r;
}

void PPLuaScript::dumpStack(lua_State* L)
{
	::dumpStack(L);
}

void PPLuaScript::startProcess()
{
	coL = NULL;
	int top=lua_gettop(L);
	lua_getglobal(L,"___scriptIdleFunc");
	if (lua_isfunction(L,-1)) {
		lua_settop(L,top);
		coL = lua_newthread(L);
		lua_getglobal(L,"___scriptIdleFunc");
		luaL_checktype(L,-1, LUA_TFUNCTION);
		lua_xmove(L, coL, 1);       //スタックの値をLからcoLに1つプッシュ
    lua_setglobal(L,"___scriptCorutine");
	}
	lua_settop(L,top);
	alive = checkProcess();
}

bool PPLuaScript::doProcess()
{
	if (checkProcess()) {
		int top=lua_gettop(L);
		if (coL) {
#ifdef __LUAJIT__
			int status = lua_resume(coL,0);
#else
			int status = lua_resume(coL,L,0);
#endif
			if (status == LUA_OK || status == LUA_YIELD) {
			} else {
				lua_xmove(coL, L, 1);    //スタックの値をcoLからLに1つプッシュ
        error_handler(coL);
			}
		}
		lua_settop(L,top);
		return true;
	} else {
		return false;
	}
}

bool PPLuaScript::checkProcess()
{
	bool ret=false;
	int top=lua_gettop(L);
//	lua_getglobal(L,"___scriptCoroutine");
	lua_State *co = coL;//lua_tothread(L,-1);
	if (co) {
		ret=true;
		switch (lua_status(co)) {
		case LUA_YIELD:
			break;
		case LUA_OK:
			{
				lua_Debug d;
				if (lua_getstack(co, 0, &d) > 0) {
				} else if (lua_gettop(co) == 0) {
					ret = false;
				} else {
				}
			}
			break;
		default:
			ret = false;
			break;
		}
	} else {
		ret = false;
	}
	lua_settop(L,top);
	return ret;
}

bool PPLuaScript::execString(const char* script)
{
  bool r=true;
  lua_pushcfunction(L,error_handler);
  int nRet;
  nRet = luaL_loadstring(L,script);
	if (nRet != 0) {
    error_handler(L);
    printf("%s\n",errorMessage.c_str());
    lua_pop(L, 1);
    r = false;
  } else {
    nRet = lua_pcall(L,0,LUA_MULTRET,-2);
    //lua_gc(L, LUA_GCCOLLECT, 0);
    if (nRet != 0)
    {
      printf("%s\n",errorMessage.c_str());
      lua_pop(L, 1);
      r = false;
    }
  }
  return r;
}

bool PPLuaScript::isFunction(const char* function_name)
{
	bool r = false;
	// get the function from lua
	lua_getglobal(L, function_name);
	// is it a function
	if (lua_isfunction(L,-1)) {
		r = true;
	}
	lua_settop( L, 0 );
	return r;
}

bool PPLuaScript::addSearchPath(const char* path)
{
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
    const char* cur_path =  lua_tostring( L, -1 ); // grab path string from top of stack
    lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
    lua_pushfstring(L, "%s;%s/?.lua", cur_path, path);
    lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
    lua_pop( L, 1 ); // get rid of package table from top of stack
    return 0; // all done!
}

void PPLuaScript::addLuaLoader(lua_CFunction func)
{
#ifdef __COCOS2DX__
	if (! func)
	{
		return;
	}
                                                           // stack content after the invoking of the function
	// get loader table
	lua_getglobal(L, "package");                     // package
	lua_getfield(L, -1, "loaders");                  // package, loaders

	// insert loader into index 2
	lua_pushcfunction(L, func);                      // package, loaders, func
	for (int i = lua_objlen(L, -2) + 1; i > 2; --i)
	{
		lua_rawgeti(L, -2, i - 1);                   // package, loaders, func, function
		// we call lua_rawgeti, so the loader table now is at -3
		lua_rawseti(L, -3, i);                       // package, loaders, func
	}
	lua_rawseti(L, -2, 2);                           // package, loaders

	// set loaders into package
	lua_setfield(L, -2, "loaders");                  // package

	lua_pop(L, 1);
#endif
}

void PPLuaScript::drawDisplayList(const char* ppgraph)
{
	int top=lua_gettop(L);
	lua_getglobal(L,ppgraph);
	if (lua_istable(L,-1)) {
		lua_getfield(L,-1,"top");
		if (lua_istable(L,-1)) {
			int t=lua_gettop(L);
			while (true) {
				lua_getfield(L,-1,"draw");
				if (lua_isfunction(L,-1)) {
					lua_pushvalue(L,-2);
					lua_pcall(L,1,0,0);
				}
				lua_settop(L,t);
				
				lua_getfield(L,-1,"next");
				if (lua_istable(L,-1)) {
					lua_replace(L,t);
				} else {
					break;
				}
				lua_settop(L,t);
			}
		}
	}
	lua_settop(L,top);
}

PPWorld* PPLuaArg::World(lua_State* L)
{
	int top=lua_gettop(L);
	lua_getglobal(L,"__PPWorld");
	PPWorld* world = (PPWorld*)lua_touserdata(L,-1);
	lua_settop(L,top);
	return world;
}

static bool isKindOfClass(lua_State* L,const char* classname)
{
	int top = lua_gettop(L);
  int classtop = top;
  bool collectclass=false;
  if (!lua_isnil(L,classtop)) {
    do {
      lua_getfield(L,classtop,PICO_CLASSNAME);
      if (lua_isnil(L,-1)) {
        break;
      }
      std::string name = lua_tostring(L,-1);
      if (name == classname) {
        collectclass = true;
        break;
      }
      lua_getfield(L,classtop,PICO_SUPER);
      if (lua_isnil(L,-1)) {
        break;
      }
      lua_getmetatable(L,-1);
      if (lua_isnil(L,-1)) {
        break;
      }
      classtop = lua_gettop(L);
    } while (true);
  }
  lua_settop(L,top);
  return collectclass;
}

void* PPLuaArg::UserData(lua_State* L,const char* classname,bool nullcheck)
{
  return UserData(L,1,classname,nullcheck);
}

void* PPLuaArg::UserData(lua_State* L,int idx,const char* classname,bool nullcheck)
{
  std::string err;
	int top = lua_gettop(L);
	void* userdata = NULL;
#ifdef __LUAJIT__
	if (lua_type(L,idx) == LUA_TUSERDATA) {
    void** obj=(void**)lua_topointer(L,idx);
    void* userdata = *obj;
    return userdata;
  }
#endif
	if (lua_type(L,idx) == LUA_TTABLE) {
		lua_getmetatable(L,idx);
    if (!isKindOfClass(L,classname)) {
      lua_settop(L,top);
      if (nullcheck) {
        lua_Debug ar;
        lua_getstack(L,0,&ar);
        lua_getinfo(L, "Snl", &ar);
        err = "illegal instance method '";
        err += ar.name;
        err += "' call.";
        lua_pushstring(L,err.c_str());
      }
      return NULL;
    }
		lua_getfield(L,-1,PPGAMEINSTNACE);
		userdata = lua_touserdata(L,-1);
	}
	lua_settop(L,top);

  if (nullcheck) {
    if (userdata == NULL) {
      lua_Debug ar;
      lua_getstack(L,0,&ar);
      lua_getinfo(L, "Snl", &ar);
      std::string err;
      if (ar.name) {
        err = "illegal instance method '";
        err += ar.name;
        err += "' call.";
      } else {
        err = "illegal instance method call.";
      }
      lua_pushstring(L,err.c_str());
    }
  }
	return userdata;
}

void* PPLuaArg::UserData(lua_State* L,std::string &classname,bool nullcheck)
{
  return UserData(L,1,classname,nullcheck);
}

void* PPLuaArg::UserData(lua_State* L,int idx,std::string &classname,bool nullcheck)
{
  return UserData(L,idx,classname.c_str(),nullcheck);
}

void PPLuaScript::idle()
{
	if (alive) {
		alive = doProcess();
	}
	alive = checkProcess();
}

void PPLuaScript::makeObjectTable(PPObject** object,int num,const char* valuename,const char* classname)
{
  lua_createtable(L, num, 0);
  for (int i=0;i<num;i++) {
    int top=lua_gettop(L);
    PPLuaScript::newObject(L,classname,object[i],NULL);
    lua_rawseti(L,-2,i+1);
    lua_settop(L,top);
  }
  lua_setglobal(L,valuename);
}

#pragma mark -

#define PPAssert(LUA_IDX,LUA_TYPE) if (lua_gettop(L)<LUA_IDX || lua_type(L,LUA_IDX)!=LUA_TYPE) return luaL_error(L,"illegal instance method call.");

static int funcPPLength(lua_State *L)
{
	int n=0;
	lua_Number len=0;
	lua_Number x1,y1;
	{
		if (lua_istable(L,n+1)) {
			lua_getfield(L,n+1,"x");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+1,1);
			}
			x1 = lua_tonumber(L,-1);
      
			lua_getfield(L,n+1,"y");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+1,2);
			}
			y1 = lua_tonumber(L,-1);
		} else {
			x1 = lua_tonumber(L,n+1);
			y1 = lua_tonumber(L,n+2);
			n=1;
		}
	}
  
	if (lua_isnil(L,n+2)) {
		len = sqrt(x1*x1+y1*y1);
	} else {
		lua_Number x2,y2;
		x2=y2=0;
		if (lua_istable(L,n+2)) {
			lua_getfield(L,n+2,"x");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+2,1);
			}
			x2 = lua_tonumber(L,-1);
      
			lua_getfield(L,n+2,"y");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+2,2);
			}
			y2 = lua_tonumber(L,-1);
		} else {
			x2 = lua_tonumber(L,n+2);
			y2 = lua_tonumber(L,n+3);
		}
		len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
	}
  
	lua_pushnumber(L,len);
	return 1;
}

static int funcPPRect(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
	PPRect r;
	if (s->argCount > 0 && s->isTable(L,-1)) {
		r = s->getRect(L,-1);
	} else
  if (s->argCount > 1) {
    r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
  }
  
	return s->returnRect(L,r);
}

int PPLuaArg::getPPPoint(lua_State *L,int idx,PPPoint &p)
{
  int n=0;
  float v[2]={0};
  int top=lua_gettop(L);
  for (int i=idx;i<=top&&n<2;i++) {
    if (lua_type(L,i)==LUA_TTABLE) {
      lua_getfield(L,i,"x");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=2) break;
      lua_getfield(L,i,"y");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n<2) {
#ifdef __LUAJIT__
        size_t len = lua_objlen(L,i);
#else
        size_t len = lua_rawlen(L,i);
#endif
        for (int j=0;j<len;j++) {
          lua_rawgeti(L,i,j+1);
          if (!lua_isnil(L,-1)) {
            v[n++]=lua_tonumber(L,-1);
            if (n>=2) {
              break;
            }
          }
        }
      }
    } else {
      v[n++]=lua_tonumber(L,i);
    }
  }
  if (n>=1) {
    p.x=v[0];
  }
  if (n>=2) {
    p.y=v[1];
  }
  return n;
}

int PPLuaArg::getPPSize(lua_State *L,int idx,float &w,float &h)
{
  int n=0;
  float v[2]={0};
  int top=lua_gettop(L);
  for (int i=idx;i<=top&&n<2;i++) {
    if (lua_type(L,i)==LUA_TTABLE) {
      lua_getfield(L,i,"width");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=2) break;
      lua_getfield(L,i,"height");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=2) break;
      lua_getfield(L,i,"x");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=2) break;
      lua_getfield(L,i,"y");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n<2) {
#ifdef __LUAJIT__
        size_t len = lua_objlen(L,i);
#else
        size_t len = lua_rawlen(L,i);
#endif
        for (int j=0;j<len;j++) {
          lua_rawgeti(L,i,j+1);
          if (!lua_isnil(L,-1)) {
            v[n++]=lua_tonumber(L,-1);
            if (n>=2) {
              break;
            }
          }
        }
      }
    } else {
      v[n++]=lua_tonumber(L,i);
    }
  }
  if (n>=1) {
    w=v[0];
    h=v[0];
  }
  if (n>=2) {
    h=v[1];
  }
  return n;
}

int PPLuaArg::getPPRect(lua_State *L,int idx,PPRect &r)
{
  int n=0;
  float v[4]={0};
  int top=lua_gettop(L);
  for (int i=idx;i<=top&&n<4;i++) {
    if (lua_type(L,i)==LUA_TTABLE) {
      lua_getfield(L,i,"x");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=4) break;
      lua_getfield(L,i,"y");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=4) break;
      lua_getfield(L,i,"width");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n>=4) break;
      lua_getfield(L,i,"height");
      if (!lua_isnil(L,-1)) {
        v[n++]=lua_tonumber(L,-1);
      }
      if (n<4) {
#ifdef __LUAJIT__
        size_t len = lua_objlen(L,i);
#else
        size_t len = lua_rawlen(L,i);
#endif
        for (int j=0;j<len;j++) {
          lua_rawgeti(L,i,j+1);
          if (!lua_isnil(L,-1)) {
            v[n++]=lua_tonumber(L,-1);
            if (n>=4) {
              break;
            }
          }
        }
      }
    } else {
      v[n++]=lua_tonumber(L,i);
    }
  }
  if (n>=1) {
    r.x=v[0];
  }
  if (n>=2) {
    r.y=v[1];
  }
  if (n>=3) {
    r.width=v[2];
  }
  if (n>=4) {
    r.height=v[3];
  }
  return n;
}

static int funcPPPoint(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
	PPPoint p;
  PPLuaArg::getPPPoint(L,1,p);
	return s->returnPoint(L,p);
}

static int funcPPIterator(lua_State *L)
{
	int top=lua_gettop(L);
	if (top>=2) {
		if (lua_istable(L,1)) {
#ifdef __LUAJIT__
			int n=(int)lua_objlen(L,1);
#else
			int n=(int)lua_rawlen(L,1);
#endif
			lua_settop(L,top);
			if (lua_isfunction(L,2)) {
				for (int i=0;i<n;i++) {
					lua_pushvalue(L,2);
					lua_pushinteger(L,i+1);
					lua_rawgeti(L,1,i+1);
					lua_call(L,2,1);
					if (!lua_isnil(L,-1)) {
						return 1;
					}
					lua_settop(L,top);
				}
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static void getMyPoint(lua_State* L,int idx,PPPoint& p)
{
  int top=lua_gettop(L);
  lua_getfield(L,idx,"x");
  if (!lua_isnil(L,-1)) {
    p.x=lua_tonumber(L,-1);
  }
  lua_getfield(L,idx,"y");
  if (!lua_isnil(L,-1)) {
    p.y=lua_tonumber(L,-1);
  }
  lua_settop(L,top);
}

static int funcPPPointLength(lua_State *L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPPoint m;
  getMyPoint(L,1,m);
	PPPoint p;
  int r=PPLuaArg::getPPPoint(L,2,p);
  if (r<2) {
    lua_pushnumber(L,m.length());
  } else {
    lua_pushnumber(L,m.length(p));
  }
  return 1;
}

static int funcPPPointMove(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
	PPPoint p;
  PPLuaArg::getPPPoint(L,2,p);
  PPPoint m;
  getMyPoint(L,1,m);
  m=m+p;
  lua_pushnumber(L,m.x);
  lua_setfield(L,1,"x");
  lua_pushnumber(L,m.y);
  lua_setfield(L,1,"y");
  return s->returnPoint(L,m);
}

static int funcPPPointUnm(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPPoint m;
  getMyPoint(L,1,m);
  return s->returnPoint(L,PPPoint(-m.x,-m.y));
}

static int funcPPPointAdd(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPPoint m;
  getMyPoint(L,1,m);
	PPPoint p;
  int r=PPLuaArg::getPPPoint(L,2,p);
  if (r<2) {
    return s->returnPoint(L,PPPoint(m.x+p.x,m.y+p.x));
  }
  return s->returnPoint(L,m+p);
}

static int funcPPPointSub(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPPoint m;
  getMyPoint(L,1,m);
	PPPoint p;
  int r=PPLuaArg::getPPPoint(L,2,p);
  if (r<2) {
    return s->returnPoint(L,PPPoint(m.x-p.x,m.y-p.x));
  }
  return s->returnPoint(L,m-p);
}

static int funcPPPointDiv(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPPoint m;
  getMyPoint(L,1,m);
	PPPoint p;
  int r=PPLuaArg::getPPPoint(L,2,p);
  if (r<2) {
    if (p.x == 0) {
      return s->returnPoint(L,PPPoint(NAN,NAN));
    }
    return s->returnPoint(L,PPPoint(m.x/p.x,m.y/p.x));
  }
  if (p.x == 0 || p.y == 0) {
    float x=m.x;
    float y=m.y;
    if (p.x == 0) {
      x=NAN;
    } else {
      x/=p.x;
    }
    if (p.y == 0) {
      y=NAN;
    } else {
      y/=p.y;
    }
    return s->returnPoint(L,PPPoint(x,y));
  }
  return s->returnPoint(L,m/p);
}

static int funcPPPointMul(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPPoint m;
  getMyPoint(L,1,m);
	PPPoint p;
  int r=PPLuaArg::getPPPoint(L,2,p);
  if (r<2) {
    return s->returnPoint(L,PPPoint(m.x*p.x,m.y*p.x));
  }
  return s->returnPoint(L,m*p);
}

static void getMyRect(lua_State* L,PPRect &r)
{
  int top=lua_gettop(L);
  lua_getfield(L,1,"x");
  if (!lua_isnil(L,-1)) {
    r.x=lua_tonumber(L,-1);
  }
  lua_getfield(L,1,"y");
  if (!lua_isnil(L,-1)) {
    r.y=lua_tonumber(L,-1);
  }
  lua_getfield(L,1,"width");
  if (!lua_isnil(L,-1)) {
    r.width=lua_tonumber(L,-1);
  }
  lua_getfield(L,1,"height");
  if (!lua_isnil(L,-1)) {
    r.height=lua_tonumber(L,-1);
  }
  lua_settop(L,top);
}

static int funcPPRectMin(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  return s->returnPoint(L,r.rect_min());
}

static int funcPPRectMax(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  return s->returnPoint(L,r.rect_max());
}

static int funcPPRectCenter(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  return s->returnPoint(L,PPPoint(r.x+r.width/2,r.y+r.height/2));
}

static int funcPPRectEqualToSize(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  PPRect t;
  PPLuaArg::getPPRect(L,2,t);
  bool b=(r.width==t.width && r.height==t.height);
  lua_pushboolean(L,b);
  return 1;
}

static int funcPPRectEqualToRect(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  PPRect t;
  PPLuaArg::getPPRect(L,2,t);
  bool b=(r.x==t.x && r.y==t.y && r.width==t.width && r.height==t.height);
  lua_pushboolean(L,b);
  return 1;
}

static int funcPPRectIsEmpty(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  bool b=(r.width==0 && r.height==0);
  lua_pushboolean(L,b);
  return 1;
}

static int funcPPRectMove(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
	PPPoint p;
  PPLuaArg::getPPPoint(L,2,p);
  r=r+p;
  lua_pushnumber(L,r.x);
  lua_setfield(L,1,"x");
  lua_pushnumber(L,r.y);
  lua_setfield(L,1,"y");
  lua_pushnumber(L,r.width);
  lua_setfield(L,1,"width");
  lua_pushnumber(L,r.height);
  lua_setfield(L,1,"height");
  return s->returnRect(L,r);
}

static int funcPPRectPosition(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
	PPPoint p;
  int q=PPLuaArg::getPPPoint(L,2,p);
  if (q>0) {
    lua_pushnumber(L,p.x);
    lua_setfield(L,1,"x");
    lua_pushnumber(L,p.y);
    lua_setfield(L,1,"y");
    r.x=p.x;
    r.y=p.y;
    //return 0;
  }
  return s->returnRect(L,PPRect(r.x,r.y,r.width,r.height));
}

static int funcPPRectSize(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  float w=0,h=0;
  int q=PPLuaArg::getPPSize(L,2,w,h);
  if (q>0) {
    lua_pushnumber(L,w);
    lua_setfield(L,1,"width");
    lua_pushnumber(L,h);
    lua_setfield(L,1,"height");
    r.width = w;
    r.height = h;
    //return 0;
  }
  return s->returnRect(L,PPRect(0,0,r.width,r.height));
}

static int funcPPRectScale(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
	PPPoint p;
  int q=PPLuaArg::getPPPoint(L,2,p);
  if (q>0) {
    if (q==1) {
      p.y=p.x;
    }
    r.width=r.width*p.x;
    r.height=r.height*p.y;
    lua_pushnumber(L,r.width);
    lua_setfield(L,1,"width");
    lua_pushnumber(L,r.height);
    lua_setfield(L,1,"height");
    //return 0;
  }
  return s->returnRect(L,r);
}

static int funcPPRectInset(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
	PPPoint p;
  int q=PPLuaArg::getPPPoint(L,2,p);
  if (q>0) {
    if (q==1) {
      p.y=p.x;
    }
    r.x+=p.x;
    r.y+=p.y;
    r.width-=p.x*2;
    r.height-=p.y*2;
    lua_pushnumber(L,r.x);
    lua_setfield(L,1,"x");
    lua_pushnumber(L,r.y);
    lua_setfield(L,1,"y");
    lua_pushnumber(L,r.width);
    lua_setfield(L,1,"width");
    lua_pushnumber(L,r.height);
    lua_setfield(L,1,"height");
//    return 0;
  }
  return s->returnRect(L,r);
}

static int funcPPRectContain(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
	PPPoint p;
  int q=PPLuaArg::getPPPoint(L,2,p);
  bool ret=false;
  if (q>0) {
    ret = (r.x <= p.x && r.y <= p.y && p.x < r.x+r.width && p.y < r.y+r.height);
  }
  lua_pushboolean(L,ret);
  return 1;
}

static int funcPPRectIntersect(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  PPRect t;
  PPLuaArg::getPPRect(L,2,t);
  float wx=r.x-t.x+r.width;
  float wy=r.y-t.y+r.height;
  bool ret = (wx>=0 && wx<r.width+t.width && wy>=0 && wy<r.height+t.height);
  lua_pushboolean(L,ret);
  return 1;
}

static int funcPPRectUnion(lua_State* L)
{
  PPAssert(1,LUA_TTABLE);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect r;
  getMyRect(L,r);
  PPRect t;
  PPLuaArg::getPPRect(L,2,t);
  PPPoint min1=r.rect_min();
  PPPoint max1=r.rect_max();
  PPPoint min2=t.rect_min();
  PPPoint max2=t.rect_max();
  if (min1.x > min2.x) { min1.x = min2.x; }
  if (min1.y > min2.y) { min1.y = min2.y; }
  if (max1.x < max2.x) { max1.x = max2.x; }
  if (max1.y < max2.y) { max1.y = max2.y; }
  return s->returnRect(L,PPRect(min1.x,min1.y,max1.x-min1.x,max1.y-min1.y));
}

static int funcPPRectUnm(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect m;
  getMyRect(L,m);
  return s->returnRect(L,PPRect(-m.x,-m.y,m.width,m.height));
}

static int funcPPRectAdd(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect m;
  getMyRect(L,m);
  PPRect t;
  int r=PPLuaArg::getPPRect(L,2,t);
  if (r>0) {
    if (r<2) {
      m.x=m.x+t.x;
      m.y=m.y+t.x;
    } else {
      m=m+t.rect_min();
    }
  }
  return s->returnRect(L,m);
}

static int funcPPRectSub(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect m;
  getMyRect(L,m);
  PPRect t;
  int r=PPLuaArg::getPPRect(L,2,t);
  if (r>0) {
    if (r<2) {
      m.x=m.x-t.x;
      m.y=m.y-t.x;
    } else {
      m=m-t.rect_min();
    }
  }
  return s->returnRect(L,m);
}

static int funcPPRectDiv(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect m;
  getMyRect(L,m);
  PPRect t;
  int r=PPLuaArg::getPPRect(L,2,t);
  if (r>0) {
    if (r<2) {
      if (t.x == 0) {
        return s->returnRect(L,PPRect(NAN,NAN,NAN,NAN));
      }
      m.x=m.x/t.x;
      m.y=m.y/t.x;
      m.width=m.width/t.x;
      m.height=m.height/t.x;
    } else {
      if (t.x == 0 || t.y == 0) {
        float x=m.x;
        float y=m.y;
        float w=m.width;
        float h=m.height;
        if (t.x == 0) {
          x=NAN;
          w=NAN;
        } else {
          x/=t.x;
          w/=t.x;
        }
        if (t.y == 0) {
          y=NAN;
          h=NAN;
        } else {
          y/=t.y;
          h/=t.y;
        }
        return s->returnRect(L,PPRect(x,y,w,h));
      }
      m.x=m.x/t.x;
      m.y=m.y/t.y;
      m.width=m.width/t.x;
      m.height=m.height/t.y;
    }
  }
  return s->returnRect(L,m);
}

static int funcPPRectMul(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  PPRect m;
  getMyRect(L,m);
  PPRect t;
  int r=PPLuaArg::getPPRect(L,2,t);
  if (r>0) {
    if (r<2) {
      m.x=m.x*t.x;
      m.y=m.y*t.x;
      m.width=m.width*t.x;
      m.height=m.height*t.x;
    } else {
      m.x=m.x*t.x;
      m.y=m.y*t.y;
      m.width=m.width*t.x;
      m.height=m.height*t.y;
    }
  }
  return s->returnRect(L,m);
}

void PPLuaScript::setupGeometryCommand()
{
  int top=lua_gettop(L);
  
  lua_createtable(L,0,2);
  lua_pushcfunction(L,funcPPPointLength);
  lua_setfield(L,-2,"length");
  lua_pushcfunction(L,funcPPPointMove);
  lua_setfield(L,-2,"move");
  lua_setglobal(L,"pppointImp");

  lua_createtable(L,0,6);
  lua_pushcfunction(L,funcPPPointUnm);
  lua_setfield(L,-2,"__unm");
  lua_pushcfunction(L,funcPPPointAdd);
  lua_setfield(L,-2,"__add");
  lua_pushcfunction(L,funcPPPointSub);
  lua_setfield(L,-2,"__sub");
  lua_pushcfunction(L,funcPPPointDiv);
  lua_setfield(L,-2,"__div");
  lua_pushcfunction(L,funcPPPointMul);
  lua_setfield(L,-2,"__mul");
  lua_getglobal(L,"pppointImp");
  lua_setfield(L,-2,"__index");
  lua_setglobal(L,"pppoint_mt");

  lua_createtable(L,0,17);
  lua_pushcfunction(L,funcPPRectMin);
  lua_setfield(L,-2,"min");
  lua_pushcfunction(L,funcPPRectMax);
  lua_setfield(L,-2,"max");
  lua_pushcfunction(L,funcPPRectCenter);
  lua_setfield(L,-2,"center");
  lua_pushcfunction(L,funcPPRectEqualToSize);
  lua_setfield(L,-2,"equalToSize");
  lua_pushcfunction(L,funcPPRectEqualToRect);
  lua_setfield(L,-2,"equalToRect");
  lua_pushcfunction(L,funcPPRectIsEmpty);
  lua_setfield(L,-2,"isEmpty");
  lua_pushcfunction(L,funcPPRectMove);
  lua_setfield(L,-2,"move");
  lua_pushcfunction(L,funcPPRectPosition);
  lua_setfield(L,-2,"position");
  lua_pushcfunction(L,funcPPRectPosition);
  lua_setfield(L,-2,"pos");
  lua_pushcfunction(L,funcPPRectSize);
  lua_setfield(L,-2,"size");
  lua_pushcfunction(L,funcPPRectScale);
  lua_setfield(L,-2,"scale");
  lua_pushcfunction(L,funcPPRectInset);
  lua_setfield(L,-2,"inset");
  lua_pushcfunction(L,funcPPRectContain);
  lua_setfield(L,-2,"contain");
  lua_pushcfunction(L,funcPPRectIntersect);
  lua_setfield(L,-2,"intersect");
  lua_pushcfunction(L,funcPPRectUnion);
  lua_setfield(L,-2,"union");
  lua_pushcfunction(L,funcPPPointLength);
  lua_setfield(L,-2,"length");
  lua_getglobal(L,"ppobject");
  lua_getfield(L,-1,"hitCheck");
  lua_setfield(L,-3,"hitCheck");
  lua_pop(L,1);
  lua_setglobal(L,"pprectImp");

  lua_createtable(L,0,6);
  lua_pushcfunction(L,funcPPRectUnm);
  lua_setfield(L,-2,"__unm");
  lua_pushcfunction(L,funcPPRectAdd);
  lua_setfield(L,-2,"__add");
  lua_pushcfunction(L,funcPPRectSub);
  lua_setfield(L,-2,"__sub");
  lua_pushcfunction(L,funcPPRectDiv);
  lua_setfield(L,-2,"__div");
  lua_pushcfunction(L,funcPPRectMul);
  lua_setfield(L,-2,"__mul");
  lua_getglobal(L,"pprectImp");
  lua_setfield(L,-2,"__index");
  lua_setglobal(L,"pprect_mt");

	addCommand("pplength",funcPPLength);
	addCommand("pprect",funcPPRect);
	addCommand("pppoint",funcPPPoint);
	addCommand("ppforeach",funcPPIterator);
  
  lua_settop(L,top);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
