#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Number.h"
#include "FlMML_StaticDef.h"
#include "FlMML_Vector.h"

namespace FlMML {

class MOscMod {
public:
	static const int TABLE_LEN=1<<16;
	static const int PHASE_SFT=14;
	static const int PHASE_LEN=TABLE_LEN<<PHASE_SFT;
	static const int PHASE_HLF=TABLE_LEN<<(PHASE_SFT-1);
	static const int PHASE_MSK=PHASE_LEN-1;
protected:
	Number m_frequency;
	int m_freqShift;
	int m_phase;
public:
	MOscMod() {
		m_frequency = 0;
		m_freqShift = 0;
		m_phase = 0;
		resetPhase();
		setFrequency(440.0);
	}
	virtual ~MOscMod() {
	}
	virtual void setShortMode() {
	}
	virtual void setLongMode() {
	}
	virtual void resetPhase() {
		m_phase=0;
	}
	virtual void addPhase(int time) {
		m_phase=(m_phase+m_freqShift*time)&PHASE_MSK;
	}
	virtual Number getNextSample() {
		return 0;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		return 0;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool>* syncin,int start,int end) {
		getSamples(samples,start,end);
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool>* syncout,int start,int end) {
		getSamples(samples,start,end);
	}
	virtual Number getFrequency() {
		return m_frequency;
	}
	virtual void setFrequency(Number frequency) {
		m_frequency=frequency;
		m_freqShift=frequency*(PHASE_LEN/MSequencer_RATE44100);
	}
	virtual void setWaveNo(int waveNo) {
	}
	virtual void setNoteNo(int noteNo) {
	}
	virtual void setNoiseFreq(Number frequency) {
	}
	virtual void restoreFreq() {
	}
	virtual void disableResetPhase() {
	}
};

}
