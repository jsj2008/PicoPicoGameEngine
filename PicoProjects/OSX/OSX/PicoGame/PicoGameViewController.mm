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
  NSRect frameRect = [window frame];
  NSRect contentRect = [window frameRectForContentRect:frameRect];
  float dw = frameRect.size.width -contentRect.size.width;
  float dh = frameRect.size.height-contentRect.size.height;
  if ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenWidth"]) {
    contentRect.size.width = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenWidth"] intValue];
  }
  if ([[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenHeight"]) {
    contentRect.size.height = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"screenHeight"] intValue];
  }
  frameRect.size = contentRect.size;
  frameRect.size.width  += dw;
  frameRect.size.height += dh;
  [window setFrame:frameRect display:YES];
	
	[game1 startGame:self];
#ifndef PICO_USE_LUA_IO
  [game1 disableIO:self];
#endif
	//[game2 startGame:self];
	
	[window orderFront:self];

  NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
  [center addObserver:self selector:@selector(openWeb:) name:@"PPOpenWeb" object:nil];
}

- (void)windowWillClose:(NSNotification *)notification
{
	PPSync();
	[NSApp terminate:self];
}

- (void)openWeb:(NSNotification*)notification
{
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[[notification object] objectForKey:@"url"]]];
}

@end
