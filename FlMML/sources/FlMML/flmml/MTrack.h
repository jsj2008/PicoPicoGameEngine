#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;



#include "FlMML_Number.h"
#include "IChannel.h"
#include "MChannel.h"
//#include "MSignal.h"
#include "MEvent.h"
#include "FlMML_Math.h"
#include "MStatus.h"
#include "MPolyChannel.h"
#include "FlMML_Array.h"

namespace FlMML {

class MTrack;

class MTrack {
public:
	static const int TEMPO_TRACK=0;
	static const int FIRST_TRACK=1;
	static const int DEFAULT_BPM=120;
	int m_trackNo;
private:
	Number m_bpm;			// beat per minute
	Number m_spt;			// samples per tick
	IChannel* m_ch;			// channel (instrument)
	Number m_needle;		// delta time
	int m_volume;			// default volume    (max:127)
	Number m_gate;			// default gate time (max:1.0)
	int m_gate2;			// gate time 2
	vector<MEvent> m_events;
	int m_pointer;			// current event no.
	int m_loopPointer;
	unsigned int m_loopCount;
	int m_delta;
	int m_isEnd;
	bool m_playEnd;
	unsigned int m_globalTick;
	//	int m_signalCnt;
	Number m_lfoWidth;
	unsigned int m_totalMSec;
public:
	//	int m_signalInterval;
	bool m_infinityLoop;
private:
	bool m_polyFound;
	unsigned int m_chordBegin;
	unsigned int m_chordEnd;
	bool m_chordMode;
	MEngineData* m_waveData;
public:
	MTrack(int trackNo,MEngineData* waveData) {
		m_waveData = waveData;
		m_trackNo = trackNo;
		m_bpm=0;
		m_spt=0;
		m_isEnd=0;
		m_ch=new MChannel(m_waveData);
		m_ch->m_trackNo = m_trackNo;
		m_needle=0.0;
		m_polyFound=false;
		playTempo(DEFAULT_BPM);
		m_volume=100;
		recGate(15.0/16.0);
		recGate2(0);
		//m_events=new Vector<MEvent> ();
		m_events.clear();
		m_events.reserve(100);
		m_pointer=0;
		m_delta=0;
		m_globalTick=0;
		//		m_signalInterval=96/4;
		//		m_signalCnt=0;
		m_lfoWidth=0.0;
		m_totalMSec=0;
		m_chordBegin=0;
		m_chordEnd=0;
		m_chordMode=false;
		m_loopPointer = 0;
		m_loopCount = 0;
		m_infinityLoop = false;
		m_playSound = false;
		m_playEnd = false;
	}
	virtual ~MTrack() {
//printf("delete MTrack\n");
		delete m_ch;
		m_events.clear();
	}
	
	int getNumEvents() {
		int n = (int)m_events.size();
		if (n == 0) {
			if (m_delta > 0) {
				n = 1;
			}
		}
		return n;
	}
	
	int loopCount() {
		return m_loopCount;
	}
	
	void replay() {
		m_bpm=0;
		m_spt=0;
		m_isEnd=0;
		m_needle=0.0;
		m_polyFound=false;
		playTempo(DEFAULT_BPM);
		m_volume=100;
		recGate(15.0/16.0);
		recGate2(0);
		m_ch->reset();
		//m_events=new Vector<MEvent> ();
		m_pointer=0;
		m_delta=0;
		m_globalTick=0;
		//		m_signalInterval=96/4;
		//		m_signalCnt=0;
		m_lfoWidth=0.0;
		//m_totalMSec=0;
		m_chordBegin=0;
		m_chordEnd=0;
		m_chordMode=false;
		m_loopPointer = 0;
		m_loopCount = 0;
		m_playSound = false;
		m_playEnd = false;
	}

	void onGetSamples(VNumber* samples,int start,int end/*,MSignal* signal=NULL*/) {
		if (m_ch) {
			m_ch->getSamples(samples,end,start,end);
		}
	}
	
	void eventLog(const char* str,int val=0,int vol=0) {
		printf("%s %d,%d\n",str,val,vol);
	}
	
