//
//  luaApiHook.c
//  Lua
//
//  Created by Hiromitsu Yamaguchi on 12/07/15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "luaApiHook.h"

static luaApiHook_CountFunc _func1 = NULL;
static luaApiHook_SetErrorLineNumberFunc _func2 = NULL;

void luaApiHook_Init(lua_State* L,luaApiHook_CountFunc func1,luaApiHook_SetErrorLineNumberFunc func2)
{
	_func1 = func1;
	_func2 = func2;
}

void luaApiHook_Count(lua_State* L)
{
	if (_func1) (*_func1)(L);
}

void luaApiHook_SetErrorLineNumber(lua_State* L,const char* src,int line)
{
	if (_func2) (*_func2)(L,src,line);
}
