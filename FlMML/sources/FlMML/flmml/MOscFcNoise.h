#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MOscMod.h"
#include "FlMML_Vector.h"

namespace FlMML {


class MOscFcNoise : public MOscMod {
public:
	static const int FC_NOISE_PHASE_SFT=10;
	static const int FC_NOISE_PHASE_SEC=(1789773<<FC_NOISE_PHASE_SFT);
	static const int FC_NOISE_PHASE_DLT=FC_NOISE_PHASE_SEC/44100;
protected:
	static int s_interval[];
	
	int m_fcr;
	int m_snz;
	Number m_val;
private:
	Number getValue() {
		m_fcr>>=1;
		m_fcr|=((m_fcr^(m_fcr>>m_snz))&1)<<15;
		return (m_fcr&1)?1.0:-1.0;
	}
public:
	virtual void setShortMode() {
		m_snz=6;
	}
	virtual void setLongMode() {
		m_snz=1;
	}
	MOscFcNoise() {
		boot();
		
		setLongMode();
		m_fcr=0x8000;
		m_val=getValue();
		setNoiseFreq(0);
	}
	virtual void resetPhase() {
	}
	virtual void addPhase(int time) {
		m_phase=m_phase+FC_NOISE_PHASE_DLT*time;
		while(m_phase>=m_freqShift){
			m_phase-=m_freqShift;
			m_val=getValue();
		}
	}
	static void boot() {
	}
	virtual Number getNextSample() {
		Number val=m_val;
		Number sum=0;
		Number cnt=0;
		int delta=FC_NOISE_PHASE_DLT;
		while(delta>=m_freqShift){
			delta-=m_freqShift;
			m_phase=0;
			sum+=getValue();
			cnt+=1.0;
		}
		if (cnt>0){
			m_val=sum/cnt;
		}
		m_phase+=delta;
		if (m_phase>=m_freqShift){
			m_phase-=m_freqShift;
			m_val=getValue();
		}
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		int fcr=m_fcr;
		int phase=m_phase;
		Number val=m_val;
		Number sum=0;
		Number cnt=0;
		int delta=FC_NOISE_PHASE_DLT+ofs;
		while(delta>=m_freqShift){
			delta-=m_freqShift;
			m_phase=0;
			sum+=getValue();
			cnt+=1.0;
		}
		if (cnt>0){
			m_val=sum/cnt;
		}
		m_phase+=delta;
		if (m_phase>=m_freqShift){
			m_phase=m_freqShift;
			m_val=getValue();
		}
		
		m_fcr=fcr;
		m_phase=phase;
		getNextSample();
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		//Number val;
		for (i=start;i<end;i++){
			(samples)[i]=getNextSample();
		}
	}
	virtual void setFrequency(Number frequency) {
		//m_frequency = frequency;
		m_freqShift=FC_NOISE_PHASE_SEC/frequency;
	}
	virtual void setNoiseFreq(int no) {
		if (no<0)no=0;
		if (no>15)no=15;
		m_freqShift=s_interval[no]<<FC_NOISE_PHASE_SFT; // as interval
	}
	virtual void setNoteNo(int noteNo) {
		setNoiseFreq(noteNo);
	}
};

}