	void onSampleData(VNumber* samples,int start,int end/*,MSignal* signal=NULL*/) {
//		if (m_playSound) {
//			m_ch->getSamples(samples,end,start,end);
//			return;
//		}
	
		if (isEnd())return ;
		//		int startCnt=m_signalCnt;
		//		if (signal!=NULL)signal->reset();
		//
		//		if (m_globalTick==0&&signal!=NULL){
		//			signal->add(0,0,0);
		//		}
		for (int i=start;i<end;){
			
			int exec=0;
			int eLen=(int )m_events.size();
			MEvent* e;
			double delta;
			int loopCount=0;
			int needle=m_needle;
			do{
				exec=0;
				if (m_pointer<eLen&&loopCount<=1){
					e=&m_events[m_pointer];
					delta=e->getDelta()*m_spt;
					if (m_needle>=delta){
						
						exec=1;
						switch(e->getStatus()){
							case MStatus::NOTE_ON:
								//eventLog("NOTE_ON",e->getNoteNo());
								m_ch->noteOn(e->getNoteNo(),e->getVelocity());
								break;
							case MStatus::NOTE_OFF:
								//eventLog("NOTE_OFF",e->getNoteNo());
								m_ch->noteOff(e->getNoteNo());
								break;
							case MStatus::NOTE:
								//eventLog("NOTE");
								m_ch->setNoteNo(e->getNoteNo());
								break;
							case MStatus::VOLUME:
								//eventLog("VOLUME");
								break;
							case MStatus::LOOP_END:
								//eventLog("LOOP_END");
								if (m_infinityLoop) {
//									if (m_loopCount == 0) {
										m_loopPointer = m_pointer;
//									} else {
//										m_pointer = m_loopPointer;
//									}
//printf("LOOP_START(%d) %d\n",m_trackNo,m_loopCount);
								}else {
									m_loopCount ++;
//printf("LOOP_END(%d) %d\n",m_trackNo,m_loopCount);
								}
								break;
							case MStatus::TEMPO:
								//eventLog("TEMPO");
								playTempo(e->getTempo());
								break;
							case MStatus::FORM:
								//eventLog("FORM",e->getForm(),e->getSubForm());
								m_ch->setForm(e->getForm(),e->getSubForm());
								break;
							case MStatus::ENVELOPE1_ATK:
								//eventLog("ENVELOPE1_ATK");
								m_ch->setEnvelope1Atk(e->getEnvelopeA());
								break;
							case MStatus::ENVELOPE1_ADD:
								//eventLog("ENVELOPE1_ADD");
								m_ch->setEnvelope1Point(e->getEnvelopeT(),e->getEnvelopeL());
								break;
							case MStatus::ENVELOPE1_REL:
								//eventLog("ENVELOPE1_REL");
								m_ch->setEnvelope1Rel(e->getEnvelopeR());
								break;
							case MStatus::ENVELOPE2_ATK:
								//eventLog("ENVELOPE2_ATK");
								m_ch->setEnvelope2Atk(e->getEnvelopeA());
								break;
							case MStatus::ENVELOPE2_ADD:
								//eventLog("ENVELOPE2_ADD");
								m_ch->setEnvelope2Point(e->getEnvelopeT(),e->getEnvelopeL());
								break;
							case MStatus::ENVELOPE2_REL:
								//eventLog("ENVELOPE2_REL");
								m_ch->setEnvelope2Rel(e->getEnvelopeR());
								break;
							case MStatus::NOISE_FREQ:
								//eventLog("NOISE_FREQ");
								m_ch->setNoiseFreq(e->getNoiseFreq());
								break;
							case MStatus::PWM:
								//eventLog("PWM");
								m_ch->setPWM(e->getPWM());
								break;
							case MStatus::PAN:
								//eventLog("PAN");
								m_ch->setPan(e->getPan());
								break;
							case MStatus::FORMANT:
								//eventLog("FORMANT");
								m_ch->setFormant(e->getVowel());
								break;
							case MStatus::DETUNE:
								//eventLog("DETUNE");
								m_ch->setDetune(e->getDetune());
								break;
							case MStatus::LFO_FMSF:
								//eventLog("LFO_FMSF");
								m_ch->setLFOFMSF(e->getLFOForm(),e->getLFOSubForm());
								break;
							case MStatus::LFO_DPWD:
								//eventLog("LFO_DPWD");
								m_lfoWidth=e->getLFOWidth()*m_spt;
								m_ch->setLFODPWD(e->getLFODepth(),44100.0/m_lfoWidth);
								break;
							case MStatus::LFO_DLTM:
								//eventLog("LFO_DLTM");
								m_ch->setLFODLTM(e->getLFODelay()*m_spt,e->getLFOTime()*m_lfoWidth);
								break;
							case MStatus::LFO_TARGET:
								//eventLog("LFO_TARGET");
								m_ch->setLFOTarget(e->getLFOTarget());
								break;
							case MStatus::LPF_SWTAMT:
								//eventLog("LPF_SWTAMT");
								m_ch->setLpfSwtAmt(e->getLPFSwt(),e->getLPFAmt());
								break;
							case MStatus::LPF_FRQRES:
								//eventLog("LPF_FRQRES");
								m_ch->setLpfFrqRes(e->getLPFFrq(),e->getLPFRes());
								break;
							case MStatus::VOL_MODE:
								//eventLog("VOL_MODE");
								m_ch->setVolMode(e->getVolMode());
								break;
							case MStatus::INPUT:
								//eventLog("INPUT");
								m_ch->setInput(e->getInputSens(),e->getInputPipe());
								break;
							case MStatus::OUTPUT:
								//eventLog("OUTPUT");
								m_ch->setOutput(e->getOutputMode(),e->getOutputPipe());
								break;
							case MStatus::EXPRESSION:
								//eventLog("EXPRESSION");
								m_ch->setExpression(e->getExpression());
								break;
							case MStatus::RINGMODULATE:
								//eventLog("RINGMODULATE");
								m_ch->setRing(e->getRingSens(),e->getRingInput());
								break;
							case MStatus::SYNC:
								//eventLog("SYNC");
								m_ch->setSync(e->getSyncMode(),e->getSyncPipe());
								break;
							case MStatus::PORTAMENTO:
								//eventLog("PORTAMENTO");
								m_ch->setPortamento(e->getPorDepth()*100,e->getPorLen()*m_spt);
								break;
							case MStatus::MIDIPORT:
								//eventLog("MIDIPORT",e->getMidiPort());
								m_ch->setMidiPort(e->getMidiPort());
								break;
							case MStatus::MIDIPORTRATE:
								//eventLog("MIDIPORTRATE",e->getMidiPortRate());
							{
								Number rate=e->getMidiPortRate();
								m_ch->setMidiPortRate((8-(rate*7.99/128))/rate);
							}
								break;
							case MStatus::BASENOTE:
								//eventLog("BASENOTE");
								m_ch->setPortBase(e->getPortBase()*100);
								break;
							case MStatus::POLY:
								//eventLog("POLY");
								m_ch->setVoiceLimit(e->getVoiceCount());
								break;
							case MStatus::HW_LFO:
								//eventLog("HW_LFO");
								m_ch->setHwLfo(e->getHwLfoData());
								break;
							case MStatus::SOUND_OFF:
								//eventLog("SOUND_OFF");
								m_ch->setSoundOff();
								break;
							case MStatus::RESET_ALL:
								//eventLog("RESET_ALL");
								m_ch->reset();
								break;
							case MStatus::CLOSE:
								//eventLog("CLOSE");
								m_ch->close();
//printf("CLOSE(%d)\n",m_trackNo);
								break;
							case MStatus::PLAY_END:
								//eventLog("PLAY_END");
								m_playEnd = true;
//								m_ch->close();
//printf("CLOSE(%d)\n",m_trackNo);
								break;
							case MStatus::EOT:
								//eventLog("EOT");
								if (m_infinityLoop) {
									m_pointer = m_loopPointer;
									m_loopCount ++;
									
									//空トラックの無限ループを防ぐ
									if (needle==m_needle) {
										loopCount++;
									} else {
										needle=m_needle;
									}
//									if (loopCount > 1) {
//										m_isEnd=1;
//									}

//printf("INFINIRY_LOOP(%d) %d\n",m_trackNo,m_loopCount);
								} else {
									m_isEnd=1;
//printf("END_CHANNEL(%d)\n",m_trackNo);
								}
								break;
							case MStatus::NOP:
								//eventLog("NOP");
								break;
							default :
								//eventLog("other");
								break;
						}
						m_needle-=delta;
						m_pointer++;
					}
				}
			}while(exec);

			// create a short wave
			double di;
			if (m_pointer<eLen){
				e=&m_events[m_pointer];
				delta=e->getDelta()*m_spt;
				di=Math::ceil(delta-m_needle);
				if (i+di>=end)di=end-i;
				m_needle+=di;
				//				if (signal==NULL)
				m_ch->getSamples(samples,end,i,di);
				i+=di;
				if (di==0) {
					m_isEnd = 1;
					break;
				}
			}
			else {
//printf("END_CHANNEL(%d)\n",m_trackNo);
				m_isEnd = 1;
				break;
			}
			
			
			//			if (signal!=NULL){
			//				m_signalCnt+=di;
			//				int intervalSample=int (m_signalInterval*m_spt);
			//				while(m_signalCnt>=intervalSample){
			//					m_globalTick+=m_signalInterval;
			//					signal->add(int ((intervalSample-startCnt)*(1000.0/44100.0)),m_globalTick,0);
			//					m_signalCnt-=intervalSample;
			//					startCnt=0;
			//				}
			//			}
		}
		//		if (signal!=NULL)signal->terminate();
	}
	
