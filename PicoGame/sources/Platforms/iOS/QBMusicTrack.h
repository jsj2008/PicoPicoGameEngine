/*-----------------------------------------------------------------------------------------------
	名前	QBMusicTrack.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioToolbox.h>

#define kNumberBuffers  3                              // 1
struct AQPlayerState {
    AudioStreamBasicDescription   mDataFormat;                    // 2
    AudioQueueRef                 mQueue;                         // 3
    AudioQueueBufferRef           mBuffers[kNumberBuffers];       // 4
    AudioFileID                   mAudioFile;                     // 5
    UInt32                        bufferByteSize;                 // 6
    SInt64                        mCurrentPacket;                 // 7
    UInt32                        mNumPacketsToRead;              // 8
    AudioStreamPacketDescription  *mPacketDescs;                  // 9
    bool                          mIsRunning;                     // 10
	UInt32						  mLoopPoint;
	bool						  mLoop;
	Float32						  mGain;
	bool						  mIsPlaying;
};

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

@interface QBMusicTrack : NSObject {
	struct AQPlayerState aqData;
	
	Float32	mGain;
	Float32 mMasterGain;
	Float32 mPause;
	
	bool						  mPlay;
	//bool						  mPause;
	
	NSString*					  mFile;
}

- (void)play:(NSString*)path;
- (void)play:(NSString*)path withLoop:(int)loopPoint;
//- (void)play;
- (void)close;
//- (void)replay;
- (void)setVolume:(float)v;
- (float)getVolume;
- (void)setMasterVolume:(float)v;
- (float)getMasterVolume;
- (BOOL)isRunning;
- (void)pause;
- (void)resume;

- (void)post:(NSString*)path;
- (void)post:(NSString*)path withLoop:(int)loopPoint;

- (void)myThreadMainMethod;//:(id)sender;

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
