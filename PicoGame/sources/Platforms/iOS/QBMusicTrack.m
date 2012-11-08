/*-----------------------------------------------------------------------------------------------
	名前	QBMusicTrack.m
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "QBMusicTrack.h"

#if	1
static QBMusicTrack* t=nil;

int QBSound_InitACC()
{
	if (t == nil) {
		t = [[QBMusicTrack alloc] init];
		//lock = [[NSLock alloc] init];
		//[NSThread detachNewThreadSelector:@selector(myThreadMainMethod) toTarget:t withObject:nil];
	}
	return 0;
}

#else
#import "QBMusicOSTrack.h"
static QBMusicOSTrack* t=nil;

int QBSound_InitACC()
{
	if (t == nil) {
		t = [[QBMusicOSTrack alloc] init];
		//lock = [[NSLock alloc] init];
		//[NSThread detachNewThreadSelector:@selector(myThreadMainMethod) toTarget:t withObject:nil];
	}
	return 0;
}
#endif

static NSLock* lock=nil;
//static int startQue=0;
static int initQue=0;

int QBSound_LoopStreamPlayAAC(const char* filename,int looppoint)
{
	QBSound_InitACC();
	[t play:[[NSFileManager defaultManager] stringWithFileSystemRepresentation:filename length:strlen(filename)] withLoop:looppoint];
	return 0;
}

int QBSound_StreamPlayAAC(const char* filename)
{
	QBSound_InitACC();
	[t play:[[NSFileManager defaultManager] stringWithFileSystemRepresentation:filename length:strlen(filename)]];
	return 0;
}

int QBSound_SetVolumeAAC(float volume)
{
	QBSound_InitACC();
	[t setVolume:volume];
	return 0;
}

float QBSound_GetVolumeAAC()
{
	QBSound_InitACC();
	return [t getVolume];
}

int QBSound_IsPlayingAAC()
{
	QBSound_InitACC();
	return [t isRunning];
}

int QBSound_PauseAAC()
{
	QBSound_InitACC();
	[t pause];
	return 0;
}

int QBSound_ResumeAAC()
{
	QBSound_InitACC();
	[t resume];
	return 0;
}

int QBSound_StopAAC()
{
	QBSound_InitACC();
	[t close];
	return 0;
}

int QBSound_SetMasterVolumeAAC(float volume)
{
	QBSound_InitACC();
	[t setMasterVolume:volume];
	return 0;
}

float QBSound_GetMasterVolumeAAC()
{
	QBSound_InitACC();
	return [t getMasterVolume];
}

static void HandleOutputBuffer (
    void                *aqData,
    AudioQueueRef       inAQ,
    AudioQueueBufferRef inBuffer
) {
    struct AQPlayerState *pAqData = (struct AQPlayerState *) aqData;        // 1
    if (pAqData->mIsRunning == 0) return;                     // 2
    UInt32 numBytesReadFromFile;                              // 3
    UInt32 numPackets = pAqData->mNumPacketsToRead;           // 4
    AudioFileReadPackets (
        pAqData->mAudioFile,
        false,
        &numBytesReadFromFile,
        pAqData->mPacketDescs, 
        pAqData->mCurrentPacket,
        &numPackets,
        inBuffer->mAudioData 
    );

/*
if (pAqData->mPacketDescs) {
	printf("mStartOffset %d\n",(int)pAqData->mPacketDescs->mStartOffset);
	printf("mVariableFramesInPacket %d\n",pAqData->mPacketDescs->mVariableFramesInPacket);
	printf("mDataByteSize %d\n",pAqData->mPacketDescs->mDataByteSize);
}
*/

    if (numPackets > 0) {                                     // 5
        inBuffer->mAudioDataByteSize = numBytesReadFromFile;  // 6
       AudioQueueEnqueueBuffer ( 
            pAqData->mQueue,
            inBuffer,
            (pAqData->mPacketDescs ? numPackets : 0),
            pAqData->mPacketDescs
        );
        pAqData->mCurrentPacket += numPackets;                // 7 
//printf("%d\n",pAqData->mCurrentPacket);
    } else {
		if (!pAqData->mLoop) {

        AudioQueueStop (
            pAqData->mQueue,
            false
        );

		pAqData->mIsPlaying = false; 
		} else {
		numPackets = pAqData->mNumPacketsToRead;           // 4
		pAqData->mCurrentPacket = pAqData->mLoopPoint/pAqData->mDataFormat.mFramesPerPacket;
    AudioFileReadPackets (
        pAqData->mAudioFile,
        false,
        &numBytesReadFromFile,
        pAqData->mPacketDescs, 
        pAqData->mCurrentPacket,
        &numPackets,
        inBuffer->mAudioData 
    );
        inBuffer->mAudioDataByteSize = numBytesReadFromFile;  // 6
#if 1
		AudioQueueEnqueueBufferWithParameters(
            pAqData->mQueue,
            inBuffer,
            (pAqData->mPacketDescs ? numPackets : 0),
            pAqData->mPacketDescs,
			(pAqData->mLoopPoint % pAqData->mDataFormat.mFramesPerPacket),	//inTrimFramesAtStart
			0,		//inTrimFramesAtEnd
			0,		//inNumParamValues
			NULL,	//inParamValues
			0,		//inStartTime
			NULL	//outActualStartTime
		);
#else
       AudioQueueEnqueueBuffer ( 
            pAqData->mQueue,
            inBuffer,
            (pAqData->mPacketDescs ? numPackets : 0),
            pAqData->mPacketDescs
        );
#endif
        pAqData->mCurrentPacket += numPackets;                // 7 
/*
        AudioQueueStop (
            pAqData->mQueue,
            false
        );
        pAqData->mIsRunning = false; 
*/
		}
    }
}

