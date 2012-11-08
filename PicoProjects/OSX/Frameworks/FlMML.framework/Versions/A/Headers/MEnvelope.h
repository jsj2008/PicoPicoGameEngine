#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MEnvelopePoint.h"
#include "FlMML_Number.h"
#include "FlMML_Math.h"
#include "FlMML_StaticDef.h"
#include "FlMML_Vector.h"

namespace FlMML {

// Many-Point Envelope
// 高速化のためにサンプルごとに音量を加算/減算する形に。
// Numberの仮数部は52bitもあるからそんなに誤差とかは気にならないはず。
class MEnvelope {
private:
	MEnvelopePoint* m_envelopePoint;
	MEnvelopePoint* m_envelopeLastPoint;
	MEnvelopePoint* m_currentPoint;
	Number m_releaseTime;
	Number m_currentVal;
	Number m_releaseStep;
	bool m_releasing;
	Number m_step;
	bool m_playing;
	int m_counter;
	int m_timeInSamples;
protected:
	static int s_init;
	static const int s_volumeLen=256;
	static Number s_volumeMap[3][s_volumeLen];
public:
	// 以前のバージョンとの互換性のためにADSRで初期化
	MEnvelope(Number attack,Number decay,Number sustain,Number release) {
		m_step = 0;
		m_counter = 0;
		m_timeInSamples = 0;
		m_releaseTime = 0;
		m_envelopePoint = NULL;
		m_envelopeLastPoint = NULL;
		m_currentPoint = NULL;
		setAttack(attack);
		addPoint(decay,sustain);
		setRelease(release);
		m_playing=false;
		m_currentVal=0;
		m_releasing=true;
		m_releaseStep=0;
	}
	virtual ~MEnvelope() {
		if (m_envelopePoint) delete m_envelopePoint;
	}
	
	static void boot() {
		if (!s_init){
			int i;
			// MEnvelopeのエンベロープは256段階であることに注意する。
			for (i=0;i<3;i++){
				s_volumeMap[i][0]=0.0;
			}
			for (i=1;i<s_volumeLen;i++){
				s_volumeMap[0][i]=i/255.0;
				s_volumeMap[1][i]=Math::pow(10.0,(i-255.0)*(48.0/(255.0*20.0))); // min:-48db
				s_volumeMap[2][i]=Math::pow(10.0,(i-255.0)*(96.0/(255.0*20.0))); // min:-96db
			}
			s_init=1;
		}
	}
	
	void setAttack(Number attack) {
		if (m_envelopePoint) {
			delete m_envelopePoint;
		}
		m_envelopePoint=m_envelopeLastPoint=new MEnvelopePoint();
		m_envelopePoint->time=0;
		m_envelopePoint->level=0;
		addPoint(attack,1.0);
	}
	void setRelease(Number release) {
		m_releaseTime=((release>0)?release:(1.0/127.0))*MSequencer_RATE44100;
		// 現在のボリュームなどを設定
		if (m_playing&&!m_releasing){
			m_counter=m_timeInSamples;
			m_currentPoint=m_envelopePoint;
			while(m_currentPoint->next!=NULL&&m_counter>=m_currentPoint->next->time){
				m_currentPoint=m_currentPoint->next;
				m_counter-=m_currentPoint->time;
			}
			if (m_currentPoint->next==NULL){
				m_currentVal=m_currentPoint->level;
			}else {
				m_step=(m_currentPoint->next->level-m_currentPoint->level)/m_currentPoint->next->time;
				m_currentVal=m_currentPoint->level+(m_step*m_counter);
			}
		}
	}
	void addPoint(Number time,Number level) {
		MEnvelopePoint* point=new MEnvelopePoint();
		point->time=time*MSequencer_RATE44100;
		point->level=level;
		m_envelopeLastPoint->next=point;
		m_envelopeLastPoint=point;
	}
	
	void triggerEnvelope(int zeroStart) {
		m_playing=true;
		m_releasing=false;
		m_currentPoint=m_envelopePoint;
		m_currentVal=m_currentPoint->level=(zeroStart)?0:m_currentVal;
		m_step=(1.0-m_currentVal)/m_currentPoint->next->time;
		m_timeInSamples=m_counter=0;
	}
	
