/*-----------------------------------------------------------------------------------------------
 名前	PPGameView_App.mm
 説明
 作成	2012.07.22 by H.Yamaguchi
 更新
 -----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 インクルードファイル
 -----------------------------------------------------------------------------------------------*/

#import <Carbon/Carbon.h>
#import "PPGameView_App.h"
#import <OpenGL/OpenGL.h>
#import "QBSoundMac.h"
#import "PPGame.h"
#import "PPGameUtil.h"
#import "PPGameController.h"
#import "QBGame.h"

#define JOY_PAD_UP		0x01
#define JOY_PAD_DOWN	0x02
#define JOY_PAD_LEFT	0x04
#define JOY_PAD_RIGHT	0x08
#define JOY_PAD_Z		0x10
#define JOY_PAD_X		0x20
#define JOY_PAD_SPACE	0x80
#define JOY_PAD_1		0x0100
#define JOY_PAD_2		0x0200
#define JOY_PAD_3		0x0400
#define JOY_PAD_4		0x0800
#define JOY_PAD_5		0x1000
#define JOY_PAD_6		0x2000
#define JOY_PAD_7		0x4000
#define JOY_PAD_8		0x8000
#define JOY_PAD_9		0x10000
#define JOY_PAD_ESC		0x20000
#define JOY_PAD_RETURN	0x40000
#define JOY_PAD_START	0x40000
#define JOY_PAD_SHIFT	0x80000
#define JOY_PAD_CONTROL	0x100000
#define JOY_PAD_SELECT	0x100000
#define JOY_PAD_OPTION	0x200000
//#define PAD_ANY		0x400000
//#define JOY_PAD_X_UP	0x800000
//#define JOY_PAD_X_DOWN	0x1000000
#define JOY_PAD_A		0x1000000
#define JOY_PAD_B		0x2000000
#define JOY_PAD_C		0x4000000
#define JOY_PAD_D		0x8000000
#define JOY_PAD_E		0x10000000
#define JOY_PAD_F		0x20000000
#define JOY_PAD_G		0x40000000
#define JOY_PAD_H		0x80000000

#define JOY_UP1 5
#define JOY_DOWN1 7
#define JOY_LEFT1 8
#define JOY_RIGHT1 6
#define JOY_UP2 13
#define JOY_DOWN2 15
#define JOY_LEFT2 16
#define JOY_RIGHT2 14
#define JOY_L1 11
#define JOY_L2 10
#define JOY_R1 12
#define JOY_R2 9
#define JOY_START 4
#define JOY_SELECT 1

#define JOY_POV 57
#define JOY_AX1 48
#define JOY_AY1 49
#define JOY_AX2 50
#define JOY_AY2 53

static int joy[256];

class DisplayLinkLocker {
public:
	DisplayLinkLocker(pthread_mutex_t *_locker,const char* name) {
		pthread_mutex_lock(_locker);
		locker = _locker;
	}
	virtual ~DisplayLinkLocker() {
		pthread_mutex_unlock(locker);
	}
	
	pthread_mutex_t *locker;
};

static bool __boot = false;
static pthread_mutex_t mMutex;

