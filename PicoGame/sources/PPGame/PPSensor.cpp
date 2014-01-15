/*-----------------------------------------------------------------------------------------------
	名前	PPSensor.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPSensor.h"
#include <stdlib.h>
#include "PPLuaScript.h"
#if TARGET_OS_IPHONE
#include "PPSensoriOS.h"
#endif

PPSensor* PPSensor::instance = NULL;

PPSensor* PPSensor::sharedManager()
{
	if (PPSensor::instance == NULL) {
#if TARGET_OS_IPHONE
		PPSensor::instance = new PPSensoriOS();
#else
		PPSensor::instance = new PPSensor();
#endif
	}
	return PPSensor::instance;
}

PPSensor::PPSensor()
{
}

PPSensor::~PPSensor()
{
}

void PPSensor::startAccelerometer(float interval)
{
}

void PPSensor::stopAccelerometer()
{
}

bool PPSensor::accelerometerAvailable()
{
	return false;
}

PPVertex PPSensor::getAccelerometer()
{
	PPVertex v;
	return v;
}

static int funcStart(lua_State *L) {
  if (lua_gettop(L)>0) {
    if (lua_gettop(L)>1) {
      PPSensor::sharedManager()->startAccelerometer(lua_tonumber(L,2));
    } else {
      PPSensor::sharedManager()->startAccelerometer(lua_tonumber(L,1));
    }
  }
	return 0;
}

static int funcStop(lua_State *L) {
	PPSensor::sharedManager()->stopAccelerometer();
	return 0;
}

static int funcIsAvailable(lua_State *L) {
	lua_pushboolean(L,PPSensor::sharedManager()->accelerometerAvailable());
	return 1;
}

static int funcValue(lua_State *L) {
	lua_createtable(L,0,3);
	PPVertex v = PPSensor::sharedManager()->getAccelerometer();
	lua_pushnumber(L,v.x);
	lua_setfield(L,-2,"x");
	lua_pushnumber(L,v.y);
	lua_setfield(L,-2,"y");
	lua_pushnumber(L,v.z);
	lua_setfield(L,-2,"z");
	return 1;
}

void PPSensor::openAccelerometerLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
	s->openModule(name,NULL,0,superclass);
		s->addCommand("start",funcStart);
		s->addCommand("stop",funcStop);
		s->addCommand("isAvailable",funcIsAvailable);
		s->addCommand("value",funcValue);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
