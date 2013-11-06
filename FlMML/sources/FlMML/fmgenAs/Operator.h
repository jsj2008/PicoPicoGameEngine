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


#include "FM.h"
#include "EGPhase.h"

namespace FlMML {

class Operator {
public:
	Chip* chip_;
	int out_;int out2_;
	int in2_;
private:
	//  Phase Generator ------------------------------------------------------
	int dp_;// ΔP
	int detune_;// Detune
	int detune2_;// DT2
	int multiple_;// Multiple
public:
	int pg_count_;// Phase 現在値
	int pg_diff_;// Phase 差分値
	int pg_diff_lfo_;// Phase 差分値 >> x
	
	//  Envelop Generator ---------------------------------------------------
	int type_;/*OpType*/;           // OP の種類 (M, N...)
	int bn_;// Block/Note
	int eg_level_;// EG の出力値
	int eg_level_on_next_phase_;// 次の eg_phase_ に移る値
	int eg_count_;// EG の次の変移までの時間
	int eg_count_diff_;// eg_count_ の差分
	int eg_out_;// EG+TL を合わせた出力値
	int tl_out_;// TL 分の出力値
	int eg_rate_;
	int eg_curve_count_;
	int ssg_offset_;
	int ssg_vector_;
	int ssg_phase_;
	
	int key_scale_rate_;// key scale rate
	int eg_phase_;
	int* ams_;
	int ms_;
private:
	int tl_;// Total Level   (0-127)
	int tl_latch_;// Total Level Latch (for CSM mode)
	int ar_;// Attack Rate   (0-63)
	int dr_;// Decay Rate    (0-63)
	int sr_;// Sustain Rate  (0-63)
	int sl_;// Sustain Level (0-127)
	int rr_;// Release Rate  (0-63)
	int ks_;// Keyscale      (0-3)
	int ssg_type_;// SSG-Type Envelop Control
	
	bool keyon_;
public:
	bool amon_;// enable Amplitude Modulation
private:
	bool param_changed_;// パラメータが更新された
	bool mute_;
	
	static const int notetable[];
	static const int dttable[];
public:
	static const int decaytable1[][8];
private:
	static const int decaytable2[];
public:
	static const int attacktable[][8];
private:
	static const int ssgenvtable[][2][3][2];
public:
	static int sinetable[];
	static int cltable[];
private:
	static bool tablehasmade;
public:
	Operator() {
		chip_=NULL;

		MakeTable();
		
		// EG Part
		ar_=dr_=sr_=rr_=key_scale_rate_=0;
		ams_=FM::amtable[0][0];
		mute_=false;
		keyon_=false;
		tl_out_=0;
		ssg_type_=0;
		
		multiple_=0;
		detune_=0;
		detune2_=0;
		
		ms_=0;
	}
	
	void SetChip(Chip* chip) {
		chip_=chip;
	}
	
	void Reset() {
		// EG part
		tl_=tl_latch_=127;
		ShiftPhase(EGPhase::off);
		eg_count_=0;
		eg_curve_count_=0;
		ssg_phase_=0;
		// PG part
		pg_count_=0;
		// OP part
		out_=out2_=0;
		param_changed_=true;
	}
private:
	static void MakeTable() {
		if (tablehasmade) return;
		int i;
		int j;
		// 対数テーブルの作成
		for (i=0,j=0;i<256;i++){
			int v=(int )(Math::floor(Math::pow(2.0,13.0-i/256.0)));
			v=(v+2)&~3;
			cltable[j++]=v;
			cltable[j++]=-v;
		}
		i=j;
		while(j<FM::FM_CLENTS){
			cltable[j++]=cltable[i++-512]/2;
		}
		
		// サインテーブルの作成
		Number log2=Math::log(2.0);
		for (i=0;i<FM::FM_OPSINENTS/2;i++){
			Number r=(i*2+1)*Math::PI/FM::FM_OPSINENTS;
			Number q=-256*Math::log(Math::sin(r))/log2;
			int s=(int )(Math::floor(q+0.5))+1;
			sinetable[i]=s*2;
			sinetable[FM::FM_OPSINENTS/2+i]=s*2+1;
		}
		
		FM::MakeLFOTable();
		tablehasmade=true;
	}
public:
	void SetDPBN(int dp,int bn) {
		dp_=dp;
		bn_=bn;
		param_changed_=true;
	}
	
