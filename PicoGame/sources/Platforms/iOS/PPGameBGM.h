/*-----------------------------------------------------------------------------------------------
	名前	PPGameBGM.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#if TARGET_OS_IPHONE
#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

@interface PPGameBGM : NSObject <MPMediaPickerControllerDelegate> {
	MPMediaItemCollection		*music;
	UIViewController* controller;
	NSString* key;
	BOOL selectedPlay;
	BOOL popover;
	CGRect popoverPos;
}

@property (nonatomic, readonly) MPMusicPlayerController* musicPlayer;
@property (nonatomic, retain) MPMediaItemCollection* music;
@property (nonatomic, retain) UIViewController* controller;
@property (nonatomic, copy) NSString* key;
@property (nonatomic, assign) BOOL selectedPlay;
@property (nonatomic, assign) BOOL popover;

+ (MPMusicPlayerController*)musicPlayer;
+ (void)stop;

- (void)selectBGM:(UIViewController*)controller;

- (void)play;
- (void)playOneTime;
- (void)stop;

- (void)reset;

- (bool)saveForKey:(NSString*)keyName;
- (bool)loadForKey:(NSString*)keyName;

- (BOOL)selected;
- (void)popoverPosition:(CGRect)rect;

@end
#else
#import <Cocoa/Cocoa.h>


@interface PPGameBGM : NSObject {
	BOOL selectedPlay;
	NSString* key;
}

@property (nonatomic, assign) BOOL selectedPlay;
@property (nonatomic, copy) NSString* key;

- (void)play;
- (void)playOneTime;
- (void)stop;

- (void)reset;

- (bool)saveForKey:(NSString*)keyName;
- (bool)loadForKey:(NSString*)keyName;

@end
#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