void DeriveBufferSize (
    AudioStreamBasicDescription *ASBDesc,                            // 1
    UInt32                      maxPacketSize,                       // 2
    Float64                     seconds,                             // 3
    UInt32                      *outBufferSize,                      // 4
    UInt32                      *outNumPacketsToRead                 // 5
) {
    static const int maxBufferSize = 0x4000;                        // 6
    static const int minBufferSize = 0x4000;                         // 7
 
    if (ASBDesc->mFramesPerPacket != 0) {                             // 8
        Float64 numPacketsForTime =
            ASBDesc->mSampleRate / ASBDesc->mFramesPerPacket * seconds;
        *outBufferSize = numPacketsForTime * maxPacketSize;
    } else {                                                         // 9
        *outBufferSize =
            maxBufferSize > maxPacketSize ?
                maxBufferSize : maxPacketSize;
    }
 
    if (                                                             // 10
        *outBufferSize > maxBufferSize &&
        *outBufferSize > maxPacketSize
    )
        *outBufferSize = maxBufferSize;
    else {                                                           // 11
        if (*outBufferSize < minBufferSize)
            *outBufferSize = minBufferSize;
    }
 
    *outNumPacketsToRead = *outBufferSize / maxPacketSize;           // 12
}

void audioQueuePropertyListenerProc(  
                                    void *                  inUserData,
                                    AudioQueueRef           inAQ,
                                    AudioQueuePropertyID    inID)
{
	//printf("audioQueuePropertyListenerProc %d\n",inID);
}

@implementation QBMusicTrack

- (id)init
{
	self = [super init];
	mMasterGain = 1.0;
	mPause = 1.0;
	return self;
}

- (void)play:(NSString*)path
{
	[self play:path withLoop:-1];
}