	//  準備
	void Prepare() {
		if (param_changed_==false){
			return ;
		}
		param_changed_=false;
		//  PG Part
		pg_diff_=((dp_+dttable[detune_+bn_])*chip_->GetMulValue(detune2_,multiple_));
		pg_diff_lfo_=pg_diff_>>11;
		
		// EG Part
		key_scale_rate_=bn_>>(3-ks_);
		tl_out_=mute_?0x3ff:tl_*8;
		
		switch(eg_phase_){
			case EGPhase::attack:
				SetEGRate(ar_!=0?Math::min(63,ar_+key_scale_rate_):0);
				break;
			case EGPhase::decay:
				SetEGRate(dr_!=0?Math::min(63,dr_+key_scale_rate_):0);
				eg_level_on_next_phase_=sl_*8;
				break;
			case EGPhase::sustain:
				SetEGRate(sr_!=0?Math::min(63,sr_+key_scale_rate_):0);
				break;
			case EGPhase::release:
				SetEGRate(Math::min(63,rr_+key_scale_rate_));
				break;
		}
		
		// SSG-EG
		if (ssg_type_!=0&&(eg_phase_!=EGPhase::release)){
			int m=(ar_>=((ssg_type_==8||ssg_type_==12)?56:60))?1:0;
			ssg_offset_=ssgenvtable[ssg_type_&7][m][ssg_phase_][0]*0x200;
			ssg_vector_=ssgenvtable[ssg_type_&7][m][ssg_phase_][1];
		}
		// LFO
		ams_=FM::amtable[(int )(type_)][amon_?(ms_>>4)&3:0];
		
		EGUpdate();
	}
public:
	//  envelop の eg_phase_ 変更
	void ShiftPhase(int nextphase) {
		switch(nextphase){
			case EGPhase::attack:// Attack Phase
				tl_=tl_latch_;
				if (ssg_type_!=0){
					ssg_phase_=ssg_phase_+1;
					if (ssg_phase_>2)
						ssg_phase_=1;
					
					int m=(ar_>=((ssg_type_==8||ssg_type_==12)?56:60))?1:0;
					
					ssg_offset_=ssgenvtable[ssg_type_&7][m][ssg_phase_][0]*0x200;
					ssg_vector_=ssgenvtable[ssg_type_&7][m][ssg_phase_][1];
				}
				if ((ar_+key_scale_rate_)<62){
					SetEGRate(ar_!=0?Math::min(63,ar_+key_scale_rate_):0);
					eg_phase_=EGPhase::attack;
					break;
				}
// C#           goto case EGPhase.decay;				
			case EGPhase::decay:// Decay Phase
				if (sl_!=0){
					eg_level_=0;
					eg_level_on_next_phase_=((ssg_type_!=0)?Math::min(sl_*8,0x200):sl_*8);
					
					SetEGRate(dr_!=0?Math::min(63,dr_+key_scale_rate_):0);
					eg_phase_=EGPhase::decay;
					break;
				}
// C#           goto case EGPhase.sustain;
			case EGPhase::sustain:// Sustain Phase
				eg_level_=sl_*8;
				eg_level_on_next_phase_=(ssg_type_!=0)?0x200:0x400;
				
				SetEGRate(sr_!=0?Math::min(63,sr_+key_scale_rate_):0);
				eg_phase_=EGPhase::sustain;
				break;
			case EGPhase::release:// Release Phase
				if (ssg_type_!=0){
					eg_level_=eg_level_*ssg_vector_+ssg_offset_;
					ssg_vector_=1;
					ssg_offset_=0;
				}
				if (eg_phase_==EGPhase::attack||(eg_level_<FM::FM_EG_BOTTOM)){
					eg_level_on_next_phase_=0x400;
					SetEGRate(Math::min(63,rr_+key_scale_rate_));
					eg_phase_=EGPhase::release;
					break;
				}
// C#           goto case EGPhase.off;				
			case EGPhase::off:// off
			default :
				eg_level_=FM::FM_EG_BOTTOM;
				eg_level_on_next_phase_=FM::FM_EG_BOTTOM;
				EGUpdate();
				SetEGRate(0);
				eg_phase_=EGPhase::off;
				break;
		}
	}
public:
	//  Block/F-Num
	void SetFNum(int f) {
		dp_=(f&2047)<<((f>>11)&7);
		bn_=notetable[(f>>7)&127];
		param_changed_=true;
	}
	
