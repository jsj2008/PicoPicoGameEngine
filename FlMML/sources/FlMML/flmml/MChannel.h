#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "IChannel.h"
#include "MEnvelope.h"
#include "MOscillator.h"
#include "MOscMod.h"
#include "MFilter.h"
#include "MFormant.h"
#include "FlMML_Number.h"
#include "FlMML_Math.h"
#include "FlMML_Vector.h"
#include "FlMML_StaticDef.h"
#include "MEngineData.h"

#define S_E_1 (e-s)

namespace FlMML {

class MChannel : public IChannel {
private:
	static const int LFO_TARGET_PITCH=0;
	static const int LFO_TARGET_AMPLITUDE=1;
	static const int LFO_TARGET_CUTOFF=2;
	static const int LFO_TARGET_PWM=3;
	static const int LFO_TARGET_FM=4;
	static const int LFO_TARGET_PANPOT=5;
	
	int m_noteNo;
	int m_detune;
	int m_freqNo;
	MEnvelope* m_envelope1;	// for VCO
	MEnvelope* m_envelope2;	// for VCF
	MOscillator* m_oscSet1;	// for original wave
	MOscMod* m_oscMod1;
	MOscillator* m_oscSet2;	// for Pitch LFO
	MOscMod* m_oscMod2;
	int m_osc2Connect;
	Number m_osc2Sign;
	MFilter* m_filter;
	int m_filterConnect;
	MFormant* m_formant;
	Number m_expression;	// expression (max:1.0)
	Number m_velocity;		// velocity (max:1.0)
	Number m_ampLevel;		// amplifier level (max:1.0)
	Number m_pan;			// left 0.0 - 1.0 right
	int m_onCounter;
	int m_lfoDelay;
	Number m_lfoDepth;
	int m_lfoEnd;
	int m_lfoTarget;
	Number m_lpfAmt;
	Number m_lpfFrq;
	Number m_lpfRes;
	Number m_pulseWidth;
	int m_volMode;
	Number m_inSens;
	int m_inPipe;
	int m_outMode;
	int m_outPipe;
	Number m_ringSens;
	int m_ringPipe;
	int m_syncMode;
	int m_syncPipe;
	
	Number m_portDepth;
	Number m_portDepthAdd;
	int m_portamento;
	Number m_portRate;
	int m_lastFreqNo;
	
	bool m_slaveVoice;		// 従属ボイスか？
	Number m_voiceid;		// ボイスID
protected:
	static const int s_lfoDelta=245;
	
	MEngineData* m_waveData;
	MChannelData* m_data;
public:
	MChannel(MEngineData* waveData) {
		m_waveData = waveData;
		m_data = &waveData->channelData;
		resetMember();
	}
	virtual ~MChannel() {
		deleteMember();
	}
	
	void resetMember() {
		m_noteNo=0;
		m_detune=0;
		m_freqNo=0;
		m_osc2Connect=0;
		m_filterConnect=0;
		m_volMode=0;
		m_onCounter=0;
		m_lfoDelay=0;
		m_lfoDepth=0.0;
		m_lfoEnd=0;
		m_lpfAmt=0;
		m_lpfFrq=0;
		m_lpfRes=0;
		m_pulseWidth=0.5;
		m_portDepth=0;
		m_portDepthAdd=0;
		m_lastFreqNo=4800;
		m_portamento=0;
		m_portRate=0;
		m_voiceid=0;
		m_slaveVoice=false;
		m_ampLevel=0;
		m_pan=0;
		m_velocity=0;
		m_lfoTarget=0;
		m_inSens=0;
		m_inPipe=0;
		m_outMode=0;
		m_outPipe=0;
		m_ringSens=0;
		m_ringPipe=0;
		m_syncMode=0;
		m_syncPipe=0;
		m_osc2Sign=0;
		m_envelope1=new MEnvelope(0.0,60.0/127.0,30.0/127.0,1.0/127.0);
		m_envelope2=new MEnvelope(0.0,30.0/127.0,0.0,1.0);
		m_oscSet1=new MOscillator(m_waveData);
		m_oscMod1=m_oscSet1->getCurrent();
		m_oscSet2=new MOscillator(m_waveData);
		m_oscSet2->asLFO();
		m_oscSet2->setForm(MOscillator::SINE);
		m_oscMod2=m_oscSet2->getCurrent();
		m_filter=new MFilter(m_data);
		m_formant=new MFormant();
		setExpression(127);
		setVelocity(100);
		setPan(64);
		setInput(0,0);
		setOutput(0,0);
		setRing(0,0);
		setSync(0,0);
	}
	
