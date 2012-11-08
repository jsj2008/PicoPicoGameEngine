/*-----------------------------------------------------------------------------------------------
	名前	PicoPicoGamesHDAppDelegate.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PicoPicoGamesHDAppDelegate.h"
#import "QBSoundMac.h"
#import "PPSensoriOS.h"

@implementation PicoPicoGamesHDAppDelegate

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
	viewController = [[PPGameViewController alloc] initWithNibName:@"PPGameViewControllerHD" bundle:nil];
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

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
