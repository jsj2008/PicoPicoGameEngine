#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MOscMod.h"
#include "FlMML_Vector.h"
#include "MEngineData.h"

namespace FlMML {

class MOscGbWave : public MOscMod {
public:
protected:
	MOscGbWaveData* m_data;
	int m_waveNo;
public:
	MOscGbWave(MOscGbWaveData* data) {
		m_data = data;
	}
	virtual void setWaveNo(int waveNo) {
		if (waveNo>=MOscGbWaveData::MAX_WAVE)waveNo=MOscGbWaveData::MAX_WAVE-1;
		if (!m_data->s_table[waveNo])waveNo=0;
		m_waveNo=waveNo;
	}
	virtual Number getNextSample() {
		Number val=m_data->s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=m_data->s_table[m_waveNo][((m_phase+ofs)&PHASE_MSK)>>(PHASE_SFT+11)];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=m_data->s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool> syncin,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			if (syncin[i]){
				resetPhase();
			}
			(samples)[i]=m_data->s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool> syncout,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=m_data->s_table[m_waveNo][m_phase>>(PHASE_SFT+11)];
			m_phase+=m_freqShift;
			syncout[i]=(m_phase>PHASE_MSK);
			m_phase&=PHASE_MSK;
		}
	}
};

}
