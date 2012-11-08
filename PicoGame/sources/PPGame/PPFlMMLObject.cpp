/*-----------------------------------------------------------------------------------------------
	名前	PPFlMMLObject.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPFlMMLObject.h"
#include "QBSound.h"

PPFlMMLObject::PPFlMMLObject(PPWorld* world) : PPObject(world)
{
	noteNo = 60;
}

PPFlMMLObject::~PPFlMMLObject()
{
}

#pragma mark -

#include "PPLuaScript.h"


static int funcFlMMLPlay(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
	if (!snd->isPlayingMML(m->index)) 
	{
		if (s->argCount > 0) {
			if (s->isString(L,0)) {
				snd->playMML(s->args(0),m->index);
			} else {
				return luaL_argerror(L,1,"illegal argment");
			}
		} else {
			snd->playMML("",m->index);
		}
	}
	return 0;
}

static int funcFlMMLClear(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
//	if (snd->isPlayingMML(m->index) || snd->isPausedMML(m->index)) {
//		snd->stopMML(m->index);
//	}
	snd->preloadMML("",m->index);
	return 0;
}

static int funcFlMMLPreload(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPWorld* world = PPLuaArg::World(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(world,L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	if (s->argCount > 0) {
		QBSound* snd = QBSound::sharedSound();
//		if (snd->isPlayingMML(m->index)) {
//			snd->stopMML(m->index);
//		}
		if (s->isString(L,0)) {
			snd->preloadMML(s->args(0),m->index);
		} else {
			return luaL_argerror(L,1,"illegal argment");
//			s->errorMessage = "error illegal argment FlMML:preload";
		}
	} else {
		return luaL_argerror(L,1,"no value");
	}
	return 0;
}

static int funcFlMMLStop(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	snd->stopMML(m->index);
	return 0;
}

static int funcFlMMLPause(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (snd->isPlayingMML(m->index)) {
		snd->pauseMML(m->index);
	}
	return 0;
}

static int funcFlMMLResume(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (snd->isPausedMML(m->index)) {
		snd->resumeMML(m->index);
	}
	return 0;
}

static int funcFlMMLIsPlaying(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	lua_pushboolean(L,snd->isPlayingMML(m->index));
	return 1;
}


static int funcFlMMLIsPaused(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	lua_pushboolean(L,snd->isPausedMML(m->index));
	return 1;
}

static int funcFlMMLVolume(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		snd->setVolumeMML(m->index,s->number(0));
		return 0;
	}
	lua_pushnumber(L,snd->getVolumeMML(m->index));
	return 1;
}

static int funcFlMMLNoteOn(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		if (s->argCount == 1) {
			snd->noteOn(m->index,(int)s->integer(0),m->noteNo,snd->getVolumeMML(m->index));
		} else
		if (s->argCount == 2) {
			snd->noteOn(m->index,(int)s->integer(0),(int)s->integer(1),snd->getVolumeMML(m->index));
			m->noteNo = (int)s->integer(1);
		} else {
			snd->noteOn(m->index,(int)s->integer(0),(int)s->integer(1),s->number(2));
			m->noteNo = (int)s->integer(1);
		}
	} else {
		return luaL_argerror(L,0+2,lua_pushfstring(L,"missing channel no."));
	}
	if (snd->isPlayingMML(m->index)) {
		snd->pauseMML(m->index);
	}
	return 0;
}

static int funcFlMMLNoteOff(lua_State* L)
{
	PPFlMMLObject* m = (PPFlMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
//	PPFlMMLObject* m = (PPFlMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		snd->noteOff(m->index,(int)s->integer(0));
	} else {
		return luaL_argerror(L,0+2,lua_pushfstring(L,"missing channel no."));
	}
	return 0;
}

void PPFlMMLObject::openLibrary(PPLuaScript* script,const char* name,const char* superclass)
{
	script->openModule(name,this,0,superclass);
		script->addCommand("play",funcFlMMLPlay);
//		script->addCommand("clear",funcFlMMLClear);
		script->addCommand("stop",funcFlMMLClear);
		script->addCommand("pause",funcFlMMLPause);
		script->addCommand("preload",funcFlMMLPreload);
		script->addCommand("resume",funcFlMMLResume);
		script->addCommand("isPlaying",funcFlMMLIsPlaying);
		script->addCommand("isPaused",funcFlMMLIsPaused);
		script->addCommand("volume",funcFlMMLVolume);
		script->addCommand("noteOn",funcFlMMLNoteOn);
		script->addCommand("noteOff",funcFlMMLNoteOff);
	script->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
