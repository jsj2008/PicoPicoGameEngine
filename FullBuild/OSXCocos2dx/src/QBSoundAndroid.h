/*-----------------------------------------------------------------------------------------------
	名前	QBSoundAndroid.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __QBSOUNDANDROID_H__
#define __QBSOUNDANDROID_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <pthread.h>
#include "QBSound.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class QBSoundAndroid : public QBSound {
public:
	QBSoundAndroid(int maxChannel);
	virtual ~QBSoundAndroid() {
	};

	virtual int Init();
	virtual int Exit();
	virtual int Reset();
	virtual int fill_sound_buffer(void* buffer,int size);

	virtual int play(const char* mml,int channel=0,bool loop=false);
	virtual int stop(int trackindex);
	virtual int isPlaying(int channel=0);
	virtual int setVolume(int track,float volume=1.0);
	virtual float getVolume(int track);
	virtual void setWav9(int no,int initVol,bool loop,const char* data);
	virtual void setWav10(int no,const char* data);
	virtual void setWav13(int no,const char* data);

	virtual int setMasterVolume(float volume);
	virtual float getMasterVolume();

//	virtual int sysPause();
//	virtual int sysPlay();

	virtual int playMML(const char* mml,int track);
	virtual int preloadMML(const char* mml,int track);
	virtual int stopMML(int track);
	virtual int pauseMML(int track);
	virtual int resumeMML(int track);
	virtual bool isPlayingMML(int track);
	virtual bool isPausedMML(int track);
	virtual int setVolumeMML(int track,float volume);
	virtual float getVolumeMML(int track);

	virtual void noteOn(int track,int ch,int noteNo,int velocity);
	virtual void noteOff(int track,int ch);

	pthread_mutex_t mMutex;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};
#endif

#endif //__QBSOUNDANDROID_H__

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
