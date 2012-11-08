#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Number.h"
#include "MStatus.h"

namespace FlMML {

class MEvent {
private:
	int m_delta;
	int m_status;
	int m_data0;
	int m_data1;
	unsigned int m_tick;
	Number TEMPO_SCALE; // bpm小数点第二位まで有効
public:
	MEvent(unsigned int tick) {
		TEMPO_SCALE=100;
		set(MStatus::NOP,0,0);
		setTick(tick);
	}
	
	void set(int status,int data0,int data1) {
		m_status=status;
		m_data0=data0;
		m_data1=data1;
	}
	
	void setEOT() {set(MStatus::EOT,0,0);}
	void setNoteOn(int noteNo,int vel) {set(MStatus::NOTE_ON,noteNo,vel);}
	void setNoteOff(int noteNo,int vel) {set(MStatus::NOTE_OFF,noteNo,vel);}
	void setTempo(Number tempo) {set(MStatus::TEMPO,tempo*TEMPO_SCALE,0);}
	void setVolume(int vol) {set(MStatus::VOLUME,vol,0);}
	void setLoopEnd() {set(MStatus::LOOP_END,0,0);}
	void setNote(int noteNo) {set(MStatus::NOTE,noteNo,0);}
	void setForm(int form,int sub) {set(MStatus::FORM,form,sub);}
	void setEnvelope1Atk(int a) {set(MStatus::ENVELOPE1_ATK,a,0);}
	void setEnvelope1Point(int t,int l) {set(MStatus::ENVELOPE1_ADD,t,l);}
	void setEnvelope1Rel(int r) {set(MStatus::ENVELOPE1_REL,r,0);}
	void setEnvelope2Atk(int a) {set(MStatus::ENVELOPE2_ATK,a,0);}
	void setEnvelope2Point(int t,int l) {set(MStatus::ENVELOPE2_ADD,t,l);}
	void setEnvelope2Rel(int r) {set(MStatus::ENVELOPE2_REL,r,0);}
	void setNoiseFreq(int f) {set(MStatus::NOISE_FREQ,f,0);}
	void setPWM(int w) {set(MStatus::PWM,w,0);}
	void setPan(int p) {set(MStatus::PAN,p,0);}
	void setFormant(int vowel) {set(MStatus::FORMANT,vowel,0);}
	void setDetune(int d) {set(MStatus::DETUNE,d,0);}
	void setLFOFMSF(int fm,int sf) {set(MStatus::LFO_FMSF,fm,sf);}
	void setLFODPWD(int dp,int wd) {set(MStatus::LFO_DPWD,dp,wd);}
	void setLFODLTM(int dl,int tm) {set(MStatus::LFO_DLTM,dl,tm);}
	void setLFOTarget(int target) {set(MStatus::LFO_TARGET,target,0);}
	void setLPFSWTAMT(int swt,int amt) {set(MStatus::LPF_SWTAMT,swt,amt);}
	void setLPFFRQRES(int frq,int res) {set(MStatus::LPF_FRQRES,frq,res);}
	void setPlayEnd() {set(MStatus::PLAY_END,0,0);}
	void setClose() {set(MStatus::CLOSE,0,0);}
	void setVolMode(int m) {set(MStatus::VOL_MODE,m,0);}
	void setInput(int sens,int pipe) {set(MStatus::INPUT,sens,pipe);}
	void setOutput(int mode,int pipe) {set(MStatus::OUTPUT,mode,pipe);}
	void setExpression(int ex) {set(MStatus::EXPRESSION,ex,0);}
	void setRing(int sens,int pipe) {set(MStatus::RINGMODULATE,sens,pipe);}
	void setSync(int mode,int pipe) {set(MStatus::SYNC,mode,pipe);}
	void setDelta(int delta) {m_delta=delta;}
	void setTick(unsigned int tick) {m_tick=tick;}
	void setPortamento(int depth,int len) {set(MStatus::PORTAMENTO,depth,len);}
	void setMidiPort(int mode) {set(MStatus::MIDIPORT,mode,0);}
	void setMidiPortRate(int rate) {set(MStatus::MIDIPORTRATE,rate,0);}
	void setPortBase(int base) {set(MStatus::BASENOTE,base,0);}
	void setPoly(int voiceCount) {set(MStatus::POLY,voiceCount,0);}
	void setResetAll() {set(MStatus::RESET_ALL,0,0);}
	void setSoundOff() {set(MStatus::SOUND_OFF,0,0);}
	
	void setHwLfo(int w,int f,int pmd,int amd,
				  int pms,int ams,int s) {set(MStatus::HW_LFO,((w&3)<<27)|((f&0xff)<<19)|((pmd&0x7f)<<12)|((amd&0x7f)<<5)|((pms&7)<<2)|(ams&3),0);}
	int getStatus() {return m_status;}
	int getDelta() {return m_delta;}
	unsigned int getTick() {return m_tick;}
	int getNoteNo() {return m_data0;}
	int getVelocity() {return m_data1;}
	Number getTempo() {return Number (m_data0)/TEMPO_SCALE;}
	int getVolume() {return m_data0;}
	int getForm() {return m_data0;}
	int getSubForm() {return m_data1;}
	int getEnvelopeA() {return m_data0;}
	int getEnvelopeT() {return m_data0;}
	int getEnvelopeL() {return m_data1;}
	int getEnvelopeR() {return m_data0;}
	int getNoiseFreq() {return m_data0;}
	int getPWM() {return m_data0;}
	int getPan() {return m_data0;}
	int getVowel() {return m_data0;}
	int getDetune() {return m_data0;}
	int getLFODepth() {return m_data0;}
	int getLFOWidth() {return m_data1;}
	int getLFOForm() {return m_data0;}
	int getLFOSubForm() {return m_data1;}
	int getLFODelay() {return m_data0;}
	int getLFOTime() {return m_data1;}
	int getLFOTarget() {return m_data0;}
	int getLPFSwt() {return m_data0;}
	int getLPFAmt() {return m_data1;}
	int getLPFFrq() {return m_data0;}
	int getLPFRes() {return m_data1;}
	int getVolMode() {return m_data0;}
	int getInputSens() {return m_data0;}
	int getInputPipe() {return m_data1;}
	int getOutputMode() {return m_data0;}
	int getOutputPipe() {return m_data1;}
	int getExpression() {return m_data0;}
	int getRingSens() {return m_data0;}
	int getRingInput() {return m_data1;}
	int getSyncMode() {return m_data0;}
	int getSyncPipe() {return m_data1;}
	int getPorDepth() {return m_data0;}
	int getPorLen() {return m_data1;}
	int getMidiPort() {return m_data0;}
	int getMidiPortRate() {return m_data0;}
	int getPortBase() {return m_data0;}
	int getVoiceCount() {return m_data0;}
	int getHwLfoData() {return m_data0;}
};

}
