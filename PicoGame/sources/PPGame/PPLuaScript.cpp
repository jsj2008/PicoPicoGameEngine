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

#ifdef __COCOS2DX__
#include "Cocos2dxLuaLoader.h"
#include "CCFileUtils.h"
#endif

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lua/ldebug.h>
#include <lua/luaApiHook.h>
}

//PPLuaScript* PPLuaScript::script = NULL;
//int PPLuaScript::timeoutCount = 0;

static int l_setErrorMessage(lua_State* L)
{
	//lua_getglobal(L,"PPLuaScript");
	//PPLuaScript* script = PPLuaScript::script;//(PPLuaScript*)lua_touserdata(L,-1);
	PPLuaArg* script = PPLuaArg::ErrorTarget(L);
	int top = lua_gettop(L);
	const char* name = lua_tostring(L,-top);
	script->errorMessage = name;
	return 0;
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

void* PPLuaArg::init(lua_State* L)
{
	int top = lua_gettop(L);
	resetWord();
	int n=1;
	userdata = NULL;
	if (lua_type(L,1) == LUA_TTABLE) {
#ifdef USERMETA
		lua_getmetatable(L,1);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		userdata = lua_touserdata(L,-1);
#else
		lua_pushstring(L,PPGAMEINSTNACE);
		lua_rawget(L,1);
		userdata = lua_touserdata(L,-1);
#endif
		if (userdata) {
			n=2;
		}
	}
	for (int i=n;i<=top;i++) {
		_number[argCount] = lua_tonumber(L,i);
		_integer[argCount] = lua_tointeger(L,i);
		_boolean[argCount] = lua_toboolean(L,i);
		const char* str = lua_tostring(L,i);
		if (str) {
			addWord(str);
		} else {
			addWord("");
		}
	}
	for (int i=top+1;i<MAX_ARG;i++) {
		_number[i] = 0;
		_integer[i] = 0;
		_boolean[i] = false;
	}
	return userdata;
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
	def.r = tableTableNumber(L,argn,field,"r",def.r);
	def.g = tableTableNumber(L,argn,field,"g",def.g);
	def.b = tableTableNumber(L,argn,field,"b",def.b);
	def.a = tableTableNumber(L,argn,field,"a",def.a);
	return def;
}

void PPLuaArg::resetWord()
{
	for (int i=0;i<argCount;i++) {
		_str[i] = "";
	}
	argCount = 0;
}

void PPLuaArg::addWord(const char* str)
{
	if (argCount >= MAX_ARG) {
printf("error !! arg buffer is overflow\n");
	} else {
		if (str == NULL) {
			_str[argCount] = "";
//printf("NULL argCount %d\n",argCount+1);
		} else {
			_str[argCount] = str;
//printf("%s argCount %d\n",str,argCount+1);
		}
		argCount ++;
	}
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
	if (gc) {
		lua_pushcfunction(L,gc);
		lua_setfield(L,metatable,"__gc");
	}
	if (superclass) {
		lua_getglobal(L,superclass);
		lua_setfield(L,metatable,"__index");
	}
	lua_setmetatable(L,methods);
	lua_settop(L,0);
	_module = new std::string(name);
}

void PPLuaScript::addIntegerValue(const char* name,int value)
{
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushinteger(L, value);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		if (!lua_istable(L,-1)) {
			lua_settop(L,0);
			lua_createtable(L,0,0);
			lua_setglobal(L,_module->c_str());
		}
		lua_pushstring(L,name);
		lua_pushinteger(L,value);
		lua_rawset(L,-3);
		lua_settop(L,0);
	}
}

void PPLuaScript::addNumberValue(const char* name,float value)
{
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushinteger(L, value);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		if (!lua_istable(L,-1)) {
			lua_settop(L,0);
			lua_createtable(L,0,0);
			lua_setglobal(L,_module->c_str());
		}
		lua_pushstring(L,name);
		lua_pushnumber(L,value);
		lua_rawset(L,-3);
		lua_settop(L,0);
	}
}

void PPLuaScript::addBoolValue(const char* name,bool value)
{
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushboolean(L,value);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		if (!lua_istable(L,-1)) {
			lua_settop(L,0);
			lua_createtable(L,0,0);
			lua_setglobal(L,_module->c_str());
		}
		lua_pushstring(L,name);
		lua_pushboolean(L,value);
		lua_rawset(L,-3);
		lua_settop(L,0);
	}
}

void PPLuaScript::addStringValue(const char* name,const char* str)
{
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushstring(L,str);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		if (!lua_istable(L,-1)) {
			lua_settop(L,0);
			lua_createtable(L,0,0);
			lua_setglobal(L,_module->c_str());
		}
		lua_pushstring(L,name);
		lua_pushstring(L,str);
		lua_rawset(L,-3);
		lua_settop(L,0);
	}
}

void PPLuaScript::addMetaTable(const char* name,lua_CFunction func)
{
	if (strcmp(_module->c_str(),"") == 0) {
//		lua_pushcfunction(L,func);
//		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		if (!lua_istable(L,-1)) {
			lua_settop(L,0);
			lua_createtable(L,0,0);
			lua_setglobal(L,_module->c_str());
		}

		lua_createtable(L,0,0);
//		lua_pushstring(L,name);
		lua_pushcfunction(L,func);
		lua_setfield(L,-2,name);
//		lua_rawset(L,-3);
		lua_setmetatable(L,-2);
		lua_settop(L,0);
	}
}

