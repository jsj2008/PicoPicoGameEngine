/*-----------------------------------------------------------------------------------------------
	名前	QBSoundLinux.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBSoundLinux.h"
#include <math.h>

class QBSoundLocker {
public:
	QBSoundLocker(pthread_mutex_t *_locker) {
		locker = _locker;
		pthread_mutex_lock(locker);
	}
	virtual ~QBSoundLocker() {
		pthread_mutex_unlock(locker);
	}
	
	pthread_mutex_t *locker;
};

QBSoundLinux::QBSoundLinux(int maxChannel) : QBSound(maxChannel)
{
}

int QBSoundLinux::Init()
{
	QBSound::Init();
	
	mBufferSize=1024;
	mSndBuffer = calloc(1,mBufferSize);
	
	stopWave = false;
}

int QBSoundLinux::play(const char *note,int track,bool loop)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::play(note,track,loop);
}

int QBSoundLinux::Exit()
{
	return 0;
}

int QBSoundLinux::Reset()
{
	QBSoundLocker locker(&mMutex);
	return QBSound::Reset();
}

int QBSoundLinux::fill_sound_buffer(void* buffer,int size)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::fill_sound_buffer(buffer,size);
}

int QBSoundLinux::stop(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::stop(track);
}

int QBSoundLinux::isPlaying(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPlaying(track);
}

int QBSoundLinux::setVolume(int track,float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setVolume(track,volume);
}

float QBSoundLinux::getVolume(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getVolume(track);
}

int QBSoundLinux::setMasterVolume(float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setMasterVolume(volume);
}

float QBSoundLinux::getMasterVolume()
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getMasterVolume();
}

int QBSoundLinux::playMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::playMML(mml,i);
}

int QBSoundLinux::stopMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::stopMML(i);
}

int QBSoundLinux::preloadMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::preloadMML(mml,i);
}

int QBSoundLinux::pauseMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::pauseMML(i);
}

int QBSoundLinux::resumeMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::resumeMML(i);
}

bool QBSoundLinux::isPlayingMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPlayingMML(i);
}

bool QBSoundLinux::isPausedMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPausedMML(i);
}

int QBSoundLinux::setVolumeMML(int i,float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setVolumeMML(i,volume);
}

float QBSoundLinux::getVolumeMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getVolumeMML(i);
}

void QBSoundLinux::noteOn(int i,int ch,int noteNo,int velocity)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::noteOn(i,ch,noteNo,velocity);
}

void QBSoundLinux::noteOff(int i,int ch)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::noteOff(i,ch);
}

void QBSoundLinux::setWav9(int no,int initVol,bool loop,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav9(no,initVol,loop,data);
}

void QBSoundLinux::setWav10(int no,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav10(no,data);
}

void QBSoundLinux::setWav13(int no,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav13(no,data);
}

void QBSoundLinux::playSound()
{
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
