//
//  luaApiHook.h
//  Lua
//
//  Created by Hiromitsu Yamaguchi on 12/07/15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef Lua_luaApiHook_h
#define Lua_luaApiHook_h

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*luaApiHook_CountFunc) (lua_State* L);
typedef void (*luaApiHook_SetErrorLineNumberFunc) (lua_State* L,const char* src,int line);

void luaApiHook_Init(lua_State* L,luaApiHook_CountFunc func1,luaApiHook_SetErrorLineNumberFunc func2);

void luaApiHook_Count(lua_State* L);
void luaApiHook_SetErrorLineNumber(lua_State* L,const char* src,int line);

#ifdef __cplusplus
};
#endif

#endif
