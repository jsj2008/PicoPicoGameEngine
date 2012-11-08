/*-----------------------------------------------------------------------------------------------
	名前	PPFlMMLSeq.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPFlMMLSeq.h"
#include <sys/time.h>
//#include <Accelerate/Accelerate.h>

namespace FlMML {

PPFlMMLSeq::PPFlMMLSeq()
{
	volume = 1.0;
}

PPFlMMLSeq::~PPFlMMLSeq()
{
}

void PPFlMMLSeq::play()
{
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
}

void PPFlMMLSeq::disconnectAll()
{
	if (MSequencer::isPlaying() || m_status == STATUS_SOUND) {
		MSequencer::stop();
	}
	MSequencer::disconnectAll();
}

unsigned int PPFlMMLSeq::getTimer()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return ((double)(tv.tv_sec)*1000 + (double)(tv.tv_usec)* 0.001);
}

void PPFlMMLSeq::fillSoundBuffer(void* buffer,int size)
{
#ifdef __FLOAT_BUFFER__
	#define __BUFFER_TYPE__ float
	int ssize = size/sizeof(__BUFFER_TYPE__);
	__BUFFER_TYPE__* b = (__BUFFER_TYPE__*)buffer;
	bool playSound = false;
	if (m_status==STATUS_BUFFERING || m_status==STATUS_PLAY) {
		playSound = true;
	}
	if (m_status == STATUS_SOUND) {
		for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
			if (m_trackArray[i]->m_playSound) {
				playSound = true;
			}
		}
	}
	if (playSound) {
		m_buffer.reserve(ssize);
		for (int i=0;i<ssize;i++) {
			m_buffer[i] = 0;
		}
		if (m_status==STATUS_BUFFERING || m_status==STATUS_PLAY) {
			for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
				m_trackArray[i]->onSampleData(m_buffer.array,0,ssize/2);
			}
			m_status = STATUS_PLAY;
		}
		if (m_status == STATUS_SOUND) {
			for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
				if (m_trackArray[i]->m_playSound) {
					m_trackArray[i]->onGetSamples(m_buffer.array,0,ssize/2);
				}
			}
		}
		for (int i=0;i<ssize;i++) {
			Number n = m_buffer[i]*m_masterVolume;;
			if (n < -1.0f) n = -1.0f;
			if (n >  1.0f) n =  1.0f;
			b[i] += n;
		}
	}
#else
	#define __BUFFER_TYPE__ signed short
	int ssize = size/sizeof(__BUFFER_TYPE__);
	__BUFFER_TYPE__* b = (__BUFFER_TYPE__*)buffer;
	bool playSound = false;
	if (m_status==STATUS_BUFFERING || m_status==STATUS_PLAY) {
		playSound = true;
	}
	if (m_status == STATUS_SOUND) {
		for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
			if (m_trackArray[i]->m_playSound) {
				playSound = true;
			}
		}
	}
	if (playSound) {
		m_buffer.reserve(ssize);
		for (int i=0;i<ssize;i++) {
			m_buffer[i] = 0;
		}
		if (m_status==STATUS_BUFFERING || m_status==STATUS_PLAY) {
			for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
				m_trackArray[i]->onSampleData(m_buffer.array,0,ssize/2);
			}
			m_status = STATUS_PLAY;
		}
		if (m_status == STATUS_SOUND) {
			for (int i=MTrack::FIRST_TRACK;i<m_trackArray.size();i++) {
				if (m_trackArray[i]->m_playSound) {
					m_trackArray[i]->onGetSamples(m_buffer.array,0,ssize/2);
				}
			}
		}
		for (int i=0;i<ssize;i++) {
			Number n = m_buffer[i]*m_masterVolume;;
			if (n < -1.0f) n = -1.0f;
			if (n >  1.0f) n =  1.0f;
			b[i] += (__BUFFER_TYPE__)(n*0x7fff);
		}
	}
#endif
}

}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
