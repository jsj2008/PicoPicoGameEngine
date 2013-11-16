/*-----------------------------------------------------------------------------------------------
	名前	PPGameView.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "PPGameSprite.h"
#import "QBSound.h"
#import "PPGameButton.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

@class PPGame;

@protocol PPGameViewDelegate<NSObject>

- (PPGame*)gameController;

@end

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface PPGameView : UIView {
	
	PPGameSprite*		_projector;
//	PPGame*				_game;

	CGPoint				touchLocation;
	bool				touchScreen;

    /* The pixel dimensions of the backbuffer */
    GLint				width;
    GLint				height;
	
	NSMutableSet*		touchesSet;

	NSInteger animationFrameInterval;
    BOOL animating;
    id displayLink;
    BOOL displayLinkSupported;
	
	int arrowKeyTouchNo;

	unsigned long staticKey;
	
@private
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
    GLuint viewRenderbuffer, viewFramebuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
    
    NSTimer *animationTimer;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic,readonly) CGPoint touchLocation;
@property (nonatomic,readonly) bool touchScreen;
@property (nonatomic,readonly)  GLint width;
@property (nonatomic,readonly)  GLint height;
@property (nonatomic,readonly)  PPGameSprite* projector;
//@property (nonatomic,retain) IBOutlet PPGame* game;
@property (nonatomic,assign) unsigned long staticKey;
@property (nonatomic,assign) IBOutlet id<PPGameViewDelegate> delegate;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView;

- (unsigned long)staticButton;
- (NSMutableSet*)touchesSet;

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
