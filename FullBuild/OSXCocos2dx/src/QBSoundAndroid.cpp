/*-----------------------------------------------------------------------------------------------
	名前	QBSoundAndroid.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBSoundAndroid.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

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

QBSoundAndroid::QBSoundAndroid(int maxChannel) : QBSound(maxChannel)
{
}

int QBSoundAndroid::Exit()
{
	int r=0;
	pthread_mutex_lock(&mMutex);
	r = QBSound::Exit();
	pthread_mutex_unlock(&mMutex);
	pthread_mutex_destroy(&mMutex);
	return r;
}

int QBSoundAndroid::Reset()
{
	QBSoundLocker locker(&mMutex);
	return QBSound::Reset();
}

int QBSoundAndroid::Init()
{
	QBSound::Init();
	pthread_mutex_init(&mMutex,NULL);
	return 0;
}

int QBSoundAndroid::play(const char *note,int track,bool loop)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::play(note,track,loop);
}

int QBSoundAndroid::fill_sound_buffer(void* buffer,int size)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::fill_sound_buffer(buffer,size);
}

int QBSoundAndroid::stop(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::stop(track);
}

int QBSoundAndroid::isPlaying(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPlaying(track);
}

int QBSoundAndroid::setVolume(int track,float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setVolume(track,volume);
}

float QBSoundAndroid::getVolume(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getVolume(track);
}

//int QBSoundAndroid::sysPause()
//{
//	mPause = 0.0;
//	return 0;
//}

//int QBSoundAndroid::sysPlay()
//{
//	mPause = 1.0;
//	return 0;
//}

int QBSoundAndroid::setMasterVolume(float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setMasterVolume(volume);
}

float QBSoundAndroid::getMasterVolume()
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getMasterVolume();
}

int QBSoundAndroid::playMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::playMML(mml,i);
}

int QBSoundAndroid::stopMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::stopMML(i);
}

int QBSoundAndroid::preloadMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::preloadMML(mml,i);
}

int QBSoundAndroid::pauseMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::pauseMML(i);
}

int QBSoundAndroid::resumeMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::resumeMML(i);
}

bool QBSoundAndroid::isPlayingMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPlayingMML(i);
}

bool QBSoundAndroid::isPausedMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPausedMML(i);
}

int QBSoundAndroid::setVolumeMML(int i,float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setVolumeMML(i,volume);
}

float QBSoundAndroid::getVolumeMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getVolumeMML(i);
}

void QBSoundAndroid::noteOn(int i,int ch,int noteNo,int velocity)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::noteOn(i,ch,noteNo,velocity);
}

void QBSoundAndroid::noteOff(int i,int ch)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::noteOff(i,ch);
}

void QBSoundAndroid::setWav9(int no,int initVol,bool loop,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav9(no,initVol,loop,data);
}

void QBSoundAndroid::setWav10(int no,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav10(no,data);
}

void QBSoundAndroid::setWav13(int no,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav13(no,data);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