	//  １サンプル合成
public:
	//  ISample を envelop count (2π) に変換するシフト量  
	static const int IS2EC_SHIFT=((20+FM::FM_PGBITS)-13);
private:
	int SINE(int s) {
		return sinetable[(s)&(FM::FM_OPSINENTS-1)];
	}
	int LogToLin(int a) {
		return (a<FM::FM_CLENTS)?cltable[a]:0;
	}
	
	void EGUpdate() {
		int a;
		if (ssg_type_==0)a=tl_out_+eg_level_;
		else a=tl_out_+eg_level_*ssg_vector_+ssg_offset_;
		if (a<0x3ff)eg_out_=a<<(1+2);
		else eg_out_=0x3ff<<(1+2);
	}
	
	void SetEGRate(int rate) {
		eg_rate_=rate;
		eg_count_diff_=decaytable2[(rate/4)>>0]*chip_->GetRatio();
	}
	
	//  EG 計算
	void EGCalc() {
		eg_count_=(2047*3)<<FM::FM_RATIOBITS; // ##この手抜きは再現性を低下させる
		
		if (eg_phase_==EGPhase::attack){
			int c=attacktable[eg_rate_][eg_curve_count_&7];
			if (c>=0){
				eg_level_-=1+(eg_level_>>c);
				if (eg_level_<=0)
					ShiftPhase(EGPhase::decay);
			}
			EGUpdate();
		}
		else {
			if (ssg_type_==0){
				eg_level_+=decaytable1[eg_rate_][eg_curve_count_&7];
				if (eg_level_>=eg_level_on_next_phase_)
					ShiftPhase(eg_phase_+1);
				EGUpdate();
			}
			else {
				eg_level_+=4*decaytable1[eg_rate_][eg_curve_count_&7];
				if (eg_level_>=eg_level_on_next_phase_){
					EGUpdate();
					switch(eg_phase_){
						case EGPhase::decay:
							ShiftPhase(EGPhase::sustain);
							break;
						case EGPhase::sustain:
							ShiftPhase(EGPhase::attack);
							break;
						case EGPhase::release:
							ShiftPhase(EGPhase::off);
							break;
					}
				}
			}
		}
		eg_curve_count_++;
	}
	
	void EGStep() {
		eg_count_-=eg_count_diff_;
		
		// EG の変化は全スロットで同期しているという噂もある
		if (eg_count_<=0)
			EGCalc();
	}
	
	//  PG 計算
	//  ret:2^(20+PGBITS) / cycle
	int PGCalc() {
		int ret=pg_count_;
		pg_count_+=pg_diff_;
		return ret;
	}
	
	int PGCalcL() {
		int ret=pg_count_;
		pg_count_+=pg_diff_+((pg_diff_lfo_*chip_->GetPMV())>>5);
		return ret;
	}

public:
	//  OP 計算
	//  in: ISample (最大 8π)
	int Calc(int ii) {
		// *******************************
		// EGStep(); // ssg関連は削除した。
		eg_count_-=eg_count_diff_;
		if (eg_count_<=0){
			eg_count_=(2047*3)<<FM::FM_RATIOBITS;
			if (eg_phase_==EGPhase::attack){
				int c=attacktable[eg_rate_][eg_curve_count_&7];
				if (c>=0){
					eg_level_-=1+(eg_level_>>c);
					if (eg_level_<=0)ShiftPhase(EGPhase::decay);
				}
			}
			else {
				eg_level_+=decaytable1[eg_rate_][eg_curve_count_&7];
				if (eg_level_>=eg_level_on_next_phase_)ShiftPhase(eg_phase_+1);
			}
			int a=tl_out_+eg_level_;
			if (a<0x3ff)eg_out_=a<<(1+2);
			else eg_out_=0x3ff<<(1+2);
			eg_curve_count_++;
		}
		
		out2_=out_;
		
		//  PGCalc();;
		int pgo=pg_count_;
		pg_count_+=pg_diff_;
		int pgin=pgo>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS);
		pgin+=ii>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS-(2+IS2EC_SHIFT));
		
