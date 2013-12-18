//
//  PPScriptGameEngine.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/15.
//
//

#import "PPScriptGameEngine.h"
#import <PicoGame/PPScriptGame.h>
#import "PicoGame.h"
#include <iostream>

@implementation PPScriptGameEngine
@synthesize mainLuaName=_mainLuaName;

- (QBGame*)game
{
	//PPGameSetDataPath([[[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"GameData/main.lua"] stringByDeletingLastPathComponent] fileSystemRepresentation]);

	if (game == NULL) {
    [self setupResourceFolder];

		PPScriptGame* g = new PicoGame();
		NSString* sp = [[NSBundle mainBundle] pathForResource:[self.mainLuaName stringByDeletingPathExtension] ofType:[self.mainLuaName pathExtension]];
		if (sp) {
			g->luaScriptPath = [sp fileSystemRepresentation];
		}
		game = g;
	}
	return game;
}

@end
