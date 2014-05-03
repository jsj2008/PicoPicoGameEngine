/*-----------------------------------------------------------------------------------------------
	名前	PPSensor.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef PicoPicoGames_PPSensor_h
#define PicoPicoGames_PPSensor_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameGeometry.h"
#include <stdlib.h>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;

class PPSensor {
public:
	static PPSensor* sharedManager();
	static PPSensor* instance;

	PPSensor();
	virtual ~PPSensor();
	
	virtual void startAccelerometer(float interval);
	virtual void stopAccelerometer();
	virtual bool accelerometerAvailable();
	virtual PPVertex getAccelerometer();
	
	static void openAccelerometerLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