- (void)play:(NSString*)path withLoop:(int)loopPoint
{
	if (path != mFile) {
		[mFile release];
		mFile = [path retain];
	}

	//if (aqData.mIsRunning) {
		[self close];
	//	aqData.mIsRunning = false;
	//}

	NSURL* url = [NSURL fileURLWithPath:path];
	//AudioFileID outAFID;
//NSLog(path);
//printf("1\n");

	OSStatus result = AudioFileOpenURL((CFURLRef)url, kAudioFileReadPermission, 0, &aqData.mAudioFile);
//printf("%d\n",result);
//printf("2\n");

if (loopPoint != -999) {
if (loopPoint < 0) {
	aqData.mLoop = false;
} else {
	aqData.mLoop = true;
	aqData.mLoopPoint = loopPoint;
}
}

aqData.mIsPlaying = true;

	//char str[1024];
	//strcpy(str,[path cString]);
	//strcat(str,".dat");
	//fp = fopen(str,"wb");

	//AudioStreamBasicDescription outFormat;
	//UInt64 outDataSize;

	UInt32 thePropSize = sizeof(aqData.mDataFormat);
	result = AudioFileGetProperty(aqData.mAudioFile, kAudioFilePropertyDataFormat, &thePropSize, &aqData.mDataFormat);
//printf("3\n");
/*
{
	printf("mSampleRate %lf\n",(double)aqData.mDataFormat.mSampleRate);
	printf("mFormatID %ld\n",aqData.mDataFormat.mFormatID);
	printf("mFormatFlags %ld\n",aqData.mDataFormat.mFormatFlags);
	printf("mBytesPerPacket %ld\n",aqData.mDataFormat.mBytesPerPacket);
	printf("mFramesPerPacket %ld\n",aqData.mDataFormat.mFramesPerPacket);
	printf("mBytesPerFrame %ld\n",aqData.mDataFormat.mBytesPerFrame);
	printf("mChannelsPerFrame %ld\n",aqData.mDataFormat.mChannelsPerFrame);
	printf("mBitsPerChannel %ld\n",aqData.mDataFormat.mBitsPerChannel);
	printf("mReserved %ld\n",aqData.mDataFormat.mReserved);
}
*/
aqData.mIsRunning = true;                          // 1

//printf("%d\n",result);

	//thePropSize = sizeof(UInt64);
	//result = AudioFileGetProperty(aqData.mAudioFile, kAudioFilePropertyAudioDataByteCount, &thePropSize, &outDataSize);

if (initQue==0) {
	AudioQueueNewOutput (                                // 1
		&aqData.mDataFormat,                             // 2
		HandleOutputBuffer,                              // 3
		&aqData,                                         // 4
		CFRunLoopGetCurrent(),                           // 5
		kCFRunLoopCommonModes,                           // 6
		0,                                               // 7
		&aqData.mQueue                                   // 8
	);
}

//printf("4\n");

	UInt32 maxPacketSize;
	UInt32 propertySize = sizeof (maxPacketSize);
	result = AudioFileGetProperty (                               // 1
		aqData.mAudioFile,                               // 2
		kAudioFilePropertyPacketSizeUpperBound,          // 3
		&propertySize,                                   // 4
		&maxPacketSize                                   // 5
	);

//printf("%d\n",result);

//printf("5\n");

	DeriveBufferSize (                                   // 6
		&aqData.mDataFormat,                              // 7
		maxPacketSize,                                   // 8
		0.5,                                             // 9
		&aqData.bufferByteSize,                          // 10
		&aqData.mNumPacketsToRead                        // 11
	);

//printf("aqData.mNumPacketsToRead　%d\n",aqData.mNumPacketsToRead);

bool isFormatVBR = (                                       // 1
    aqData.mDataFormat.mBytesPerPacket == 0 ||
    aqData.mDataFormat.mFramesPerPacket == 0
);
 
if (isFormatVBR) {                                         // 2
    aqData.mPacketDescs =
      (AudioStreamPacketDescription*) calloc (1,
        aqData.mNumPacketsToRead * sizeof (AudioStreamPacketDescription)
      );
} else {                                                   // 3
    aqData.mPacketDescs = NULL;
}

if (aqData.mPacketDescs) {
//	printf("mStartOffset %d\n",(int)aqData.mPacketDescs->mStartOffset);
//	printf("mVariableFramesInPacket %d\n",aqData.mPacketDescs->mVariableFramesInPacket);
//	printf("mDataByteSize %d\n",aqData.mPacketDescs->mDataByteSize);
}

UInt32 cookieSize = sizeof (UInt32);                   // 1
bool couldNotGetProperty =                             // 2
    AudioFileGetPropertyInfo (                         // 3
        aqData.mAudioFile,                             // 4
        kAudioFilePropertyMagicCookieData,             // 5
        &cookieSize,                                   // 6
        NULL                                           // 7
    );
 
if (!couldNotGetProperty && cookieSize) {              // 8
    char* magicCookie =
        (char *) malloc (cookieSize);
 
    AudioFileGetProperty (                             // 9
        aqData.mAudioFile,                             // 10
        kAudioFilePropertyMagicCookieData,             // 11
        &cookieSize,                                   // 12
        magicCookie                                    // 13
    );
 
    AudioQueueSetProperty (                            // 14
        aqData.mQueue,                                 // 15
        kAudioQueueProperty_MagicCookie,               // 16
        magicCookie,                                   // 17
        cookieSize                                     // 18
    );
 
    free (magicCookie);                                // 19
}

#if 0
{
	UInt32 size = sizeof (AudioFileRegionList);                   // 1

	AudioFileRegionList r;
	r.mSMPTE_TimeType = 0;
	r.mNumberRegions = 0;
	r.mRegions[0].mRegionID = 1;
	r.mRegions[0].mName = 0;
	r.mRegions[0].mFlags = kAudioFileRegionFlag_LoopEnable;
	r.mRegions[0].mNumberMarkers = 0;
	r.mRegions[0].mMarkers[0].mFramePosition = 0;
	r.mRegions[0].mMarkers[0].mName = 0;
	r.mRegions[0].mMarkers[0].mSMPTETime.mHours = 0;
	r.mRegions[0].mMarkers[0].mSMPTETime.mMinutes = 0;
	r.mRegions[0].mMarkers[0].mSMPTETime.mSeconds = 0;
	r.mRegions[0].mMarkers[0].mSMPTETime.mFrames = 0;
	r.mRegions[0].mMarkers[0].mSMPTETime.mSubFrameSampleOffset = 0;
	r.mRegions[0].mMarkers[0].mType = kAudioFileMarkerType_Generic;
	r.mRegions[0].mMarkers[0].mReserved = 0;
	r.mRegions[0].mMarkers[0].mChannel = 0;

	AudioQueueSetProperty (                            // 14
		aqData.mQueue,                                 // 15
		kAudioFilePropertyRegionList,               // 16
		&r,                                   // 17
		size									 // 18
	);
}
#endif

aqData.mCurrentPacket = 0;                                // 1
 
//printf("6\n");

for (int i = 0; i < kNumberBuffers; ++i) {                // 2
    AudioQueueAllocateBuffer (                            // 3
        aqData.mQueue,                                    // 4
        aqData.bufferByteSize,                            // 5
        &aqData.mBuffers[i]                               // 6
    );
 
    HandleOutputBuffer (                                  // 7
        &aqData,                                          // 8
        aqData.mQueue,                                    // 9
        aqData.mBuffers[i]                                // 10
    );

}

if (mGain == 0) mGain = 0.6;
aqData.mGain = mGain;                                       // 1
    // Optionally, allow user to override gain setting here
AudioQueueSetParameter (                                  // 2
    aqData.mQueue,                                        // 3
    kAudioQueueParam_Volume,                              // 4
    aqData.mGain                                                  // 5
);

aqData.mIsRunning = true;                          // 1

AudioTimeStamp t;
t.mSampleTime = 0;
t.mFlags = kAudioTimeStampSampleTimeValid;

//AudioQueueAddPropertyListener(aqData.mQueue,kAudioQueueProperty_IsRunning,audioQueuePropertyListenerProc,&aqData);

//printf("7\n");

#if 0
[lock lock];
startQue = 1;
[lock unlock];
#else
if (initQue==0) {
AudioQueueStart (                                  // 2
    aqData.mQueue,                                 // 3
    NULL                                          // 4
);
}
//initQue=1;
#endif
mPlay = TRUE;
	[self setVolume:mGain];
//mPause = FALSE;
//printf("8\n");
//NSLog(@"play %d",aqData.mIsRunning);
}

