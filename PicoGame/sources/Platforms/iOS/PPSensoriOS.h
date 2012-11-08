/*-----------------------------------------------------------------------------------------------
	名前	PPSensoriOS.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef PicoPicoGames_PPSensoriOS_h
#define PicoPicoGames_PPSensoriOS_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPSensor.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPSensoriOS : public PPSensor {
public:
	PPSensoriOS();
	virtual ~PPSensoriOS();
	
	virtual void startAccelerometer(float interval);
	virtual void stopAccelerometer();
	virtual bool accelerometerAvailable();
	virtual PPVertex getAccelerometer();
	PPVertex accelerometer;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/