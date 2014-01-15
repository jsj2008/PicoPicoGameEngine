//
//  PPScriptDebugEngine.m
//  PicoGame
//
//  Created by 山口 博光 on 2014/01/10.
//
//

#import "PPScriptDebugEngine.h"
#import "DebugPicoGame.h"

@implementation PPScriptDebugEngine

- (QBGame*)game
{
	if (game == NULL) {
		PPScriptGame* g = new DebugPicoGame();
		game = g;
	}
	return game;
}

@end
