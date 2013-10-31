//
//  PPPicoGame.mm
//  PicoGame
//
//  Created by 山口 博光 on 2013/10/31.
//
//

#import "PPPicoGame.h"

@implementation PPPicoGame

- (NSString*)docPath:(NSString*)dbfile
{
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
  return [documentsDirectory stringByAppendingPathComponent:dbfile];
}

- (QBGame*)game
{
	if (game == NULL) {
		PPScriptGame* g = new PPScriptGame();
    NSFileManager* fm = [NSFileManager defaultManager];
    NSString* mainPath = [self docPath:@"main.lua"];
    NSString* sp;
    if ([fm fileExistsAtPath:mainPath]) {
      sp = mainPath;
      PPGameSetDataPath([[mainPath stringByDeletingLastPathComponent] fileSystemRepresentation]);
    } else {
      sp = [[NSBundle mainBundle] pathForResource:@"main" ofType:@"lua"];
    }
		if (sp) {
			g->luaScriptPath = [sp fileSystemRepresentation];
		}
		game = g;
	}
	return game;
}

@end
