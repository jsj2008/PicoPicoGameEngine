/*-----------------------------------------------------------------------------------------------
	名前	PPGameView.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "PPGame.h"
#import "PPGameView.h"
#if TARGET_OS_IPHONE
#import <GameController/GCController.h>
#endif

void __PPGame_Set3GSLater(int later);

#define USE_DEPTH_BUFFER 1

@interface PPGameView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@implementation PPGameView

@synthesize projector=_projector;
@synthesize animating;
@synthesize context;
@synthesize animationTimer;
@dynamic animationFrameInterval;
//@synthesize game=_game;
@synthesize width,height;
@synthesize touchLocation,touchScreen;
@synthesize staticKey;

- (PPGameSprite*)projector
{
	if (_projector==NULL) {
		_projector = new PPGameSprite();
	}
	return _projector;
}

- (PPGame*)game
{
	if ([self.delegate respondsToSelector:@selector(gameController)]) {
		return [self.delegate gameController];
	}
	return nil;
}

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {

        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

#if 0
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		if (!context) {
			context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
			__PPGame_Set3GSLater(0);
		}
#else        
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		__PPGame_Set3GSLater(0);
#endif
        if (!context || ![EAGLContext setCurrentContext:context]) {
			context = nil;
        }
        
        animationFrameInterval = 1;// / 60.0;
		
		animating = FALSE;
		displayLinkSupported = FALSE;
		displayLink = nil;

       // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
        // class is used as fallback when it isn't available.
        NSString *reqSysVer = @"3.1";
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
            displayLinkSupported = TRUE;

		if (self.projector != NULL) {
			self.projector->animationFrameInterval = animationFrameInterval;
		}

		if (QBSound::sharedSound()) {
			QBSound::sharedSound()->Reset();
		}
    }
    return self;
}

- (void)drawView {
	[EAGLContext setCurrentContext:context];

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glViewport(0, 0, width, height);

	PPGame*	_game = self.game;
	if (_game==nil) return;
	
	if (self.projector) {
		self.projector->InitBuffer([_game polySize]);
//		_game.view = self;
		self.projector->setWorld(_game.game);
//		[_game setView:self];

		//if (initGameFlag == false) {
		if (_game.initFlag == NO) {
			[_game start];
			_game.initFlag = YES;
		}
		
		_game.staticButton = [self staticButton];
		{
			NSMutableSet* set = [NSMutableSet set];
			NSEnumerator* e = [self.touchesSet objectEnumerator];
			UITouch* t;
			while (t = [e nextObject]) {
				CGPoint location = [t locationInView:self];
				location.x *= self.contentScaleFactor;
				location.y *= self.contentScaleFactor;
				[set addObject:[NSValue valueWithCGPoint:location]];
			}
			_game.touchesSet = set;
		}
		//	initGameFlag = true;
		//}
		_game.screenWidth = width;
		_game.screenHeight = height;
		if (self.projector) {
			[_game textureIdle];
		}
		self.projector->SetViewPort();
		self.projector->InitOT();

		if (self.projector) {
			self.projector->st.screenWidth = self.width;//160;
			self.projector->st.screenHeight = self.height;//416/2;
	//		self.projector->st.viewPort = PPRect(0,0,self.width,self.height);
	//		self.projector->st.windowSize.width = self.width;//160;
	//		self.projector->st.windowSize.height = self.height;//416/2;
			self.projector->st.renderbuffer = viewRenderbuffer;
			self.projector->st.framebuffer = viewFramebuffer;
			self.projector->ClearScreen2D(0.0f, 0.0f, 0.0f);
		}

    if (self.width == 0 || self.height == 0) {
    } else {
      if ([_game idle] != 0) {
        
      }
    }
		if (self.projector) {
			if ([_game draw] == 0) {
				self.projector->SetViewPort();
				[_game drawOT];
				self.projector->DrawOT();
				self.projector->ResetOT();
			}
			QBGame::blinkCounter++;
		}

		glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
		[context presentRenderbuffer:GL_RENDERBUFFER_OES];
		
		if (self.projector) {
			if (animationFrameInterval!=self.projector->animationFrameInterval) {
				[self setAnimationInterval:self.projector->animationFrameInterval];
			}
		}
		
		QBSound* snd = QBSound::sharedSound();
		if (snd) snd->idleSEMML();
	}
}

- (void)layoutSubviews {
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self drawView];
}

- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &width);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &height);

    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, width, height);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    return YES;
}

- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;

        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation {
	if (!animating) {
		if (displayLinkSupported) {
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.
			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		} else {
			self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView) userInfo:nil repeats:YES];
		}
		animating = YES;
	}
}


- (void)stopAnimation {
    if (animating) {
		if (displayLinkSupported) {
			[displayLink invalidate];
			displayLink = nil;
		} else {
			[self.animationTimer invalidate];
			self.animationTimer = nil;
		}
        animating = FALSE;
	}
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
    [animationTimer invalidate];
    animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationFrameInterval = interval;
//    if (animationTimer) {
        [self stopAnimation];
        [self startAnimation];
//    }
}

- (void)dealloc {
 	if (_projector) {
		delete _projector;
	}
	[self stopAnimation];
//	[game setView:nil];
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];
	[touchesSet release];
	
//	[game release];
	
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (touchesSet == nil) touchesSet = [[NSMutableSet set] retain];
	[touchesSet unionSet:touches];

    UITouch*	touch = [[event touchesForView:self] anyObject];
	touchLocation = [touch locationInView:self];
	touchScreen = true;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (touchesSet == nil) touchesSet = [[NSMutableSet set] retain];
	[touchesSet unionSet:touches];

    UITouch*	touch = [[event touchesForView:self] anyObject];
	touchLocation = [touch locationInView:self];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (touchesSet == nil) touchesSet = [[NSMutableSet set] retain];
	[touchesSet minusSet:touches];

    UITouch*	touch = [[event touchesForView:self] anyObject];
	touchLocation = [touch locationInView:self];
	touchScreen = false;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (touchesSet == nil) touchesSet = [[NSMutableSet set] retain];
	[touchesSet minusSet:touches];

    UITouch*	touch = [[event touchesForView:self] anyObject];
	touchLocation = [touch locationInView:self];
	touchScreen = false;
}

#import "iCadeState.h"

- (unsigned long)staticButton
{
	unsigned long key = 0;

	if (staticKey & iCadeJoystickUp) key |= PAD_UP;
	if (staticKey & iCadeJoystickDown) key |= PAD_DOWN;
	if (staticKey & iCadeJoystickLeft) key |= PAD_LEFT;
	if (staticKey & iCadeJoystickRight) key |= PAD_RIGHT;
	if (staticKey & iCadeButtonA) key |= PAD_A;
	if (staticKey & iCadeButtonB) key |= PAD_B;
	if (staticKey & iCadeButtonC) key |= PAD_C;
	if (staticKey & iCadeButtonD) key |= PAD_D;
	if (staticKey & iCadeButtonE) key |= PAD_E;
	if (staticKey & iCadeButtonF) key |= PAD_F;
	if (staticKey & iCadeButtonG) key |= PAD_G;
	if (staticKey & iCadeButtonH) key |= PAD_H;
	if (staticKey & iCadeButtonSetup) key |= PAD_SetUP;
	if (staticKey & iCadeButtonStart) key |= PAD_Start;

  if ([[[UIDevice currentDevice] systemVersion] floatValue]>=7.0) {
    if ([GCController controllers].count>0) {
      GCController* gc = [[GCController controllers] objectAtIndex:0];
      if (gc.gamepad.buttonA.pressed) key |= PAD_A;
      if (gc.gamepad.buttonB.pressed) key |= PAD_B;
      if (gc.gamepad.buttonX.pressed) key |= PAD_X;
      if (gc.gamepad.buttonY.pressed) key |= PAD_Y;
      if (gc.gamepad.leftShoulder.pressed) key |= PAD_L;
      if (gc.gamepad.rightShoulder.pressed) key |= PAD_R;
      if (gc.gamepad.dpad.up.pressed) key |= PAD_UP;
      if (gc.gamepad.dpad.down.pressed) key |= PAD_DOWN;
      if (gc.gamepad.dpad.left.pressed) key |= PAD_LEFT;
      if (gc.gamepad.dpad.right.pressed) key |= PAD_RIGHT;
    }
  }

	return key;
}

- (NSMutableSet*)touchesSet
{
	return touchesSet;
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
