/*-----------------------------------------------------------------------------------------------
	名前	PPGameStick.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGameStick_h__
#define __PPGameStick_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __MACOSX_APP__
#include "PPJoyReader.h"
#endif

#include "PPGameStickClass.h"

#define MAX_JOYSTICK_NUM 4
#define MAX_JOYSTICK_AXIS 6
#define MAX_JOYSTICK_BUTTON 32
#define MAX_JOYSTICK_HAT_SWITCH 4

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;

class PPGameStickElement : public PPObject {
public:
	PPGameStickElement(PPWorld* world) : PPObject(world) {}

	int index;
	bool exist;

	int numAxis;
	float axis[MAX_JOYSTICK_AXIS];
	float axisRaw[MAX_JOYSTICK_AXIS];
	float axisMin[MAX_JOYSTICK_AXIS];
	float axisMax[MAX_JOYSTICK_AXIS];
	float axisCalibMin[MAX_JOYSTICK_AXIS];
	float axisCalibMax[MAX_JOYSTICK_AXIS];
	float axisCalibCenter[MAX_JOYSTICK_AXIS];

	float _axisCalibMin[MAX_JOYSTICK_AXIS];
	float _axisCalibMax[MAX_JOYSTICK_AXIS];
	float _axisCalibCenter[MAX_JOYSTICK_AXIS];

	bool axisCalib[MAX_JOYSTICK_AXIS];
	int numButton;
	bool button[MAX_JOYSTICK_BUTTON];
	int numHatSwitch;
	int hatSwitch[MAX_JOYSTICK_HAT_SWITCH];
	
	unsigned short productID;
	unsigned short vendorID;

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);
	
	void calibration() {
		for (int i=0;i<MAX_JOYSTICK_AXIS;i++) {
			if (axisCalib[i]) {
				if (axisCalibMin[i] > axisRaw[i]) axisCalibMin[i] = axisRaw[i];
				if (axisCalibMax[i] < axisRaw[i]) axisCalibMax[i] = axisRaw[i];
				axisCalibCenter[i] = axisRaw[i];
			}
		}
	}
	
	void saveCalibration();
	void loadCalibration();
};

class PPGameStick : public PPObject {
public:
	PPGameStick(PPWorld* world);
	virtual ~PPGameStick();
	
	static PPGameStick* stickInstance;
	
	virtual void start();
	virtual void reset();
	
	virtual void stepIdle();
	
	virtual void read();
	
	virtual int numDevices();

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

	PPGameStickElement* stick[MAX_JOYSTICK_NUM];

private:
#ifdef _WIN32
	JOYCAPS joy_caps[MAX_JOYSTICK_NUM];
	JOYINFOEX joy[MAX_JOYSTICK_NUM];
#endif

#ifdef __MACOSX_APP__
	int numJoystick;
	PPJoyReader joy[MAX_JOYSTICK_NUM];
#endif
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
