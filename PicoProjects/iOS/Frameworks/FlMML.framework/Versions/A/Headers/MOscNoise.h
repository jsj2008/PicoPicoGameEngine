#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Math.h"
#include "MOscMod.h"
#include "MOscMod.h"

namespace FlMML {

class MOscNoise : public MOscMod {
public:
	static const int TABLE_MSK=TABLE_LEN-1;
	static const int NOISE_PHASE_SFT=30;
	static const int NOISE_PHASE_MSK=(1<<NOISE_PHASE_SFT)-1;
protected:
	Number m_noiseFreq;
	unsigned int m_counter;
	bool m_resetPhase;
	static int s_init;
	static Number s_table[TABLE_LEN];
public:
	MOscNoise() {
		boot();
		
		setNoiseFreq(1.0);
		m_phase=0;
		m_counter=0;
		m_resetPhase=true;
	}
	virtual void disableResetPhase() {
		m_resetPhase=false;
	}
	static void boot() {
		if (s_init)return ;
		for (int i=0;i<TABLE_LEN;i++){
			s_table[i]=Math::random()*2.0-1.0;
		}
		s_init=1;
	}
	virtual void resetPhase() {
		if (m_resetPhase)m_phase=0;
	}
	virtual void addPhase(int time) {
		m_counter=(m_counter+m_freqShift*time);
		m_phase=(m_phase+(m_counter>>NOISE_PHASE_SFT))&TABLE_MSK;
		m_counter&=NOISE_PHASE_MSK;
	}
	virtual Number getNextSample() {
		Number val=s_table[m_phase];
		m_counter=(m_counter+m_freqShift);
		m_phase=(m_phase+(m_counter>>NOISE_PHASE_SFT))&TABLE_MSK;
		m_counter&=NOISE_PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=s_table[(m_phase+(ofs<<PHASE_SFT))&TABLE_MSK];
		m_counter=(m_counter+m_freqShift);
		m_phase=(m_phase+(m_counter>>NOISE_PHASE_SFT))&TABLE_MSK;
		m_counter&=NOISE_PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=s_table[m_phase];
			m_counter=(m_counter+m_freqShift);
			m_phase=(m_phase+(m_counter>>NOISE_PHASE_SFT))&TABLE_MSK;
			m_counter&=NOISE_PHASE_MSK;
		}
	}
	virtual void setFrequency(Number frequency) {
		m_frequency=frequency;
	}
	virtual void setNoiseFreq(Number frequency) {
		m_noiseFreq=frequency*(1<<NOISE_PHASE_SFT);
		m_freqShift=m_noiseFreq;
	}
	virtual void restoreFreq() {
		m_freqShift=m_noiseFreq;
	}
};

}
