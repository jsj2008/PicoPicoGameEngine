/*-----------------------------------------------------------------------------------------------
	名前	PPGameController.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PPGameController.h"
#import "PPGameView_App.h"
#import "PPGame.h"
#import "QBGame.h"

#ifndef NO_COCOSDENSHION
#include <CocosDenshion/SimpleAudioEngine.h>
#endif

@implementation PPGameController
@synthesize game=_game;

- (IBAction)startGame:(id)sender
{
	if (openGLView==nil) return;
	
	if (_isInitialized == NO) {
		_isInitialized = YES;

		if (self.game == nil) {
			PPGame* game = [[PPGame alloc] init];
			self.game = game;
			[game release];
		}

		openGLView.game = self.game;

		NSNumber* density = [[NSUserDefaults standardUserDefaults] objectForKey:@"densityValue"];
		if (density == nil) density = [NSNumber numberWithInt:10];
		[openGLView.game game]->scale_factor = [density intValue]/10.0;
		[openGLView setNeedsDisplay:YES];
		
		[openGLView setMainController:self];
		
		[openGLView startAnimation];
		isAnimating = YES;
	}

	NSNumber* density = [[NSUserDefaults standardUserDefaults] objectForKey:@"densityValue"];
	if (density == nil) density = [NSNumber numberWithInt:10];
	[openGLView.game game]->scale_factor = [density intValue]/10.0;
	[openGLView setNeedsDisplay:YES];
}

- (IBAction)goFullScreen:(id)sender
{
	if (isInFullScreenMode) {
		[self goWindow:sender];
		return;
	}
	isInFullScreenMode = YES;
	
	// Pause the non-fullscreen view
	[openGLView stopAnimation];
		
	// Mac OS X 10.6 and later offer a simplified mechanism to create full-screen contexts
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
	NSRect mainDisplayRect, viewRect;
	
	// Create a screen-sized window on the display you want to take over
	// Note, mainDisplayRect has a non-zero origin if the key window is on a secondary display
	mainDisplayRect = [[NSScreen mainScreen] frame];
	fullScreenWindow = [[NSWindow alloc] initWithContentRect:mainDisplayRect styleMask:NSBorderlessWindowMask 
													 backing:NSBackingStoreBuffered defer:YES];
	
	// Set the window level to be above the menu bar
	[fullScreenWindow setLevel:NSMainMenuWindowLevel+1];
	
	// Perform any other window configuration you desire
	[fullScreenWindow setOpaque:YES];
	[fullScreenWindow setHidesOnDeactivate:YES];
	
	// Create a view with a double-buffered OpenGL context and attach it to the window
	// By specifying the non-fullscreen context as the shareContext, we automatically inherit the OpenGL objects (textures, etc) it has defined
	viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
	fullScreenView = [[PPGameView alloc] initWithFrame:viewRect shareContext:[openGLView openGLContext]];
	[fullScreenWindow setContentView:fullScreenView];
	
	fullScreenView.game = self.game;
//	fullScreenView.projector = openGLView.projector;

	// Show the window
	[fullScreenWindow makeKeyAndOrderFront:self];
	
	// Set the scene with the full-screen viewport and viewing transformation
	//[scene setViewportRect:viewRect];
	
	// Assign the view's MainController to self
	[fullScreenView setMainController:self];
	
	[fullScreenView swapProjector:openGLView];
	
	if (!isAnimating) {
		// Mark the view as needing drawing to initalize its contents
		[fullScreenView setNeedsDisplay:YES];
	}
	else {
		// Start playing the animation
		[fullScreenView startAnimation];
	}
	
#else
	openGLView.fullScreen = YES;

	// Mac OS X 10.5 and eariler require additional work to capture the display and set up a special context
	// This demo uses CGL for full-screen rendering on pre-10.6 systems. You may also use NSOpenGL to achieve this.
	
	CGLPixelFormatObj pixelFormatObj;
	GLint numPixelFormats;
	
	// Capture the main display
	CGDisplayCapture(kCGDirectMainDisplay);
	
	// Set up an array of attributes
	CGLPixelFormatAttribute attribs[] = {
		
		// The full-screen attribute
		kCGLPFAFullScreen,
		
		// The display mask associated with the captured display
		// We may be on a multi-display system (and each screen may be driven by a different renderer), so we need to specify which screen we want to take over. For this demo, we'll specify the main screen.
		kCGLPFADisplayMask, (CGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
		
		// Attributes common to full-screen and non-fullscreen
		kCGLPFAAccelerated,
		kCGLPFANoRecovery,
		kCGLPFADoubleBuffer,
		kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
		kCGLPFADepthSize, (CGLPixelFormatAttribute)16,
        (CGLPixelFormatAttribute)0
    };
	
	// Create the full-screen context with the attributes listed above
	// By specifying the non-fullscreen context as the shareContext, we automatically inherit the OpenGL objects (textures, etc) it has defined
	CGLChoosePixelFormat(attribs, &pixelFormatObj, &numPixelFormats);
	CGLCreateContext(pixelFormatObj,(CGLContextObj)[[openGLView openGLContext] CGLContextObj],&fullScreenContextObj);
	CGLDestroyPixelFormat(pixelFormatObj);
	
	if (!fullScreenContextObj) 
	{
        NSLog(@"Failed to create full-screen context");
		CGReleaseAllDisplays();
		[self goWindow:self];
        return;
    }
	
	// Set the current context to the one to use for full-screen drawing
	CGLSetCurrentContext(fullScreenContextObj);
	
	// Attach a full-screen drawable object to the current context
	CGLSetFullScreen(fullScreenContextObj);
	
    // Lock us to the display's refresh rate
    GLint newSwapInterval = 1;
    CGLSetParameter(fullScreenContextObj, kCGLCPSwapInterval, &newSwapInterval);
	
	[openGLView setViewPort:NSMakeRect(0, 0, CGDisplayPixelsWide(kCGDirectMainDisplay), CGDisplayPixelsHigh(kCGDirectMainDisplay))];

	// Perform the application's main loop until exiting full-screen
	// The shift here is from a model in which we passively receive events handed to us by the AppKit (in window mode)
	// to one in which we are actively driving event processing (in full-screen mode)
	while (isInFullScreenMode)
	{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		
		// Check for and process input events
        NSEvent *event;
        while (event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES])
		{
            switch ([event type])
			{
                case NSLeftMouseDown:
                    [openGLView mouseDown:event];
                    break;
					
                case NSLeftMouseUp:
                    [openGLView mouseUp:event];
                    break;
					
                case NSLeftMouseDragged:
                    [openGLView mouseDragged:event];
                    break;
					
                case NSKeyDown:
                    [self keyDown:event];
                    break;
					
                default:
                    break;
            }
        }
		
		[openGLView render];
		
		CGLFlushDrawable(fullScreenContextObj);
		
		[pool release];
		
	}
	
#endif
}

- (IBAction)goWindow:(id)sender
{
	if (isInFullScreenMode == NO) return;
	
	[openGLView stopAnimation];
	
	isInFullScreenMode = NO;
	openGLView.fullScreen = NO;
	
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
	
	// Release the screen-sized window and view
	[fullScreenWindow release];
	[fullScreenView release];
	
#else
	
	// Set the current context to NULL
	CGLSetCurrentContext(NULL);
	// Clear the drawable object
	CGLClearDrawable(fullScreenContextObj);
	// Destroy the rendering context
	CGLDestroyContext(fullScreenContextObj);
	// Release the displays
	CGReleaseAllDisplays();
	
#endif
	
	// Switch to the non-fullscreen context
	[[openGLView openGLContext] makeCurrentContext];

	[openGLView swapProjector:fullScreenView];
	
	if (!isAnimating) {
		// Mark the view as needing drawing
		// The animation has advanced while we were in full-screen mode, so its current contents are stale
		[openGLView setNeedsDisplay:YES];
	}
	else {
		// Continue playing the animation
		[openGLView startAnimation];
	}
}

- (void)awakeFromNib
{
}

- (void)dealloc
{
	//[scene release];
	self.game = nil;
	[super dealloc];
}

//- (PPGame*)scene
//{
//	return scene;
//}

//- (CFAbsoluteTime) renderTime
//{
//	return renderTime;
//}

//- (void) setRenderTime:(CFAbsoluteTime)time
//{
//	renderTime = time;
//}

- (void)startAnimation
{
	if (!isAnimating)
	{
		if (!isInFullScreenMode)
			[openGLView startAnimation];
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
		else
			[fullScreenView startAnimation];
#endif
		isAnimating = YES;
	}
}

- (void)stopAnimation
{
	if (isAnimating)
	{
		if (!isInFullScreenMode)
			[openGLView stopAnimation];
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
		else
			[fullScreenView stopAnimation];
#endif
		isAnimating = NO;
	}
}

- (void)toggleAnimation
{
	if (isAnimating)
		[self stopAnimation];
	else
		[self startAnimation];
}

- (void)keyDown:(NSEvent *)event
{
    unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
    switch (c) {
        case 27:
			if (isInFullScreenMode)
				[self goWindow:self];
            break;
			
        default:
            break;
    }
}

- (IBAction)reloadData:(id)sender
{
	[self stopAnimation];
#ifndef NO_COCOSDENSHION
	CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
#endif
	[self.game reloadData];
	[self startAnimation];
}

- (IBAction)changeWindowSize:(NSMenuItem*)sender
{
	NSString* title = [sender title];
	NSArray* a = [title componentsSeparatedByString:@"x"];
	NSRect frameRect = [window frame];
	frameRect.size.width = [[a objectAtIndex:0] intValue];
	frameRect.size.height = [[a objectAtIndex:1] intValue];
	if (frameRect.size.width > 0 && frameRect.size.height > 0) {
		frameRect = [window frameRectForContentRect:frameRect];
		[window setFrame:frameRect display:YES];
	}
}

- (IBAction)deleteAllTexture:(id)sender
{
	CGLLockContext((CGLContextObj)[[openGLView openGLContext] CGLContextObj]);
	[self.game game]->projector->textureManager->unbindAllTextureForDebug();
	CGLUnlockContext((CGLContextObj)[[openGLView openGLContext] CGLContextObj]);
}

- (IBAction)reloadAllTexture:(id)sender
{
	CGLLockContext((CGLContextObj)[[openGLView openGLContext] CGLContextObj]);
	[self.game game]->projector->textureManager->reloadAllTexture();
	CGLUnlockContext((CGLContextObj)[[openGLView openGLContext] CGLContextObj]);
}

- (IBAction)changeDensity:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInt:(int)[sender tag]] forKey:@"densityValue"];
	[self.game game]->scale_factor=[sender tag]/10.0;
}

- (IBAction)changeFullScreen:(id)sender
{
	if (isInFullScreenMode)
		[self goWindow:sender];
	else
		[self goFullScreen:sender];
}

- (void)windowWillClose:(NSNotification *)notification
{
	[NSApp terminate:self];
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
