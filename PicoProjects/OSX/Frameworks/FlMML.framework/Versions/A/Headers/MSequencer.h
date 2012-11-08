#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
using namespace std;

#include "MChannel.h"
#include "MOscillator.h"
#include "MEnvelope.h"

#include "FlMML_Number.h"
#include "MTrack.h"
#include "FlMML_Math.h"
#include "FlMML_StaticDef.h"
#include "FlMML_Array.h"
#include "MEngineData.h"

namespace FlMML {

class FlMMLPlayer;

class MSequencer {
public:
	//	Function* onSignal;
	static const int BUFFER_SIZE=MSequencerData::BUFFER_SIZE;
	//	static const int BUFFER_SIZE=8192;
	static const int RATE44100=MSequencer_RATE44100;
protected:
	static const int STATUS_STOP=0;
	static const int STATUS_PAUSE=1;
	static const int STATUS_BUFFERING=2;
	static const int STATUS_PLAY=3;
	static const int STATUS_LAST=4;
	static const int STATUS_SOUND=5;
	static const int STEP_NONE=0;
	static const int STEP_PRE=1;
	static const int STEP_TRACK=2;
	static const int STEP_POST=3;
	static const int STEP_COMPLETE=4;
	Vector<Number> m_buffer;
	int m_playSide;
	int m_playSize;
	int m_step;
	int m_processTrack;
	int m_processOffset;
	bool m_output; //! 現在バッファ書き込み中かどうか
	Array<MTrack> m_trackArray;
	unsigned int m_globalTick;
	int m_multiple;
	unsigned int m_pausedPos;
	FILE* m_fp;
	int m_status;
public:
	string m_filepath;
	float m_masterVolume;
	FlMMLPlayer* engine;
	unsigned int m_startTime;
	
	MSequencer(/*int multiple=32*/) : engine(NULL) {
		m_multiple=32;
		m_output=false;
		m_trackArray.nofree = true;
		m_playSide=1;
		m_playSize=0;
		m_step=STEP_NONE;
		m_pausedPos=0;
//		m_buffer=NULL;
		setMasterVolume(100);
		stop();
	}
	
	virtual ~MSequencer();
	
	virtual unsigned int getTimer() {
		return 0;
	}
	
	virtual void start() {
	}
	
	virtual void play() {
		m_startTime = getTimer();
		if (m_status!=STATUS_PAUSE){
			stop();
			m_globalTick=0;
			for (int i=0;i<m_trackArray.size();i++){
				m_trackArray[i]->seekTop();
			}
			m_status=STATUS_BUFFERING;
			processStart();
		}
		else {
			m_status=STATUS_PLAY;
		}
	}
	
	virtual void stop() {
		m_status=STATUS_STOP;
		m_pausedPos=0;
	}
	
	virtual void pause() {
		m_pausedPos=getNowMSec();
		m_status=STATUS_PAUSE;
	}
	
	virtual void setMasterVolume(int vol) {
		m_masterVolume = ((float)(Math::min(Math::max(vol, 0), 127)))/127.0f;
//		m_masterVolume *= m_masterVolume;
//		m_masterVolume = ((float)(Math::min(Math::max(vol, 0), 127)))/127.0f;
	}
	
	virtual bool isPlaying() {
		if  (m_status>STATUS_PAUSE && m_status!=STATUS_SOUND) {
			for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
				if (!m_trackArray[i]->isPlayEnd()) return true;
			}
		}
		return false;
	}
	
	virtual bool isPaused() {
		return (m_status==STATUS_PAUSE);
	}
	
	virtual void disconnectAll() {
		//		while(m_trackArray.pop()){}
		for (int i=0;i<m_trackArray.size();i++) {
			m_trackArray[i]->m_playSound = false;
		}
		m_trackArray.clear();
		m_status=STATUS_STOP;
	}
	
	virtual void connect(MTrack* track) {
		//		track->m_signalInterval=m_signalInterval;
		m_trackArray.push(track);
	}
	
	virtual unsigned int getGlobalTick() {
		return m_globalTick;
	}
	
	virtual void noteOn(int track,int noteNo,int velocity);
	virtual void noteOff(int track);
	
protected:
private:
	void reqStop() {
		//		m_stopTimer->start();
	}
	void reqBuffering() {
		m_status = STATUS_BUFFERING;
		//		m_buffTimer->start();
	}
	
	void processStart() {
#if 0
printf("processStart()\n");
		#define BUFFER_TYPE float
//		#define OUT_BUFFER_TYPE float
		#define OUT_BUFFER_TYPE signed short
//		#define OUT_BUFFER_TYPE int32_t
		m_step=STEP_PRE;
		m_processOffset=0;
		
		m_fp = fopen(m_filepath.c_str(),"wb");
		if (m_fp == NULL) return;
		
		int size=1024/8;
		BUFFER_TYPE* buffer = (BUFFER_TYPE*)malloc(size*sizeof(BUFFER_TYPE));
		OUT_BUFFER_TYPE* outBuffer = (OUT_BUFFER_TYPE*)malloc(size*sizeof(OUT_BUFFER_TYPE));
		Vector<BUFFER_TYPE> b;
		b.nofree = true;
		b.array = buffer;
		b.size_of_array = size;
		b.count = b.size_of_array-1;
		int nLen=(int)m_trackArray.size();

		for (int i=0;i<nLen;i++) {
			m_trackArray[i]->m_infinityLoop = false;
		}
		
		while (1) {
			float b[size];
			for (int i=0;i<size;i++) {
				b[i] = 0;
			}
			for (int i=0;i<nLen;i++) {
				m_trackArray[i]->onSampleData(b,0,size/2);
			}
			for (int i=0;i<size;i++) {
				b[i] *= m_masterVolume;
				if (b[i] < -1.0) b[i] = -1.0;
				if (b[i] >  1.0) b[i] =  1.0;
			}
			
			for (int i=0;i<size;i++) {
//				outBuffer[i] = buffer[i];				//float
				if (buffer[i] >= 1.0) {
					outBuffer[i] = 32767;
				} else
				if (buffer[i] <= -1.0) {
					outBuffer[i] = -32768;
				} else {
					outBuffer[i] = buffer[i]*32767;			//signed short
				}
//				outBuffer[i] = buffer[i]*2147483647;	//int32_t
			}
			fwrite(outBuffer,1,size*sizeof(OUT_BUFFER_TYPE),m_fp);
			
			if (m_trackArray[MTrack::TEMPO_TRACK]->isEnd()){
				break;
			}
		}
		
		if (buffer) free(buffer);
		if (outBuffer) free(outBuffer);
		
		fclose(m_fp);
		m_fp = NULL;
#endif
	}
	
	
public:
	virtual unsigned int getTotalMSec() {
		if (m_trackArray[MTrack::TEMPO_TRACK]) {
			return m_trackArray[MTrack::TEMPO_TRACK]->getTotalMSec();
		}
		return 0;
	}
	virtual unsigned int getNowMSec() {
		unsigned int now=0;
		unsigned int tot=getTotalMSec();
		switch(m_status){
			case STATUS_PLAY:
			case STATUS_LAST:
				now=getTimer()-m_startTime+m_pausedPos;
				return (now<tot)?now:tot;
			default :
				return m_pausedPos;
		}
		return 0;
	}
	virtual string getNowTimeStr() {
		int sec=Math::ceil(getNowMSec()/1000.0);
		char str[256];
		sprintf(str,"%02d:%02d",(sec/60),(sec%60));
		string s = str;
		return s;
	}
};

}