	void deleteMember() {
		if (m_envelope1) delete m_envelope1;
		m_envelope1 = NULL;
		if (m_envelope2) delete m_envelope2;
		m_envelope2 = NULL;
		if (m_oscSet1) delete m_oscSet1;
		m_oscSet1 = NULL;
		if (m_oscSet2) delete m_oscSet2;
		m_oscSet2 = NULL;
		if (m_filter) delete m_filter;
		m_filter = NULL;
		if (m_formant) delete m_formant;
		m_formant = NULL;
	}
	
	void setExpression(int ex) {
		m_expression=m_data->s_volumeMap[m_volMode][ex];
		m_ampLevel=m_velocity*m_expression;
		((MOscOPM*)(m_oscSet1->getMod(MOscillator::OPM)))->setExpression(m_expression);
	}
	void setVelocity(int velocity) {
		m_velocity=m_data->s_volumeMap[m_volMode][velocity];
		m_ampLevel=m_velocity*m_expression;
		((MOscOPM*)(m_oscSet1->getMod(MOscillator::OPM)))->setVelocity(velocity);
	}
	virtual void setNoteNo(int noteNo,bool tie=true) {
		m_noteNo=noteNo;
		m_freqNo=m_noteNo*MChannelData::PITCH_RESOLUTION+m_detune;
		m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		
		if (m_portamento==1){
			if (!tie){
				m_portDepth=m_lastFreqNo-m_freqNo;
			}
			else {
				m_portDepth+=(m_lastFreqNo-m_freqNo);
			}
			m_portDepthAdd=(m_portDepth<0)?m_portRate:m_portRate*-1;
		}
		m_lastFreqNo=m_freqNo;
	}
	void setDetune(int detune) {
		m_detune=detune;
		m_freqNo=m_noteNo*MChannelData::PITCH_RESOLUTION+m_detune;
		m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
	}
	int getNoteNo() {
		return m_noteNo;
	}
	bool isPlaying() {
		if (m_oscSet1->getForm()==MOscillator::OPM){
			return ((MOscOPM*)(m_oscSet1->getCurrent()))->IsPlaying();
		}
		else {
			return m_envelope1->isPlaying();
		}
		return false;
	}
	Number getId() {
		return m_voiceid;
	}
	int getVoiceCount() {
		return isPlaying()?1:0;
	}
	void setSlaveVoice(bool f) {
		m_slaveVoice=f;
	}
	void noteOnWidthId(int noteNo,int velocity,Number id) {
		m_voiceid=id;
		noteOn(noteNo,velocity);
	}
	void noteOn(int noteNo,int velocity) {
		setNoteNo(noteNo,false);
		m_envelope1->triggerEnvelope(0);
		m_envelope2->triggerEnvelope(1);
		m_oscMod1->resetPhase();
		if (m_oscMod2) m_oscMod2->resetPhase();
		m_filter->reset();
		setVelocity(velocity);
		m_onCounter=0;
		
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		modPulse->setPWM(m_pulseWidth);
		
		m_oscSet1->getMod(MOscillator::FC_NOISE)->setNoteNo(m_noteNo);
		m_oscSet1->getMod(MOscillator::GB_NOISE)->setNoteNo(m_noteNo);
		m_oscSet1->getMod(MOscillator::GB_S_NOISE)->setNoteNo(m_noteNo);
		m_oscSet1->getMod(MOscillator::FC_DPCM)->setNoteNo(m_noteNo);
		m_oscSet1->getMod(MOscillator::OPM)->setNoteNo(m_noteNo);
	}
	void noteOff(int noteNo) {
		if (noteNo<0||noteNo==m_noteNo){
			m_envelope1->releaseEnvelope();
			m_envelope2->releaseEnvelope();
			((MOscOPM*)(m_oscSet1->getMod(MOscillator::OPM)))->noteOff();
		}
	}
	void setSoundOff() {
		m_envelope1->soundOff();
		m_envelope2->soundOff();
	}
	void close() {
		noteOff(m_noteNo);
		m_filter->setSwitch(0);
	}
	void setNoiseFreq(Number frequency) {
		MOscNoise* modNoise=(MOscNoise*)(m_oscSet1->getMod(MOscillator::NOISE));
		modNoise->setNoiseFreq(1.0-frequency*(1.0/128.0));
	}
	void setForm(int form,int subform) {
		m_oscMod1=m_oscSet1->setForm(form);
		m_oscMod1->setWaveNo(subform);
	}
	void setEnvelope1Atk(int attack) {
		m_envelope1->setAttack(attack*(1.0/127.0));
	}
	void setEnvelope1Point(int time,int level) {
		m_envelope1->addPoint(time*(1.0/127.0),level*(1.0/127.0));
	}
	void setEnvelope1Rel(int release) {
		m_envelope1->setRelease(release*(1.0/127.0));
	}
	void setEnvelope2Atk(int attack) {
		m_envelope2->setAttack(attack*(1.0/127.0));
	}
	void setEnvelope2Point(int time,int level) {
		m_envelope2->addPoint(time*(1.0/127.0),level*(1.0/127.0));
	}
	void setEnvelope2Rel(int release) {
		m_envelope2->setRelease(release*(1.0/127.0));
	}
	void setPWM(int pwm) {
		if (m_oscSet1->getForm()!=MOscillator::FC_PULSE){
			MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
			if (pwm<0){
				modPulse->setMIX(1);
				pwm*=-1;
			}
			else {
				modPulse->setMIX(0);
			}
			m_pulseWidth=pwm*0.01;
			modPulse->setPWM(m_pulseWidth);
		}
		else {
			MOscPulse* modFcPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::FC_PULSE));
			if (pwm<0)pwm*=-1;	// 以前との互換のため
			modFcPulse->setPWM(0.125*Number (pwm));
		}
	}
	void setPan(int pan) {
		// left 1 - 64 - 127 right
		// master_vol = (0.25 * 2)
		m_pan=(pan-1)*(0.5/63.0);
		if (m_pan<0)m_pan=0;
	}
	void setFormant(int vowel) {
		if (vowel>=0)m_formant->setVowel(vowel);
		else m_formant->disable();
	}
	void setLFOFMSF(int form,int subform) {
		m_oscMod2=m_oscSet2->setForm((form>=0)?form-1:-form-1);
		if (m_oscMod2) m_oscMod2->setWaveNo(subform);
		m_osc2Sign=(form>=0)?1.0:-1.0;
		if (form<0)form=-form;
		form--;
		if (form>=MOscillator::MAX)m_osc2Connect=0;
	}
	void setLFODPWD(int depth,Number freq) {
		m_lfoDepth=depth;
		m_osc2Connect=(depth==0)?0:1;
		m_oscMod2->setFrequency(freq);
		m_oscMod2->resetPhase();
		((MOscNoise*)(m_oscSet2->getMod(MOscillator::NOISE)))->setNoiseFreq(freq/MSequencer_RATE44100);
	}
	void setLFODLTM(int delay,int time) {
		m_lfoDelay=delay;
		m_lfoEnd=(time>0)?m_lfoDelay+time:0;
	}
	void setLFOTarget(int target) {
		m_lfoTarget=target;
	}
	void setLpfSwtAmt(int swt,int amt) {
		if (-3<swt&&swt<3&&swt!=m_filterConnect){
			m_filterConnect=swt;
			m_filter->setSwitch(swt);
		}
		m_lpfAmt=((amt<-127)?-127:(amt<127)?amt:127)*MChannelData::PITCH_RESOLUTION;
	}
	void setLpfFrqRes(int frq,int res) {
		if (frq<0)frq=0;
		if (frq>127)frq=127;
		m_lpfFrq=frq*MChannelData::PITCH_RESOLUTION;
		m_lpfRes=res*(1.0/127.0);
		if (m_lpfRes<0.0)m_lpfRes=0.0;
		if (m_lpfRes>1.0)m_lpfRes=1.0;
	}
	void setVolMode(int m) {
		switch(m){
			case 0:
			case 1:
			case 2:
				m_volMode=m;
				break;
		}
	}
	void setInput(int i,int p) {
		m_inSens=(1<<(i-1))*(1.0/8.0)*MOscMod::PHASE_LEN;
		m_inPipe=p;
	}
	void setOutput(int o,int p) {
		m_outMode=o;
		m_outPipe=p;
	}
	void setRing(int s,int p) {
		m_ringSens=(1<<(s-1))/8.0;
		m_ringPipe=p;
	}
	void setSync(int m,int p) {
		m_syncMode=m;
		m_syncPipe=p;
	}
	void setPortamento(int depth,Number len) {
		m_portamento=0;
		m_portDepth=depth;
		m_portDepthAdd=(Number (m_portDepth)/len)*-1;
	}
	void setMidiPort(int mode) {
		m_portamento=mode;
		m_portDepth=0;
	}
	void setMidiPortRate(Number rate) {
		m_portRate=rate;
	}
	void setPortBase(int base) {
		m_lastFreqNo=base;
	}
	void setVoiceLimit(int voiceLimit) {
		// 無視
	}
	void setHwLfo(int data) {
		int w=(data>>27)&0x03;
		int f=(data>>19)&0xFF;
		int pmd=(data>>12)&0x7F;
		int amd=(data>>5)&0x7F;
		int pms=(data>>2)&0x07;
		int ams=(data>>0)&0x03;
		MOscOPM* fm=((MOscOPM*)(m_oscSet1->getMod(MOscillator::OPM)));
		fm->setWF(w);
		fm->setLFRQ(f);
		fm->setPMD(pmd);
		fm->setAMD(amd);
		fm->setPMSAMS(pms,ams);
	}
	void reset() {
		deleteMember();
		resetMember();
		// 基本
		setSoundOff();
		m_pulseWidth=0.5;
		m_voiceid=0;
		setForm(0,0);
		setDetune(0);
		setExpression(127);
		setVelocity(100);
		setPan(64);
		setVolMode(0);
		setNoiseFreq(0.0);
		// LFO
		setLFOFMSF(0,0);
		m_osc2Connect=0;
		m_onCounter=0;
		m_lfoTarget=0;
		m_lfoDelay=0;
		m_lfoDepth=0.0;
		m_lfoEnd=0;
		// フィルタ
		setLpfSwtAmt(0,0);
		setLpfFrqRes(0,0);
		setFormant(-1);
		// パイプ
		setInput(0,0);
		setOutput(0,0);
		setRing(0,0);
		setSync(0,0);
		// ポルタメント
		m_portDepth=0;
		m_portDepthAdd=0;
		m_lastFreqNo=4800;
		m_portamento=0;
		m_portRate=0;
	}
	void clearOutPipe(int max,int start,int delta) {
		int end=start+delta;
		if (end>=max)end=max;
		if (m_outMode==1){
			for (int i=start;i<end;i++){
				m_data->s_pipeArr[m_outPipe][i]=0.0;
			}
		}
	}
