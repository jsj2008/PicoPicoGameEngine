/*-----------------------------------------------------------------------------------------------
	名前	PPGame.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PPGame.h"
#import "PPGameStart.h"
#import "PPGamePoly.h"
#import "PPGameSprite.h"
#if TARGET_OS_IPHONE
#import "PPGameView.h"
#else
#import "PPGameView_App.h"
#endif

@interface PPGame ()  <PPGameViewDelegate>

@end

@implementation PPGame

@synthesize initFlag;
@synthesize screenWidth;
@synthesize screenHeight;
@synthesize windowWidth;
@synthesize windowHeight;
@synthesize staticButton;
#if TARGET_OS_IPHONE
@synthesize touchesSet;
#else
@synthesize touchLocation;
@synthesize touchScreen;
@synthesize touchUpScreen;
#endif

- (id)init
{
	if ((self = [super init]) != nil) {
		self.windowWidth = 320;
		self.windowHeight = 480;
		[self setupResourceFolder];
	}
	return self;
}

- (id)retain
{
	return [super retain];
}

- (void)dealloc
{
	[self exit];
#if TARGET_OS_IPHONE
	self.touchesSet = nil;
#else
#endif
	[super dealloc];
}

- (int)draw
{
	if ([self game]) {
		[self game]->DrawGL();
	}
	return 0;
}

- (int)drawPost
{
	return 0;
}

- (int)drawOT
{
	[self drawPost];
	return 0;
}

- (void)resignActive
{
}

- (void)becomeActive
{
}

- (void)textureIdle
{
	PPGameSprite* g = self.game->projector;
	if (g == NULL) return;
	
	if ([self textures]) {
		if (!g->initLoadTex) {
			if (g->textureManager->setTextureList([self textures]) == 0) {
				g->initLoadTex = true;
			}
		}
	}
}

#define PPGAME_MAX_POLY (10000*10)

- (int)polySize
{
	return PPGAME_MAX_POLY;
}

#pragma mark -

- (PPGameTextureInfo*)textures
{
	return self.game->TextureInfo();
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
		delete game;
	}
	game = NULL;
	return true;
}

- (unsigned long)getKey
{
	unsigned long key = 0;
	
	key |= self.staticButton;

	return key;
}

- (int)idle
{
	if (self.game) {
		self.game->blinkCounter ++;
		self.game->setTouchCount(0);
#if TARGET_OS_IPHONE
		NSEnumerator* e = [self.touchesSet objectEnumerator];
		NSValue* t;
		while (t = [e nextObject]) {
			CGPoint location = [t CGPointValue];
			game->setTouchPosition(location.x,location.y);
		}
#else
		if (self.touchScreen) {
			self.game->setTouchPosition(self.touchLocation.x,self.touchLocation.y);
		}
		if (self.touchUpScreen) {
			self.touchScreen = false;
			self.touchUpScreen = false;
		}
#endif
	}
	unsigned long key = [self getKey];
	if (self.game) {
		self.game->screenWidth = screenWidth;
		self.game->screenHeight = screenHeight;
		self.game->gameIdle(key);
	}
	return 0;
}

#pragma mark -

- (void)setupResourceFolder
{
	BOOL useResFolder = NO;
	if ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"useResourcesFolder"]) {
		useResFolder = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"useResourcesFolder"] boolValue];
	}
	if (useResFolder) {
		NSString* p = [[[[NSBundle mainBundle] bundlePath] stringByDeletingLastPathComponent] stringByAppendingPathComponent:@"Resources"];
		PPGameSetDataPath([p fileSystemRepresentation]);
		PPGameSetDataSubPath([p fileSystemRepresentation]);
	}
}

- (QBGame*)game
{
	if (game == NULL) {
		game = PPGameStart();
	}
	return game;
}

- (void)setAccelerometerX:(float)x Y:(float)y Z:(float)z
{
	if (game) {
		game->accelerometerValue[0] = x;
		game->accelerometerValue[1] = y;
		game->accelerometerValue[2] = z;
	}
}

- (void)setGyroX:(float)x Y:(float)y Z:(float)z
{
	if (game) {
		game->gyroValue[0] = x;
		game->gyroValue[1] = y;
		game->gyroValue[2] = z;
	}
}

- (void)setMagnetometerX:(float)x Y:(float)y Z:(float)z
{
	if (game) {
		game->magnetometerValue[0] = x;
		game->magnetometerValue[1] = y;
		game->magnetometerValue[2] = z;
	}
}

- (void)reloadData
{
	if (game) {
		game->reloadData();
	}
}

- (PPGame*)gameController
{
	return self;
}

- (void)disableIO
{
	if (game) {
    game->disableIO();
  }
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