	void releaseEnvelope() {
		m_releasing=true;
		m_releaseStep=(m_currentVal/m_releaseTime);
	}
	
	void soundOff() {
		releaseEnvelope();
		m_playing=false;
	}
	
	Number getNextAmplitudeLinear() {
		if (!m_playing)return 0;
		
		if (!m_releasing){
			if (m_currentPoint->next==NULL){
				m_currentVal=m_currentPoint->level;
			}else {
				bool processed=false;
				while(m_counter>=m_currentPoint->next->time){
					m_counter=0;
					m_currentPoint=m_currentPoint->next;
					if (m_currentPoint->next==NULL){
						m_currentVal=m_currentPoint->level;
						processed=true;
						break;
					}else {
						m_step=(m_currentPoint->next->level-m_currentPoint->level)/m_currentPoint->next->time;
						m_currentVal=m_currentPoint->level;
						processed=true;
					}
				}
				if (!processed){
					m_currentVal+=m_step;
				}
				m_counter++;
			}
		}else {
			m_currentVal-=m_releaseStep;
		}
		if (m_currentVal<=0&&m_releasing){
			m_playing=false;
			m_currentVal=0;
		}
		m_timeInSamples++;
		return m_currentVal;
	}
	void ampSamplesLinear(VNumber* samples,int start,int end,Number velocity) {
		int i;Number amplitude=m_currentVal*velocity;
		for (i=start;i<end;i++){
			if (!m_playing){
				(samples)[i]=0;
				continue;
			}
			
			if (!m_releasing){
				if (m_currentPoint->next==NULL){
					
				}else {
					bool processed=false;
					while(m_counter>=m_currentPoint->next->time){
						m_counter=0;
						m_currentPoint=m_currentPoint->next;
						if (m_currentPoint->next==NULL){
							m_currentVal=m_currentPoint->level;
							processed=true;
							break;
						}else {
							m_step=(m_currentPoint->next->level-m_currentPoint->level)/m_currentPoint->next->time;
							m_currentVal=m_currentPoint->level;
							processed=true;
						}
					}
					if (!processed){
						m_currentVal+=m_step;
					}
					amplitude=m_currentVal*velocity;
					m_counter++;
				}
			}else {
				m_currentVal-=m_releaseStep;
				amplitude=m_currentVal*velocity;
			}
			if (m_currentVal<=0&&m_releasing){
				m_playing=false;
				amplitude=m_currentVal=0;
			}
			m_timeInSamples++;
			(samples)[i]*=amplitude;
		}
	}
	void ampSamplesNonLinear(VNumber* samples,int start,int end,Number velocity,int volMode) {
		int i;
		for (i=start;i<end;i++){
			if (!m_playing){
				(samples)[i]=0;
				continue;
			}
			
			if (!m_releasing){
				if (m_currentPoint->next==NULL){
					m_currentVal=m_currentPoint->level;
				}else {
					bool processed=false;
					while(m_counter>=m_currentPoint->next->time){
						m_counter=0;
						m_currentPoint=m_currentPoint->next;
						if (m_currentPoint->next==NULL){
							m_currentVal=m_currentPoint->level;
							processed=true;
							break;
						}else {
							m_step=(m_currentPoint->next->level-m_currentPoint->level)/m_currentPoint->next->time;
							m_currentVal=m_currentPoint->level;
							processed=true;
						}
					}
					if (!processed){
						m_currentVal+=m_step;
					}
					m_counter++;
				}
			}else {
				m_currentVal-=m_releaseStep;
			}
			if (m_currentVal<=0&&m_releasing){
				m_playing=false;
				m_currentVal=0;
			}
			m_timeInSamples++;
			
			int cv=(m_currentVal*255);
			if (cv>255){
				cv=0;	// 0にするのは過去バージョンを再現するため。
			}
			(samples)[i]*=s_volumeMap[volMode][cv]*velocity;
		}
	}
	bool isPlaying() {
		return m_playing;
	}
	bool isReleasing() {
		return m_releasing;
	}
};

}
