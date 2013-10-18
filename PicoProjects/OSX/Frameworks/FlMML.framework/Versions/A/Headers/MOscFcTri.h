#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Math.h"
#include "MOscMod.h"
#include "FlMML_Vector.h"

namespace FlMML {

class MOscFcTri : public MOscMod {
public:
	static const int FC_TRI_TABLE_LEN=(1<<5);
	static const int MAX_WAVE=2;
protected:
	static int s_init;
	static Number s_table[MAX_WAVE][FC_TRI_TABLE_LEN];
	int m_waveNo;
public:
	MOscFcTri() {
		boot();
		
		setWaveNo(0);
	}
	static void boot() {
		if (s_init)return ;
		int i;
		for (i=0;i<16;i++){
			s_table[0][i]=s_table[0][31-i]=Number (i)*2.0/15.0-1.0;
		}
		for (i=0;i<32;i++){
			s_table[1][i]=(i<8)?Number (i)*2.0/14.0:((i<24)?Number (8-i)*2.0/15.0+1.0:Number (i-24)*2.0/15.0-1.0);
		}
		s_init=1;
	}
	virtual Number getNextSample() {
		Number val=s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=s_table[m_waveNo][((m_phase+ofs)&PHASE_MSK)>>(PHASE_SFT+11)];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool>* syncin,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			if ((*syncin)[i]){
				resetPhase();
			}
			(samples)[i]=s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool>* syncout,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
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
