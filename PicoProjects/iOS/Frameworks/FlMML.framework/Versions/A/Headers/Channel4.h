// ---------------------------------------------------------------------------
//	FM Sound Generator - Core Unit
//	Copyright (C) cisc 1998, 2003.
//	Copyright (C) 2011 ALOE. All rights reserved.
// ---------------------------------------------------------------------------
#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "Operator.h"
#include "FM.h"

namespace FlMML {

class Channel4 {
public:
	int fb;
	int buf[4];
	int* pms;
	int ix[3];
	int ox[3];
	int algo_;
	Chip* chip_;
	
	Operator op[4];
private:
	static const int fbtable[];
	static const int kftable[];
	static const int kctable[];
	static const int iotable[8][6];
public:
	Channel4() {
		SetAlgorithm(0);
		pms=FM::pmtable[0][0];
	}
	
	//	オペレータの種類 (LFO) を設定
	void SetType(int type) {
		for (int i=0;i<4;i++)op[i].type_=type;
	}
	
	//	セルフ・フィードバックレートの設定 (0-7)
	void SetFB(int feedback) {
		fb=fbtable[feedback];
	}
	
	//	OPNA 系 LFO の設定
	void SetMS(int ms) {
		op[0].SetMS(ms);
		op[1].SetMS(ms);
		op[2].SetMS(ms);
		op[3].SetMS(ms);
	}
	
	//	チャンネル・マスク
	void Mute(bool m) {
		for (int i=0;i<4;i++)op[i].Mute(m);
	}
	
	//	内部パラメータを再計算
	void Refresh() {
		for (int i=0;i<4;i++)op[i].Refresh();
	}
	
	void SetChip(Chip* chip) {
		chip_=chip;
		for (int i=0;i<4;i++)op[i].SetChip(chip);
	}
	
	// リセット
	void Reset() {
		op[0].Reset();
		op[1].Reset();
		op[2].Reset();
		op[3].Reset();
	}
	
	//	Calc の用意
	int Prepare() {
		op[0].Prepare();
		op[1].Prepare();
		op[2].Prepare();
		op[3].Prepare();
		
		pms=FM::pmtable[op[0].type_][op[0].ms_&7];
		int key=(op[0].IsOn()||op[1].IsOn()||op[2].IsOn()||op[3].IsOn())?1:0;
		int lfo=(op[0].ms_&(op[0].amon_||op[1].amon_||op[2].amon_||op[3].amon_?0x37:7))!=0?2:0;
		return key|lfo;
	}
	
	//	F-Number/BLOCK を設定
	void SetFNum(int f) {
		for (int i=0;i<4;i++)op[i].SetFNum(f);
	}
	
	//	KC/KF を設定
	void SetKCKF(int kc,int kf) {
		int oct=19-((kc>>4)&7);
		int kcv=kctable[kc&0x0f];
		kcv=(kcv+2)/4*4;
		int dp=kcv*kftable[kf&0x3f];
		dp>>=16+3;
		dp<<=16+3;
		dp>>=oct;
		int bn=(kc>>2)&31;
		op[0].SetDPBN(dp,bn);
		op[1].SetDPBN(dp,bn);
		op[2].SetDPBN(dp,bn);
		op[3].SetDPBN(dp,bn);
	}
	
	//	キー制御
	void KeyControl(int key) {
		if ((key&0x1)!=0)op[0].KeyOn();else op[0].KeyOff();
		if ((key&0x2)!=0)op[1].KeyOn();else op[1].KeyOff();
		if ((key&0x4)!=0)op[2].KeyOn();else op[2].KeyOff();
		if ((key&0x8)!=0)op[3].KeyOn();else op[3].KeyOff();
	}
	
	//	アルゴリズムを設定
	void SetAlgorithm(int algo) {
		ix[0]=iotable[algo][0];
		ox[0]=iotable[algo][1];
		ix[1]=iotable[algo][2];
		ox[1]=iotable[algo][3];
		ix[2]=iotable[algo][4];
		ox[2]=iotable[algo][5];
		op[0].ResetFB();
		algo_=algo;
	}
	
	//	アルゴリズムを取得
	int GetAlgorithm() {
		return algo_;
	}
	
