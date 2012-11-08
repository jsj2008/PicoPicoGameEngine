/*-----------------------------------------------------------------------------------------------
	名前	PPSensorAndroid.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef PicoPicoGames_PPSensorAndroid_h
#define PicoPicoGames_PPSensorAndroid_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPSensor.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPSensorAndroid : public PPSensor {
public:
	PPSensorAndroid();
	virtual ~PPSensorAndroid();
	
	virtual void startAccelerometer(float interval);
	virtual void stopAccelerometer();
	virtual bool accelerometerAvailable();
	virtual PPVertex getAccelerometer();
	PPVertex accelerometer;
	
	bool _accelerometerEnabled;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
