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

#ifdef __USE_OGG_VORBIS__
#include <map>
#include "QBStreamSound.h"

typedef map<unsigned int,QBStreamSound*> PlayerList;
static PlayerList streamTrack;
static int soundID = 1;
#endif

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

#ifdef __USE_OGG_VORBIS__
static void *LoaderThreadProc(void *arg)
{
	QBSoundMac* snd = (QBSoundMac*)arg;
	bool r=false;
	while (!r) {
    {
      QBSoundLocker locker(&snd->mMutex,"threadProc");
      
      PlayerList::iterator it = streamTrack.begin();
      while(it != streamTrack.end()) {
        (*it).second->streamLoad();
        ++it;
      }
      r = snd->mThreadEnd;
    }
		usleep(1000);
	}
	return NULL;
}
#endif

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

int QBSoundMac::Exit()
{
	QBSoundLocker locker(&mMutex,"Exit");
	mThreadEnd = true;
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

#ifdef __USE_OGG_VORBIS__
	mThreadEnd = false;
	pthread_create(&mLoaderThread,NULL,LoaderThreadProc,this);
#endif

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
	int r= QBSound::fill_sound_buffer(buffer,size);

#ifdef __USE_OGG_VORBIS__
  PlayerList::iterator it = streamTrack.begin();
  while(it != streamTrack.end()) {
    if ((*it).second->streamdone && (*it).first > 0) {
      delete (*it).second;
      streamTrack.erase(it++);
    } else {
      (*it).second->fill_sound_buffer(buffer,size,mMasterVolume);
      ++it;
    }
  }
#endif
  
  return r;
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

#pragma mark -

#ifdef __USE_OGG_VORBIS__

static QBStreamSound* findPlayer(int track)
{
  QBStreamSound* player=NULL;
  PlayerList::iterator p = streamTrack.find(track);
  if (p != streamTrack.end()) {
    player = p->second;
  } else {
    player = new QBStreamSound();
    streamTrack.insert(make_pair(track,player));
  }
  return player;
}

void QBSoundMac::streamPlay(const char* filename,int track)
{
	QBSoundLocker locker(&mMutex,"streamPlay");
  findPlayer(track)->play(filename);
}

void QBSoundMac::streamLoopPlay(const char* filename,long long looppoint,int track)
{
	QBSoundLocker locker(&mMutex,"streamPlay");
  findPlayer(track)->play(filename,looppoint);
}

void QBSoundMac::streamStop(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  findPlayer(track)->stop();
}

void QBSoundMac::streamPause(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  findPlayer(track)->pause();
}

void QBSoundMac::streamResume(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  streamTrack[track]->resume();
}

void QBSoundMac::streamRewind(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  findPlayer(track)->rewind();
}

void QBSoundMac::streamWillPlay(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  findPlayer(track)->willplay();
}

bool QBSoundMac::streamIsPlaying(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  PlayerList::iterator p = streamTrack.find(track);
  if (p == streamTrack.end()) {
    return false;
  }
  return findPlayer(track)->isPlaying();
}

void QBSoundMac::streamSetVolume(float volume,int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  findPlayer(track)->setVolume(volume);
}

float QBSoundMac::streamGetVolume(int track)
{
	QBSoundLocker locker(&mMutex,"streamStop");
  return findPlayer(track)->getVolume();
}

bool QBSoundMac::streamTest(const char* filename)
{
  return QBStreamSound::test(filename);
}

#pragma mark -

class EffectBuffer {
public:
	signed short* pcmbuffer;
	unsigned long pcmsize;
	int pcmchannel;
  std::string fname;

  EffectBuffer() : pcmbuffer(NULL) {
    
  }
  virtual ~EffectBuffer() {
    if (pcmbuffer) free(pcmbuffer);
printf("unload effect %s\n",fname.c_str());
  }

  bool load(const char* filename) {
    fname=filename;
    int ret=-1;
		FILE* fp = fopen(filename,"r");
    if (fp) {
      do {
        fseek(fp,0,SEEK_END);
        size_t size = ftell(fp);
        if (size==0) {
          fclose(fp);
          return -1;
        }
        fseek(fp,0,SEEK_SET);
        pcmbuffer = (signed short*)malloc(size);
        if (pcmbuffer) {
          pcmsize = size;
          fread(pcmbuffer,size,1,fp);
          ret=0;
        }
      } while (0);
      fclose(fp);
    }
printf("load effect %s\n",filename);
    return ret;
  }
};

typedef map<unsigned int, EffectBuffer *> EffectList;

static unsigned int _Hash(const char *key)
{
    size_t len = strlen(key);
    const char *end=key+len;
    unsigned int hash;

    for (hash = 0; key < end; key++)
    {
        hash *= 16777619;
        hash ^= (unsigned int) (unsigned char) toupper(*key);
    }
    return (hash);
}

static EffectList& sharedList()
{
    static EffectList s_List;
    return s_List;
}

void QBSoundMac::preloadEffect(const char* filename)
{
	QBSoundLocker locker(&mMutex,"preloadEffect");
  do {
    if (filename==NULL) break;
  
    unsigned int nRet = _Hash(filename);
    
    if (sharedList().end() != sharedList().find(nRet)) break;
    
    sharedList().insert(make_pair(nRet,new EffectBuffer()));
    EffectBuffer* buffer = sharedList()[nRet];
    
    if (buffer->load(filename)!=0) {
      sharedList().erase(nRet);
    }

  } while (0);
}

unsigned int QBSoundMac::playEffect(const char* filename,bool bLoop,float pitch, float pan, float gain)
{
	QBSoundLocker locker(&mMutex,"playEffect");
  if (filename) {
    unsigned int nRet = _Hash(filename);
  
    do {
      if (filename==NULL) break;
    
      unsigned int nRet = _Hash(filename);
      
      if (sharedList().end() != sharedList().find(nRet)) break;
      
      sharedList().insert(make_pair(nRet,new EffectBuffer()));
      EffectBuffer* buffer = sharedList()[nRet];
      
      if (buffer->load(filename)!=0) {
        sharedList().erase(nRet);
      }

    } while (0);

    EffectList::iterator p = sharedList().find(nRet);
    if (p != sharedList().end())
    {
      EffectBuffer* effect = p->second;
      findPlayer(soundID)->setVolume(gain);
      findPlayer(soundID)->play((unsigned char*)effect->pcmbuffer,effect->pcmsize);
      soundID ++;
      if (soundID > 9999) soundID = 1;
    }

    return soundID;
  }
  return 0;
}

void QBSoundMac::stopEffect(unsigned int nSoundId)
{
	QBSoundLocker locker(&mMutex,"stopEffect");
  PlayerList::iterator p = streamTrack.find(nSoundId);
  if (p != streamTrack.end()) {
    findPlayer(nSoundId)->stop();
  }
}

void QBSoundMac::unloadEffect(const char* filename)
{
	QBSoundLocker locker(&mMutex,"unloadEffect");
  if (filename) {
    unsigned int nRet = _Hash(filename);

    EffectList::iterator p = sharedList().find(nRet);
    if (p != sharedList().end())
    {
      delete p->second;
      p->second = NULL;
      sharedList().erase(nRet);
    }
  }
}

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
