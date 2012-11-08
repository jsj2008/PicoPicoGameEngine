#pragma once

#include <iostream>
#include <string>

using namespace std;

#include "FlMML_Number.h"
#include "OPM.h"
#include "FlMML_Math.h"
#include "MOscMod.h"
#include "MEngineData.h"

#define MOscOPM_OPM_RATIO 0.0 //-192.048495012562; // 1200.0*Math.Log(3580000.0/OPM_CLOCK)/Math.Log(2.0); 

namespace FlMML {

/**
 * FM音源ドライバ MOscOPM for AS3
 * @author ALOE
 */
class MOscOPM : public MOscMod {
public:
	// 音色メモリ数
	static const int MAX_WAVE=MOscOPMData::MAX_WAVE;
	// 動作周波数 (Hz)
	static const int OPM_CLOCK=MOscOPMData::OPM_CLOCK;// 4000000;
	// 3.58MHz(基本)：動作周波数比 (cent)
	// パラメータ長
	static const int TIMB_SZ_M=MOscOPMData::TIMB_SZ_M;	// #OPM
	static const int TIMB_SZ_N=MOscOPMData::TIMB_SZ_N;	// #OPN
	// パラメータタイプ
	static const int TYPE_OPM=MOscOPMData::TYPE_OPM;
	static const int TYPE_OPN=MOscOPMData::TYPE_OPN;
private:
	OPM* m_fm;
	VNumber m_oneSample[1];
	int m_opMask;
	int m_velocity;
	int m_al;
	int m_tl[4];
	
	MOscOPMData* m_data;
	
public:
	MOscOPM(MOscOPMData* data) {
		m_data = data;
//		m_oneSample = new Vector<Number>(1);
		
		m_fm=new OPM();
		m_velocity=127;
		m_al=0;
		
		//boot();
		m_fm->Init(OPM_CLOCK,MSequencer_RATE44100);
		m_fm->Reset();
		m_fm->SetVolume(m_data->s_comGain);
		setOpMask(15);
		setWaveNo(0);
	}
	virtual ~MOscOPM() {
//		delete m_oneSample;
		delete m_fm;
	}
	
protected:
	void loadTimbre(Vector<int>* p) {
		SetFBAL((*p)[1],(*p)[0]);
		
		int i;int s;
		for (i=2,s=0;s<4;s++,i+=11){
			SetDT1ML(MOscOPMData::slottable[s],(*p)[i+8],(*p)[i+7]);
			m_tl[s]=(*p)[i+5];
			SetTL(MOscOPMData::slottable[s],(*p)[i+5]);
			SetKSAR(MOscOPMData::slottable[s],(*p)[i+6],(*p)[i+0]);
			SetDRAMS(MOscOPMData::slottable[s],(*p)[i+1],(*p)[i+10]);
			SetDT2SR(MOscOPMData::slottable[s],(*p)[i+9],(*p)[i+2]);
			SetSLRR(MOscOPMData::slottable[s],(*p)[i+4],(*p)[i+3]);
		}
		
		setVelocity(m_velocity);
		setOpMask((*p)[i+0]);
		setWF((*p)[i+1]);
		setLFRQ((*p)[i+2]);
		setPMD((*p)[i+3]);
		setAMD((*p)[i+4]);
		setPMSAMS((*p)[i+5],(*p)[i+6]);
		setNENFRQ((*p)[i+7],(*p)[i+8]);
	}
private:
	// レジスタ操作系 (非公開)
	void SetFBAL(int fb,int al) {
		int pan=3;
		m_al=al&7;
		m_fm->SetReg(0x20,((pan&3)<<6)|((fb&7)<<3)|(al&7));
	}
	void SetDT1ML(int slot,int DT1,int MUL) {
		m_fm->SetReg((2<<5)|((slot&3)<<3),((DT1&7)<<4)|(MUL&15));
	}
	void SetTL(int slot,int TL) {
		if (TL<0)TL=0;
		if (TL>127)TL=127;
		m_fm->SetReg((3<<5)|((slot&3)<<3),TL&0x7F);
	}
	void SetKSAR(int slot,int KS,int AR) {
		m_fm->SetReg((4<<5)|((slot&3)<<3),((KS&3)<<6)|(AR&0x1f));
	}
	void SetDRAMS(int slot,int DR,int AMS) {
		m_fm->SetReg((5<<5)|((slot&3)<<3),((AMS&1)<<7)|(DR&0x1f));
	}
	void SetDT2SR(int slot,int DT2,int SR) {
		m_fm->SetReg((6<<5)|((slot&3)<<3),((DT2&3)<<6)|(SR&0x1f));
	}
	void SetSLRR(int slot,int SL,int RR) {
		m_fm->SetReg((7<<5)|((slot&3)<<3),((SL&15)<<4)|(RR&0x0f));
	}
public:
	// レジスタ操作系 (公開)
	void setPMSAMS(int PMS,int AMS) {
		m_fm->SetReg(0x38,((PMS&7)<<4)|((AMS&3)));
	}
	void setPMD(int PMD) {
		m_fm->SetReg(0x19,0x80|(PMD&0x7f));
	}
	void setAMD(int AMD) {
		m_fm->SetReg(0x19,0x00|(AMD&0x7f));
	}
	void setNENFRQ(int NE,int NFQR) {
		m_fm->SetReg(0x0f,((NE&1)<<7)|(NFQR&0x1F));
	}
	void setLFRQ(int f) {
		m_fm->SetReg(0x18,f&0xff);
	}
	void setWF(int wf) {
		m_fm->SetReg(0x1b,wf&3);
	}
	void noteOn() {
		m_fm->SetReg(0x01,0x02);
		m_fm->SetReg(0x01,0x00);
		m_fm->SetReg(0x08,m_opMask<<3);
	}
	void noteOff() {
		m_fm->SetReg(0x08,0x00);
	}
	
