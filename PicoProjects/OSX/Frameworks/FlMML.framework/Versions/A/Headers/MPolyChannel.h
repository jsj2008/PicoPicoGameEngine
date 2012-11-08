#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <float.h>
using namespace std;


#include "MChannel.h"
#include "FlMML_Math.h"
#include "IChannel.h"
#include "FlMML_Number.h"

namespace FlMML {

class MPolyChannel : public IChannel {
protected:
	int m_form;
	int m_subform;
	int m_volMode;
	Number m_voiceId;
	MChannel* m_lastVoice;
	int m_voiceLimit;
	MChannel** m_voices;
	int m_voiceLen;
public:
	MPolyChannel(int voiceLimit,MEngineData* waveData) {
		m_voices = new MChannel*[voiceLimit];
		for (int i=0;i<voiceLimit;i++) {
			m_voices[i] = new MChannel(waveData);
		}
		m_voiceLen = voiceLimit;
		m_form=MOscillator::FC_PULSE;
		m_subform=0;
		m_voiceId=0;
		m_volMode=0;
		m_voiceLimit=voiceLimit;
		m_lastVoice=NULL;
	}
	virtual ~MPolyChannel() {
		delete[] m_voices;
	}
	
	void setExpression(int ex) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setExpression(ex);
	}
	
	void setVelocity(int velocity) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setVelocity(velocity);
	}
	
	virtual void setNoteNo(int noteNo,bool tie=true) {
		if (m_lastVoice!=NULL&&m_lastVoice->isPlaying()){
			m_lastVoice->setNoteNo(noteNo,tie);
		}
	}
	
	void setDetune(int detune) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setDetune(detune);
	}
	
	int getVoiceCount() {
		int i;
		int c=0;
		for (i=0;i<m_voiceLen;i++){
			c+=m_voices[i]->getVoiceCount();
		}
		return c;
	}
	
	void noteOn(int noteNo,int velocity) {
		int i=0;
		MChannel* vo=NULL;

		// ボイススロットに空きがあるようだ
		if (getVoiceCount()<=m_voiceLimit){
			for (i=0;i<m_voiceLen;i++){
				if (m_voices[i]->isPlaying()==false){
					vo=m_voices[i];
					break;
				}
			}
		}
		// やっぱ埋まってたので一番古いボイスを探す
		if (vo==NULL){
			if (m_voiceLen > 0) {
				Number minId=FLT_MAX;
				for (i=0;i<m_voiceLen;i++){
					if (minId>m_voices[i]->getId()){
						minId=m_voices[i]->getId();
						vo=m_voices[i];
					}
				}
			}
		}
		// 発音する
		if (vo) {
			vo->setForm(m_form,m_subform);
			vo->setVolMode(m_volMode);
			vo->noteOnWidthId(noteNo,velocity,m_voiceId++);
			m_lastVoice=vo;
		}
	}
	
	void noteOff(int noteNo) {
		for (int i=0;i<m_voiceLen;i++){
			if (m_voices[i]->getNoteNo()==noteNo){
				m_voices[i]->noteOff(noteNo);
			}
		}
	}
	
	void setSoundOff() {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setSoundOff();
	}
	
	void close() {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->close();
	}
	
	void setNoiseFreq(Number frequency) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setNoiseFreq(frequency);
	}
	
	void setForm(int form,int subform) {
		// ノートオン時に適用する
		m_form=form;
		m_subform=subform;
	}
	
	void setEnvelope1Atk(int attack) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setEnvelope1Atk(attack);
	}
	
	void setEnvelope1Point(int time,int level) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setEnvelope1Point(time,level);
	}
	
	void setEnvelope1Rel(int release) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setEnvelope1Rel(release);
	}
	
	void setEnvelope2Atk(int attack) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setEnvelope2Atk(attack);
	}
	
	void setEnvelope2Point(int time,int level) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setEnvelope2Point(time,level);
	}
	
	void setEnvelope2Rel(int release) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setEnvelope2Rel(release);
	}
	
	void setPWM(int pwm) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setPWM(pwm);
	}
	
	void setPan(int pan) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setPan(pan);
	}
	
	void setFormant(int vowel) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setFormant(vowel);
	}
	
	void setLFOFMSF(int form,int subform) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setLFOFMSF(form,subform);
	}
	
	void setLFODPWD(int depth,Number freq) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setLFODPWD(depth,freq);
	}
	
	void setLFODLTM(int delay,int time) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setLFODLTM(delay,time);
	}
	
	void setLFOTarget(int target) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setLFOTarget(target);
	}
	
	void setLpfSwtAmt(int swt,int amt) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setLpfSwtAmt(swt,amt);
	}
	
	void setLpfFrqRes(int frq,int res) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setLpfFrqRes(frq,res);
	}
	
	void setVolMode(int m) {
		// ノートオン時に適用する
		m_volMode=m;
	}
	
	void setInput(int ii,int p) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setInput(ii,p);
	}
	
	void setOutput(int oo,int p) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setOutput(oo,p);
	}
	
	void setRing(int s,int p) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setRing(s,p);
	}
	
	void setSync(int m,int p) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setSync(m,p);
	}
	
	void setPortamento(int depth,Number len) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setPortamento(depth,len);
	}
	
	void setMidiPort(int mode) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setMidiPort(mode);
	}
	
	void setMidiPortRate(Number rate) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setMidiPortRate(rate);
	}
	
	void setPortBase(int portBase) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setPortBase(portBase);
	}
	
	void setVoiceLimit(int voiceLimit) {
		m_voiceLimit=Math::max(1,Math::min(voiceLimit,m_voiceLen));
	}
	
	void setHwLfo(int data) {
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->setHwLfo(data);
	}
	
	void reset() {
		m_form=0;
		m_subform=0;
		m_voiceId=0;
		m_volMode=0;
		for (int i=0;i<m_voiceLen;i++)m_voices[i]->reset();
	}
	
	virtual void getSamples(VNumber* samples,int max,int start,int delta) {
		bool slave=false;
		for (int i=0;i<m_voiceLen;i++){
			if (m_voices[i]->isPlaying()){
				m_voices[i]->setSlaveVoice(slave);
				m_voices[i]->getSamples(samples,max,start,delta);
				slave=true;
			}
		}
		if (slave==false){
			m_voices[0]->clearOutPipe(max,start,delta);
		}
	}
};

}
