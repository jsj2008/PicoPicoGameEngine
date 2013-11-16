/*-----------------------------------------------------------------------------------------------
	名前	PPJoyReader.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPJoyReader__
#define __PPJoyReader__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

const int PPJoyReaderMaxNumAxis=6;
const int PPJoyReaderMaxNumButton=32;
const int PPJoyReaderMaxNumHatSwitch=4;

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sysexits.h>
#include <IOKit/hid/IOHIDLib.h>
#include "PPGameStickClass.h"

class  PPJoyReader
{
public:
	static IOHIDManagerRef hidManager;
	static CFMutableArrayRef devArray;

	int joyId;
	IOHIDDeviceRef hidDev;
	char regPath[512];
	
	unsigned short productID;
	unsigned short vendorID;

	PPJoyReaderAxis axis[PPJoyReaderMaxNumAxis];
	PPJoyReaderButton button[PPJoyReaderMaxNumButton];
	PPJoyReaderHatSwitch hatSwitch[PPJoyReaderMaxNumHatSwitch];

	PPJoyReader();
	int SetUpInterface(int joyId,IOHIDDeviceRef hidDev);
	void Read(void);

protected:
	void AddAxis(int axisId,IOHIDElementRef elem,int min,int max,int scaledMin,int scaledMax);

public:
	static int SetUpJoystick(int &nJoystick,PPJoyReader joystick[],int maxNumJoystick);
};

int PPJoyReaderSetUpJoystick(int &nJoystick,PPJoyReader joystick[],int maxNumJoystick);

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
