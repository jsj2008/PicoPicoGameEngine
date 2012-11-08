/*-----------------------------------------------------------------------------------------------
	名前	PPGameMap.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameMap.h"
#include "PPGameUtil.h"

PPGameMap::PPGameMap(const char* path) : texture(NULL)
{
//	NSDictionary* d = [[NSDictionary dictionaryWithContentsOfFile:[NSString stringWithUTF8String:path]] retain];
	NSDictionary* d = [[NSDictionary dictionaryWithContentsOfFile:[NSString stringWithUTF8String:PPGameDataPath(path)]] retain];

	if (d==nil) {
		PPReadErrorSet(path);
		return;
	}
	
	map[0].init();
	map[1].init();
	map[2].init();
	map[0].decompress([[d objectForKey:@"bg0"] bytes]);
	map[1].decompress([[d objectForKey:@"bg1"] bytes]);
	map[2].decompress([[d objectForKey:@"bg2"] bytes]);
	
	map[0].eventNum = 0;
	if ([d objectForKey:@"event"]) {
		map[0].eventNum = (int)[[d objectForKey:@"event"] count];
		if (map[0].eventNum > 0) {
			map[0].event = new PPGameMapEvent[map[0].eventNum];
			for (int i=0;i<map[0].eventNum;i++) {
				NSDictionary* e = [[d objectForKey:@"event"] objectAtIndex:i];
				map[0].event[i].dx = [[e objectForKey:@"dx"] intValue];
				map[0].event[i].dy = [[e objectForKey:@"dy"] intValue];
				map[0].event[i].x = [[e objectForKey:@"spx"] intValue];
				map[0].event[i].y = [[e objectForKey:@"spy"] intValue];
				map[0].event[i].w = [[e objectForKey:@"epx"] intValue]-[[e objectForKey:@"spx"] intValue]+1;
				map[0].event[i].h = [[e objectForKey:@"epy"] intValue]-[[e objectForKey:@"spy"] intValue]+1;
				strcpy(map[0].event[i].name,[[e objectForKey:@"name"] UTF8String]);
			}
		}
	}
	if ([d objectForKey:@"texture"]) {
		const char* t = [[d objectForKey:@"texture"] UTF8String];
		if (t) {
			texture = (char*)malloc(strlen(t)+1);
			strcpy(texture,t);
		}
	}
	map[1].eventNum = 0;
	map[2].eventNum = 0;
	
	[d release];
}

PPGameMap::~PPGameMap()
{
	map[0].freedata();
	map[1].freedata();
	map[2].freedata();
	if (texture) free(texture);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
