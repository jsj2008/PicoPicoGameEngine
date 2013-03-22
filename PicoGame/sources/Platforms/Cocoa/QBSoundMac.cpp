/*-----------------------------------------------------------------------------------------------
	名前	QBSoundMac.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBSoundMac.h"
#include <pthread.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AUComponent.h>
#include <unistd.h>
#include <sys/time.h>

class QBSoundLocker {
public:
	QBSoundLocker(pthread_mutex_t *_locker,const char* name) {
		pthread_mutex_lock(_locker);
		locker = _locker;
	}
	virtual ~QBSoundLocker() {
		pthread_mutex_unlock(locker);
	}
	
	pthread_mutex_t *locker;
};

static OSStatus callback(
	void *inRefCon,
	AudioUnitRenderActionFlags* ioActionFlags,
	const AudioTimeStamp* inTimeStamp,
	UInt32 inBusNumber,
	UInt32 inNumberFrames,
	AudioBufferList* ioData)
{
	QBSoundMac* snd = (QBSoundMac*)inRefCon;
	if (snd) {
		UInt32 bytesToRead;
		bytesToRead = ioData->mBuffers[0].mDataByteSize;
		snd->fill_sound_buffer(ioData->mBuffers[0].mData,bytesToRead);
	}
	return 0;
}

#if TARGET_OS_IPHONE
static void InterruptionListener(void *inUserData,UInt32 inInterruption)
{
/*
    AudioUnit *remoteIO = (AudioUnit*)inUserData;
	
    if (inInterruption == kAudioSessionEndInterruption) {
        AudioSessionSetActive(true);
        AudioOutputUnitStart(*remoteIO);
        printf("EndInterruption\n");
    }
    
    if (inInterruption == kAudioSessionBeginInterruption) {
        AudioOutputUnitStop(*remoteIO);
        printf("BeginInterruption\n");
    }
*/
}

int QBSound_SetCategory(const char* category)
{
	int type = 0;
	if (category == NULL) {
	} else
	if (strcmp(category,"ambient") == 0) {
		type = 1;
	}
	switch (type) {
	case 0:
		{
			AudioSessionInitialize(NULL, NULL, InterruptionListener, NULL);
			
			UInt32 category = kAudioSessionCategory_SoloAmbientSound;
			AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
			
			AudioSessionSetActive(true);
		}
		break;
	case 1:
		{
			AudioSessionInitialize(NULL, NULL, InterruptionListener, NULL);
			
			UInt32 category = kAudioSessionCategory_AmbientSound;
			AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
			
			AudioSessionSetActive(true);
		}
		break;
	}
	return 0;
}
#endif

QBSoundMac::QBSoundMac(int maxChannel) : mGraph(NULL),QBSound(maxChannel)
{
}

//static void *LoaderThreadProc(void *arg)
//{
//	QBSoundMac* snd = (QBSoundMac*)arg;
//	bool r=false;
//	while (!r) {
//		pthread_mutex_lock(&snd->mMutex);
//		r = snd->mThreadEnd;
//		pthread_mutex_unlock(&snd->mMutex);
//		usleep(1000);
//	}
//	return NULL;
//}

int QBSoundMac::Exit()
{
//	pthread_mutex_lock(&mMutex);
//	mThreadEnd = true;
//	pthread_mutex_unlock(&mMutex);
	pthread_join(mLoaderThread,NULL);
	return 0;
}

int QBSoundMac::Reset()
{
	QBSoundLocker locker(&mMutex,"reset");
	return QBSound::Reset();
}

int QBSoundMac::Init()
{
	
	QBSound::Init();

	pthread_mutex_init(&mMutex,NULL);

	mOpenGraph = false;
	mInitGraph = false;
	
	return 0;
}