static unsigned long H_GetImdKey(void)
{
	unsigned long retval;
	retval = 0;
	{
		struct TPADTBL {
			int no;
			unsigned long keymask;
			unsigned long padbit;
		};
		struct TPADTBL PadTbl[] = {
			{3,0x40000000,JOY_PAD_UP},		//UP
			{3,0x20000000,JOY_PAD_DOWN},	//DOWN
			{3,0x10000000,JOY_PAD_RIGHT},	//RIGHT
			{3,0x08000000,JOY_PAD_LEFT},	//LEFT
			{0,0x00000040,JOY_PAD_Z},		//Z
			{0,0x00000080,JOY_PAD_X},		//X
			{0,0x00000100,JOY_PAD_CONTROL},	//C
			{0,0x00000200,JOY_PAD_CONTROL},	//V
			{1,0x00020000,JOY_PAD_SPACE},	//SPACE
			{1,0x00200000,JOY_PAD_ESC},		//ESC
			{1,0x00000010,JOY_PAD_SPACE},	//RETURN
			{1,0x01000000,JOY_PAD_SPACE},	//SHIFT
			{1,0x08000000,JOY_PAD_SPACE},	//CONTROL
      
			{2,0x00000800,JOY_PAD_LEFT},	//テンキーの1
			{2,0x00000800,JOY_PAD_DOWN},	//テンキーの1
			{2,0x00000800,JOY_PAD_1},		//テンキーの1
			{2,0x00001000,JOY_PAD_DOWN},	//テンキーの2
			{2,0x00001000,JOY_PAD_2},		//テンキーの2
			{2,0x00002000,JOY_PAD_RIGHT},	//テンキーの3
			{2,0x00002000,JOY_PAD_DOWN},	//テンキーの3
			{2,0x00002000,JOY_PAD_3},		//テンキーの3
			{2,0x00004000,JOY_PAD_LEFT},	//テンキーの4
			{2,0x00004000,JOY_PAD_4},		//テンキーの4
			{2,0x00008000,JOY_PAD_5},		//テンキーの5
			{2,0x00008000,JOY_PAD_DOWN},	//テンキーの5
			{2,0x00000001,JOY_PAD_RIGHT},	//テンキーの6
			{2,0x00000001,JOY_PAD_6},		//テンキーの6
			{2,0x00000002,JOY_PAD_LEFT},	//テンキーの7
			{2,0x00000002,JOY_PAD_UP},		//テンキーの7
			{2,0x00000002,JOY_PAD_7},		//テンキーの7
			{2,0x00000008,JOY_PAD_UP},		//テンキーの8
			{2,0x00000008,JOY_PAD_8},		//テンキーの8
			{2,0x00000010,JOY_PAD_RIGHT},	//テンキーの9
			{2,0x00000010,JOY_PAD_UP},		//テンキーの9
			{2,0x00000010,JOY_PAD_9},		//テンキーの9
			{1,0x04000000,JOY_PAD_UP},		//I
			{1,0x40000000,JOY_PAD_LEFT},	//J
			{1,0x20000000,JOY_PAD_RIGHT},	//L
			{1,0x00400000,JOY_PAD_DOWN},	//M
			{1,0x00000004,JOY_PAD_OPTION},	//M
      
			{0,0x00000001,JOY_PAD_A},	//A
			{0,0x00000800,JOY_PAD_B},	//B
			{0,0x00000100,JOY_PAD_C},	//C
			{0,0x00000004,JOY_PAD_D},	//D
			{0,0x00004000,JOY_PAD_E},	//E
			{0,0x00000008,JOY_PAD_F},	//F
			{0,0x00000020,JOY_PAD_G},	//G
			{0,0x00000010,JOY_PAD_H},	//H
		};
		KeyMap theKeys;
		unsigned long *ptr;
		int i;
    
		GetKeys(theKeys);
		ptr = (unsigned long *)&theKeys;
		for (i=0;i<sizeof(PadTbl)/sizeof(struct TPADTBL);i++) {
			unsigned long k = *((unsigned long*)&theKeys[PadTbl[i].no]);
			if (k & PadTbl[i].keymask)
				retval |= PadTbl[i].padbit;
		}
	}
	return retval;
}

@implementation PPGameView

@synthesize projector=_projector;
@synthesize game;
@synthesize touchLocation,touchScreen;
@synthesize touchUpScreen;
@synthesize fullScreen;
@synthesize reloadTexture;

- (PPGameSprite*)projector
{
	if (_projector==NULL) {
		_projector = new PPGameSprite();
		_projector->InitBuffer([self.game polySize]);
	}
	return _projector;
}

- (NSOpenGLContext*)openGLContext
{
	return openGLContext;
}

- (NSOpenGLPixelFormat*)pixelFormat
{
	return pixelFormat;
}

- (void)setMainController:(PPGameController*)theController;
{
	controller = theController;
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[self drawView];
	
	[pool release];
  return kCVReturnSuccess;
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
  CVReturn result = [(PPGameView*)displayLinkContext getFrameForTime:outputTime];
  return result;
}

