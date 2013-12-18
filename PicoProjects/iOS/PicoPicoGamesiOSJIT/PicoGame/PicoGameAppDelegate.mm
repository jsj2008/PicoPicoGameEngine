#import "PicoGameAppDelegate.h"
#import <PicoGame/PPGameUtil.h>

@implementation PicoGameAppDelegate

@synthesize window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
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
