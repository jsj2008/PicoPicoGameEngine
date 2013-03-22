//
//  PicoGameViewController.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/15.
//
//

#import "PicoGameViewController.h"
#import <PicoGame/PPGameUtil.h>

@implementation PicoGameViewController

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
//	[window setFrameUsingName:@"mainWindow"];
	
	NSRect frameRect = [window frame];
	frameRect.size.width = 640;
	if ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenWidth"]) {
		frameRect.size.width = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenWidth"] intValue];
	}
	frameRect.size.height = 480;
	if ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenHeight"]) {
		frameRect.size.height = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenHeight"] intValue];
	}
	frameRect = [window frameRectForContentRect:frameRect];
	[window setFrame:frameRect display:YES];
	
	[game1 startGame:self];
	[game2 startGame:self];
	
	[window orderFront:self];
}

- (void)windowWillClose:(NSNotification *)notification
{
	PPSync();
	[NSApp terminate:self];
}

@end
