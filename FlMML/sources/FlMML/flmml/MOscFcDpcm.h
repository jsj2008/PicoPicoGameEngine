#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "MOscMod.h"
#include "FlMML_Vector.h"
#include "MEngineData.h"

namespace FlMML {

/**
   DPCM Oscillator by OffGao
   09/05/11：作成
   09/11/05：波形データ格納処理で、データが32bitごとに1bit抜けていたのを修正
 */
class MOscFcDpcm : public MOscMod {
public:
protected:
	int m_readCount;	//次の波形生成までのカウント値
	int m_address;		//読み込み中のアドレス位置
	int m_bit;			//読み込み中のビット位置
	int m_wav;			//現在のボリューム
	int m_length;		//残り読み込み長
	int m_ofs;			//前回のオフセット
	
	MOscFcDpcmData* m_data;
	
	int m_waveNo;
	static int s_interval[]; //音程
public:
	MOscFcDpcm(MOscFcDpcmData* data) {
		m_data = data;
		m_readCount=0;
		m_address=0;
		m_bit=0;
		m_wav=0;
		m_length=0;
		m_ofs=0;
		setWaveNo(0);
	}
	virtual void setWaveNo(int waveNo) {
		if (waveNo>=MOscFcDpcmData::MAX_WAVE)waveNo=MOscFcDpcmData::MAX_WAVE-1;
		if (!m_data->s_table[waveNo])waveNo=0;
		m_waveNo=waveNo;
	}
private:
	Number getValue() {
		if (m_length>0){
			if ((m_data->s_table[m_waveNo][m_address]>>m_bit)&1){
				if (m_wav<126)m_wav+=2;
			}else {
				if (m_wav>1)m_wav-=2;
			}
			m_bit++;
			if (m_bit>=32){
				m_bit=0;
				m_address++;
			}
			m_length--;
			if (m_length==0){
				if (m_data->s_loopFg[m_waveNo]){
					m_address=0;
					m_bit=0;
					m_length=m_data->s_length[m_waveNo];
				}
			}
			return (m_wav-64)/64.0;
		}else {
			return (m_wav-64)/64.0;
		}
	}
public:
	virtual void resetPhase() {
		m_phase=0;
		m_address=0;
		m_bit=0;
		m_ofs=0;
		m_wav=m_data->s_intVol[m_waveNo];
		m_length=m_data->s_length[m_waveNo];
		
	}
	virtual Number getNextSample() {
		Number val=(m_wav-64)/64.0;
		m_phase=(m_phase+m_freqShift)&PHASE_MSK;
		while(MOscFcDpcmData::FC_DPCM_NEXT<=m_phase){
			m_phase-=MOscFcDpcmData::FC_DPCM_NEXT;
			//CPU負荷軽減のため
			//val = getValue();
			{
				if (m_length>0){
					if ((m_data->s_table[m_waveNo][m_address]>>m_bit)&1){
						if (m_wav<126)m_wav+=2;
					}else {
						if (m_wav>1)m_wav-=2;
					}
					m_bit++;
					if (m_bit>=32){
						m_bit=0;
						m_address++;
					}
					m_length--;
					if (m_length==0){
						if (m_data->s_loopFg[m_waveNo]){
							m_address=0;
							m_bit=0;
							m_length=m_data->s_length[m_waveNo];
						}
					}
					val=(m_wav-64)/64.0;
				}else {
					val=(m_wav-64)/64.0;
				}
			}
		}
		return val;
	}
	virtual Number getNextSampleOfs(Integer ofs) {
		Number val=(m_wav-64)/64.0;
		m_phase=(m_phase+m_freqShift+((ofs-m_ofs)>>(PHASE_SFT-7)))&PHASE_MSK;
		while(MOscFcDpcmData::FC_DPCM_NEXT<=m_phase){
			m_phase-=MOscFcDpcmData::FC_DPCM_NEXT;
			//CPU負荷軽減のため
			//val = getValue();
			{
				if (m_length>0){
					if ((m_data->s_table[m_waveNo][m_address]>>m_bit)&1){
						if (m_wav<126)m_wav+=2;
					}else {
						if (m_wav>1)m_wav-=2;
					}
					m_bit++;
					if (m_bit>=32){
						m_bit=0;
						m_address++;
					}
					m_length--;
					if (m_length==0){
						if (m_data->s_loopFg[m_waveNo]){
							m_address=0;
							m_bit=0;
							m_length=m_data->s_length[m_waveNo];
						}
					}
					val=(m_wav-64)/64.0;
				}else {
					val=(m_wav-64)/64.0;
				}
			}
		}
		m_ofs=(int)ofs;
		return val;
	}
	virtual void getSamples(VNumber* samples,int start,int end) {
		int i;
		Number val=(m_wav-64)/64.0;
		for (i=start;i<end;i++){
			m_phase=(m_phase+m_freqShift)&PHASE_MSK;
			while(MOscFcDpcmData::FC_DPCM_NEXT<=m_phase){
				m_phase-=MOscFcDpcmData::FC_DPCM_NEXT;
				//CPU負荷軽減のため
				//val = getValue();
				{
					if (m_length>0){
						if ((m_data->s_table[m_waveNo][m_address]>>m_bit)&1){
							if (m_wav<126)m_wav+=2;
						}else {
							if (m_wav>1)m_wav-=2;
						}
						m_bit++;
						if (m_bit>=32){
							m_bit=0;
							m_address++;
						}
						m_length--;
						if (m_length==0){
							if (m_data->s_loopFg[m_waveNo]){
								m_address=0;
								m_bit=0;
								m_length=m_data->s_length[m_waveNo];
							}
						}
						val=(m_wav-64)/64.0;
					}else {
						val=(m_wav-64)/64.0;
					}
				}
			}
			(samples)[i]=val;
		}
	}
	virtual void setFrequency(Number frequency) {
		//m_frequency = frequency;
		m_freqShift=frequency*(1<<(MOscFcDpcmData::FC_DPCM_PHASE_SFT+4)); // as interval
	}
	void setDpcmFreq(int no) {
		if (no<0)no=0;
		if (no>15)no=15;
		m_freqShift=(MOscFcDpcmData::FC_CPU_CYCLE<<MOscFcDpcmData::FC_DPCM_PHASE_SFT)/s_interval[no]; // as interval
	}
	virtual void setNoteNo(int noteNo) {
		setDpcmFreq(noteNo);
	}
};

}
