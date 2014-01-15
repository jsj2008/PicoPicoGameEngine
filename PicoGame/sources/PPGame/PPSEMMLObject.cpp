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
#ifdef _OBJMEM_DEBUG_
  objname="PPSEMMLObject";
  printf("alloc %s\n",objname);
  fflush(stdout);
#endif
}

PPSEMMLObject::~PPSEMMLObject()
{
}

void PPSEMMLObject::start()
{
	PPObject::start();
  QBSound* snd = QBSound::sharedSound();
  if (snd) snd->play("@@",index);
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
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaArg::UserData(L,PPSEMMLObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
#if TARGET_OS_IPHONE
	if (s->argCount > 0) {
		m->play = true;
		m->semml = s->args(0);
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
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaArg::UserData(L,PPSEMMLObject::className);
  PPUserDataAssert(m!=NULL);
	QBSound* snd = QBSound::sharedSound();
	if (snd->isPlaying(m->index)) {
		snd->stop(m->index);
	}
#if TARGET_OS_IPHONE
	m->play = false;
#endif
	return 0;
}

static int funcIsPlaying(lua_State* L)
{
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaArg::UserData(L,PPSEMMLObject::className);
  PPUserDataAssert(m!=NULL);
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
	PPSEMMLObject* m = (PPSEMMLObject*)PPLuaArg::UserData(L,PPSEMMLObject::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	QBSound* snd = QBSound::sharedSound();
	if (s->argCount > 0) {
		snd->setVolume(m->index,s->number(0));
		return 0;
	}
	lua_pushnumber(L,snd->getVolume(m->index));
	return 1;
}

std::string PPSEMMLObject::className;

void PPSEMMLObject::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
  PPSEMMLObject::className=name;
	s->openModule(name,NULL,0,superclass);
		s->addCommand("play",funcPlay);
		s->addCommand("stop",funcStop);
		s->addCommand("isPlaying",funcIsPlaying);
		s->addCommand("volume",funcVolume);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
