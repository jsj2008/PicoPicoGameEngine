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

class PPGameSprite;

/*-----------------------------------------------------------------------------------------------
 クラス
 -----------------------------------------------------------------------------------------------*/

@class PPGame;

@protocol PPGameViewDelegate<NSObject>

- (PPGame*)gameController;

@end

@interface PPGameView : UIView {
	
	PPGameSprite*		_projector;
  
	CGPoint				touchLocation;
	bool				touchScreen;
  
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
  GLuint viewRenderbuffer, viewFramebuffer;
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