int QBSoundMac::initGraph()
{
	OSStatus err;

	mAudioFormat.mSampleRate = 44100.000000;
	mAudioFormat.mFormatID = kAudioFormatLinearPCM;

#ifdef __FLOAT_BUFFER__
    mAudioFormat.mFormatFlags = 0
	| kLinearPCMFormatFlagIsPacked
	| kLinearPCMFormatFlagIsFloat
#if __BIG_ENDIAN__
	| kLinearPCMFormatFlagIsBigEndian
#endif
	;
	mAudioFormat.mBytesPerPacket = 8;
	mAudioFormat.mFramesPerPacket = 1;
	mAudioFormat.mBytesPerFrame = 8;
	mAudioFormat.mChannelsPerFrame = 2;
	mAudioFormat.mBitsPerChannel = 32;
	mAudioFormat.mReserved = 0;
#else
    mAudioFormat.mFormatFlags = 0
	| kLinearPCMFormatFlagIsPacked
	| kLinearPCMFormatFlagIsSignedInteger 
#if __BIG_ENDIAN__
	| kLinearPCMFormatFlagIsBigEndian
#endif
	;
	mAudioFormat.mBytesPerPacket = 4;
	mAudioFormat.mFramesPerPacket = 1;
	mAudioFormat.mBytesPerFrame = 4;
	mAudioFormat.mChannelsPerFrame = 2;
	mAudioFormat.mBitsPerChannel = 16;
	mAudioFormat.mReserved = 0;
#endif

	err = NewAUGraph(&mGraph);

#if !TARGET_OS_IPHONE
    AudioComponentDescription description;
    description.componentSubType = kAudioUnitSubType_DefaultOutput;
#else
	AudioComponentDescription description;
	description.componentSubType = kAudioUnitSubType_RemoteIO;
#endif
    description.componentType = kAudioUnitType_Output;
    description.componentManufacturer = kAudioUnitManufacturer_Apple;
    description.componentFlags = 0;
    description.componentFlagsMask = 0;
#if 1
	err = AUGraphAddNode(mGraph,&description,&mOutputNode);
#else
	err = AUGraphNewNode(mGraph,&description, 0, NULL, &mOutputNode);
#endif

	err = AUGraphOpen(mGraph);

#if 1
	err = AUGraphNodeInfo(mGraph,mOutputNode,NULL,&mAudioUnit);
#else
	err = AUGraphGetNodeInfo(mGraph,mOutputNode,NULL,NULL,NULL,&mAudioUnit);
#endif

   err =  AudioUnitSetProperty(mAudioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &mAudioFormat,
                                  sizeof(AudioStreamBasicDescription));

	{
		AURenderCallbackStruct input;
		input.inputProc = callback;
		input.inputProcRefCon = this;
	err = 	AudioUnitSetProperty(mAudioUnit,
									  kAudioUnitProperty_SetRenderCallback,
									  kAudioUnitScope_Input,
									  0,
									  &input, sizeof(input));
	}
	
//	{
//		AudioUnitSetParameter (mAudioUnit,
//										kHALOutputParam_Volume,
//										kAudioUnitScope_Global,0,1,0);
//	}

	err = AUGraphInitialize(mGraph);
	//err = AUGraphStart(mGraph);
	
	//pthread_mutex_init(&mLoaderThreadMutex,NULL);
//#ifndef _IPHONE
//	mThreadEnd = false;
//	pthread_create(&mLoaderThread,NULL,LoaderThreadProc,this);
//#endif

//	mOpenGraph = false;
//	mInitGraph = false;

//	startAUGraph();

//printf("openAUGraph\n");
	return err;
}

void QBSoundMac::startAUGraphWithLock()
{
	QBSoundLocker locker(&mMutex,"startAUGraphWithLock");
	startAUGraph();
}

void QBSoundMac::stopAUGraphWithLock()
{
	QBSoundLocker locker(&mMutex,"stopAUGraphWithLock");
	stopAUGraph();
}

void QBSoundMac::startAUGraph()
{
//printf("startAUGraph\n");
	if (!mOpenGraph) {
		if (!mInitGraph) {
			initGraph();
			mInitGraph = true;
		}
		AUGraphStop(mGraph);
		AUGraphStart(mGraph);
	}
	mOpenGraph = true;
}

void QBSoundMac::stopAUGraph()
{
//printf("stopAUGraph\n");
	if (mOpenGraph) AUGraphStop(mGraph);
	mOpenGraph = false;
}

void QBSoundMac::stopAll()
{
//printf("stopAll\n");
	QBSoundLocker locker(&mMutex,"stopAll");
	return QBSound::stopAll();
}