- (void)setupDisplayLink
{
	if (!__boot) {
		pthread_mutex_init(&mMutex,NULL);
		__boot=true;
	}
  
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
	
	// Set the display link for the current renderer
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
}

- (id)initWithFrame:(NSRect)frameRect
{
	self = [self initWithFrame:frameRect shareContext:nil];
	return self;
}

- (id)initWithFrame:(NSRect)frameRect shareContext:(NSOpenGLContext*)context
{
  NSOpenGLPixelFormatAttribute attribs[] =
  {
		kCGLPFAAccelerated,
		kCGLPFANoRecovery,
		kCGLPFADoubleBuffer,
		//NSOpenGLPFAMultiScreen,
		kCGLPFAColorSize, 24,
		kCGLPFADepthSize, 16,
		0
  };
	
  pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	
  if (!pixelFormat)
		NSLog(@"No OpenGL pixel format");
	
	// NSOpenGLView does not handle context sharing, so we draw to a custom NSView instead
	openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:context];
	
	if (self = [super initWithFrame:frameRect]) {
		[[self openGLContext] makeCurrentContext];
		
		// Synchronize buffer swaps with vertical refresh rate
		animationFrameInterval = 1;
		[[self openGLContext] setValues:&animationFrameInterval forParameter:NSOpenGLCPSwapInterval];
		
		[self setupDisplayLink];
		
		// Look for changes in view size
		// Note, -reshape will not be called automatically on size changes because NSView does not export it to override
		[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(reshape)
                                                 name:NSViewGlobalFrameDidChangeNotification
                                               object:self];
	}
  
	[self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];
	return self;
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	return NSDragOperationAll;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
	NSPasteboard *pboard;
	NSDragOperation sourceDragMask;
	
	sourceDragMask = [sender draggingSourceOperationMask];
	pboard = [sender draggingPasteboard];
	
	if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
		NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
		for (NSString* s in files) {
			if ([[s pathExtension] compare:@"lua" options:NSCaseInsensitiveSearch]==NSOrderedSame) {
				PPGameSetDataPath((const char*)[[s stringByDeletingLastPathComponent] fileSystemRepresentation]);
				PPGameSetMainLua((const char*)[[s lastPathComponent] fileSystemRepresentation]);
				[controller reloadData:self];
				break;
			}
		}
	}
  return YES;
}

- (void)lockFocus
{
	[super lockFocus];
	if ([[self openGLContext] view] != self)
		[[self openGLContext] setView:self];
}