	// 音色選択
	virtual void setWaveNo(int waveNo) {
		if (waveNo>=MAX_WAVE)waveNo=MAX_WAVE-1;
		if (m_data->s_table[waveNo].array==NULL)waveNo=0;
		m_fm->SetVolume(m_data->s_comGain);// コモンゲイン適用
		loadTimbre(&m_data->s_table[waveNo]);
	}
	
	// ノートオン
	virtual void setNoteNo(int noteNo) {
		noteOn();
	}
	
	// オペレータマスク
	void setOpMask(int mask) {
		m_opMask=mask&0xF;
	}
	
	// 0～127のベロシティを設定 (キャリアのトータルレベルが操作される)
	void setVelocity(int vel) {
		m_velocity=vel;
		if ((MOscOPMData::carrierop[m_al]&0x08)!=0)SetTL(MOscOPMData::slottable[0],m_tl[0]+(127-m_velocity));else SetTL(MOscOPMData::slottable[0],m_tl[0]);
		if ((MOscOPMData::carrierop[m_al]&0x10)!=0)SetTL(MOscOPMData::slottable[1],m_tl[1]+(127-m_velocity));else SetTL(MOscOPMData::slottable[1],m_tl[1]);
		if ((MOscOPMData::carrierop[m_al]&0x20)!=0)SetTL(MOscOPMData::slottable[2],m_tl[2]+(127-m_velocity));else SetTL(MOscOPMData::slottable[2],m_tl[2]);
		if ((MOscOPMData::carrierop[m_al]&0x40)!=0)SetTL(MOscOPMData::slottable[3],m_tl[3]+(127-m_velocity));else SetTL(MOscOPMData::slottable[3],m_tl[3]);
	}
	
	// 0～1.0のエクスプレッションを設定
	void setExpression(Number ex) {
		m_fm->SetExpression(ex);
	}
	
	virtual void setFrequency(Number frequency) {
		if (m_frequency==frequency){
			return ;
		}
		MOscMod::setFrequency(frequency);
		
		// 指示周波数からMIDIノート番号(≠FlMMLノート番号)を逆算する（まったくもって無駄・・）
		int n=(int )(1200.0*Math::log(frequency/440.0)*Math::LOG2E+5700.0+MOscOPM_OPM_RATIO+0.5);
		int note=n/100;
		int cent=n%100;
		
		// key flaction
		int kf=(int )(64.0*cent/100.0+0.5);
		// key code
		//    ------ octave ----  -------- note ---------
		int kc=(((note-1)/12)<<4)|MOscOPMData::kctable[(note+1200)%12];
		
		m_fm->SetReg(0x30,kf<<2);
		m_fm->SetReg(0x28,kc);
	}
	
	virtual Number getNextSample() {
		m_fm->Mix(m_oneSample,0,1);
		return (m_oneSample)[0];
	}
	
	virtual Number getNextSampleOfs(Integer ofs) {
		m_fm->Mix(m_oneSample,0,1);
		return (m_oneSample)[0];
	}
	
	virtual void getSamples(VNumber* samples,int start,int end) {
		m_fm->Mix(samples,start,end-start);
	}
	
	bool IsPlaying() {
		return m_fm->IsOn(0);
	}
};

}
