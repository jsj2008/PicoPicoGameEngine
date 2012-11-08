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

@implementation PicoGameAppDelegate

@synthesize window;
@synthesize viewController;

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

	viewController = [[PPGameViewController alloc] initWithNibName:@"PPGameViewController" bundle:nil];
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
//	[glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
//	[glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[viewController exitGame];
//	[glView stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	if ([viewController isKindOfClass:[PPGameViewController class]]) {
		[viewController exitGame];
	}

	[[NSNotificationCenter defaultCenter] removeObserver: self];
	[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
	
	//[PPGameBGM stop];
	exit(0);
}

- (void)dealloc
{
	[window release];
//	[glView release];
	[super dealloc];
}

@end