	//  合成
	int Calc() {
		int r=0;
		switch(algo_){
			case 0:
				op[2].Calc(op[1].Out());
				op[1].Calc(op[0].Out());
				r=op[3].Calc(op[2].Out());
				op[0].CalcFB(fb);
				break;
			case 1:
				op[2].Calc(op[0].Out()+op[1].Out());
				op[1].Calc(0);
				r=op[3].Calc(op[2].Out());
				op[0].CalcFB(fb);
				break;
			case 2:
				op[2].Calc(op[1].Out());
				op[1].Calc(0);
				r=op[3].Calc(op[0].Out()+op[2].Out());
				op[0].CalcFB(fb);
				break;
			case 3:
				op[2].Calc(0);
				op[1].Calc(op[0].Out());
				r=op[3].Calc(op[1].Out()+op[2].Out());
				op[0].CalcFB(fb);
				break;
			case 4:
				op[2].Calc(0);
				r=op[1].Calc(op[0].Out());
				r+=op[3].Calc(op[2].Out());
				op[0].CalcFB(fb);
				break;
			case 5:
				r=op[2].Calc(op[0].Out());
				r+=op[1].Calc(op[0].Out());
				r+=op[3].Calc(op[0].Out());
				op[0].CalcFB(fb);
				break;
			case 6:
				r=op[2].Calc(0);
				r+=op[1].Calc(op[0].Out());
				r+=op[3].Calc(0);
				op[0].CalcFB(fb);
				break;
			case 7:
				r=op[2].Calc(0);
				r+=op[1].Calc(0);
				r+=op[3].Calc(0);
				r+=op[0].CalcFB(fb);
				break;
		}
		return r;
	}
	
	//  合成
	int CalcL() {
		chip_->SetPMV(pms[chip_->GetPML()]);
		
		int r=0;
		switch(algo_){
			case 0:
				op[2].CalcL(op[1].Out());
				op[1].CalcL(op[0].Out());
				r=op[3].CalcL(op[2].Out());
				op[0].CalcFBL(fb);
				break;
			case 1:
				op[2].CalcL(op[0].Out()+op[1].Out());
				op[1].CalcL(0);
				r=op[3].CalcL(op[2].Out());
				op[0].CalcFBL(fb);
				break;
			case 2:
				op[2].CalcL(op[1].Out());
				op[1].CalcL(0);
				r=op[3].CalcL(op[0].Out()+op[2].Out());
				op[0].CalcFBL(fb);
				break;
			case 3:
				op[2].CalcL(0);
				op[1].CalcL(op[0].Out());
				r=op[3].CalcL(op[1].Out()+op[2].Out());
				op[0].CalcFBL(fb);
				break;
			case 4:
				op[2].CalcL(0);
				r=op[1].CalcL(op[0].Out());
				r+=op[3].CalcL(op[2].Out());
				op[0].CalcFBL(fb);
				break;
			case 5:
				r=op[2].CalcL(op[0].Out());
				r+=op[1].CalcL(op[0].Out());
				r+=op[3].CalcL(op[0].Out());
				op[0].CalcFBL(fb);
				break;
			case 6:
				r=op[2].CalcL(0);
				r+=op[1].CalcL(op[0].Out());
				r+=op[3].CalcL(0);
				op[0].CalcFBL(fb);
				break;
			case 7:
				r=op[2].CalcL(0);
				r+=op[1].CalcL(0);
				r+=op[3].CalcL(0);
				r+=op[0].CalcFBL(fb);
				break;
		}
		return r;
	}
	
	//  合成
	int CalcN(int noise) {
		buf[1]=buf[2]=buf[3]=0;
		buf[0]=op[0].Out();op[0].CalcFB(fb);
		buf[ox[0]]+=op[1].Calc(buf[ix[0]]);
		buf[ox[1]]+=op[2].Calc(buf[ix[1]]);
		int o=op[3].Out();
		op[3].CalcN(noise);
		return buf[ox[2]]+o;
	}
	
	//  合成
	int CalcLN(int noise) {
		chip_->SetPMV(pms[chip_->GetPML()]);
		buf[1]=buf[2]=buf[3]=0;
		buf[0]=op[0].Out();op[0].CalcFBL(fb);
		buf[ox[0]]+=op[1].CalcL(buf[ix[0]]);
		buf[ox[1]]+=op[2].CalcL(buf[ix[1]]);
		int o=op[3].Out();
		op[3].CalcN(noise);
		return buf[ox[2]]+o;
	}
	
};

}
