#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Number.h"
#include "MOscMod.h"
#include "MEngineData.h"

namespace FlMML {

typedef struct _MOscPicoWaveTrack {
	float w;
	float f;
	float noise;
	signed long lastlevel;
	int v;
	float fv;
	int type;
} MOscPicoWaveTrack;

#define S_PI        3.14159265358979323846264338327950288f   /* pi */
#define NOISE_LONG 0x4000

class MOscPicoWave : public MOscMod {
public:
	static const int MAX_WAVE=6;
protected:
	static int s_init;
	int m_waveNo;
public:
	MOscPicoWave() {
		boot();
		memset(&track,0,sizeof(track));
		setWaveNo(0);
	}
	static void boot() {
		if (s_init)return ;
		create_noise(noise_lut,13,NOISE_LONG);
		s_init=1;
	}
	virtual Number getNextSample() {
		Number val=get_level();
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=get_level();
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=get_level();
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncIn(VNumber* samples,Vector<bool>* syncin,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			if ((*syncin)[i]){
				resetPhase();
			}
			(samples)[i]=get_level();
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		}
	}
	virtual void getSamplesWithSyncOut(VNumber* samples,Vector<bool>* syncout,int start,int end) {
		int i;
		for (i=start;i<end;i++){
			(samples)[i]=get_level();
			m_phase+=m_freqShift;
			(*syncout)[i]=(m_phase>PHASE_MSK);
			m_phase&=PHASE_MSK;
		}
	}
	virtual void setWaveNo(int waveNo) {
		if (waveNo>=MAX_WAVE)waveNo=MAX_WAVE-1;
		if (waveNo<0)waveNo=0;
		m_waveNo=waveNo;
		track.type = m_waveNo;
	}

	static unsigned char noise_lut[NOISE_LONG];

	static void create_noise(unsigned char *buf, const int bits, int size)
	{
		static int m = 0x0011;
		int xor_val, i;

		for (i = 0; i < size; i++) {
			xor_val = m & 1;
			m >>= 1;
			xor_val ^= (m & 1);
			m |= xor_val << (bits - 1);
			buf[i] = m;
		}
	}

//	static float freqtable[11];
	
	Number get_level() {
//		int note = 0;		//m_freqShiftとm_phaseからnoteとoctを作成m_frequency
//		int oct = 0;
//		if (note == 0) {
//			track.f = freqtable[oct];
//		} else {
//			track.f = freqtable[oct] * powf(2.0,note/12.0);
//		}
		track.v = 0x7fff;//*(8/64.0);
		track.f = m_frequency;
		if (track.type == 5) {
			track.noise = (2.0*S_PI/(1000+powf(((10000-track.f)/10000),10)*2000000));
		}
		signed long s = get_level(&track);
		if (s >= 0x7fff) s =  0x7fff;
		if (s < -0x7fff) s = -0x7fff;
		
		return ((float)s)/0x7fff;
	}
	
	MOscPicoWaveTrack track;

	signed long get_level(MOscPicoWaveTrack *track)
	{
		signed long s = 0;
		float w;

		if (track->f == 0) {
			track->w = 0;
			if (track->lastlevel > 0) {
				track->lastlevel --;
			} else
			if (track->lastlevel < 0) {
				track->lastlevel ++;
			}
			return track->lastlevel;
		}

		switch (track->type) {
		case 5:
			//track->w += (2.0*S_PI/(1000));//(2.0*S_PI*track->f/44100.0)/powf(10,track->f/100);
			//track->w += (2.0*S_PI/(2000000));//(2.0*S_PI*track->f/44100.0)/powf(10,track->f/100);
			track->w += track->noise;//(2.0*S_PI/(1000+powf(((10000-track->f)/10000),10)*2000000));//(2.0*S_PI*track->f/44100.0)/powf(10,track->f/100);
			while (track->w >= 2*S_PI) track->w -= 2*S_PI;
			while (track->w < 0) track->w += 2*S_PI;
			break;
		default:
			track->w += 2.0*S_PI*track->f/44100.0;
			while (track->w >= 2*S_PI) track->w -= 2*S_PI;
			while (track->w < 0) track->w += 2*S_PI;
			break;
		}
		w = track->w;
		switch (track->type) {
		case 0:
			s += (signed short)(track->v*0.5*(w>S_PI?1:-1));			//矩形波(PSG的)
			break;
		case 1:
			s += (signed short)(track->v*0.5*(w<2*S_PI/12?1:-1));		//矩形波(ファミコン的)
			break;
		case 2:
			s += (signed short)(track->v*0.5*sinf((w+S_PI)));			//サイン波形
			break;
		case 3:
			s += (signed short)(track->v*0.5*(w-S_PI)/S_PI);			//三角波
			break;
		case 4:
			s += (signed short)(track->v*0.5*(((w>S_PI?(2*S_PI-w):w)/*-S_PI/2*/)/(S_PI/2))*4);		//三角波(ファミコン的)
			break;
		case 5:
			//s += (signed short)(track->v*((w>S_PI?1:-1)*(((((float)noise_lut[(int)(w/(2*S_PI)*NOISE_LONG)])/256.0)-1)*1.0)));			//矩形波(PSG的)
			s += (signed short)(track->v*1.5*(((((float)noise_lut[(int)(w/(2*S_PI)*(NOISE_LONG))])/256.0)-1)));	//ノイズ
			break;
		}
		track->lastlevel = s;
		return s;
	}

};

}
