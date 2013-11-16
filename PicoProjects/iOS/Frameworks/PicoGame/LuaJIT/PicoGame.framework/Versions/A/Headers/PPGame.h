/*-----------------------------------------------------------------------------------------------
	名前	PPGame.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import <Foundation/Foundation.h>
#import "PPGamePoly.h"
#import "PPGameSprite.h"
#if TARGET_OS_IPHONE
#import "PPGameView.h"
#else
#import "PPGameView_App.h"
#endif
#import "QBGame.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

@interface PPGame : NSObject <PPGameViewDelegate> {
#if TARGET_OS_IPHONE
	UIViewController* controller;
#else
	id controller;
#endif
	bool initFlag;
	
	int screenWidth;
	int screenHeight;
	
	int windowWidth;
	int windowHeight;

	QBGame* game;
	BOOL later3GS;
	
	unsigned long staticButton;
#if TARGET_OS_IPHONE
	NSMutableSet* touchesSet;
#else
	CGPoint touchLocation;
	bool touchScreen;
	bool touchUpScreen;
#endif
}

@property (nonatomic,assign) bool initFlag;
@property (nonatomic,assign) int screenWidth;
@property (nonatomic,assign) int screenHeight;

@property (nonatomic,assign) int windowWidth;
@property (nonatomic,assign) int windowHeight;

@property (nonatomic,assign) unsigned long staticButton;
#if TARGET_OS_IPHONE
@property (nonatomic,retain) NSMutableSet* touchesSet;
#else
@property (nonatomic,assign) CGPoint touchLocation;
@property (nonatomic,assign) bool touchScreen;
@property (nonatomic,assign) bool touchUpScreen;
#endif

- (bool)start;
- (int)idle;
- (int)draw;
- (int)drawPost;
- (bool)exit;
- (void)resignActive;
- (void)becomeActive;

- (PPGameTextureInfo*)textures;

- (int)drawOT;
- (void)textureIdle;

- (QBGame*)game;

- (int)polySize;

- (void)reloadData;

- (void)setAccelerometerX:(float)x Y:(float)y Z:(float)z;
- (void)setGyroX:(float)x Y:(float)y Z:(float)z;
- (void)setMagnetometerX:(float)x Y:(float)y Z:(float)z;

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
