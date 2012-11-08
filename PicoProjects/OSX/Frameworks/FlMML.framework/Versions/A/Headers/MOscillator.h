#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "MOscMod.h"
#include "MOscSine.h"
#include "MOscSaw.h"
#include "MOscTriangle.h"
#include "MOscPulse.h"
#include "MOscNoise.h"
#include "MOscFcTri.h"
#include "MOscFcNoise.h"
#include "MOscFcDpcm.h"
#include "MOscWave.h"
#include "MOscGbWave.h"
#include "MOscGbLNoise.h"
#include "MOscGbSNoise.h"
#include "MOscOPM.h"
#include "MOscPicoWave.h"

namespace FlMML {

class MOscillator {
public:
	static const int SINE=0;
	static const int SAW=1;
	static const int TRIANGLE=2;
	static const int PULSE=3;
	static const int NOISE=4;
	static const int FC_PULSE=5;
	static const int FC_TRI=6;
	static const int FC_NOISE=7;
	static const int FC_S_NOISE=8;
	static const int FC_DPCM=9;
	static const int GB_WAVE=10;
	static const int GB_NOISE=11;
	static const int GB_S_NOISE=12;
	static const int WAVE=13;
	static const int OPM=14;
	static const int PICOWAVE=15;
	static const int MAX=16;
protected:
	static int s_init;
	MOscMod* m_osc[MAX];
	int m_form;
public:
	MOscillator(MEngineData* waveData) {
		boot();
		
		m_osc[SINE]=new MOscSine();
		m_osc[SAW]=new MOscSaw();
		m_osc[TRIANGLE]=new MOscTriangle();
		m_osc[PULSE]=new MOscPulse();
		m_osc[NOISE]=new MOscNoise();
		m_osc[FC_PULSE]=new MOscPulse();
		m_osc[FC_TRI]=new MOscFcTri();
		m_osc[FC_NOISE]=new MOscFcNoise();
		m_osc[FC_S_NOISE]=NULL;
		//2009.05.10 OffGao MOD 1L addDPCM
		//m_osc[FC_DPCM] = new MOscMod();
		m_osc[FC_DPCM]=new MOscFcDpcm(&waveData->dpcmData);
		m_osc[GB_WAVE]=new MOscGbWave(&waveData->gbWaveData);
		m_osc[GB_NOISE]=new MOscGbLNoise();
		m_osc[GB_S_NOISE]=new MOscGbSNoise();
		m_osc[WAVE]=new MOscWave(&waveData->waveData);
		m_osc[OPM]=new MOscOPM(&waveData->opmData);
		m_osc[PICOWAVE]=new MOscPicoWave();

		setForm(PULSE);
		setNoiseToPulse();
	}
	virtual ~MOscillator() {
		delete m_osc[SINE];
		delete m_osc[SAW];
		delete m_osc[TRIANGLE];
		delete m_osc[PULSE];
		delete m_osc[NOISE];
		delete m_osc[FC_PULSE];
		delete m_osc[FC_TRI];
		delete m_osc[FC_NOISE];
		delete m_osc[FC_S_NOISE];
		delete m_osc[FC_DPCM];
		delete m_osc[GB_WAVE];
		delete m_osc[GB_NOISE];
		delete m_osc[GB_S_NOISE];
		delete m_osc[WAVE];
		delete m_osc[OPM];
		delete m_osc[PICOWAVE];
	}
	void asLFO() {
		if (m_osc[NOISE])((MOscNoise*)(m_osc[NOISE]))->disableResetPhase();
	}
	static void boot() {
		if (s_init)return ;
		MOscSine::boot();
		MOscSaw::boot();
		MOscTriangle::boot();
		MOscPulse::boot();
		MOscNoise::boot();
		MOscFcTri::boot();
		MOscFcNoise::boot();
		//2009.05.10 OffGao ADD 1L addDPCM
		//MOscFcDpcm::boot();
		//MOscGbWave::boot();
		MOscGbLNoise::boot();
		MOscGbSNoise::boot();
		//MOscWave::boot();
		//MOscOPM::boot();
		s_init=1;
	}
	MOscMod* setForm(int form) {
		MOscNoise* modNoise;
		MOscFcNoise* modFcNoise;
		if (form>=MAX)form=MAX-1;
		m_form=form;
		switch(form){
			case NOISE:
				modNoise=(MOscNoise*)(m_osc[NOISE]);
				modNoise->restoreFreq();
				break;
			case FC_NOISE:
				modFcNoise=(MOscFcNoise*)(getMod(FC_NOISE));
				modFcNoise->setLongMode();
				break;
			case FC_S_NOISE:
				modFcNoise=(MOscFcNoise*)(getMod(FC_S_NOISE));
				modFcNoise->setShortMode();
				break;
		}
		return getMod(form);
	}
	int getForm() {
		return m_form;
	}
	MOscMod* getCurrent() {
		return getMod(m_form);
	}
	MOscMod* getMod(int form) {
		if (form < 0) return NULL;
		return (form!=FC_S_NOISE)?m_osc[form]:m_osc[FC_NOISE];
	}
private:
	void setNoiseToPulse() {
		MOscPulse* modPulse=(MOscPulse*)(getMod(PULSE));
		MOscNoise* modNoise=(MOscNoise*)(getMod(NOISE));
		modPulse->setNoise(modNoise);
	}
};

}
