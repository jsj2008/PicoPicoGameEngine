//
//  PPScriptGameEngine.h
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/15.
//
//

#import <PicoGame/PPGame.h>

@interface PPScriptGameEngine : PPGame
{
	NSString* _mainLuaName;
}

@property (strong,nonatomic) NSString* mainLuaName;

@end
