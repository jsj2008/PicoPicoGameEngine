/*-----------------------------------------------------------------------------------------------
	名前	PPGameCocoa.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PPGameCocoa.h"
#import "PPGameStart.h"

@implementation PPGameCocoa

- (void)dealloc
{
NSLog(@"dealloc PPGameCocoa");
	[super dealloc];
}

- (QBGame*)game
{
	if (game == NULL) {
		game = PPGameStart();
		BOOL useResFolder = NO;
		if ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"useResourcesFolder"]) {
			useResFolder = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"useResourcesFolder"] boolValue];
		}
		if (useResFolder) {
			NSString* p = [[[[NSBundle mainBundle] bundlePath] stringByDeletingLastPathComponent] stringByAppendingPathComponent:@"Resources"];
			PPGameSetDataPath([p fileSystemRepresentation]);
		}
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

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