	void seek(int delta) {
		m_delta+=delta;
		m_globalTick+=delta;
		m_chordEnd=Math::max(m_chordEnd,m_globalTick);
	}
	
	void seekChordStart() {
		m_globalTick=m_chordBegin;
	}
	
	void recDelta(MEvent* e) {
		e->setDelta(m_delta);
		m_delta=0;
	}
	
	void recNote(int noteNo,int len,int vel,int keyon=1,int keyoff=1) {
		MEvent* e0=makeEvent();
		if (keyon){
			e0->setNoteOn(noteNo,vel);
		}
		else {
			e0->setNote(noteNo);
		}
		pushEvent(e0);
		if (keyoff){
			int gate;
			gate=(int )(len*m_gate)-m_gate2;
			if (gate<=0)gate=0;
			seek(gate);
			recNoteOff(noteNo,vel);
			seek(len-gate);
			if (m_chordMode){
				seekChordStart();
			}
		}
		else {
			seek(len);
		}
	}
	
	void recNoteOff(int noteNo,int vel) {
		MEvent* e=makeEvent();
		e->setNoteOff(noteNo,vel);
		pushEvent(e);
	}
	
	void recRest(int len) {
		seek(len);
		if (m_chordMode){
			m_chordBegin+=len;
		}
	}
	
