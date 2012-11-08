/*-----------------------------------------------------------------------------------------------
	名前	PPSensorAndroid.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPSensorAndroid.h"
#include <stdlib.h>

PPSensorAndroid::PPSensorAndroid()
{
	_accelerometerEnabled = false;
}

PPSensorAndroid::~PPSensorAndroid()
{
}

void PPSensorAndroid::startAccelerometer(float interval)
{
	_accelerometerEnabled = true;
}

void PPSensorAndroid::stopAccelerometer()
{
	_accelerometerEnabled = false;
	accelerometer.x = 0;
	accelerometer.y = 0;
	accelerometer.z = 0;
}

bool PPSensorAndroid::accelerometerAvailable()
{
	return true;
}

PPVertex PPSensorAndroid::getAccelerometer()
{
	return accelerometer;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
