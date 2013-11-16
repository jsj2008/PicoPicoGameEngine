/*-----------------------------------------------------------------------------------------------
	名前	PPGameStick.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameStick.h"
#include "PPGameUtil.h"

PPGameStick* PPGameStick::stickInstance = NULL;

PPJoyReaderElement::PPJoyReaderElement()
{
	exist=0;
#ifdef __MACOSX_APP__
	elem=NULL;
#endif
	value=0;
}

PPJoyReaderAxis::PPJoyReaderAxis()
{
	min=0;
	max=0;
//	scaledMin=0;
//	scaledMax=0;
	calibCenter=0;
	calibMin=0;
	calibMax=0;
}

void PPJoyReaderAxis::init(int _min,int _max,int _scaledMin,int _scaledMax)
{
	exist=1;
	min=_min;
	max=_max;
//	scaledMin=_scaledMin;
//	scaledMax=_scaledMax;

	calibCenter=(min+max)/2;
	calibMin=_scaledMin;
	calibMax=_scaledMax;
}

double PPJoyReaderAxis::GetCalibratedValue(void) const
{
	double calib;

	if(calibCenter<value && calibMax!=calibCenter)
	{
		calib=(double)(value-calibCenter)/(double)(calibMax-calibCenter);
	}
	else if(value<calibCenter && calibMin!=calibCenter)
	{
		calib=(double)(value-calibCenter)/(double)(calibCenter-calibMin);
	}
	else
	{
		return 0.0;
	}

	if(calib>1.0)
	{
		calib=1.0;
	}
	if(calib<-1.0)
	{
		calib=-1.0;
	}

	return calib;
}

void PPJoyReaderAxis::CaptureCenter(void)
{
	calibCenter=value;
}

void PPJoyReaderAxis::BeginCaptureMinMax(void)
{
	calibMin=calibCenter+1000;
	calibMax=calibCenter-1000;
}

void PPJoyReaderAxis::CaptureMinMax(void)
{
	if(value<calibMin)
	{
		calibMin=value;
	}
	if(value>calibMax)
	{
		calibMax=value;
	}
}

void PPJoyReaderAxis::CenterFromMinMax(void)
{
	calibCenter=(calibMin+calibMax)/2;
}

PPJoyReaderButton::PPJoyReaderButton()
{
}

PPJoyReaderHatSwitch::PPJoyReaderHatSwitch()
{
	valueNeutral=0;
	value0Deg=1;
	value90Deg=3;
	value180Deg=5;
	value270Deg=7;
}

int PPJoyReaderHatSwitch::GetDiscreteValue(void) const
{
	if(value==valueNeutral)
	{
		return 0;
	}
	else if(value==value0Deg)
	{
		return 1;
	}
	else if(value==value90Deg)
	{
		return 3;
	}
	else if(value==value180Deg)
	{
		return 5;
	}
	else if(value270Deg==value)
	{
		return 7;
	}
	else if(value0Deg<value && value<value90Deg)
	{
		return 2;
	}
	else if(value90Deg<value && value<value180Deg)
	{
		return 4;
	}
	else if(value180Deg<value && value<value270Deg)
	{
		return 6;
	}
	else if(value270Deg<value)
	{
		return 8;
	}
	return 0;
}

#pragma mark -
	
void PPGameStickElement::saveCalibration()
{
	char product[256];
	char vendor[256];
	char indexstr[256];
	sprintf(product,"%x",productID);
	sprintf(vendor,"%x",vendorID);
	sprintf(indexstr,"%d",index);
	std::string basekey = "stick_0x"+std::string(vendor)+"_0x"+std::string(product)+"_"+std::string(indexstr);
	for (int i=0;i<numAxis;i++) {
		char num[64];
		sprintf(num,"[%d]",i);
		PPSetNumber((basekey+"_calibMin"+num).c_str(),axisCalibMin[i],false);
		PPSetNumber((basekey+"_calibMax"+num).c_str(),axisCalibMax[i],false);
		PPSetNumber((basekey+"_calibCenter"+num).c_str(),axisCalibCenter[i],true);
	}
}

void PPGameStickElement::loadCalibration()
{
	char product[256];
	char vendor[256];
	char indexstr[256];
	sprintf(product,"%x",productID);
	sprintf(vendor,"%x",vendorID);
	sprintf(indexstr,"%d",index);
	std::string basekey = "stick_0x"+std::string(vendor)+"_0x"+std::string(product)+"_"+std::string(indexstr);
	for (int i=0;i<numAxis;i++) {
		char num[64];
		sprintf(num,"[%d]",i);
		axisCalibMin[i] = PPGetNumber((basekey+"_calibMin"+num).c_str(),axisCalibMin[i]);
		axisCalibMax[i] = PPGetNumber((basekey+"_calibMax"+num).c_str(),axisCalibMax[i]);
		axisCalibCenter[i] = PPGetNumber((basekey+"_calibCenter"+num).c_str(),axisCalibCenter[i]);
	}
}

#pragma mark -

PPGameStick::PPGameStick(PPWorld* world) : PPObject(world)
{
	stickInstance = this;
	for (int i=0;i<MAX_JOYSTICK_NUM;i++) {
		stick[i] = new PPGameStickElement(world);
#ifdef _OBJMEM_DEBUG_
    stick[i]->objname = "PPGameStickElement";
#endif
	}
}

PPGameStick::~PPGameStick()
{
	for (int i=0;i<MAX_JOYSTICK_NUM;i++) {
		delete stick[i];
	}
}

void PPGameStick::start()
{
	PPObject::start();
	reset();
}

void PPGameStick::reset()
{
	for (int i=0;i<MAX_JOYSTICK_NUM;i++) {
		stick[i]->exist = false;
		stick[i]->index = i;
		stick[i]->numAxis = 0;
		stick[i]->numButton = 0;
		stick[i]->numHatSwitch = 0;
		stick[i]->productID = 0;
		stick[i]->vendorID = 0;
	}
	
#ifdef _WIN32
	for (int i=0;i<numDevices()&&i<MAX_JOYSTICK_NUM;i++) {
		joy[i].dwSize = sizeof(JOYINFOEX);
		joy[i].dwFlags = JOY_RETURNALL;//JOY_RETURNBUTTONS | JOY_RETURNPOV | JOY_RETURNX | JOY_RETURNY;
		if (joyGetPosEx(i,&joy[i]) == JOYERR_NOERROR) {
			joyGetDevCaps(i,&joy_caps[i],sizeof(JOYCAPS));

			//printf("wMid 0x%x\n",(int)joy_caps[i].wMid);
			//printf("wPid 0x%x\n",(int)joy_caps[i].wPid);
			//printf("szPname %s\n",joy_caps[i].szPname);
			//printf("szRegKey %s\n",joy_caps[i].szRegKey);
			//printf("szOEMVxD %s\n",joy_caps[i].szOEMVxD);

			stick[i]->exist = true;
			stick[i]->productID = joy_caps[i].wPid;
			stick[i]->vendorID = joy_caps[i].wMid;

			{
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy_caps[i].wXmax;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy_caps[i].wXmin;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy_caps[i].wXmin+joy_caps[i].wXmax)/2;
				stick[i]->numAxis++;
			}
			{
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy_caps[i].wYmax;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy_caps[i].wYmin;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy_caps[i].wYmin+joy_caps[i].wYmax)/2;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASZ) {
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy_caps[i].wZmax;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy_caps[i].wZmin;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy_caps[i].wZmin+joy_caps[i].wZmax)/2;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASR) {
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy_caps[i].wRmax;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy_caps[i].wRmin;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy_caps[i].wRmin+joy_caps[i].wRmax)/2;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASR) {
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy_caps[i].wUmax;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy_caps[i].wUmin;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy_caps[i].wUmin+joy_caps[i].wUmax)/2;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASR) {
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy_caps[i].wVmax;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy_caps[i].wVmin;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy_caps[i].wVmin+joy_caps[i].wVmax)/2;
				stick[i]->numAxis++;
			}
		}
	}
#endif

#ifdef __MACOSX_APP__
	for (int i=0;i<MAX_JOYSTICK_NUM;i++) {
		//joy[i].ReleaseInterface();
	}
	PPJoyReaderSetUpJoystick(numJoystick,joy,MAX_JOYSTICK_NUM);
	for (int i=0;i<numDevices()&&i<MAX_JOYSTICK_NUM;i++) {
		joy[i].Read();
		for(int j=0;j<PPJoyReaderMaxNumAxis;j++) {
			if(joy[i].axis[j].exist!=0) {
				stick[i]->exist = true;
				stick[i]->axisCalibMax[stick[i]->numAxis] = stick[i]->axisMax[stick[i]->numAxis] = joy[i].axis[j].max;
				stick[i]->axisCalibMin[stick[i]->numAxis] = stick[i]->axisMin[stick[i]->numAxis] = joy[i].axis[j].min;
				stick[i]->axisCalibCenter[stick[i]->numAxis] = (joy[i].axis[j].min+joy[i].axis[j].max)/2;
				stick[i]->numAxis++;
			}
		}
		stick[i]->productID = joy[i].productID;
		stick[i]->vendorID = joy[i].vendorID;
		stick[i]->loadCalibration();
	}
	//PPJoyReaderLoadJoystickCalibrationInfo(numJoystick,joystick);
#endif
}

void PPGameStick::stepIdle()
{
}

int PPGameStick::numDevices()
{
#ifdef _WIN32
	return joyGetNumDevs();
#endif
#ifdef __MACOSX_APP__
	return numJoystick;
#endif
	return 0;
}

void PPGameStick::read()
{
	for (int i=0;i<MAX_JOYSTICK_NUM;i++) {
		stick[i]->numAxis = 0;
		stick[i]->numButton = 0;
		stick[i]->numHatSwitch = 0;
	}
#ifdef _WIN32
	for (int i=0;i<numDevices()&&i<MAX_JOYSTICK_NUM;i++) {
		joy[i].dwSize = sizeof(JOYINFOEX);
		joy[i].dwFlags = JOY_RETURNALL;//JOY_RETURNBUTTONS | JOY_RETURNPOV | JOY_RETURNX | JOY_RETURNY;
		if (joyGetPosEx(i,&joy[i]) == JOYERR_NOERROR) {
			joyGetDevCaps(i,&joy_caps[i],sizeof(JOYCAPS));
			for (int j=0;j<joy_caps[i].wNumButtons;j++) {
				if (joy[i].dwButtons & (1<<j)) {
					stick[i]->button[stick[i]->numButton] = true;
				} else {
					stick[i]->button[stick[i]->numButton] = false;
				}
				stick[i]->numButton++;
			}
			
			if (joy_caps[i].wCaps & JOYCAPS_HASPOV) {
				float min,max,scaledMin,scaledMax;
				scaledMin = min = 0;
				scaledMax = max = 36000;
				double scaled=(((joy[i].dwPOV - min) * (scaledMax - scaledMin) / (max - min)) + scaledMin)/100.0;
				if(scaled<-0.001 || 359.999<scaled)
				{
					stick[i]->hatSwitch[stick[i]->numHatSwitch] = 0;
				}
				else
				{
					stick[i]->hatSwitch[stick[i]->numHatSwitch] = 1+(int)((scaled+22.5)/45.0);
				}
				stick[i]->numHatSwitch++;
			}
			
			PPJoyReaderAxis axis;
			{
				axis.init(joy_caps[i].wXmin,joy_caps[i].wXmax,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].dwXpos;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = axis.value;
				stick[i]->numAxis++;
			}
			{
				axis.init(joy_caps[i].wYmin,joy_caps[i].wYmax,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].dwYpos;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = axis.value;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASZ) {
				axis.init(joy_caps[i].wZmin,joy_caps[i].wZmax,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].dwZpos;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = axis.value;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASR) {
				axis.init(joy_caps[i].wRmin,joy_caps[i].wRmax,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].dwRpos;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = axis.value;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASU) {
				axis.init(joy_caps[i].wUmin,joy_caps[i].wUmax,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].dwUpos;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = axis.value;
				stick[i]->numAxis++;
			}
			if (joy_caps[i].wCaps & JOYCAPS_HASV) {
				axis.init(joy_caps[i].wVmin,joy_caps[i].wVmax,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].dwVpos;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = axis.value;
				stick[i]->numAxis++;
			}
		}
	}
#endif

#ifdef __MACOSX_APP__
	for(int i=0;i<numDevices()&&i<MAX_JOYSTICK_NUM;i++) {
		joy[i].Read();
		for(int j=0;j<PPJoyReaderMaxNumAxis;j++) {
			if(joy[i].axis[j].exist!=0) {
				PPJoyReaderAxis axis;
				axis.init(joy[i].axis[j].min,joy[i].axis[j].max,stick[i]->axisCalibMin[stick[i]->numAxis],stick[i]->axisCalibMax[stick[i]->numAxis]);
				axis.value = joy[i].axis[j].value;
				axis.calibCenter = stick[i]->axisCalibCenter[stick[i]->numAxis];
				stick[i]->axis[stick[i]->numAxis] = axis.GetCalibratedValue();
				stick[i]->axisRaw[stick[i]->numAxis] = joy[i].axis[j].value;
				stick[i]->numAxis++;
			}
		}
		for(int j=0;j<PPJoyReaderMaxNumButton;j++) {
			if(joy[i].button[j].exist!=0) {
				if (joy[i].button[j].value != 0) {
					stick[i]->button[stick[i]->numButton] = true;
				} else {
					stick[i]->button[stick[i]->numButton] = false;
				}
				stick[i]->numButton++;
			}
		}
		for(int j=0;j<PPJoyReaderMaxNumHatSwitch;j++) {
			if(joy[i].hatSwitch[j].exist!=0) {
				stick[i]->hatSwitch[stick[i]->numHatSwitch] = joy[i].hatSwitch[j].GetDiscreteValue();
				stick[i]->numHatSwitch++;
			}
		}
	}
#endif

	for (int i=0;i<numDevices()&&i<MAX_JOYSTICK_NUM;i++) {
		stick[i]->calibration();
	}
}

#pragma mark -

#include "PPLuaScript.h"

static int functionReset(lua_State* L)
{
	//PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPGameStick* m = PPGameStick::stickInstance;//s->userdata;
	m->reset();
	return 0;
}

void PPGameStick::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
	s->makeObjectTable((PPObject**)stick,MAX_JOYSTICK_NUM,name);
	//PPObject::openLibrary(s,name);
	s->openModule(name,this,0,superclass);
		s->addCommand("reset",functionReset);
	s->closeModule();
}

static int funcNumAxis(lua_State* L)
{
//	PPGameStick* o = (PPGameStick*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPGameStickElement* m = (PPGameStickElement*)s->userdata;
	lua_pushinteger(L,m->numAxis);
	return 1;
}

static int funcAxis(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPGameStickElement* m = (PPGameStickElement*)s->userdata;
	float value = 0;
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numAxis) {
			if (s->argCount > 3) {
				PPJoyReaderAxis axis;
				axis.init(m->axisMin[s->integer(0)-1],m->axisMax[s->integer(0)-1]
					,s->number(2,m->axisMin[s->integer(0)-1])
					,s->number(3,m->axisMax[s->integer(0)-1]));
				axis.calibCenter = s->number(1,m->axisRaw[s->integer(0)-1]);
				axis.value = m->axisRaw[s->integer(0)-1];
				value = axis.GetCalibratedValue();
			} else
			if (s->argCount > 1 && s->isTable(L,1)) {
				PPJoyReaderAxis axis;
				axis.init(m->axisMin[s->integer(0)-1],m->axisMax[s->integer(0)-1]
					,s->tableNumber(L,1,"calibMin",m->axisMin[s->integer(0)-1])
					,s->tableNumber(L,1,"calibMax",m->axisMax[s->integer(0)-1]));
				axis.calibCenter = s->tableNumber(L,1,"calibCenter",m->axisRaw[s->integer(0)-1]);
				axis.value = m->axisRaw[s->integer(0)-1];
				value = axis.GetCalibratedValue();
			} else {
				value = m->axis[s->integer(0)-1];
			}
		}
	}
	lua_pushnumber(L,value);
	return 1;
}

static int funcAxisInfo(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPGameStickElement* m = (PPGameStickElement*)s->userdata;
	float value = 0;
	float min = 0;
	float max = 0;
	float cmin = 0;
	float cmax = 0;
	float center = 0;
	if (s->argCount > 0) {
		if (s->argCount > 1 && s->isTable(L,1)) {
			m->axisCalibMin[s->integer(0)-1] = s->tableNumber(L,1,"calibMin",m->axisCalibMin[s->integer(0)-1]);
			m->axisCalibMax[s->integer(0)-1] = s->tableNumber(L,1,"calibMax",m->axisCalibMax[s->integer(0)-1]);
			m->axisCalibCenter[s->integer(0)-1] = s->tableNumber(L,1,"calibCenter",m->axisCalibCenter[s->integer(0)-1]);
		}
		if (s->integer(0) >= 1 && s->integer(0) <= m->numAxis) {
			value = m->axisRaw[s->integer(0)-1];
			min = m->axisMin[s->integer(0)-1];
			max = m->axisMax[s->integer(0)-1];
			cmin = m->axisCalibMin[s->integer(0)-1];
			cmax = m->axisCalibMax[s->integer(0)-1];
			center = m->axisCalibCenter[s->integer(0)-1];
		}
	}
	lua_createtable(L, 0, 6);
	lua_pushnumber(L,value);
	lua_setfield(L, -2, "value");
	lua_pushnumber(L,min);
	lua_setfield(L, -2, "min");
	lua_pushnumber(L,max);
	lua_setfield(L, -2, "max");
	lua_pushnumber(L,cmax);
	lua_setfield(L, -2, "calibMax");
	lua_pushnumber(L,cmin);
	lua_setfield(L, -2, "calibMin");
	lua_pushnumber(L,center);
	lua_setfield(L, -2, "calibCenter");
	return 1;
}

static int funcNumButton(lua_State* L)
{
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	lua_pushinteger(L,m->numButton);
	return 1;
}

static int funcButton(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	bool value = false;
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numButton) {
			value = m->button[s->integer(0)-1];
		}
	}
	lua_pushboolean(L,value);
	return 1;
}

static int funcNumHatSwitch(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	lua_pushinteger(L,m->numHatSwitch);
	return 1;
}

static int funcHatSwitch(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	int value = 0;
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numHatSwitch) {
			value = m->hatSwitch[s->integer(0)-1];
		}
	}
	lua_pushinteger(L,value);
	return 1;
}

static int funcStopAxisCalibration(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numAxis) {
			m->axisCalib[s->integer(0)-1] = false;
			m->saveCalibration();
		}
	}
	return 0;
}

static int funcCancelAxisCalibration(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numAxis) {
			int i=(int)s->integer(0)-1;
			m->axisCalibMin[i] = m->_axisCalibMin[i];
			m->axisCalibMax[i] = m->_axisCalibMax[i];
			m->axisCalibCenter[i] = m->_axisCalibCenter[i];
			m->axisCalib[i] = false;
		}
	}
	return 0;
}

static int funcStartAxisCalibration(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numAxis) {
			int i=(int)s->integer(0)-1;
			m->_axisCalibMin[i] = m->axisCalibMin[i];
			m->_axisCalibMax[i] = m->axisCalibMax[i];
			m->_axisCalibCenter[i] = m->axisCalibCenter[i];
			m->axisCalibMin[i] = m->axisRaw[i];
			m->axisCalibMax[i] = m->axisRaw[i];
			m->axisCalib[i] = true;
		}
	}
	return 0;
}

static int funcResetAxisCalibration(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->integer(0) >= 1 && s->integer(0) <= m->numAxis) {
			int i=(int)s->integer(0)-1;
			m->axisCalibMin[i] = m->axisMin[i];
			m->axisCalibMax[i] = m->axisMax[i];
			m->axisCalibCenter[i] = (m->axisMin[i]+m->axisMax[i])/2;
			m->_axisCalibMin[i] = m->axisCalibMin[i];
			m->_axisCalibMax[i] = m->axisCalibMax[i];
			m->_axisCalibCenter[i] = m->axisCalibCenter[i];
		}
	}
	return 0;
}

static int functionProductID(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	lua_pushinteger(L,m->productID);
	return 1;
}

static int functionVendorID(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	lua_pushinteger(L,m->productID);
	return 1;
}

static int functionIndex(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	lua_pushinteger(L,m->index);
	return 1;
}

static int functionExist(lua_State* L)
{
	PPGameStickElement* m = (PPGameStickElement*)PPLuaScript::UserData(L);
	lua_pushboolean(L,m->exist);
	return 1;
}

void PPGameStickElement::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
//	PPObject::openLibrary(s,name);
	s->openModule(name,this,0,superclass);
		s->addCommand("numAxis",funcNumAxis);
		s->addCommand("axis",funcAxis);
		s->addCommand("axisInfo",funcAxisInfo);
		s->addCommand("numButton",funcNumButton);
		s->addCommand("button",funcButton);
		s->addCommand("numHatSwitch",funcNumHatSwitch);
		s->addCommand("hatSwitch",funcHatSwitch);
		s->addCommand("stopAxisCalibration",funcStopAxisCalibration);
		s->addCommand("cancelAxisCalibration",funcCancelAxisCalibration);
		s->addCommand("startAxisCalibration",funcStartAxisCalibration);
		s->addCommand("resetAxisCalibration",funcResetAxisCalibration);
		s->addCommand("productID",functionProductID);
		s->addCommand("vendorID",functionVendorID);
		s->addCommand("index",functionIndex);
		s->addCommand("exist",functionExist);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
