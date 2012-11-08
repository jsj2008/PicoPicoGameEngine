/*-----------------------------------------------------------------------------------------------
	名前	QBGameController.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "QBGameController.h"
#if TARGET_OS_IPHONE
#import "PPGameViewController.h"
#endif

@implementation QBGameController

- (PPGameTextureInfo*)textures
{
	[self game];
	return game->TextureInfo();
}

#include <sys/types.h>
#include <sys/sysctl.h>

- (BOOL)platform3GSLater
{
	BOOL r = YES;
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char *)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	/*
	Possible values:
	"iPhone1,1" = iPhone 1G
	"iPhone1,2" = iPhone 3G
	"iPhone2,1" = iPhone 3GS
	"iPod1,1"   = iPod touch 1G
	"iPod2,1"   = iPod touch 2G
	*/
	// NSString *platform = [NSString stringWithUTF8String:machine];

	if (strcmp(machine,"iPhone1,1") == 0
	 || strcmp(machine,"iPhone1,2") == 0
	 || strcmp(machine,"iPod1,1") == 0
	 || strcmp(machine,"iPod2,1") == 0) {
		r = NO;
	}

	free(machine);
	
	return r;
}

- (bool)start
{
	[self game];
	later3GS = [self platform3GSLater];
	return true;
}

- (bool)exit
{
	if (game) {
//NSLog(@"delete game");
		delete game;
	}
	game = NULL;
	return true;
}

- (unsigned long)getKey
{
	unsigned long key = 0;
	
	key |= [_view staticButton]; 

	return key;
}

- (int)idle
{
	if (game) {
		game->blinkCounter ++;
		game->setTouchCount(0);
#if TARGET_OS_IPHONE
		NSMutableSet* touchesSet = [_view touchesSet];
		NSEnumerator* e = [touchesSet objectEnumerator];
		UITouch* t;
		while (t = [e nextObject]) {
			CGPoint location = [t locationInView:_view];
			game->setTouchPosition(location.x,location.y);
		}
#else
		if (_view.touchScreen) {
			game->setTouchPosition(_view.touchLocation.x,_view.touchLocation.y);
		}
		if (_view.touchUpScreen) {
			_view.touchScreen = false;
			_view.touchUpScreen = false;
		}
#endif
	}
	unsigned long key = [self getKey];
	if (game) {
		game->screenWidth = screenWidth;
		game->screenHeight = screenHeight;
		game->gameIdle(key);
//		if (game->exitGame) {
//			game->exitGame = false;
//			[self closeGame];
//		}
	}
	return 0;
}

- (QBGame*)game
{
	return nil;
}

#ifdef __GAMEKIT__

- (void)showLeaderboard
{
	PPGameViewController* c = (PPGameViewController*)controller;
	if ([c respondsToSelector:@selector(showLeaderboard)]) {
		[c showLeaderboard];
	}
}

- (void)showAchievements
{
	PPGameViewController* c = (PPGameViewController*)controller;
	if ([c respondsToSelector:@selector(showAchievements)]) {
		[c showAchievements];
	}
}

- (void)setScore:(int)score forCategory:(NSString*)category
{
	PPGameViewController* c = (PPGameViewController*)controller;
	if ([c respondsToSelector:@selector(reportScore:forCategory:)]) {
		[c reportScore:(int64_t)score forCategory:category];
	}
}

- (void)setAchieve:(NSString*)identifier withPercent:(float)percent
{
	PPGameViewController* c = (PPGameViewController*)controller;
	if ([c respondsToSelector:@selector(reportAchievementIdentifier:percentComplete:)]) {
		[c reportAchievementIdentifier:identifier percentComplete:percent];
	}
}

#endif

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
