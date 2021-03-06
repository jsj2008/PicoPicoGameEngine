//
//  PPScriptGameEngine.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/15.
//
//

#import "PPScriptGameEngine.h"
#import <PicoGame/PPScriptGame.h>
#import <iostream>

@implementation PPScriptGameEngine
@synthesize mainLuaName=_mainLuaName;

- (QBGame*)game
{
	if (game == NULL) {
		PPScriptGame* g = new PPScriptGame();
		NSString* sp = [[NSBundle mainBundle] pathForResource:[self.mainLuaName stringByDeletingPathExtension] ofType:[self.mainLuaName pathExtension]];
		if (sp) {
			g->luaScriptPath = [sp fileSystemRepresentation];
NSLog(@"%@",sp);
		}
		game = g;
	}
	return game;
}

@end
