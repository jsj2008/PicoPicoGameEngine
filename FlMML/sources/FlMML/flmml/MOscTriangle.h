#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MOscMod.h"

namespace FlMML {

class MOscTriangle : public MOscMod {
public:
	static const int MAX_WAVE=2;
protected:
	static int s_init;
	static Number s_table[MAX_WAVE][TABLE_LEN];
	int m_waveNo;
public:
	MOscTriangle() {
		boot();
		
		setWaveNo(0);
	}
	static void boot() {
		if (s_init)return ;
		Number d0=1.0/TABLE_LEN;
		Number p0;
		int i;
		for (i=0,p0=0.0;i<TABLE_LEN;i++){
			s_table[0][i]=(p0<0.50)?(1.0-4.0*p0):(1.0-4.0*(1.0-p0));
			s_table[1][i]=(p0<0.25)?(0.0-4.0*p0):((p0<0.75)?(-2.0+4.0*p0):(4.0-4.0*p0));
			p0+=d0;
		}
		s_init=1;
	}
	virtual Number getNextSample() {
		Number val=s_table[m_waveNo][m_phase>>PHASE_SFT];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=s_table[m_waveNo][((m_phase+ofs)&PHASE_MSK)>>PHASE_SFT];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=s_table[m_waveNo][m_phase>>PHASE_SFT];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool>* syncin,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			if ((*syncin)[i]){
				resetPhase();
			}
			(samples)[i]=s_table[m_waveNo][m_phase>>PHASE_SFT];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool>* syncout,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=s_table[m_waveNo][m_phase>>PHASE_SFT];
			m_phase+=m_freqShift;
			(*syncout)[i]=(m_phase>PHASE_MSK);
			m_phase&=PHASE_MSK;
		}
	}
	virtual void setWaveNo(int waveNo) {
		if (waveNo>=MAX_WAVE)waveNo=MAX_WAVE-1;
		if (waveNo<0)waveNo=0;
		m_waveNo=waveNo;//Math::min(waveNo,MAX_WAVE-1);
	}
};

}