- (void)reshape
{
	// This method will be called on the main thread when resizing, but we may be drawing on a secondary thread through the display link
	// Add a mutex around to avoid the threads accessing the context simultaneously
	DisplayLinkLocker locker(&mMutex,"reset");
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// Delegate to the scene object to update for a change in the view size
	//[[controller scene] setViewportRect:[self bounds]];
  
	viewPort = [self bounds];
  
	[[self openGLContext] update];
  [self render];
	[[self openGLContext] flushBuffer];
	
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawView
{
	if (interval<0) interval = animationFrameInterval;
	interval ++;
	if (interval >= animationFrameInterval) {
		DisplayLinkLocker locker(&mMutex,"reset");
		CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
		
		// Make sure we draw to the right context
		[[self openGLContext] makeCurrentContext];
		
		// Delegate to the scene object for rendering
		//[self setViewPort:[self bounds]];
		[self render];
		[[self openGLContext] flushBuffer];
		
		CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
		interval =0;
	}
}

//- (void)setViewPort:(NSRect)rect
//{
//	viewPort = rect;
//
////	glViewport(0, 0, rect.size.width, rect.size.height);
////
////	glMatrixMode(GL_PROJECTION);
////    glLoadIdentity();
////    gluPerspective(30, rect.size.width / rect.size.height, 1.0, 1000.0);
////	glMatrixMode(GL_MODELVIEW);
//}

- (void)drawRect:(NSRect)rect
{
	if (!CVDisplayLinkIsRunning(displayLink)) {
		viewPort = [self bounds];
    //		[self setViewPort:rect];
    //		[self drawView];
    
		DisplayLinkLocker locker(&mMutex,"reset");
		CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
		
		// Make sure we draw to the right context
		[[self openGLContext] makeCurrentContext];
		
		// Delegate to the scene object for rendering
		//[self setViewPort:[self bounds]];
		[self render];
		[[self openGLContext] flushBuffer];
		
		CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	}
  
}

- (void)render
{
  
	if (_initialWait < 10) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport((GLsizei)viewPort.origin.x, (GLsizei)viewPort.origin.y, (GLsizei)viewPort.size.width, (GLsizei)viewPort.size.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		_initialWait ++;
		return;
	}
  
	viewPort = [self bounds];
	int h = viewPort.size.height;
	h %= 2;
	viewPort.origin.y = h;
	viewPort.size.width = floorf(viewPort.size.width/2)*2;
	viewPort.size.height = floorf(viewPort.size.height/2)*2;
  
	if (game) {
    //		PPGameSprite* oldprojector = PPGameSprite::select(self.projector);
    //		game.game->projector = self.projector;
		self.projector->setWorld(game.game);
    //		PPGameTextureManager::select(self.projector->textureManager);
    
    //		[game setView:self];
    
		unsigned long key = H_GetImdKey();
    
    if (QBSound::sharedSound()==NULL) {
      QBSoundMac* snd = new QBSoundMac(5);
      if (snd) {
        snd->Init();
        snd->Reset();
        snd->setMasterVolume(0.5);
      }
    }
    
		if (joy[JOY_RIGHT2]) {
			key |= JOY_PAD_SPACE;
		}
		if (joy[JOY_DOWN2]) {
			key |= JOY_PAD_Z;
		}
		
		if (![NSApp isActive]) key = 0;
    
    //	glMatrixMode(GL_PROJECTION);
    //	glLoadIdentity();
		glViewport((GLsizei)viewPort.origin.x, (GLsizei)viewPort.origin.y, (GLsizei)viewPort.size.width, (GLsizei)viewPort.size.height);
    //	glLoadIdentity();
    //
    //	glOrtho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);
    //
    //	glMatrixMode(GL_MODELVIEW);
    //	glLoadIdentity();
    
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
    
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		if (reloadTexture) {
			self.projector->textureManager->reloadAllTexture();
			reloadTexture = false;
		}
    
		staticKey = key;
    
		if (game.initFlag == NO) {
			[game start];
			game.initFlag = YES;
		}
		game.screenWidth = viewPort.size.width;
		game.screenHeight = viewPort.size.height;
		game.staticButton = [self staticButton];
		game.touchScreen = self.touchScreen;
		game.touchUpScreen = self.touchUpScreen;
		game.touchLocation = self.touchLocation;
		if (self.projector) {
			[game textureIdle];
		}
    
		if (self.projector) {
      
			self.projector->st.screenWidth = viewPort.size.width;//self.width;//160;
			self.projector->st.screenHeight = viewPort.size.height;//self.height;//416/2;
      
			self.projector->ClearScreen2D(0.0f, 0.0f, 0.0f);
		}
    
		self.projector->SetViewPort();
		self.projector->InitOT();
    
		if ([game idle] != 0) {
			
		}
		if (self.projector) {
			NSPoint p = [[self window] mouseLocationOutsideOfEventStream];
			if (fullScreen) {
				p = [[self window] convertBaseToScreen:p];
			}
			p = [self convertPoint:p fromView:nil];
			
			[game game]->mouseLocation.x = p.x;
			[game game]->mouseLocation.y = viewPort.size.height-p.y;
			
			if ([game draw] == 0) {
				[game drawOT];
				self.projector->DrawOT();
				self.projector->ResetOT();
			}
		}
    
		if (self.projector) {
			if (animationFrameInterval!=self.projector->animationFrameInterval) {
				animationFrameInterval=self.projector->animationFrameInterval;
			}
		}
    
		self.touchUpScreen = game.touchUpScreen;
		self.touchScreen = game.touchScreen;
	}
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}


- (void)keyDown:(NSEvent *)theEvent
{
  unichar c = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
  switch (c) {
    case 27:
			[controller goWindow:self];
      break;
    default:
      break;
  }
}

- (unsigned long)staticButton
{
	unsigned long key = 0;
	if (staticKey & JOY_PAD_UP) key |= PAD_UP;
	if (staticKey & JOY_PAD_DOWN) key |= PAD_DOWN;
	if (staticKey & JOY_PAD_LEFT) key |= PAD_LEFT;
	if (staticKey & JOY_PAD_RIGHT) key |= PAD_RIGHT;
	if (staticKey & JOY_PAD_Z) key |= PAD_A;
	if (staticKey & JOY_PAD_X) key |= PAD_B;
	if (staticKey & JOY_PAD_SPACE) key |= PAD_A;
  
	if (staticKey & JOY_PAD_A) key |= PAD_A;
	if (staticKey & JOY_PAD_B) key |= PAD_B;
	if (staticKey & JOY_PAD_C) key |= PAD_C;
	if (staticKey & JOY_PAD_D) key |= PAD_D;
	if (staticKey & JOY_PAD_E) key |= PAD_E;
	if (staticKey & JOY_PAD_F) key |= PAD_F;
	if (staticKey & JOY_PAD_G) key |= PAD_G;
	if (staticKey & JOY_PAD_H) key |= PAD_H;
	return key;
}

- (void)mouseDown:(NSEvent *)theEvent
{
	DisplayLinkLocker locker(&mMutex,"reset");
	NSPoint pt;
	NSSize size = viewPort.size;
	if (fullScreen) {
		pt = [[self window] convertBaseToScreen:[[self window] mouseLocationOutsideOfEventStream]];
	} else {
		pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	}
	touchLocation.x = pt.x;
	touchLocation.y = size.height-pt.y;
  
	NSPoint ptt;
	ptt.x = touchLocation.x;
	ptt.y = touchLocation.y;
  
	touchScreen = true;
	[touchesSet release];
	touchesSet = nil;
	if (touchesSet == nil) touchesSet = [[NSMutableSet set] retain];
	[touchesSet addObject:NSStringFromPoint(ptt)];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	DisplayLinkLocker locker(&mMutex,"reset");
	NSPoint pt;
	NSSize size = viewPort.size;
	if (fullScreen) {
		pt = [[self window] convertBaseToScreen:[[self window] mouseLocationOutsideOfEventStream]];
	} else {
		pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	}
	touchLocation.x = pt.x;
	touchLocation.y = size.height-pt.y;
  
	NSPoint ptt;
	ptt.x = touchLocation.x;
	ptt.y = touchLocation.y;
  
	[touchesSet release];
	touchesSet = nil;
	if (touchesSet == nil) touchesSet = [[NSMutableSet set] retain];
	[touchesSet addObject:NSStringFromPoint(ptt)];
}

- (void)mouseUp:(NSEvent *)theEvent
{
  //	touchScreen = false;
	DisplayLinkLocker locker(&mMutex,"reset");
	touchUpScreen = true;
	[touchesSet release];
	touchesSet = nil;
}

- (void)startAnimation
{
	if (displayLink && !CVDisplayLinkIsRunning(displayLink))
		CVDisplayLinkStart(displayLink);
}

- (void)stopAnimation
{
	if (displayLink && CVDisplayLinkIsRunning(displayLink))
		CVDisplayLinkStop(displayLink);
}

- (void)dealloc {
	if (_projector) {
		delete _projector;
	}
  //	if (self.projector) {
  //		self.projector->Exit();
  //		delete self.projector;
  //		self.projector = NULL;
  //	}
  
	// Stop and release the display link
	CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);
	
	// Destroy the context
	[openGLContext release];
	[pixelFormat release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:NSViewGlobalFrameDidChangeNotification
                                                object:self];
  
	pthread_mutex_destroy(&mMutex);
  
	[super dealloc];
}

- (void)swapProjector:(PPGameView*)view
{
	reloadTexture = true;
	PPGameSprite* p = view->_projector;
	view->_projector = self->_projector;
	self->_projector = p;
}

@end

/*-----------------------------------------------------------------------------------------------
 このファイルはここまで
 -----------------------------------------------------------------------------------------------*/
