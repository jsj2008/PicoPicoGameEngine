#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MOscMod.h"
#include "FlMML_Vector.h"

namespace FlMML {

/**
   Special thanks to OffGao.
 */
class MOscGbLNoise : public MOscMod {
public:
	static const int GB_NOISE_PHASE_SFT=12;
	static const int GB_NOISE_PHASE_DLT=1<<GB_NOISE_PHASE_SFT;
	static const int GB_NOISE_TABLE_LEN=32767;
	static const int GB_NOISE_TABLE_MOD=(GB_NOISE_TABLE_LEN<<GB_NOISE_PHASE_SFT)-1;
protected:
	static int s_init;
	static float s_table[GB_NOISE_TABLE_LEN];
	static int s_interval[];
	
	int m_sum;
	int m_skip;
public:
	MOscGbLNoise() {
		boot();
		
		m_sum=0;
		m_skip=0;
	}
	static void boot() {
		if (s_init)return ;
		unsigned int gbr=0xffff;
		unsigned int output=1;
//		s_table.reserve(GB_NOISE_TABLE_LEN);
		for (int i=0;i<GB_NOISE_TABLE_LEN;i++){
			if (gbr==0)gbr=1;
			gbr+=(gbr+(((gbr>>14)^(gbr>>13))&1));
			output^=(gbr&1);
			s_table[i]=(((int)output)&1)*2-1;
		}
		s_init=1;
	}
	virtual Number getNextSample() {
		Number val=s_table[m_phase>>GB_NOISE_PHASE_SFT];
		if (m_skip>0){
			val=(val+m_sum)/((Number)(m_skip+1));
		}
		m_sum=0;
		m_skip=0;
		int freqShift=m_freqShift;
		while(freqShift>GB_NOISE_PHASE_DLT){
			m_phase=(m_phase+GB_NOISE_PHASE_DLT)%GB_NOISE_TABLE_MOD;
			freqShift-=GB_NOISE_PHASE_DLT;
			m_sum+=s_table[m_phase>>GB_NOISE_PHASE_SFT];
			m_skip++;
		}
		m_phase=(m_phase+freqShift)%GB_NOISE_TABLE_MOD;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		int phase=(m_phase+ofs)%GB_NOISE_TABLE_MOD;
		Number val=s_table[(phase+((phase>>31)&GB_NOISE_TABLE_MOD))>>GB_NOISE_PHASE_SFT];
		m_phase=(m_phase+m_freqShift)%GB_NOISE_TABLE_MOD;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		Number val;
		for (i=start;i<end;i++){
			val=s_table[m_phase>>GB_NOISE_PHASE_SFT];
			if (m_skip>0){
				val=(val+m_sum)/((Number)(m_skip+1));
			}
			(samples)[i]=val;
			m_sum=0;
			m_skip=0;
			int freqShift=m_freqShift;
			while(freqShift>GB_NOISE_PHASE_DLT){
				m_phase=(m_phase+GB_NOISE_PHASE_DLT)%GB_NOISE_TABLE_MOD;
				freqShift-=GB_NOISE_PHASE_DLT;
				m_sum+=s_table[m_phase>>GB_NOISE_PHASE_SFT];
				m_skip++;
			}
			m_phase=(m_phase+freqShift)%GB_NOISE_TABLE_MOD;
		}
	}
	virtual void setFrequency(Number frequency) {
		m_frequency=frequency;
	}
	virtual void setNoiseFreq(int no) {
		if (no<0)no=0;
		if (no>63)no=63;
		m_freqShift=(1048576<<(GB_NOISE_PHASE_SFT-2))/((double)s_interval[no]*11025);
	}
	virtual void setNoteNo(int noteNo) {
		setNoiseFreq(noteNo);
	}
};

}
