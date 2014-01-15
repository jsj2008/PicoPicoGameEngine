//
//  MainViewController.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/17.
//
//

#import "MainViewController.h"
#import "PPScriptGame.h"

@implementation MainViewController

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	[self.window setFrameUsingName:@"mainWindow"];
	
	NSRect frameRect = [self.window frame];
	frameRect.size.width = 640;
	if ([[NSBundle mainBundle] infoDictionary][@"screenWidth"]) {
		frameRect.size.width = [[[NSBundle mainBundle] infoDictionary][@"screenWidth"] intValue];
	}
	frameRect.size.height = 480;
	if ([[NSBundle mainBundle] infoDictionary][@"screenHeight"]) {
		frameRect.size.height = [[[NSBundle mainBundle] infoDictionary][@"screenHeight"] intValue];
	}
	frameRect = [self.window frameRectForContentRect:frameRect];
	[self.window setFrame:frameRect display:YES];
	
	[self.game startGame:self];
  [self.game disableIO:self];    //ファイルの読み書きを制限
  
	[self.window orderFront:self];

  NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
  [center addObserver:self selector:@selector(openWeb:) name:@"PPOpenWeb" object:nil];
}

- (void)openWeb:(NSNotification*)notification
{
  NSLog(@"%@",[notification description]);
}

@end
