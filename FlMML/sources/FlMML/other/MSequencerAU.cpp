#include "MSequencerAU.h"
#include <unistd.h>
#include <sys/time.h>

namespace FlMML {

OSStatus MSequencerAU::callback(
								void *inRefCon,
								AudioUnitRenderActionFlags* ioActionFlags,
								const AudioTimeStamp* inTimeStamp,
								UInt32 inBusNumber,
								UInt32 inNumberFrames,
								AudioBufferList* ioData)
{
	MSequencerAU* snd = (MSequencerAU*)inRefCon;
	if (snd) {
		pthread_mutex_lock(&snd->mMutex);
		UInt32 bytesToRead;
//printf("%ld\n",snd->mCount);
//		snd->mCount++;
		bytesToRead = ioData->mBuffers[0].mDataByteSize;
		snd->fillSoundBuffer(ioData->mBuffers[0].mData,bytesToRead);
		pthread_mutex_unlock(&snd->mMutex);
	}
	return 0;
}

MSequencerAU::MSequencerAU(/*int multiple=32*/) : mGraph(NULL),mCount(0),MSequencer(/*multiple*/)
{
	pthread_mutex_init(&mMutex,NULL);
}

MSequencerAU::~MSequencerAU()
{
	if (mGraph) {
		AUGraphClose(mGraph);
		DisposeAUGraph(mGraph);
	}
	pthread_mutex_destroy(&mMutex);
}

void MSequencerAU::start()
{
	MSequencer::start();
	
	OSStatus err;
	
	mAudioFormat.mSampleRate = 44100.000000;
	mAudioFormat.mFormatID = kAudioFormatLinearPCM;

#if 1
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
	
//	mMixerAudioUnit = setupMixerUnit(1);
	
//	{
//	    AudioComponentDescription description;
//		description.componentType = kAudioUnitType_Mixer;
//#if !TARGET_OS_IPHONE
//		description.componentSubType = kAudioUnitSubType_MultiChannelMixer;
//#else
//		description.componentSubType = kAudioUnitSubType_AU3DMixerEmbedded;
//#endif
//		description.componentManufacturer = kAudioUnitManufacturer_Apple;
//		description.componentFlags = 0;
//		description.componentFlagsMask = 0;
//		err = AUGraphAddNode(mGraph,&description,&mMixerNode);
//	}

	{
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
		err = AUGraphAddNode(mGraph,&description,&mOutputNode);
	}
		
	err = AUGraphOpen(mGraph);

	//kAudioSessionProperty_PreferredHardwareIOBufferDuration
	
	{
		AudioUnit mAudioUnit;
		err = AUGraphNodeInfo(mGraph,mOutputNode,NULL,&mAudioUnit);
		err =  AudioUnitSetProperty(mAudioUnit,
									kAudioUnitProperty_StreamFormat,
									kAudioUnitScope_Input,
									0,
									&mAudioFormat,
									sizeof(AudioStreamBasicDescription));
		{
			AURenderCallbackStruct input;
			input.inputProc = MSequencerAU::callback;
			input.inputProcRefCon = this;
			err = 	AudioUnitSetProperty(mAudioUnit,
										 kAudioUnitProperty_SetRenderCallback,
										 kAudioUnitScope_Input,
										 0,
										 &input, sizeof(input));
		}
	}

//	{
//		AudioUnit mAudioUnit;
//		UInt32 elementCount = 1;
//		err = AUGraphNodeInfo(mGraph,mMixerNode,NULL,&mAudioUnit);
// 		err = AudioUnitSetProperty(mAudioUnit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Input, 0, &elementCount, sizeof(UInt32));
//		err = AudioUnitSetProperty(mAudioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &mAudioFormat, sizeof(AudioStreamBasicDescription));
////		err = AudioUnitSetProperty(mAudioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &mAudioFormat, sizeof(AudioStreamBasicDescription));
//		mMixerAudioUnit = mAudioUnit;
//		{
//			AURenderCallbackStruct input;
//			input.inputProc = MSequencerAU::callback;
//			input.inputProcRefCon = this;
//			err = 	AudioUnitSetProperty(mAudioUnit,
//										 kAudioUnitProperty_SetRenderCallback,
//										 kAudioUnitScope_Input,
//										 0,
//										 &input, sizeof(input));
//		}
//		AudioUnitInitialize(mAudioUnit);
////		AudioUnitSetParameter(mMixerAudioUnit, kMultiChannelMixerParam_Volume, kAudioUnitScope_Input, 0, 0.5, 0);
// 	}
	
//	AUGraphConnectNodeInput (mGraph, mMixerNode, 0, mOutputNode, 0);
	
	err = AUGraphInitialize(mGraph);
	err = AUGraphStart(mGraph);
}

void MSequencerAU::play()
{
	pthread_mutex_lock(&mMutex);
	m_startTime = getTimer();
	if (m_status!=STATUS_PAUSE){
		MSequencer::stop();
		m_globalTick=0;
		for (int i=0;i<m_trackArray.size();i++){
			m_trackArray[i]->seekTop();
		}
		m_status=STATUS_BUFFERING;
	}
	else {
		m_status=STATUS_PLAY;
	}
	pthread_mutex_unlock(&mMutex);
}

void MSequencerAU::stop()
{
	pthread_mutex_lock(&mMutex);
	MSequencer::stop();
	pthread_mutex_unlock(&mMutex);
}

void MSequencerAU::pause()
{
	pthread_mutex_lock(&mMutex);
	MSequencer::pause();
	pthread_mutex_unlock(&mMutex);
}

void MSequencerAU::setMasterVolume(int vol)
{
	pthread_mutex_lock(&mMutex);
	m_masterVolume = ((float)(Math::min(Math::max(vol, 0), 127)))/127.0f;
//	m_masterVolume *= m_masterVolume;
//	m_masterVolume = ((float)(Math::min(Math::max(vol, 0), 127)))/127.0f;
//printf("vol %d m_masterVolume %f\n",vol,m_masterVolume);
#if 1
//	AudioUnitParameterValue volume = Math::min(Math::max(vol, 0), 127);
//	OSStatus err = AudioUnitSetParameter(mMixerAudioUnit,
//			     kMultiChannelMixerParam_Volume,
//			     kAudioUnitScope_Input,
//			     0,
//			     volume/127.0f,
//			     0);
//	if ( err != noErr ) {
//		printf( "AudioUnitSetParameter(kHALOutputParam_Volume) failed. err=%ld\n", err );
//	} else {
//		printf("volume %f\n",volume/127.0f);
//	}
#endif
	pthread_mutex_unlock(&mMutex);
}

bool MSequencerAU::isPlaying()
{
	bool t;
	pthread_mutex_lock(&mMutex);
	t = MSequencer::isPlaying();
	pthread_mutex_unlock(&mMutex);
	return t;
}

bool MSequencerAU::isPaused()
{
	bool t;
	pthread_mutex_lock(&mMutex);
	t = MSequencer::isPaused();
	pthread_mutex_unlock(&mMutex);
	return t;
}

void MSequencerAU::disconnectAll()
{
	pthread_mutex_lock(&mMutex);
	if (MSequencer::isPlaying()) {
		MSequencer::stop();
	}
	MSequencer::disconnectAll();
	pthread_mutex_unlock(&mMutex);
}

void MSequencerAU::fillSoundBuffer(void* buffer,int size)
{
#if 1
	#define BUFFER_TYPE float
	int ssize = size/sizeof(BUFFER_TYPE);
//	Vector<BUFFER_TYPE> b;
//	b.nofree = true;
//	b.array = (BUFFER_TYPE*)buffer;
//	b.size_of_array = ssize;
//	b.count = b.size_of_array-1;
	float* b = (float*)buffer;
	if (m_status==STATUS_BUFFERING || m_status==STATUS_PLAY) {
		int nLen=(int)m_trackArray.size();
		for (int i=0;i<ssize;i++) {
			b[i] = 0;
		}
		for (int i=0;i<nLen;i++) {
			m_trackArray[i]->onSampleData(b,0,ssize/2);
		}
		for (int i=0;i<ssize;i++) {
			b[i] *= m_masterVolume;
			if (b[i] < -1.0) b[i] = -1.0;
			if (b[i] >  1.0) b[i] =  1.0;
		}
		if (m_trackArray[MTrack::TEMPO_TRACK]->isEnd()){
//cout << "last" << endl;
			m_status = STATUS_STOP;
		} else {
			m_status = STATUS_PLAY;
		}
	} else {
		for (int i=0;i<ssize;i++) {
			b[i] = 0;
		}
	}
#else
	#define BUFFER_TYPE unsigned short
	int ssize = size/sizeof(BUFFER_TYPE);
	BUFFER_TYPE* b = (BUFFER_TYPE*)buffer;
	if (m_status==STATUS_BUFFERING || m_status==STATUS_PLAY) {
		m_buffer[0].reserve(ssize);
		for (int i=0;i<ssize;i++) {
			m_buffer[0][i] = 0;
		}
		int nLen=(int)m_trackArray.size();
		for (int i=MTrack::FIRST_TRACK;i<nLen;i++) {
			m_trackArray[i]->onSampleData(&m_buffer[0],0,ssize/2);
		}
		m_status = STATUS_PLAY;
		for (int i=0;i<ssize;i++) {
			Number n = m_buffer[0][i];
			if (n < -1.0f) n = -1.0f;
			if (n >  1.0f) n =  1.0f;
			b[i] = (BUFFER_TYPE)(n*0x7fff);
		}
	} else {
		for (int i=0;i<ssize;i++) {
			b[i] = 0;
		}
	}
#endif
}

unsigned int MSequencerAU::getNowMSec()
{
	unsigned int t;
	pthread_mutex_lock(&mMutex);
	t = MSequencer::getNowMSec();
	pthread_mutex_unlock(&mMutex);
	return t;
}

unsigned int MSequencerAU::getTimer()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return ((double)(tv.tv_sec)*1000 + (double)(tv.tv_usec)* 0.001);
}

}