- (void)close
{
		mPlay = FALSE;
		//mPause = FALSE;

	if (aqData.mIsRunning) {
		//AudioQueueFlush(aqData.mQueue);
#if 1
/*
        AudioQueueStop (
            aqData.mQueue,
            true
        );

AudioQueueFlush(aqData.mQueue);
*/
AudioQueueDispose (                            // 1
    aqData.mQueue,                             // 2
    true                                       // 3
);

#endif 
AudioFileClose (aqData.mAudioFile);            // 4
 
free (aqData.mPacketDescs);
aqData.mPacketDescs = NULL;
}
//aqData.mIsPlaying = false;
//NSLog(@"close %d",aqData.mIsRunning);
aqData.mIsRunning = false;
}
/*
- (void)replay
{
AudioTimeStamp t;
t.mSampleTime = 0;
t.mFlags = kAudioTimeStampSampleTimeValid;
AudioQueueStop(aqData.mQueue,true);
AudioQueueStart (                                  // 2
    aqData.mQueue,                                 // 3
    NULL                                           // 4
);
}
*/
- (void)setVolume:(float)v
{
//printf("setVolume mGain %f",(float)v);
mGain = v;
aqData.mGain = v*mMasterGain*mPause;                                       // 1
    // Optionally, allow user to override gain setting here
AudioQueueSetParameter (                                  // 2
    aqData.mQueue,                                        // 3
    kAudioQueueParam_Volume,                              // 4
    aqData.mGain                                                  // 5
);
}