int QBSoundMac::play(const char *note,int track,bool loop)
{
	if (note == NULL) return 0;
	startAUGraph();
	OSStatus err;
	int r=0;
	Boolean run;
	if (mGraph) {
		err = AUGraphIsRunning(mGraph,&run);
		if (err == noErr) {
			if (run) {
#if 1
				QBSoundLocker locker(&mMutex,"play");
				r = QBSound::play(note,track,loop);
#else
				pthread_mutex_lock(&mMutex);
				r = QBSound::play(note,track,loop);
				pthread_mutex_unlock(&mMutex);
#endif
			}
		}
	}
	return r;
}

int QBSoundMac::fill_sound_buffer(void* buffer,int size)
{
	QBSoundLocker locker(&mMutex,"fill_sound_buffer");
	return QBSound::fill_sound_buffer(buffer,size);
}

int QBSoundMac::stop(int track)
{
	QBSoundLocker locker(&mMutex,"stop");
	return QBSound::stop(track);
}

int QBSoundMac::isPlaying(int track)
{
	QBSoundLocker locker(&mMutex,"isPlaying");
	return QBSound::isPlaying(track);
}

int QBSoundMac::setVolume(int track,float volume)
{
	QBSoundLocker locker(&mMutex,"setVolume");
	return QBSound::setVolume(track,volume);
}

float QBSoundMac::getVolume(int track)
{
	QBSoundLocker locker(&mMutex,"getVolume");
	return QBSound::getVolume(track);
}

//int QBSoundMac::sysPause()
//{
//	return 0;
//}

//int QBSoundMac::sysPlay()
//{
//	return 0;
//}

int QBSoundMac::setMasterVolume(float volume)
{
	QBSoundLocker locker(&mMutex,"setMasterVolume");
	return QBSound::setMasterVolume(volume);
}

float QBSoundMac::getMasterVolume()
{
	QBSoundLocker locker(&mMutex,"getMasterVolume");
	return QBSound::getMasterVolume();
}

int QBSoundMac::playMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex,"playMML");
	return QBSound::playMML(mml,i);
}

int QBSoundMac::stopMML(int i)
{
	QBSoundLocker locker(&mMutex,"stopMML");
	return QBSound::stopMML(i);
}

int QBSoundMac::preloadMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex,"preloadMML");
	return QBSound::preloadMML(mml,i);
}

int QBSoundMac::pauseMML(int i)
{
	QBSoundLocker locker(&mMutex,"pauseMML");
	return QBSound::pauseMML(i);
}

int QBSoundMac::resumeMML(int i)
{
	QBSoundLocker locker(&mMutex,"resumeMML");
	return QBSound::resumeMML(i);
}

bool QBSoundMac::isPlayingMML(int i)
{
	QBSoundLocker locker(&mMutex,"isPlayingMML");
	return QBSound::isPlayingMML(i);
}

bool QBSoundMac::isPausedMML(int i)
{
	QBSoundLocker locker(&mMutex,"isPauseMML");
	return QBSound::isPausedMML(i);
}

int QBSoundMac::setVolumeMML(int i,float volume)
{
	QBSoundLocker locker(&mMutex,"setVolumeMML");
	return QBSound::setVolumeMML(i,volume);
}

float QBSoundMac::getVolumeMML(int i)
{
	QBSoundLocker locker(&mMutex,"getVolumeMML");
	return QBSound::getVolumeMML(i);
}

void QBSoundMac::noteOn(int i,int ch,int noteNo,int velocity)
{
	QBSoundLocker locker(&mMutex,"noteOn");
	return QBSound::noteOn(i,ch,noteNo,velocity);
}

void QBSoundMac::noteOff(int i,int ch)
{
	QBSoundLocker locker(&mMutex,"noteOff");
	return QBSound::noteOff(i,ch);
}

void QBSoundMac::setWav9(int no,int initVol,bool loop,const char* data)
{
	QBSoundLocker locker(&mMutex,"setWav9");
	QBSound::setWav9(no,initVol,loop,data);
}

void QBSoundMac::setWav10(int no,const char* data)
{
	QBSoundLocker locker(&mMutex,"setWav10");
	QBSound::setWav10(no,data);
}

void QBSoundMac::setWav13(int no,const char* data)
{
	QBSoundLocker locker(&mMutex,"setWav13");
	QBSound::setWav13(no,data);
}

unsigned long QBSoundMac::idleCount() {
	QBSoundLocker locker(&mMutex,"idleCount");
	return QBSound::idleCount();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