protected:
	Number getNextCutoff() {
		Number cut=m_lpfFrq+m_lpfAmt*m_envelope2->getNextAmplitudeLinear();
		cut=m_data->getFrequency(cut)*m_oscMod1->getFrequency()*(2.0*Math::PI/(MSequencer_RATE44100*440.0));
		if (cut<(1.0/127.0))cut=0.0;
		return cut;
	}
public:
	virtual void getSamples(VNumber* samples,int max,int start,int delta) {
		int end=start+delta;
		VNumber* trackBuffer=m_data->s_samples;Number sens;VNumber* pipe;
		Number amplitude;Number rightAmplitude;
		bool playing=isPlaying();bool tmpFlag;
		Number vol;int lpffrq;Number pan;Number depth;
		int i;int j;int s;int e;
		if (end>=max)end=max;
		Number key=m_data->getFrequency(m_freqNo);
		if (m_outMode==1&&m_slaveVoice==false){
			// @o1 が指定されていれば直接パイプに音声を書き込む
			trackBuffer=m_data->s_pipeArr[m_outPipe];
		}
		if (playing){
			if (m_portDepth==0){
				if (m_inSens>=0.000001){
					if (m_osc2Connect==0){
						getSamplesF__(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamplesFP_(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamplesFW_(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_FM){
						getSamplesFF_(trackBuffer,start,end);
					}else {
						getSamplesF__(trackBuffer,start,end);
					}
				}else if (m_syncMode==2){
					if (m_osc2Connect==0){
						getSamplesI__(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamplesIP_(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamplesIW_(trackBuffer,start,end);
					}else {
						getSamplesI__(trackBuffer,start,end);
					}
				}else if (m_syncMode==1){
					if (m_osc2Connect==0){
						getSamplesO__(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamplesOP_(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamplesOW_(trackBuffer,start,end);
					}else {
						getSamplesO__(trackBuffer,start,end);
					}
				}else {
					if (m_osc2Connect==0){
						getSamples___(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamples_P_(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamples_W_(trackBuffer,start,end);
					}else {
						getSamples___(trackBuffer,start,end);
					}
				}
			}
			else {
				if (m_inSens>=0.000001){
					if (m_osc2Connect==0){
						getSamplesF_P(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamplesFPP(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamplesFWP(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_FM){
						getSamplesFFP(trackBuffer,start,end);
					}else {
						getSamplesF_P(trackBuffer,start,end);
					}
				}else if (m_syncMode==2){
					if (m_osc2Connect==0){
						getSamplesI_P(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamplesIPP(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamplesIWP(trackBuffer,start,end);
					}else {
						getSamplesI_P(trackBuffer,start,end);
					}
				}else if (m_syncMode==1){
					if (m_osc2Connect==0){
						getSamplesO_P(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamplesOPP(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamplesOWP(trackBuffer,start,end);
					}else {
						getSamplesO_P(trackBuffer,start,end);
					}
				}else {
					if (m_osc2Connect==0){
						getSamples__P(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PITCH){
						getSamples_PP(trackBuffer,start,end);
					}else if (m_lfoTarget==LFO_TARGET_PWM){
						getSamples_WP(trackBuffer,start,end);
					}else {
						getSamples__P(trackBuffer,start,end);
					}
				}
			}
		}

		if (m_oscSet1->getForm()!=MOscillator::OPM){
			if (m_volMode==0){
				m_envelope1->ampSamplesLinear(trackBuffer,start,end,m_ampLevel);
			}else {
				m_envelope1->ampSamplesNonLinear(trackBuffer,start,end,m_ampLevel,m_volMode);
			}
		}
		if (m_lfoTarget==LFO_TARGET_AMPLITUDE&&m_osc2Connect!=0){
			depth=m_osc2Sign*m_lfoDepth/127.0;
			s=start;
			for (i=start;i<end;i++){
				vol=1.0;
				if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
					vol+=m_oscMod2->getNextSample()*depth;
				}
				if (vol<0){
					vol=0;
				}
				(trackBuffer)[i]*=vol;
				m_onCounter++;
			}
		}
		if (playing&&(m_ringSens>=0.000001)){
			pipe=m_data->s_pipeArr[m_ringPipe];
			sens=m_ringSens;
			for (i=start;i<end;i++){
				(trackBuffer)[i]*=(pipe)[i]*sens;
			}
		}
		
		// フォルマントフィルタを経由した後の音声が無音であればスキップ
		tmpFlag=playing;
		playing=(playing||m_formant->checkToSilence());
		if (playing!=tmpFlag){
			for (i=start;i<end;i++)(trackBuffer)[i]=0;
		}
		if (playing){
			m_formant->run(trackBuffer,start,end);
		}
		
		// フィルタを経由した後の音声が無音であればスキップ
		tmpFlag=playing;
		playing=playing||m_filter->checkToSilence();
		if (playing!=tmpFlag){
			for (i=start;i<end;i++)(trackBuffer)[i]=0;
		}
		if (playing){
			if (m_lfoTarget==LFO_TARGET_CUTOFF&&m_osc2Connect!=0){
				depth=m_osc2Sign*m_lfoDepth;
				s=start;
				do{
					e=s+s_lfoDelta;
					if (e>end)e=end;
					lpffrq=m_lpfFrq;
					if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
						lpffrq+=m_oscMod2->getNextSample()*depth;
						m_oscMod2->addPhase(S_E_1);
					}
					if (lpffrq<0.0){
						lpffrq=0.0;
					}else if (lpffrq>127.0*MChannelData::PITCH_RESOLUTION){
						lpffrq=127.0*MChannelData::PITCH_RESOLUTION;
					}
					m_filter->run(m_data->s_samples,s,e,*m_envelope2,lpffrq,m_lpfAmt,m_lpfRes,key);
					m_onCounter+=e-s;
					s=e;
				}while(s<end);
			}else {
				m_filter->run(trackBuffer,start,end,*m_envelope2,m_lpfFrq,m_lpfAmt,m_lpfRes,key);
			}
		}
		
		if (playing){
			switch(m_outMode){
				case 0:
					
					if (m_lfoTarget==LFO_TARGET_PANPOT&&m_osc2Connect!=0){
						depth=m_osc2Sign*m_lfoDepth*(1.0/127.0);
						for (i=start;i<end;i++){
							j=i+i;
							pan=m_pan+m_oscMod2->getNextSample()*depth;
							if (pan<0){
								pan=0;
							}else if (pan>1.0){
								pan=1.0;
							}
							amplitude=(trackBuffer)[i]*0.5;
							rightAmplitude=amplitude*pan;
							(samples)[j]+=amplitude-rightAmplitude;
							j++;
							(samples)[j]+=rightAmplitude;
						}
					}else {
						for (i=start;i<end;i++){
							j=i+i;
							amplitude=(trackBuffer)[i]*0.5;
							rightAmplitude=amplitude*m_pan;
							(samples)[j]+=amplitude-rightAmplitude;
							j++;
							(samples)[j]+=rightAmplitude;
						}
					}

					break;
				case 1: // overwrite
					/* リングモジュレータと音量LFOの同時使用時に問題が出てたようなので
					   一旦戻します。 2010.09.22 tekisuke */
					pipe=m_data->s_pipeArr[m_outPipe];
					if (m_slaveVoice==false){
						for (i=start;i<end;i++){
							(pipe)[i]=(trackBuffer)[i];
						}
					}
					else {
						for (i=start;i<end;i++){
							(pipe)[i]+=(trackBuffer)[i];
						}
					}
					break;
				case 2: // add
					pipe=m_data->s_pipeArr[m_outPipe];
					for (i=start;i<end;i++){
						(pipe)[i]+=(trackBuffer)[i];
					}
					break;
			}
		}else if (m_outMode==1){
			pipe=m_data->s_pipeArr[m_outPipe];
			if (m_slaveVoice==false){
				for (i=start;i<end;i++){
					(pipe)[i]=0.0;
				}
			}
		}
	}
private:
	// 波形生成部の関数群
	// [pipe] := [_:なし], [F:FM入力], [I:Sync入力], [O:Sync出力]
	// [lfo]  := [_:なし], [P:音程], [W:パルス幅], [F:FM入力レベル]
	// [pro.] := [_:なし], [p:ポルタメント]
	// private function getSamples[pipe][lfo](samples:Vector.<Number>, start:int, end:int):void

	// パイプ処理なし, LFOなし, ポルタメントなし
	void getSamples___(VNumber* samples,int start,int end) {
		m_oscMod1->getSamples(samples,start,end);
	}
	
	// パイプ処理なし, 音程LFO, ポルタメントなし
	void getSamples_P_(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number depth=m_osc2Sign*m_lfoDepth;
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamples(samples,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// パイプ処理なし, パルス幅(@3)LFO, ポルタメントなし
	void getSamples_W_(VNumber* samples,int start,int end) {
		int s=start;int e;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			m_oscMod1->getSamples(samples,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// FM入力, LFOなし, ポルタメントなし
	void getSamplesF__(VNumber* samples,int start,int end) {
		int i;Number sens=m_inSens;VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		int fq = m_data->getFrequency(m_freqNo);
		m_oscMod1->setFrequency(fq>>0);
		for (i=start;i<end;i++){
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
		}
	}
	
	// FM入力, 音程LFO, ポルタメントなし
	void getSamplesFP_(VNumber* samples,int start,int end) {
		int i;int freqNo;Number sens=m_inSens;Number depth=m_osc2Sign*m_lfoDepth;
		VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		for (i=start;i<end;i++){
			freqNo=m_freqNo;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
			m_onCounter++;
		}
	}
	
	// FM入力, パルス幅(@3)LFO, ポルタメントなし
	void getSamplesFW_(VNumber* samples,int start,int end) {
		int i;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		Number sens=m_inSens;VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		// rev.35879 以前の挙動にあわせるため
		int fq = m_data->getFrequency(m_freqNo);
		m_oscMod1->setFrequency(fq>>0);
		for (i=start;i<end;i++){
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
			m_onCounter++;
		}
	}
	
	// FM入力, FM入力レベル, ポルタメントなし
	void getSamplesFF_(VNumber* samples,int start,int end) {
		int i;Number sens;Number depth=m_osc2Sign*m_lfoDepth*(1.0/127.0);
		VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		// rev.35879 以前の挙動にあわせるため
		int fq = m_data->getFrequency(m_freqNo);
		m_oscMod1->setFrequency(fq>>0);
		for (i=start;i<end;i++){
			sens=m_inSens;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				sens*=m_oscMod2->getNextSample()*depth;
			}
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
			m_onCounter++;
		}
	}
	
	// Sync入力, LFOなし, ポルタメントなし
	void getSamplesI__(VNumber* samples,int start,int end) {
		m_oscMod1->getSamplesWithSyncIn(samples,&m_data->s_syncSources[m_syncPipe],start,end);
	}
	
	// Sync入力, 音程LFO, ポルタメントなし
	void getSamplesIP_(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number depth=m_osc2Sign*m_lfoDepth;
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamplesWithSyncIn(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// Sync入力, パルス幅(@3)LFO, ポルタメントなし
	void getSamplesIW_(VNumber* samples,int start,int end) {
		int s=start;int e;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			m_oscMod1->getSamplesWithSyncIn(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// Sync出力, LFOなし, ポルタメントなし
	void getSamplesO__(VNumber* samples,int start,int end) {
		m_oscMod1->getSamplesWithSyncOut(samples,&m_data->s_syncSources[m_syncPipe],start,end);
	}
	
	// Sync出力, 音程LFO, ポルタメントなし
	void getSamplesOP_(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number depth=m_osc2Sign*m_lfoDepth;
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamplesWithSyncOut(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// Sync出力, パルス幅(@3)LFO, ポルタメントなし
	void getSamplesOW_(VNumber* samples,int start,int end) {
		int s=start;int e;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			m_oscMod1->getSamplesWithSyncOut(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	/*** ここから下がポルタメントありの場合 ***/

	// パイプ処理なし, LFOなし, ポルタメントあり
	void getSamples__P(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamples(samples,s,e);
			s=e;
		}while(s<end);
		if (m_portDepth==0){
			m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		}
	}
	
	// パイプ処理なし, 音程LFO, ポルタメントあり
	void getSamples_PP(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number depth=m_osc2Sign*m_lfoDepth;
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamples(samples,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// パイプ処理なし, パルス幅(@3)LFO, ポルタメントあり
	void getSamples_WP(VNumber* samples,int start,int end) {
		int s=start;int e;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;Number freqNo;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			m_oscMod1->getSamples(samples,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
		if (m_portDepth==0){
			m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		}
	}
	
	// FM入力, LFOなし, ポルタメントあり
	void getSamplesF_P(VNumber* samples,int start,int end) {
		int freqNo;int i;Number sens=m_inSens;VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		for (i=start;i<end;i++){
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=m_portDepthAdd;
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
		}
	}
	
	// FM入力, 音程LFO, ポルタメントあり
	void getSamplesFPP(VNumber* samples,int start,int end) {
		int i;int freqNo;Number sens=m_inSens;Number depth=m_osc2Sign*m_lfoDepth;
		VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		for (i=start;i<end;i++){
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=m_portDepthAdd;
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
			m_onCounter++;
		}
	}
	
	// FM入力, パルス幅(@3)LFO, ポルタメントあり
	void getSamplesFWP(VNumber* samples,int start,int end) {
		int i;int freqNo;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		Number sens=m_inSens;VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		for (i=start;i<end;i++){
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=m_portDepthAdd;
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
			m_onCounter++;
		}
	}
	
	// FM入力, FM入力レベル, ポルタメントあり
	void getSamplesFFP(VNumber* samples,int start,int end) {
		int i;int freqNo;Number sens;Number depth=m_osc2Sign*m_lfoDepth*(1.0/127.0);
		VNumber* pipe=m_data->s_pipeArr[m_inPipe];
		for (i=start;i<end;i++){
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=m_portDepthAdd;
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			sens=m_inSens;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				sens*=m_oscMod2->getNextSample()*depth;
			}
			(samples)[i]=m_oscMod1->getNextSampleOfs((pipe)[i]*sens);
			m_onCounter++;
		}
	}
	
	// Sync入力, LFOなし, ポルタメントあり
	void getSamplesI_P(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamplesWithSyncIn(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
		if (m_portDepth==0){
			m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		}
	}
	
	// Sync入力, 音程LFO, ポルタメントあり
	void getSamplesIPP(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number depth=m_osc2Sign*m_lfoDepth;
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamplesWithSyncIn(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// Sync入力, パルス幅(@3)LFO, ポルタメントあり
	void getSamplesIWP(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			m_oscMod1->getSamplesWithSyncIn(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
		if (m_portDepth==0){
			m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		}
	}
	
	// Sync出力, LFOなし, ポルタメントあり
	void getSamplesO_P(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamplesWithSyncOut(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
		if (m_portDepth==0){
			m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		}
	}
	
	// Sync出力, 音程LFO, ポルタメントあり
	void getSamplesOPP(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number depth=m_osc2Sign*m_lfoDepth;
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				freqNo+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			m_oscMod1->getSamplesWithSyncOut(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
	}
	
	// Sync出力, パルス幅(@3)LFO, ポルタメントあり
	void getSamplesOWP(VNumber* samples,int start,int end) {
		int s=start;int e;int freqNo;Number pwm;Number depth=m_osc2Sign*m_lfoDepth*0.01;
		MOscPulse* modPulse=(MOscPulse*)(m_oscSet1->getMod(MOscillator::PULSE));
		Vector<bool>* syncLine=&m_data->s_syncSources[m_syncPipe];
		do{
			e=s+s_lfoDelta;
			if (e>end)e=end;
			freqNo=m_freqNo;
			if (m_portDepth!=0){
				freqNo+=m_portDepth;
				m_portDepth+=(m_portDepthAdd*(S_E_1));
				if (m_portDepth*m_portDepthAdd>0)m_portDepth=0;
			}
			m_oscMod1->setFrequency(m_data->getFrequency(freqNo));
			pwm=m_pulseWidth;
			if (m_onCounter>=m_lfoDelay&&(m_lfoEnd==0||m_onCounter<m_lfoEnd)){
				pwm+=m_oscMod2->getNextSample()*depth;
				m_oscMod2->addPhase(S_E_1);
			}
			if (pwm<0){
				pwm=0;
			}else if (pwm>100.0){
				pwm=100.0;
			}
			modPulse->setPWM(pwm);
			m_oscMod1->getSamplesWithSyncOut(samples,syncLine,s,e);
			m_onCounter+=e-s;
			s=e;
		}while(s<end);
		if (m_portDepth==0){
			m_oscMod1->setFrequency(m_data->getFrequency(m_freqNo));
		}
	}
};

}