	void recChordStart() {
		if (m_chordMode==false){
			m_chordMode=true;
			m_chordBegin=m_globalTick;
		}
	}
	
	void recChordEnd() {
		if (m_chordMode){
			if (m_events.size()>0){
				m_delta=m_chordEnd-m_events[m_events.size()-1].getTick();
			}
			else {
				m_delta=0;
			}
			m_globalTick=m_chordEnd;
			m_chordMode=false;
		}
	}

	void recLoopEnd() {
		MEvent* e=makeEvent();
		e->setLoopEnd();
		pushEvent(e);
	}
	
	void recRestMSec(int msec) {
		int len=msec*44100/(m_spt*1000);
		seek(len);
	}
	
	void recVolume(int vol) {
		MEvent* e=makeEvent();
		e->setVolume(vol);
		pushEvent(e);
	}
	
	bool m_playSound;
	int m_playNote;
	
	void noteOn(int noteNo,int velocity) {
		m_playNote = noteNo;
		m_playSound = true;
		if (m_ch) {
			m_ch->noteOn(noteNo,velocity);
		}
	}
	
	void noteOff() {
		if (m_playSound) {
			if (m_ch) {
				m_ch->noteOff(m_playNote);
			}
		}
	}
	
protected:
	// 挿入先が同時間の場合、前に挿入する。ただし、挿入先がテンポコマンドの場合を除く。
	void recGlobal(unsigned int globalTick,MEvent* e) {
		int n=(int)m_events.size();
		unsigned int preGlobalTick=0;
		
		for (int i=0;i<n;i++){
			MEvent* en=&m_events[i];
			unsigned int nextTick=preGlobalTick+en->getDelta();
			if (nextTick>globalTick||(nextTick==globalTick&&en->getStatus()!=MStatus::TEMPO)){
				en->setDelta((int)(nextTick-globalTick));
				e->setDelta((int)(globalTick-preGlobalTick));
				{
					vector<MEvent>::iterator itr = m_events.begin();
					itr += i;
					m_events.insert(itr,*e);
				}
				return ;
			}
			preGlobalTick=nextTick;
		}

		e->setDelta((int)(globalTick-preGlobalTick));
		m_events.push_back(*e);
		
	}
	
	// 新規イベントインスタンスを得る
	MEvent* makeEvent() {
		MEvent* e=new MEvent(m_globalTick);
		e->setDelta(m_delta);
		m_delta=0;
		return e;
	}
	
	// イベントを適切に追加する
	void pushEvent(MEvent* e) {
		if (m_chordMode==false){
			m_events.push_back(*e);
			delete e;
		}
		else {
			insertEvent(e);
		}
	}
	
