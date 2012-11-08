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


//#include "JaggArray.h"
#include "FM.h"
#include "FlMML_Number.h"

namespace FlMML {

class Chip {
private:
	int ratio_;
public:
	int aml_;
	int pml_;
	int pmv_;
private:
	int multable_[4][16];
	
	static const Number dt2lv[];
public:
	Chip() {
		ratio_=0;
		aml_=0;
		pml_=0;
		pmv_=0;
		//multable_=JaggArray::I2(4,16);
		MakeTable();
	}
	
	void SetRatio(int ratio) {
		if (ratio_!=ratio){
			ratio_=ratio;
			MakeTable();
		}
	}
	void SetAML(int l) {
		aml_=l&(FM::FM_LFOENTS-1);
	}
	void SetPML(int l) {
		pml_=l&(FM::FM_LFOENTS-1);
	}
	void SetPMV(int pmv) {
		pmv_=pmv;
	}
	
	int GetMulValue(int dt2,int mul) {
		return multable_[dt2][mul];
	}
	int GetAML() {
		return aml_;
	}
	int GetPML() {
		return pml_;
	}
	int GetPMV() {
		return pmv_;
	}
	int GetRatio() {
		return ratio_;
	}
private:
	void MakeTable() {
		int h;int l;
		
		// PG Part
		for (h=0;h<4;h++){
			Number rr=dt2lv[h]*Number (ratio_)/(1<<(2+FM::FM_RATIOBITS-FM::FM_PGBITS));
			for (l=0;l<16;l++){
				int mul=(l!=0)?l*2:1;
				multable_[h][l]=(int )(mul*rr);
			}
		}
	}
};

}
