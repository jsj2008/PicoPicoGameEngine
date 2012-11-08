//
//  MEngineData.h
//  PicoPicoGames
//
//  Created by Yamaguchi Hiromitsu on 12/03/12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef PicoPicoGames_MEngineData_h
#define PicoPicoGames_MEngineData_h

#include "FlMML_Regex.h"
#include <vector>
#include <list>

namespace FlMML {

class MOscFcDpcmData {
public:
	static const int MAX_WAVE=16;
	static const int FC_CPU_CYCLE=1789773;
	static const int FC_DPCM_PHASE_SFT=2;
	static const int FC_DPCM_MAX_LEN=0xff1;//(0xff * 0x10) + 1 ファミコン準拠の最大レングス
	static const int FC_DPCM_TABLE_MAX_LEN=(FC_DPCM_MAX_LEN>>2)+2;
	static const int FC_DPCM_NEXT=44100<<FC_DPCM_PHASE_SFT;
	MOscFcDpcmData() {
		clear();
		setWave(0,127,0,"");
	}
	virtual ~MOscFcDpcmData() {
	}
	
	unsigned int s_table[MAX_WAVE][FC_DPCM_TABLE_MAX_LEN];
	int s_intVol[MAX_WAVE];
	int s_loopFg[MAX_WAVE];
	int s_length[MAX_WAVE];
	
	void clear() {
		for (int i=0;i<MOscFcDpcmData::MAX_WAVE;i++) {
			for (int j=0;j<MOscFcDpcmData::FC_DPCM_TABLE_MAX_LEN;j++) {
				s_table[i][j] = 0;
			}
			s_intVol[i] = 0;
			s_loopFg[i] = false;
			s_length[i] = 0;
		}
	}
	
	void setWave(int waveNo,int intVol,int loopFg,string wave) {
		if (waveNo < 0 || waveNo >= MAX_WAVE) return;
		s_intVol[waveNo]=intVol;
		s_loopFg[waveNo]=loopFg;
		s_length[waveNo]=0;
		
		//s_table[waveNo]=new Vector<unsigned int>(FC_DPCM_TABLE_MAX_LEN);
		int strCnt=0;
		int intCnt=0;
		int intCn2=0;
		int intPos=0;
		for (int i=0;i<FC_DPCM_TABLE_MAX_LEN;i++){
			s_table[waveNo][i]=0;
		}
		
		for (strCnt=0;strCnt<wave.length();strCnt++){
			int code=wave.at(strCnt);
			if (0x41<=code&&code<=0x5a){ //A-Z
				code-=0x41;
			}
			else if (0x61<=code&&code<=0x7a){ //a-z
				code-=0x61-26;
			}
			else if (0x30<=code&&code<=0x39){ //0-9
				code-=0x30-26-26;
			}
			else if (0x2b==code){ //+
				code=26+26+10;
			}
			else if (0x2f==code){ // /
				code=26+26+10+1;
			}
			else if (0x3d==code){ // =
				code=0;
			}
			else {
				code=0;
			}
			for (int i=5;i>=0;i--){
				s_table[waveNo][intPos]+=((code>>i)&1)<<(intCnt*8+7-intCn2);
				intCn2++;
				if (intCn2>=8){
					intCn2=0;
					intCnt++;
				}
				s_length[waveNo]++;
				if (intCnt>=4){
					intCnt=0;
					intPos++;
					if (intPos>=FC_DPCM_TABLE_MAX_LEN){
						intPos=FC_DPCM_TABLE_MAX_LEN-1;
					}
				}
			}
		}
		//レングス中途半端な場合、削る
		s_length[waveNo]-=((s_length[waveNo]-8)%0x80);
		//最大・最小サイズ調整
		if (s_length[waveNo]>FC_DPCM_MAX_LEN*8){
			s_length[waveNo]=FC_DPCM_MAX_LEN*8;
		}
		if (s_length[waveNo]==0){
			s_length[waveNo]=8;
		}
		//長さが指定されていれば、それを格納
		//if (length >= 0) s_length[waveNo] = (length * 0x10 + 1) * 8;
	}
};

class MOscGbWaveData {
public:
	static const int MAX_WAVE=32;
	static const int GB_WAVE_TABLE_LEN=(1<<5);
	MOscGbWaveData() {
		clear();
		setWave(0,"0123456789abcdeffedcba9876543210");
	}
	virtual ~MOscGbWaveData() {
	}
	