void PPLuaScript::addCommand(const char* name,lua_CFunction func)
{
//printf("    %s\n",name);
	if (strcmp(_module->c_str(),"") == 0) {
		lua_pushcfunction(L,func);
		lua_setglobal(L,name);
	} else {
		lua_getglobal(L,_module->c_str());
		if (!lua_istable(L,-1)) {
			lua_settop(L,0);
			lua_createtable(L,0,0);
			lua_setglobal(L,_module->c_str());
		}
		lua_pushstring(L,name);
		lua_pushcfunction(L,func);
		lua_rawset(L,-3);
		lua_settop(L,0);
	}
}

void PPLuaScript::closeModule()
{
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
		return _str[index].c_str();
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
void myCountHook(lua_State* L);
void mySetErrorLineNumber(lua_State* L,const char* src,int line);
static lua_Number os_clock()
{
	return ((lua_Number)clock())/((lua_Number)CLOCKS_PER_SEC);
}
void mySetErrorLineNumber(lua_State* L,const char* src,int line)
{
	PPLuaArg* s = PPLuaArg::ErrorTarget(L);
	if (s->errorLine < 0) {
		s->errorLine = line;
		if (src) {
			s->errorSrc = src;
		} else {
			s->errorSrc = "";
		}
	}
}
void myCountHook(lua_State* L)
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
//		PPLuaScript::timeoutCount = 0;
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

PPLuaScript::PPLuaScript(PPWorld* world) : PPLuaArg(world),_module(NULL)
{
	resetTimeout();
	
	//script = this;
	L = luaL_newstate();
	luaApiHook_Init(L,myCountHook,mySetErrorLineNumber);
	coL = NULL;
	luaL_openlibs(L);
#ifdef __COCOS2DX__
	addLuaLoader(loader_Android);
#endif
	_module = new std::string("");
	world->userdata = this;


	int top=lua_gettop(L);
	lua_pushlightuserdata(L,world);
	lua_setglobal(L,"__PPWorld");
	lua_settop(L,top);

	lua_register(L,"__PPLuaScriptSetErrorMessage",l_setErrorMessage);
}

PPLuaScript::~PPLuaScript()
{
	lua_close(L);
//	if (flags) delete flags;
	if (_module) delete _module;
}

//void PPLuaScript::start()
//{
//	//PPObject::start();
//}

//void PPLuaScript::startWithFlag()
//{
//	//PPObject::start();
////	if (flags == NULL) {
////		flags = new PPScriptFlag();
////	}
//}

bool PPLuaScript::load(const char* scriptfile)
{
//printf("PPLuaScript::load %s\n",scriptfile);
	bool r = true;
	errorMessage = "";
#ifdef _ANDROID
	unsigned long size;
	char *pFileContent = (char*)cocos2d::CCFileUtils::getFileData(scriptfile,"r",&size);
	if (pFileContent) {
	    char *pCodes = new char[size + 1];
	    pCodes[size] = '\0';
	    memcpy(pCodes, pFileContent, size);
	    delete[] pFileContent;
	    execString(pCodes);
	    delete []pCodes;
	}
#else
	int nRet = luaL_dofile(L,scriptfile);
	lua_gc(L, LUA_GCCOLLECT, 0);
	if (nRet != 0)
	{		        
		printf("%s", lua_tostring(L, -1));
		errorMessage = lua_tostring(L, -1);
		lua_pop(L, 1);
printf("Error %d\n",errorLine);
		r = false;
	}
//printf("PPLuaScript::load 2\n");
#endif
	alive = false;
	resetTimeout();
	//NEXT(PPLuaScript::stepIdle);
//printf("PPLuaScript::load 3\n");
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
		lua_xmove(L, coL, 1);  /* move function from L to NL */
		lua_setglobal(L,"___scriptCorutine");
	}
	lua_settop(L,top);
	alive = checkProcess();
}

bool PPLuaScript::doProcess()
{
	if (checkProcess()) {
		int top=lua_gettop(L);
		//lua_getglobal(L,"___scriptCorutine");
		lua_State *co = coL;//lua_tothread(L,-1);
		if (co) {
			int status = lua_resume(co,L,0);
			if (status == LUA_OK || status == LUA_YIELD) {
			} else {
				lua_xmove(co, L, 1);
				PPLuaArg* target = PPLuaArg::ErrorTarget(L);
 				if (target) target->errorMessage = lua_tostring(L,-1);
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
//	lua_getglobal(L,"___scriptCorutine");
	lua_State *co = coL;//lua_tothread(L,-1);
	if (co) {
		ret=true;
		switch (lua_status(co)) {
		case LUA_YIELD:
			break;
		case LUA_OK:
			{
				lua_Debug ar;
				if (lua_getstack(co, 0, &ar) > 0) {
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
	// load code into lua and call it
	int error =	luaL_dostring(L, script);
	
	// Collect
	lua_gc(L, LUA_GCCOLLECT, 0);
	
	// handle errors
	if (error)
	{
		// print error message and pop it
		printf("%s", lua_tostring(L, -1));
		errorMessage = lua_tostring(L, -1);
		lua_pop(L, 1);
		
		return false;
	}
	
	return true;
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

void* PPLuaArg::UserData(lua_State* L)
{
	int top = lua_gettop(L);
	void* userdata = NULL;
	if (lua_type(L,1) == LUA_TTABLE) {
#ifdef USERMETA
		lua_getmetatable(L,1);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		userdata = lua_touserdata(L,-1);
#else
		lua_pushstring(L,PPGAMEINSTNACE);
		lua_rawget(L,1);
		userdata = lua_touserdata(L,-1);
#endif
	}
	lua_settop(L,top);
	return userdata;
}

void PPLuaScript::idle()
{
	if (alive) {
		alive = doProcess();
	}
	alive = checkProcess();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