		//  LogToLin(); SINE();
		int sino=eg_out_+sinetable[pgin&(FM::FM_OPSINENTS-1)];
		if (sino<FM::FM_CLENTS)out_=cltable[sino];// 三項演算子は遅いという噂
		else out_=0;
		
		return out_;
	}
	
	int CalcL(int ii) {
		// EGStep(); // ssg関連は削除した。
		eg_count_-=eg_count_diff_;
		if (eg_count_<=0){
			eg_count_=(2047*3)<<FM::FM_RATIOBITS;
			if (eg_phase_==EGPhase::attack){
				int c=attacktable[eg_rate_][eg_curve_count_&7];
				if (c>=0){
					eg_level_-=1+(eg_level_>>c);
					if (eg_level_<=0)ShiftPhase(EGPhase::decay);
				}
			}
			else {
				eg_level_+=decaytable1[eg_rate_][eg_curve_count_&7];
				if (eg_level_>=eg_level_on_next_phase_)ShiftPhase(eg_phase_+1);
			}
			int a=tl_out_+eg_level_;
			if (a<0x3ff)eg_out_=a<<(1+2);
			else eg_out_=0x3ff<<(1+2);
			eg_curve_count_++;
		}
		
		//  PGCalcL(); 
		int pgo=pg_count_;
		pg_count_+=pg_diff_+((pg_diff_lfo_*chip_->pmv_)>>5);
		int pgin=pgo>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS);
		pgin+=ii>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS-(2+IS2EC_SHIFT));
		
		//  LogToLin(); SINE();
		int sino=eg_out_+sinetable[pgin&(FM::FM_OPSINENTS-1)]+ams_[chip_->aml_];
		if (sino<FM::FM_CLENTS)out_=cltable[sino];// 三項演算子は遅いという噂
		else out_=0;
		
		return out_;
	}
	
	int CalcN(int noise) {
		EGStep();
		
		int lv=Math::max(0,0x3ff-(tl_out_+eg_level_))<<1;
		
		// noise & 1 ? lv : -lv と等価
		noise=(noise&1)-1;
		out_=(lv+noise)^noise;
		
		return out_;
	}
	
	//  OP (FB) 計算
	//  Self Feedback の変調最大 = 4π
	int CalcFB(int fb) {
		// EGStep(); // ssg関連は削除した。
		eg_count_-=eg_count_diff_;
		if (eg_count_<=0){
			eg_count_=(2047*3)<<FM::FM_RATIOBITS;
			if (eg_phase_==EGPhase::attack){
				int c=attacktable[eg_rate_][eg_curve_count_&7];
				if (c>=0){
					eg_level_-=1+(eg_level_>>c);
					if (eg_level_<=0)ShiftPhase(EGPhase::decay);
				}
			}
			else {
				eg_level_+=decaytable1[eg_rate_][eg_curve_count_&7];
				if (eg_level_>=eg_level_on_next_phase_)ShiftPhase(eg_phase_+1);
			}
			int a=tl_out_+eg_level_;
			if (a<0x3ff)eg_out_=a<<(1+2);
			else eg_out_=0x3ff<<(1+2);
			eg_curve_count_++;
		}
		
		int ii=out_+out2_;
		out2_=out_;
		
		//  PGCalc() LogToLin() SINE() インライン展開
		int pgo=pg_count_;
		pg_count_+=pg_diff_;
		
		int pgin=pgo>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS);
		if (fb<31){
			pgin+=((ii<<(1+IS2EC_SHIFT))>>fb)>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS);
		}
		
		int sino=eg_out_+sinetable[pgin&(FM::FM_OPSINENTS-1)];
		if (sino<FM::FM_CLENTS)out_=cltable[sino];// 三項演算子は遅いという噂
		else out_=0;
		
		return out2_;
	}
	
	int CalcFBL(int fb) {
		// EGStep(); // ssg関連は削除した。
		eg_count_-=eg_count_diff_;
		if (eg_count_<=0){
			eg_count_=(2047*3)<<FM::FM_RATIOBITS;
			if (eg_phase_==EGPhase::attack){
				int c=attacktable[eg_rate_][eg_curve_count_&7];
				if (c>=0){
					eg_level_-=1+(eg_level_>>c);
					if (eg_level_<=0)ShiftPhase(EGPhase::decay);
				}
			}
			else {
				eg_level_+=decaytable1[eg_rate_][eg_curve_count_&7];
				if (eg_level_>=eg_level_on_next_phase_)ShiftPhase(eg_phase_+1);
			}
			int a=tl_out_+eg_level_;
			if (a<0x3ff)eg_out_=a<<(1+2);
			else eg_out_=0x3ff<<(1+2);
			eg_curve_count_++;
		}
		
		int ii=out_+out2_;
		out2_=out_;
		
		//  PGCalcL() LogToLin() SINE() インライン展開
		int pgo=pg_count_;
		pg_count_+=pg_diff_+((pg_diff_lfo_*chip_->pmv_)>>5);
		
		int pgin=pgo>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS);
		if (fb<31){
			pgin+=((ii<<(1+IS2EC_SHIFT))>>fb)>>(20+FM::FM_PGBITS-FM::FM_OPSINBITS);
		}
		
		int sino=eg_out_+sinetable[pgin&(FM::FM_OPSINENTS-1)]+ams_[chip_->aml_];
		if (sino<FM::FM_CLENTS)out_=cltable[sino];// 三項演算子は遅いという噂
		else out_=0;
		
		return out_;
	}
	
	//  フィードバックバッファをクリア
	void ResetFB() {
		out_=out2_=0;
	}
	
	//  キーオン
	void KeyOn() {
		if (!keyon_){
			keyon_=true;
			if (eg_phase_==EGPhase::off||eg_phase_==EGPhase::release){
				ssg_phase_=-1;
				ShiftPhase(EGPhase::attack);
				EGUpdate();
				in2_=out_=out2_=0;
				pg_count_=0;
			}
		}
	}
	
	//  キーオフ
	void KeyOff() {
		if (keyon_){
			keyon_=false;
			ShiftPhase(EGPhase::release);
		}
	}
	
	//  オペレータは稼働中か？
	bool IsOn() {
		return eg_phase_!=EGPhase::off;
	}
	
	//  Detune (0-7)
	void SetDT(int dt) {
		detune_=dt*0x20;
		param_changed_=true;
	}
	
	//  DT2 (0-3)
	void SetDT2(int dt2) {
		detune2_=dt2&3;
		param_changed_=true;
	}
	
	//  Multiple (0-15)
	void SetMULTI(int mul) {
		multiple_=mul;
		param_changed_=true;
	}
	
	//  Total Level (0-127) (0.75dB step)
	void SetTL(int tl,bool csm) {
		if (!csm){
			tl_=tl;param_changed_=true;
		}
		tl_latch_=tl;
	}
	
	//  Attack Rate (0-63)
	void SetAR(int ar) {
		ar_=ar;
		param_changed_=true;
	}
	
	//  Decay Rate (0-63)
	void SetDR(int dr) {
		dr_=dr;
		param_changed_=true;
	}
	
	//  Sustain Rate (0-63)
	void SetSR(int sr) {
		sr_=sr;
		param_changed_=true;
	}
	
	//  Sustain Level (0-127)
	void SetSL(int sl) {
		sl_=sl;
		param_changed_=true;
	}
	
	//  Release Rate (0-63)
	void SetRR(int rr) {
		rr_=rr;
		param_changed_=true;
	}
	
	//  Keyscale (0-3)
	void SetKS(int ks) {
		ks_=ks;
		param_changed_=true;
	}
	
	void SetAMON(bool amon) {
		amon_=amon;
		param_changed_=true;
	}
	
	void Mute(bool mute) {
		mute_=mute;
		param_changed_=true;
	}
	
	void SetMS(int ms) {
		ms_=ms;
		param_changed_=true;
	}
	
	int Out() {
		return out_;
	}
	
	void Refresh() {
		param_changed_=true;
	}
};

}
