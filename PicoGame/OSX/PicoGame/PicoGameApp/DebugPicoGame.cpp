//
//  DebugPicoGame.cpp
//  PicoGame
//
//  Created by 山口 博光 on 2014/01/10.
//
//

#include "DebugPicoGame.h"

static int allocate_count = 0;
static int allocate_count2 = 0;
static int counter=0;
//static void* memarray[40000]={0};
//static int memcount[40000]={0};

void* operator new(size_t size) throw(std::bad_alloc) {
	void* ptr= malloc(size);
//  for (int i=0;i<40000;i++) {
//    if (memarray[i] == NULL) {
//      memarray[i] = ptr;
//      memcount[i] = counter;
//      break;
//    }
//  }
  allocate_count++;
  if (allocate_count2 < allocate_count) {
    allocate_count2 = allocate_count;
  }
  return ptr;
}

void operator delete(void* pv) throw() {
//  for (int i=0;i<40000;i++) {
//    if (memarray[i]==pv) {
//      memarray[i]=NULL;
//      memcount[i] = 0;
//    }
//  }
  allocate_count--;
  free(pv);
}

static int funcAllocateCount(lua_State* L)
{
//  int n=(int)lua_tointeger(L,1);
//  for (int i=0;i<4000;i++) {
//    if (memcount[i] == n) {
//      printf("%d:%lx\n",i,(unsigned long)memarray[i]);
//    }
//  }
  lua_pushinteger(L,allocate_count);
  lua_pushinteger(L,allocate_count2);
  return 2;
}

static int funcAllocateCheck(lua_State* L)
{
  return 0;
}

void DebugPicoGame::initScript()
{
  counter++;
  allocate_count2=0;
  if (script) {
    lua_State* L = script->L;
    int top=lua_gettop(L);
    lua_getglobal(L,"ppdebug");
    if (lua_isnil(L,-1)) {
      lua_createtable(L,0,0);
    }
    lua_pushcfunction(L,funcAllocateCount);
    lua_setfield(L,-2,"allocateCount");
    lua_pushcfunction(L,funcAllocateCheck);
    lua_setfield(L,-2,"allocateCheck");
    lua_setglobal(L,"ppdebug");
    lua_settop(L,top);
  }
}