	Number s_table[MAX_WAVE][GB_WAVE_TABLE_LEN];
	
	void clear() {
		for (int i=0;i<MAX_WAVE;i++) {
			for (int j=0;j<GB_WAVE_TABLE_LEN;j++) {
				s_table[i][j] = 0;
			}
		}
	}
	void setWave(int waveNo,string wave) {
		if (waveNo < 0 || waveNo >= MAX_WAVE) return;
		//s_table[waveNo].reserve(GB_WAVE_TABLE_LEN);
		for (int i=0;i<32;i++){
			int code=wave.at(i);
//			if (code >= 'A' && code <= 'Z') {
//				code = code - 'A' + 'a';
//			}
			if (48<=code&&code<58){
				code-=48;
			}
			else if (97<=code&&code<103){
				code-=97-10;
			}
			else {
				code=0;
			}
			s_table[waveNo][i]=(Number(code)-7.5)/7.5;
		}
	}
};

class MOscWaveData {
public:
	static const int MAX_WAVE=32;
	static const int MAX_LENGTH=2048;
	static const int TABLE_LEN=1<<16;
	static const int PHASE_SFT=14;
	static const int PHASE_LEN=TABLE_LEN<<PHASE_SFT;
	static const int PHASE_MSK=PHASE_LEN-1;
	MOscWaveData() {
		for (int i=0;i<MAX_WAVE;i++) {
			s_table[i] = NULL;
		}
		//s_length = NULL;
		//s_length.reserve(MAX_WAVE);//=new Vector<Number> (MAX_WAVE);
		setWave(0,"00112233445566778899AABBCCDDEEFFFFEEDDCCBBAA99887766554433221100");
	}
	virtual ~MOscWaveData() {
		for (int i=0;i<MAX_WAVE;i++) {
			if (s_table[i]) free(s_table[i]);
		}
		//if (s_length) free(s_length);
	}
	
	Number* s_table[MAX_WAVE];
	int s_length[MAX_WAVE];
	
	void clear() {
		for (int i=0;i<MAX_WAVE;i++) {
			if (s_table[i]) free(s_table[i]);
			s_table[i] = NULL;
		}
	}

	void setWave(int waveNo,string wave) {
		if (waveNo < 0 || waveNo >= MAX_WAVE) return;
		s_length[waveNo]=0.0f;
		//s_table[waveNo]=new Vector<Number> (wave.length()/2);
		s_table[waveNo] = (Number*)calloc(wave.length()/2,sizeof(Number));//.reserve(wave.length()/2);
		s_table[waveNo][0]=0;
		int j=0;int val=0;
		for (int i=0;i<MAX_LENGTH&&i<wave.length();i++,j++){
			int code=wave.at(i);
//			if (code >= 'A' && code <= 'Z') {
//				code = code - 'A' + 'a';
//			}
			if (48<=code&&code<58){
				code-=48;
			} else if (97<=code&&code<103){
				code-=97-10;
			} else {
				code=0;
			}
			if (j&1){
				val+=code;
				s_table[waveNo][s_length[waveNo]]=(Number (val)-127.5)/127.5;
				s_length[waveNo]++;
			} else {
				val=code<<4;
			}
		}
		if (s_length[waveNo]==0)s_length[waveNo]=1;
		s_length[waveNo]=(PHASE_MSK+1)/s_length[waveNo];
	}
};

class MSequencerData {
public:
	static const int BUFFER_SIZE=512;
};

class MChannelData {
public:
	static const int PITCH_RESOLUTION=100;
	static const int FREQUENCY_LEN=128*PITCH_RESOLUTION;
	static const int s_volumeLen=128;
	static const int s_numSamples=MSequencerData::BUFFER_SIZE*32;

