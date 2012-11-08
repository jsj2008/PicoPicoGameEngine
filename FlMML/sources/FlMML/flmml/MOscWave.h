#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Number.h"
#include "MOscMod.h"
#include "MEngineData.h"

namespace FlMML {

class MOscWave : public MOscMod {
public:
protected:
	int m_waveNo;
	MOscWaveData* m_data;
public:
	MOscWave(MOscWaveData* data) {
		m_data = data;
		setWaveNo(0);
	}
	virtual void setWaveNo(int waveNo) {
		if (waveNo>=MOscWaveData::MAX_WAVE)waveNo=MOscWaveData::MAX_WAVE-1;
		if (!m_data->s_table[waveNo])waveNo=0;
		m_waveNo=waveNo;
	}
	virtual Number getNextSample() {
		if (m_data->s_table[m_waveNo]==NULL) return 0;
		Number val=m_data->s_table[m_waveNo][int(m_phase/m_data->s_length[m_waveNo])];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		if (m_data->s_table[m_waveNo]==NULL) return 0;
		Number val=m_data->s_table[m_waveNo][int(((m_phase+ofs)&PHASE_MSK)/m_data->s_length[m_waveNo])];
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		if (m_data->s_table[m_waveNo]==NULL) return;
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=m_data->s_table[m_waveNo][int(m_phase/m_data->s_length[m_waveNo])];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool>* syncin,int start,int end) {
		if (m_data->s_table[m_waveNo]==NULL) return;
		int i;
		for (i=start;i<end;i++){
			if ((*syncin)[i]){
				resetPhase();
			}
			(samples)[i]=m_data->s_table[m_waveNo][int(m_phase/m_data->s_length[m_waveNo])];
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool>* syncout,int start,int end) {
		if (m_data->s_table[m_waveNo]==NULL) return;
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=m_data->s_table[m_waveNo][int(m_phase/m_data->s_length[m_waveNo])];
			m_phase+=m_freqShift;
			(*syncout)[i]=(m_phase>PHASE_MSK);
			m_phase&=PHASE_MSK;
		}
	}
};

}
