//
//  PicoGame.cpp
//  PicoGame
//
//  Created by 山口 博光 on 2013/11/13.
//
//

#include "PicoGame.h"

PicoGame::PicoGame()
{
}

PicoGame::~PicoGame()
{
}

static int funcPreferncePath(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
    if (lua_isnil(L,2)) {
      PPSetCustomPlistPath(NULL);
    } else {
      PPSetCustomPlistPath(s->args(0));
    }
    return 0;
	}
  const char* p = PPGetCustomPlistPath();
  if (p==NULL) {
    lua_pushnil(L);
  } else {
    lua_pushstring(L,p);
  }
  return 1;
}

void PicoGame::initScript()
{
  PPSetCustomPlistPath(NULL);
	if (script) {
#if 0
    script->openModule("ppgame",NULL);
      script->addCommand("preferencePath",funcPreferncePath);
    script->closeModule();
#endif
  }
}
