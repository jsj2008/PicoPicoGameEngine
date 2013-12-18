/*-----------------------------------------------------------------------------------------------
	名前	PPGameView_App.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import "PPGameSprite.h"
#import "PPGameController.h"

@class PPGame;
@class PPGameController;

@protocol PPGameViewDelegate<NSObject>

- (PPGame*)gameController;

@end

@interface PPGameView : NSView {
	IBOutlet PPGameController* controller;
	float deepZ;
//	PPGameSprite* projector;
	PPGame* game;
	float adx;
	float ady;
	unsigned long staticKey;
	CGPoint	touchLocation;
	bool	touchScreen;
	bool	touchUpScreen;
	NSMutableSet* touchesSet;
	int _initialWait;

	NSOpenGLContext *openGLContext;
	NSOpenGLPixelFormat *pixelFormat;
	CVDisplayLinkRef displayLink;
	BOOL isAnimating;
	
	NSRect viewPort;
	GLint animationFrameInterval;
	int interval;
	
	bool fullScreen;
	bool reloadTexture;

	PPGameSprite* _projector;
}

- (id)initWithFrame:(NSRect)frameRect;
- (id)initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context;

- (NSOpenGLContext*)openGLContext;

- (void)setMainController:(PPGameController*)theController;

- (void)drawView;
- (void)render;

- (void)startAnimation;
- (void)stopAnimation;

- (void)swapProjector:(PPGameView*)view;

//- (void)setViewPort:(NSRect)rect;

@property (nonatomic,readonly) PPGameSprite* projector;
@property (nonatomic,retain) PPGame* game;
@property (nonatomic,readonly) CGPoint touchLocation;
@property (nonatomic,assign) bool touchScreen;
@property (nonatomic,assign) bool touchUpScreen;
@property (nonatomic,assign) bool fullScreen;
@property (nonatomic,assign) bool reloadTexture;

- (unsigned long)staticButton;

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
