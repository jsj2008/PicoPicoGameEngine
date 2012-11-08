#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MOscNoise.h"
#include "MOscMod.h"

namespace FlMML {

class MOscPulse : public MOscMod {
protected:
	int m_pwm;
	int m_mix;
	MOscNoise* m_modNoise;
public:
	MOscPulse() {
		boot();
		
		setPWM(0.5);
		setMIX(0);
	}
	static void boot() {
	}
	virtual Number getNextSample() {
		Number val=(m_phase<m_pwm)?1.0:(m_mix?m_modNoise->getNextSample():-1.0);
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=(((m_phase+ofs)&PHASE_MSK)<m_pwm)?1.0:(m_mix?m_modNoise->getNextSampleOfs(ofs):-1.0);
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		if (m_mix){	// MIXモード
			for (i=start;i<end;i++){
				(samples)[i]=(m_phase<m_pwm)?1.0:m_modNoise->getNextSample();
				m_phase=(m_phase+m_freqShift)&PHASE_MSK;
			}
		}
		else {		// 通常の矩形波
			for (i=start;i<end;i++){
				(samples)[i]=(m_phase<m_pwm)?1.0:-1.0;
				m_phase=(m_phase+m_freqShift)&PHASE_MSK;
			}
		}
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool>* syncin,int start,int end) {
		int i;
		if (m_mix){	// MIXモード
			for (i=start;i<end;i++){
				if ((*syncin)[i])resetPhase();
				(samples)[i]=(m_phase<m_pwm)?1.0:m_modNoise->getNextSample();
				m_phase=(m_phase+m_freqShift)&PHASE_MSK;
			}
		}
		else {		// 通常の矩形波
			for (i=start;i<end;i++){
				if ((*syncin)[i])resetPhase();
				(samples)[i]=(m_phase<m_pwm)?1.0:-1.0;
				m_phase=(m_phase+m_freqShift)&PHASE_MSK;
			}
		}
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool>* syncout,int start,int end) {
		int i;
		if (m_mix){	// MIXモード
			for (i=start;i<end;i++){
				(samples)[i]=(m_phase<m_pwm)?1.0:m_modNoise->getNextSample();
				m_phase+=m_freqShift;
				(*syncout)[i]=(m_phase>PHASE_MSK);
				m_phase&=PHASE_MSK;
			}
		}
		else {		// 通常の矩形波
			for (i=start;i<end;i++){
				(samples)[i]=(m_phase<m_pwm)?1.0:-1.0;
				m_phase+=m_freqShift;
				(*syncout)[i]=(m_phase>PHASE_MSK);
				m_phase&=PHASE_MSK;
			}
		}
	}
	void setPWM(Number pwm) {
		m_pwm=pwm*PHASE_LEN;
	}
	void setMIX(int mix) {
		m_mix=mix;
	}
	void setNoise(MOscNoise* noise) {
		m_modNoise=noise;
	}
};

}
