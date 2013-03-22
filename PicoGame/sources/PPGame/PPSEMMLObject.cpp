/*-----------------------------------------------------------------------------------------------
	名前	PPSEMMLObject.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPSEMMLObject.h"
#include "QBSound.h"

PPSEMMLObject::PPSEMMLObject(PPWorld* world) : PPObject(world)
{
	play=false;
	semml="";
}

PPSEMMLObject::~PPSEMMLObject()
{
}

void PPSEMMLObject::start()
{
	PPObject::start();
	NEXT(PPSEMMLObject::stepIdle);
}

void PPSEMMLObject::stepIdle()
{
	if (play) {
		play = false;
		QBSound* snd = QBSound::sharedSound();
		if (snd) snd->play(semml.c_str(),index);
	}
}

#pragma mark -

#include "PPLuaScript.h"

static int funcPlay(lua_State* L)
{
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
#if TARGET_OS_IPHONE
//	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		m->play = true;
		m->semml = s->args(0);
//		snd->play(s->args(0),m->index);
	}
#else
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		snd->play(s->args(0),m->index);
	}
#endif
	return 0;
}

static int funcStop(lua_State* L)
{
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (snd->isPlaying(m->index)) {
		snd->stop(m->index);
	}
#if TARGET_OS_IPHONE
//	snd->stop(m->index);
	m->play = false;
#endif
	return 0;
}

static int funcIsPlaying(lua_State* L)
{
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
#if TARGET_OS_IPHONE
	if (m->play) {
		lua_pushboolean(L,true);
		return 1;
	}
#endif
	lua_pushboolean(L,snd->isPlaying(m->index));
	return 1;
}

static int funcVolume(lua_State* L)
{
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		snd->setVolume(m->index,s->number(0));
		return 0;
	}
	lua_pushnumber(L,snd->getVolume(m->index));
	return 1;
}

static int funcWav9(lua_State* L)
{
//	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 3) {
		snd->setWav9((int)s->integer(0),(int)s->integer(1),s->boolean(2),s->args(3));
	}
	return 0;
}

static int funcWav10(lua_State* L)
{
//	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 1) {
		snd->setWav10((int)s->integer(0),s->args(1));
	}
	return 0;
}

static int funcWav13(lua_State* L)
{
//	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPSEMMLObject* m = (PPSEMMLObject*)s->userdata;
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 1) {
		snd->setWav13((int)s->integer(0),s->args(1));
	}
	return 0;
}

void PPSEMMLObject::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
//	PPObject::openLibrary(s,name);
	s->openModule(name,this,0,superclass);
		s->addCommand("play",funcPlay);
		s->addCommand("stop",funcStop);
		s->addCommand("isPlaying",funcIsPlaying);
		s->addCommand("volume",funcVolume);
//		s->addCommand("wav9",funcWav9);
//		s->addCommand("wav10",funcWav10);
//		s->addCommand("wav13",funcWav13);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