	// 挿入先が同時間の場合、後に挿入する。
	void insertEvent(MEvent* e) {
		int n=(int)m_events.size();
		unsigned int preGlobalTick=0;
		unsigned int globalTick=e->getTick();
		
		for (int i=0;i<n;i++){
			MEvent* en=&m_events[i];
			unsigned int nextTick=preGlobalTick+en->getDelta();
			if (nextTick>globalTick){
				en->setDelta(nextTick-globalTick);
				e->setDelta(globalTick-preGlobalTick);
				{
					vector<MEvent>::iterator itr = m_events.begin();
					itr += i;
					m_events.insert(itr,*e);
					delete e;
				}
				return ;
			}
			preGlobalTick=nextTick;
		}
		e->setDelta(globalTick-preGlobalTick);
		m_events.push_back(*e);
		delete e;
	}
public:
	void recTempo(unsigned int globalTick,Number tempo) {
		MEvent* e=new MEvent(globalTick);	// makeEvent()は使用してはならない
		e->setTempo(tempo);
		recGlobal(globalTick, e);
		delete e;
	}
	
	void recEOT() {
		MEvent* e=makeEvent();
		e->setEOT();
		pushEvent(e);
	}
	
	void recGate(Number gate) {
		m_gate=gate;
	}
	
	void recGate2(int gate2) {
		if (gate2<0)gate2=0;
		m_gate2=gate2;
	}
	
	void recForm(int form,int sub) {
		MEvent* e=makeEvent();
		e->setForm(form,sub);
		pushEvent(e);
	}
	
	void recEnvelope(int env,int attack,Vector<int>* times,Vector<int>* levels,int release) {
		MEvent* e=makeEvent();
		if (env==1)e->setEnvelope1Atk(attack);else e->setEnvelope2Atk(attack);
		pushEvent(e);
		int pts=(int )times->size();
		for (int i=0;i<pts;i++){
			e=makeEvent();
			if (env==1)e->setEnvelope1Point((*times)[i],(*levels)[i]);else e->setEnvelope2Point((*times)[i],(*levels)[i]);
			pushEvent(e);
		}
		e=makeEvent();
		if (env==1)e->setEnvelope1Rel(release);else e->setEnvelope2Rel(release);
		pushEvent(e);
	}
	
	void recNoiseFreq(int freq) {
		MEvent* e=makeEvent();
		e->setNoiseFreq(freq);
		pushEvent(e);
	}
	
	void recPWM(int pwm) {
		MEvent* e=makeEvent();
		e->setPWM(pwm);
		pushEvent(e);
	}
	
	void recPan(int pan) {
		MEvent* e=makeEvent();
		e->setPan(pan);
		pushEvent(e);
	}
	
	void recFormant(int vowel) {
		MEvent* e=makeEvent();
		e->setFormant(vowel);
		pushEvent(e);
	}
	
	void recDetune(int d) {
		MEvent* e=makeEvent();
		e->setDetune(d);
		pushEvent(e);
	}
	
	void recLFO(int depth,int width,int form,int subform,int delay,int time,int target) {
		MEvent* e=makeEvent();
		e->setLFOFMSF(form,subform);
		pushEvent(e);
		e=makeEvent();
		e->setLFODPWD(depth,width);
		pushEvent(e);
		e=makeEvent();
		e->setLFODLTM(delay,time);
		pushEvent(e);
		e=makeEvent();
		e->setLFOTarget(target);
		pushEvent(e);
	}
	
	void recLPF(int swt,int amt,int frq,int res) {
		MEvent* e=makeEvent();
		e->setLPFSWTAMT(swt,amt);
		pushEvent(e);
		e=makeEvent();
		e->setLPFFRQRES(frq,res);
		pushEvent(e);
	}
	
	void recVolMode(int m) {
		MEvent* e=makeEvent();
		e->setVolMode(m);
		pushEvent(e);
	}
	
	void recInput(int sens,int pipe) {
		MEvent* e=makeEvent();
		e->setInput(sens,pipe);
		pushEvent(e);
	}
	
	void recOutput(int mode,int pipe) {
		MEvent* e=makeEvent();
		e->setOutput(mode,pipe);
		pushEvent(e);
	}
	
	void recExpression(int ex) {
		MEvent* e=makeEvent();
		e->setExpression(ex);
		pushEvent(e);
	}
	
	void recRing(int sens,int pipe) {
		MEvent* e=makeEvent();
		e->setRing(sens,pipe);
		pushEvent(e);
	}
	