- (float)getVolume
{
//printf("getVolume mGain %f",(float)mGain);
	return mGain;
}

- (void)setMasterVolume:(float)v
{
	mMasterGain = v;
	[self setVolume:mGain];
}

- (float)getMasterVolume
{
	return 	mMasterGain;
}

- (BOOL)isRunning
{
	BOOL r = NO;
	if (aqData.mIsRunning/* && aqData.mIsPlaying*/) 
	{
		UInt32 value;
		UInt32 size=sizeof(value);
		AudioQueueGetProperty(aqData.mQueue,kAudioQueueProperty_IsRunning,&value,&size);
		//if (!value) aqData.mIsRunning = NO;
		//if (mPause == TRUE) return NO;
		//return value;
		r = (value != 0);
	}
//NSLog(@"pause %d",r);
	return r;
}

- (void)pause
{
#if 1
	mPause = 0.0;
	[self setVolume:mGain];
#else
#if 0
	//if (mPlay) {
		AudioQueuePause(aqData.mQueue);
		mPlay = FALSE;
		//mPause = TRUE;
		//aqData.mIsRunning = false;
//NSLog(@"pause %d",aqData.mIsRunning);
	//}
#else

		mPlay = FALSE;
		//mPause = FALSE;

	if (aqData.mIsRunning) {
		//AudioQueueFlush(aqData.mQueue);
#if 1
/*
        AudioQueueStop (
            aqData.mQueue,
            true
        );

AudioQueueFlush(aqData.mQueue);
*/
AudioQueueDispose (                            // 1
    aqData.mQueue,                             // 2
    true                                       // 3
);

#endif 
AudioFileClose (aqData.mAudioFile);            // 4
 
free (aqData.mPacketDescs);
aqData.mPacketDescs = NULL;
}
//aqData.mIsPlaying = false;
//NSLog(@"close %d",aqData.mIsRunning);
aqData.mIsRunning = false;

#endif
#endif
}

- (void)resume
{
#if 1
	mPause = 1.0;
	[self setVolume:mGain];
#else
#if 0
	if (mPlay==FALSE) {
		AudioQueueStart(aqData.mQueue,NULL);
		mPlay = TRUE;
		//mPause = FALSE;
		//aqData.mIsRunning = true;
//NSLog(@"resume %d",aqData.mIsRunning);
	}
#else

	[self play:mFile withLoop:-999];

#endif
#endif
}

- (void)post:(NSString*)path
{
	[self post:path withLoop:-1];
}

static char playname[1024]={0};
static int playloop;

- (void)post:(NSString*)path withLoop:(int)loopPoint
{
	//[self play:path withLoop:loopPoint];
	[lock lock];
	strcpy(playname,[path fileSystemRepresentation]);
	playloop = loopPoint;
	[lock unlock];
}

- (void)myThreadMainMethod//:(id)sender
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	static int f = 0;

#if 1
	while (1) {
		if (f == 0) {
			
			f = 1;
		}
		[NSThread sleepForTimeInterval:1];
	}
#else
	while (1) {
#if 1
		int state=0;
		[lock lock];
		state = startQue;
		[lock unlock];
		if (state) {
			AudioQueueStart (                                  // 2
				aqData.mQueue,                                 // 3
				NULL                                          // 4
			);
			state = 0;
		}
#else
		char playname2[1024];
		int playloop2;
		playname2[0] = 0;
		[lock lock];
		strcpy(playname2,playname);
		playloop2 = playloop;
		playname[0] = 0;
		[lock unlock];

		if (playname2[0] != 0) {
printf("myThreadMainMethod\n");
			[lock lock];
			[self play:[[NSFileManager defaultManager] stringWithFileSystemRepresentation:playname2 length:strlen(playname2)] withLoop:playloop2];
			[lock unlock];
			playname2[0] = 0;
		}
#endif
		[NSThread sleepForTimeInterval:1];
	}
#endif

	[pool release];
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
