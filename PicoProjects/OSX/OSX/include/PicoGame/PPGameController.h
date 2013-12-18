/*-----------------------------------------------------------------------------------------------
	名前	PPGameController.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import "PPGameSprite.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

@class PPGameView;
@class PPGame;

@interface PPGameController : NSResponder {

	BOOL isInFullScreenMode;
	BOOL _isInitialized;
	
	// full-screen mode
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
	NSWindow *fullScreenWindow;
	PPGameView *fullScreenView;
#else
	CGLContextObj fullScreenContextObj;
#endif
	
	// window mdoe
	IBOutlet PPGameView *openGLView;
	IBOutlet NSWindow* window;
	
//	PPGame *scene;
	BOOL isAnimating;
	//CFAbsoluteTime renderTime;

//	PPGameSprite* projector;
	PPGame* _game;
}

@property (nonatomic,retain) IBOutlet PPGame* game;

- (IBAction)goFullScreen:(id)sender;
- (IBAction)goWindow:(id)sender;

- (IBAction)startGame:(id)sender;

//- (PPGame*)scene;

- (IBAction)reloadData:(id)sender;
- (IBAction)changeWindowSize:(id)sender;

- (IBAction)deleteAllTexture:(id)sender;
- (IBAction)reloadAllTexture:(id)sender;

- (IBAction)changeDensity:(id)sender;

- (IBAction)changeFullScreen:(id)sender;

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