	static bool s_init;
	static Number s_frequencyMap[FREQUENCY_LEN];
	static Number* s_volumeMap[3];

	static const int s_samples_size = MSequencerData::BUFFER_SIZE*32;
	float s_samples[s_samples_size];

	VNumber** s_pipeArr;
	int s_pipeNum;
	Vector<bool>* s_syncSources;
	
	MChannelData() {
		s_pipeArr = NULL;
		s_syncSources = NULL;
		s_pipeNum = 0;
		if (s_init == 0) {
			//s_frequencyMap.reserve(128*MChannel::MChannelData::PITCH_RESOLUTION);
			int i;
			for (i=0;i<MChannelData::FREQUENCY_LEN;i++){
				s_frequencyMap[i]=440.0*Math::pow(2.0,(i-69*MChannelData::PITCH_RESOLUTION)/(12.0*MChannelData::PITCH_RESOLUTION));
			}
			//s_volumeLen=128;
			//s_volumeMap.reserve(3);//=new Vector<Vector<Number>*> (3);
			for (i=0;i<3;i++){
				//s_volumeMap.push_back(new Vector<Number> (s_volumeLen));
				s_volumeMap[i] = (Number*)calloc(1,s_volumeLen*sizeof(Number));
				s_volumeMap[i][0]=0.0;
			}
			for (i=1;i<s_volumeLen;i++){
				s_volumeMap[0][i]=i/127.0;
				s_volumeMap[1][i]=Math::pow(10.0,(i-127.0)*(48.0/(127.0*20.0)));
				s_volumeMap[2][i]=Math::pow(10.0,(i-127.0)*(96.0/(127.0*20.0)));
			}
			s_init = 1;
		}
//		s_samples.reserve(s_numSamples);//=new Vector<Number> (numSamples);
	}
	virtual ~MChannelData() {
//		for (int i=0;i<3;i++) {
//			if (s_volumeMap[i]) free(s_volumeMap[i]);
//		}
		if (s_pipeArr) {
			for (int i=0;i<s_pipeNum;i++) {
				delete s_pipeArr[i];
			}
			delete[] s_pipeArr;
		}
	}

	void createPipes(int num) {
		if (s_pipeArr) {
			for (int i=0;i<s_pipeNum;i++) {
				delete s_pipeArr[i];
			}
			delete[] s_pipeArr;
		}
		s_pipeNum = num;
		s_pipeArr = new float*[s_pipeNum];
		for (int i=0;i<num;i++){
			s_pipeArr[i] = new float[s_samples_size];
//			s_pipeArr[i].reserve(s_samples_size);//=new Vector<Number> (s_samples->size());
			for (int j=0;j<s_samples_size;j++){
				s_pipeArr[i][j]=0;
			}
		}
	}
	void createSyncSources(int num) {
		if (s_syncSources) delete[] s_syncSources;
		s_syncSources = new Vector<bool>[num];
		for (int i=0;i<num;i++){
			s_syncSources[i].reserve(s_samples_size);//=new Vector<bool> (s_samples->size());
			for (int j=0;j<s_samples_size;j++){
				s_syncSources[i][j]=false;
			}
		}
	}
	
	Number getFrequency(int freqNo) {
		freqNo=(freqNo<0)?0:(freqNo>=MChannelData::FREQUENCY_LEN)?MChannelData::FREQUENCY_LEN-1:freqNo;
//printf("freqNo %d,%lf\n",freqNo,s_frequencyMap[freqNo]);
		return s_frequencyMap[freqNo];
	}
};

class MOscOPMData {
public:
	static const int MAX_WAVE=128;
	static const int OPM_CLOCK=3580000;// 4000000;
	// 3.58MHz(基本)：動作周波数比 (cent)
	// パラメータ長
	static const int TIMB_SZ_M=55;	// #OPM
	static const int TIMB_SZ_N=51;	// #OPN
	// パラメータタイプ
	static const int TYPE_OPM=0;
	static const int TYPE_OPN=1;
	Vector<int> s_table[MAX_WAVE];
	Number s_comGain;