	void recSync(int mode,int pipe) {
		MEvent* e=makeEvent();
		e->setSync(mode,pipe);
		pushEvent(e);
	}
	
	void recClose() {
		MEvent* e=makeEvent();
		e->setClose();
		pushEvent(e);
	}

	void recPlayEnd() {
		MEvent* e=makeEvent();
		e->setPlayEnd();
		pushEvent(e);
	}
	
	void recPortamento(int depth,int len) {
		MEvent* e=makeEvent();
		e->setPortamento(depth,len);
		pushEvent(e);
	}
	
	void recMidiPort(int mode) {
		MEvent* e=makeEvent();
		e->setMidiPort(mode);
		pushEvent(e);
	}
	
	void recMidiPortRate(int rate) {
		MEvent* e=makeEvent();
		e->setMidiPortRate(rate);
		pushEvent(e);
	}
	
	void recPortBase(int base) {
		MEvent* e=makeEvent();
		e->setPortBase(base);
		pushEvent(e);
	}
	
	void recPoly(int voiceCount) {
		MEvent* e=makeEvent();
		e->setPoly(voiceCount);
		pushEvent(e);
		m_polyFound=true;
	}
	
	void recHwLfo(int w,int f,int pmd,int amd,int pms,int ams,int syn) {
		MEvent* e=makeEvent();
		e->setHwLfo(w,f,pmd,amd,pms,ams,syn);
		pushEvent(e);
	}
	
	int isEnd() {
		return m_isEnd;
	}

	int isPlayEnd() {
		if (m_isEnd) return true;
		if (m_playEnd) return true;
		return false;
	}
	
	unsigned int getRecGlobalTick() {
		return m_globalTick;
	}
	
	void seekTop() {
		m_globalTick=0;
	}
	
	void conduct(Array<MTrack>* trackArr,bool infinityLoop) {
		//if (infinityLoop) recLoopEnd();
		int ni=(int)m_events.size();
		int nj=(int)trackArr->size();
		double globalTick=0;
		double globalSample=0;
		Number spt=calcSpt(DEFAULT_BPM);
		int i;int j;
		MEvent* e;
		for (i=0;i<ni;i++){
			e=&m_events[i];
			globalTick+=e->getDelta();
			globalSample+=e->getDelta()*spt;
			switch(e->getStatus()){
				case MStatus::TEMPO:
					spt=calcSpt(e->getTempo());
					for (j=FIRST_TRACK;j<nj;j++){
						(*trackArr)[j]->recTempo(globalTick,e->getTempo());
					}
					break;
				default :
					break;
			}
		}
		int maxGlobalTick=0;
		for (j=FIRST_TRACK;j<nj;j++){
			if (maxGlobalTick<(*trackArr)[j]->getRecGlobalTick()) {
				maxGlobalTick=(*trackArr)[j]->getRecGlobalTick();
			}
		}
		e=makeEvent();
		e->setClose();
		recGlobal(maxGlobalTick,e);
		delete e;
		globalSample+=(maxGlobalTick-globalTick)*spt;
		
		//recRestMSec(3000);
		//if (infinityLoop) recLoopEnd();
		recEOT();
//		globalSample+=3*44100;
		
		m_totalMSec=globalSample*1000/44100;
	}
private:
	// calc number of samples per tick
	Number calcSpt(Number bpm) {
		Number tps=bpm*96.0/60.0;
		return 44100.0/tps;
	}
	// set tempo
	void playTempo(Number bpm) {
		m_bpm=bpm;
		m_spt=calcSpt(bpm);
	}
public:
	unsigned int getTotalMSec() {
		return m_totalMSec;
	}
	string getTotalTimeStr() {
		int sec=Math::ceil((m_totalMSec)/1000.0);
		char str[256];
		sprintf(str,"%02d:%02d",(sec/60),(sec%60));
		string s = str;
		return s;
	}
	
	// 発声数取得
	int getVoiceCount() {
		return m_ch->getVoiceCount();
	}
	
	// モノモードへ移行 (再生開始前に行うこと)
	void usingMono() {
		if (m_ch)delete m_ch;
		m_ch=new MChannel(m_waveData);
		m_ch->m_trackNo = m_trackNo;
	}
	
	// ポリモードへ移行 (再生開始前に行うこと)
	void usingPoly(int maxVoice) {
		if (m_ch)delete m_ch;
		m_ch=new MPolyChannel(maxVoice,m_waveData);
	}
	
	// ポリ命令を１回でも使ったか？
	bool findPoly() {
		return m_polyFound;
	}
};

}
