//
//  PicoGameAppDelegate.m
//  PicoTest
//
//  Created by 山口 博光 on 12/06/27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "PicoGameAppDelegate.h"
#import <PicoGame/QBSoundMac.h>
#import <PicoGame/PPSensoriOS.h>
#import <PicoGame/PPGameUtil.h>

@implementation PicoGameAppDelegate

@synthesize window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
	QBSound_SetCategory("ambient");
	QBSoundMac* snd = new QBSoundMac(5);
	if (snd) {
		snd->Init();
		snd->Reset();
		snd->setMasterVolume(0.5);
	}
	PPSensor::instance = new PPSensoriOS();

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	PPSync();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	PPSync();
}

@end
