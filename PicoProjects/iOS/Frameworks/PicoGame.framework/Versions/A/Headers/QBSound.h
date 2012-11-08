/*-----------------------------------------------------------------------------------------------
	名前	QBSound.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __QBSOUND_H__
#define __QBSOUND_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;

#include <FlMML/MChannel.h>

typedef struct _PSGTrack {
	
	char *note;
	char *noteptr;
	char *ptr;
	char *loopptr;
	
	float volume;
	
	int m_velocity;
	int m_noteShift;
	int m_polyVoice;
	int m_maxPipe;
	int m_maxSyncSource;
	float m_gate;
	float m_maxGate;
	int m_gate2;
//	float m_bpm;
	float m_spt;
	bool m_relativeDir;
	bool m_velDir;
	bool m_velDetail;
	int m_octave;
	int m_portamento;
	int m_beforeNote;

	unsigned long notect;
	unsigned long noteoff;
	int end;
	
	int nlen;
	int tempo;

	int _note;
	FlMML::MChannel* channel;
} PSGTrack;

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class QBSound {
public:
	static QBSound* sharedSound();
	static void exitSound();

	QBSound(int maxChannel);
	virtual ~QBSound() {};
	
	virtual void startAUGraph() {}
	virtual void stopAUGraph() {}
	
	virtual int Init();
	virtual int Exit();
	virtual int Reset();
	
	virtual void stopAll();

	virtual int play(const char* mml,int track=0,bool loop=false);
	virtual int stop(int track);
	virtual int isPlaying(int channel=0);
	virtual int setVolume(int track,float volume);
	virtual float getVolume(int track);
	virtual void setWav9(int waveNo,int initVol,bool loop,const char* data);
	virtual void setWav10(int waveNo,const char* data);
	virtual void setWav13(int waveNo,const char* data);

	virtual int setMasterVolume(float volume);
	virtual float getMasterVolume();
	
	virtual int playMML(const char* mml,int track);
	virtual int preloadMML(const char* mml,int track);
	virtual int stopMML(int track);
	virtual int pauseMML(int track);
	virtual int resumeMML(int track);
	virtual bool isPlayingMML(int track);
	virtual bool isPausedMML(int track);
	virtual int setVolumeMML(int track,float volume);
	virtual float getVolumeMML(int track);
	
	virtual void noteOn(int track,int ch,int noteNo,float velocity);
	virtual void noteOff(int track,int ch);

	PSGTrack* mTrack;
	FlMML::MEngineData waveData;
	int mMaxTrack;
	int mFlMMLNum;
	
	float mMasterVolume;

	int init_track(int maxChannel);
	virtual int fill_sound_buffer(void* buffer,int size);
	void analyze_note(PSGTrack *track);

	virtual void openSEMMLLibrary(PPLuaScript* script,const char* name);
	virtual void openFlMMLLibrary(PPLuaScript* script,const char* name);

	virtual void idleSEMML();
};

#ifdef __cplusplus
extern "C" {
#endif

#if TARGET_OS_IPHONE
int QBSound_SetCategory(const char* category);
#endif

#ifdef __cplusplus
};
#endif

#endif //__QBSOUND_H__

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
