/*-----------------------------------------------------------------------------------------------
	名前	QBSoundMac.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __QBSOUNDMAC_H__
#define __QBSOUNDMAC_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <pthread.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AUComponent.h>
#include "QBSound.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class QBSoundMac : public QBSound {
public:
	QBSoundMac(int maxChannel);
	virtual ~QBSoundMac() {
		if (mGraph) AUGraphStop(mGraph);
	};
	
	virtual void startAUGraphWithLock();
	virtual void stopAUGraphWithLock();

	virtual void startAUGraph();
	virtual void stopAUGraph();

	virtual void stopAll();

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
	
	virtual unsigned long idleCount();
	
	AudioUnit mAudioUnit;
	AudioStreamBasicDescription mAudioFormat;
	AUNode mOutputNode;
	AUGraph mGraph;
	
	pthread_mutex_t mMutex;
	
	pthread_t mLoaderThread;
	
//	bool mThreadEnd;

private:	
	int initGraph();
	
	bool mOpenGraph;
	bool mInitGraph;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};
#endif

#endif //__QBSOUNDMAC_H__

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
