/*-----------------------------------------------------------------------------------------------
	名前	QBGameController.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#import "PPGame.h"
#if TARGET_OS_IPHONE
#import "PPGameBGM.h"
#endif
#import "PPGameMap.h"
#import "QBGame.h"
#import "PPImageFont.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

@interface QBGameController : PPGame 
//{
//	QBGame* game;
//	BOOL later3GS;
//}

- (unsigned long)getKey;
- (QBGame*)game;
- (int)idle;

#ifdef __GAMEKIT__

- (void)showLeaderboard;
- (void)showAchievements;
- (void)setScore:(int)score forCategory:(NSString*)category;
- (void)setAchieve:(NSString*)identifier withPercent:(float)percent;

#endif

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
