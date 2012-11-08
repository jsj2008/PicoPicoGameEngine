/*-----------------------------------------------------------------------------------------------
	名前	PPSensoriOS.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPSensoriOS.h"
#include <stdlib.h>
#import <CoreMotion/CoreMotion.h>

static CMMotionManager* motionManager=NULL;

PPSensoriOS::PPSensoriOS()
{
	[motionManager release];
	motionManager = [[CMMotionManager alloc] init];
}

PPSensoriOS::~PPSensoriOS()
{
	[motionManager release];
}

void PPSensoriOS::startAccelerometer(float interval)
{
	motionManager.accelerometerUpdateInterval = interval;
	NSOperationQueue *opQueue = [[[NSOperationQueue alloc] init] autorelease];
	[motionManager startAccelerometerUpdatesToQueue:opQueue
					withHandler:^(CMAccelerometerData *data, NSError *error) {
						dispatch_async(dispatch_get_main_queue(), ^{
							accelerometer.x = data.acceleration.x;
							accelerometer.y = data.acceleration.y;
							accelerometer.z = data.acceleration.z;
						});
					}];
}

void PPSensoriOS::stopAccelerometer()
{
	[motionManager stopAccelerometerUpdates];
	accelerometer.x = 0;
	accelerometer.y = 0;
	accelerometer.z = 0;
}

bool PPSensoriOS::accelerometerAvailable()
{
	return motionManager.accelerometerAvailable;
}

PPVertex PPSensoriOS::getAccelerometer()
{
	return accelerometer;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
