/*-----------------------------------------------------------------------------------------------
	名前	PPGameBGM.m
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PPGameBGM.h"
#ifdef _PICOPICO_HD_
#import "PopoverBase.h"
#endif
//#import "PPGameUtil.h"

//static BOOL BGM_Selecting=NO;
#if !TARGET_IPHONE_SIMULATOR
static MPMusicPlayerController *musicPlayer = nil;
#endif
#ifdef _PICOPICO_HD_
static UIPopoverController* popOver = nil;
#endif

@implementation PPGameBGM

@synthesize controller;
@synthesize music;
@synthesize key;
@synthesize selectedPlay;
@synthesize popover;

+ (MPMusicPlayerController*)musicPlayer
{
#if !TARGET_IPHONE_SIMULATOR
	if (musicPlayer == nil) {
		//if (useiPodPlayer()) {
		//	musicPlayer = [[MPMusicPlayerController iPodMusicPlayer] retain];
		//} else {
			musicPlayer = [[MPMusicPlayerController applicationMusicPlayer] retain];
		//}
		[musicPlayer setShuffleMode: MPMusicShuffleModeOff];
		[musicPlayer setRepeatMode: MPMusicRepeatModeAll];
	}
	return musicPlayer;
#else
	return nil;
#endif
}

+ (void)stop
{
	[[PPGameBGM musicPlayer] stop];
}

- (id)init
{
	if ((self = [super init]) != nil) {
	}
	return self;
}

- (void)dealloc
{
	[key release];
	[music release];
	[super dealloc];
}

- (void)mediaPicker: (MPMediaPickerController *) mediaPicker didPickMediaItems: (MPMediaItemCollection *)mediaItemCollection
{
//NSLog(@"mediaPicker %@",key);
#ifdef _PICOPICO_HD_
	if (popover) {
		[popOver dismissPopoverAnimated:YES];
		[popOver release];
		popOver = nil;
	} else {
		if ([[UIApplication sharedApplication] respondsToSelector:@selector(setStatusBarHidden:withAnimation:)]) {
			[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:YES];
		} else {
			[[UIApplication sharedApplication] setStatusBarHidden:YES animated:YES];
		}
		[controller dismissModalViewControllerAnimated: YES];
		[controller release];
		controller = nil;
	}
#else
	if ([[UIApplication sharedApplication] respondsToSelector:@selector(setStatusBarHidden:withAnimation:)]) {
		[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:YES];
	} else {
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:YES];
	}
	[controller dismissModalViewControllerAnimated: YES];
	[controller release];
	controller = nil;
#endif
	self.music = mediaItemCollection;
	[self saveForKey:self.key];
	if (selectedPlay) [self play];
//	BGM_Selecting = NO;
	PPGame_SetSelectingBGM(NO);
//	[[UIApplication sharedApplication] setStatusBarStyle: UIStatusBarStyleBlackOpaque animated: YES];
}

- (void)mediaPickerDidCancel:(MPMediaPickerController *)mediaPicker
{
#ifdef _PICOPICO_HD_
	if (popover) {
		[popOver dismissPopoverAnimated:YES];
		[popOver release];
		popOver = nil;
	} else {
		if ([[UIApplication sharedApplication] respondsToSelector:@selector(setStatusBarHidden:withAnimation:)]) {
			[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:YES];
		} else {
			[[UIApplication sharedApplication] setStatusBarHidden:YES animated:YES];
		}
		[controller dismissModalViewControllerAnimated: YES];
		[controller release];
		controller = nil;
	}
#else
	if ([[UIApplication sharedApplication] respondsToSelector:@selector(setStatusBarHidden:withAnimation:)]) {
		[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:YES];
	} else {
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:YES];
	}
	[controller dismissModalViewControllerAnimated: YES];
	[controller release];
	controller = nil;
#endif
//	BGM_Selecting = NO;
	PPGame_SetSelectingBGM(NO);
//	[[UIApplication sharedApplication] setStatusBarStyle: UIStatusBarStyleBlackOpaque animated: YES];
}

#ifdef _PICOPICO_HD_
- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
	[popOver release];
	popOver = nil;
//	BGM_Selecting = NO;
	PPGame_SetSelectingBGM(NO);
}

- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController
{
	return YES;
}
#endif

- (void)popoverPosition:(CGRect)rect
{
	popoverPos = rect;
}

- (void)selectBGM:(UIViewController*)_controller
{
//NSLog(@"selectBGM %@",key);
	if (PPGame_GetSelectingBGM() == NO) {
		PPGame_SetSelectingBGM(YES);
//		BGM_Selecting = YES;

#ifdef _PICOPICO_HD_
		if (!popover) {
			if ([[UIApplication sharedApplication] respondsToSelector:@selector(setStatusBarHidden:withAnimation:)]) {
				[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:YES];
			} else {
				[[UIApplication sharedApplication] setStatusBarHidden:NO animated:YES];
			}
		}
#else
		if ([[UIApplication sharedApplication] respondsToSelector:@selector(setStatusBarHidden:withAnimation:)]) {
			[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:YES];
		} else {
			[[UIApplication sharedApplication] setStatusBarHidden:NO animated:YES];
		}
#endif

//#if TARGET_IPHONE_SIMULATOR
//		PopoverBase* picker = [[PopoverBase alloc] initWithNibName:@"PopoverBase" bundle:nil];
//#else
		MPMediaPickerController *picker = [[MPMediaPickerController alloc] initWithMediaTypes: MPMediaTypeAnyAudio];
		picker.delegate						= self;
		picker.allowsPickingMultipleItems	= YES;
		picker.prompt						= nil;//NSLocalizedString (@"Add songs to play", "Prompt in media item picker");
//		picker.wantsFullScreenLayout = YES;
//#endif
		self.controller = _controller;

#ifdef _PICOPICO_HD_
#if 0
		//ダイログで表示するパターン
		picker.modalPresentationStyle = UIModalPresentationFormSheet;
		[_controller presentModalViewController:picker animated: YES];
#else
		if (popover) {
			//ポップオーバーで表示するパターン
			popOver = [[UIPopoverController alloc] initWithContentViewController:picker];
			popOver.popoverContentSize = CGSizeMake(320, 480);
			popOver.delegate = self;
			[popOver presentPopoverFromRect:popoverPos inView:_controller.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		} else {
			[_controller presentModalViewController:picker animated: YES];
		}
#endif
#else
		[_controller presentModalViewController:picker animated: YES];
#endif
		[picker release];
//		[_controller resume];
	}
}

- (MPMusicPlayerController*)musicPlayer
{
	return [PPGameBGM musicPlayer];
}

- (void)play
{
	if (self.music) {
		[self.musicPlayer setQueueWithItemCollection:music];
		self.musicPlayer.currentPlaybackTime = 0;
		[self.musicPlayer setRepeatMode:MPMusicRepeatModeAll];
		[self.musicPlayer play];
	} else {
		[self stop];
	}
}

- (void)playOneTime
{
	if (self.music) {
		[self.musicPlayer setQueueWithItemCollection:music];
		self.musicPlayer.currentPlaybackTime = 0;
		[self.musicPlayer setRepeatMode:MPMusicRepeatModeNone];
		[self.musicPlayer play];
	} else {
		[self stop];
	}
}

- (void)stop
{
	[self.musicPlayer stop];
}

- (BOOL)selected
{
	if (self.music) return YES;
	return NO;
}

- (void)reset
{
//NSLog(@"reset");
	self.music = nil;
//	[self saveForKey:self.key];
}

- (bool)saveForKey:(NSString*)keyName
{
	if (keyName) {
		if (self.music) {
			NSMutableArray* a = [[NSMutableArray array] retain];
			for (MPMediaItem* item in self.music.items) {
				[a addObject:[item valueForProperty:MPMediaItemPropertyPersistentID]];
			}
			[[NSUserDefaults standardUserDefaults] setObject:a forKey:keyName];
//NSLog(@"save %@",keyName);
//NSLog([a description]);
			[a release];
			return true;
		} else {
			[[NSUserDefaults standardUserDefaults] removeObjectForKey:keyName];
		}
	}
	return false;
}

- (bool)loadForKey:(NSString*)keyName
{
	if (keyName) {
//NSLog(@"load %@",keyName);
		NSArray* a = [[NSUserDefaults standardUserDefaults] objectForKey:keyName];
//NSLog([a description]);
		if ([a isKindOfClass:[NSArray class]]) {
			NSMutableArray* items = [[NSMutableArray array] retain];
			for (NSNumber* persistentId in a) {
				MPMediaQuery* query = [[MPMediaQuery alloc] init];
				[query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:persistentId forProperty:MPMediaItemPropertyPersistentID]];
//NSLog([persistentId description]);
				if (query.items != nil) {
					if (query.items.count > 0) {
						[items addObjectsFromArray:query.items];
					}
				}
				[query release];
			}
			if (items.count > 0) {
				self.music = [MPMediaItemCollection collectionWithItems:items];
			} else {
				self.music = nil;
			}
//NSLog([self.music.items description]);
			[items release];
			return true;
		}
	}
	return false;
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