	// YM2151 アプリケーションマニュアル Fig.2.4より
	static int kctable[];
	
	// スロットのアドレス
	static int slottable[];
	
	// キャリアとなるOP
	static int carrierop[];
	
	static int defTimbre[];
	
	static int zeroTimbre[];

	MOscOPMData() {
		for (int i=0;i<MAX_WAVE;i++) {
			s_table[i].reserve(TIMB_SZ_M);
		}
		for (int i=0;i<TIMB_SZ_M;i++) {
			s_table[0].array[i]=defTimbre[i];
		}
	}

	void clearTimber() {
		for (int i=0;i<s_table->size();i++){
			if (i==0) {
				s_table[i].reserve(TIMB_SZ_M);
				for (int j=0;j<TIMB_SZ_M;j++) {
					s_table[i].array[j]=MOscOPMData::defTimbre[j];
				}
			} else {
				s_table[i].clear();
			}
		}
	}

	void setCommonGain(Number gain) {
		s_comGain=gain;
	}
	
	string trim(string str) {
		regex regexHead("^[,]*");
		regex regexFoot("[,]*$");
		str = regex_replace(str,regexHead,"",regex_constants::match_not_dot_newline);
		return regex_replace(str,regexFoot,"",regex_constants::match_not_dot_newline);
	}

	list<string> split(string str, string delim)
	{
		list<string> result;
		size_t cutAt;
		while( (cutAt = str.find_first_of(delim)) != str.npos )
		{
			if(cutAt > 0)
			{
				result.push_back(str.substr(0, cutAt));
			}
			str = str.substr(cutAt + 1);
		}
		if(str.length() > 0)
		{
			result.push_back(str);
		}
		return result;
	}

	void setTimber(int no,int type,string s) {
		if (no<0||MAX_WAVE<=no)return ;
		{
			regex reg("[,;[:space:]\\t\\r\\n]+");
			s = regex_replace(s,reg,",",regex_constants::match_not_dot_newline);
		}
		s=trim(s);
		int a[TIMB_SZ_M];
		int a_size=0;
		{
			int i=0;
			list<string> aa=split(s,",");
			list<string>::iterator iter = aa.begin(); 
			while(iter != aa.end()) {
			 	a[i] = atoi(iter->c_str());
				i++;
			 	iter++;
			}
			a_size = i;
		}
		int b[TIMB_SZ_M];
		
		// パラメータの数の正当性をチェック
		switch(type){
			case TYPE_OPM:if (a_size<2+11*4)return ;// 足りない
				break;
			case TYPE_OPN:if (a_size<2+10*4)return ;// 足りない
				break;
			default :return ; // んなものねぇよ
		}
		
		int i;int j;int l;
		
		switch(type){
			case TYPE_OPM:
				l=Math::min(TIMB_SZ_M,a_size);
				for (i=0;i<l;i++){
					b[i]=a[i];
				}
				for (;i<TIMB_SZ_M;i++){
					b[i]=MOscOPMData::zeroTimbre[i];
				}
				break;
				
			case TYPE_OPN:
				// AL FB
				for (i=0,j=0;i<2;i++,j++){
					b[i]=a[j];
				}
				// AR DR SR RR SL TL KS ML DT AM 4セット
				for (;i<46;i++){
					if ((i-2)%11==9)b[i]=0;
					else b[i]=a[j++];
				}
				l=Math::min(TIMB_SZ_N,a_size);
				for (;j<l;i++,j++){
					b[i]=a[j];
				}
				for (;i<TIMB_SZ_M;i++){
					b[i]=MOscOPMData::zeroTimbre[i];
				}
				break;
		}
		
		// 格納
		s_table[no].reserve(TIMB_SZ_M);
		for (int j=0;j<TIMB_SZ_M;j++) {
			s_table[no].array[j]=b[j];
		}
	}
};

class MEngineData {
public:
	MOscFcDpcmData dpcmData;
	MOscGbWaveData gbWaveData;
	MOscWaveData waveData;
	MChannelData channelData;
	MOscOPMData opmData;
};

}

#endif
