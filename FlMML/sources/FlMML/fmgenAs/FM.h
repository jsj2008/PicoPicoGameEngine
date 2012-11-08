// ---------------------------------------------------------------------------
//  FM Sound Generator with OPN/OPM interface
//  Copyright (C) by cisc 1998, 2003.
//  FM Sound Generator C#
//  Copyright (C) 2011 ALOE. All rights reserved.
// ---------------------------------------------------------------------------
#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "FlMML_Math.h"
#include "FlMML_Number.h"

namespace FlMML {

class FM {
public:
	
	static const int FM_SINEPRESIS=2;			// EGとサイン波の精度の差  0(低)-2(高)
	static const int FM_OPSINBITS=10;
	static const int FM_OPSINENTS=(1<<FM_OPSINBITS);
	static const int FM_EGCBITS=18;				// eg の count のシフト値
	static const int FM_LFOCBITS=14;
	static const int FM_PGBITS=9;
	static const int FM_RATIOBITS=7;			// 8-12 くらいまで？
	static const int FM_EGBITS=16;
	static const int FM_VOLBITS=14;				// fmvolumeのシフト値
	static const int FM_VOLENTS=(1<<FM_VOLBITS);
	
	//  定数その１
	//  静的テーブルのサイズ
	static const int FM_LFOBITS=8;				// 変更不可
	static const int FM_TLBITS=7;
	static const int FM_TLENTS=(1<<FM_TLBITS);
	static const int FM_LFOENTS=(1<<FM_LFOBITS);
	
	//  サイン波の精度は 2^(1/256)
	static const int FM_CLENTS=(0x1000*2);
	static const int FM_EG_BOTTOM=955;
	
	static const int FM_TLPOS=(FM_TLENTS>>2);
	
	static int pmtable[2][8][FM_LFOENTS];
	static int amtable[2][4][FM_LFOENTS];
	
private:
	static bool tablemade;
public:
	static void MakeLFOTable() {
		int i;
		int j;
		
		if (tablemade)
			return ;
		
		Number pms[2][8]=
		{
			{0,1/360.0,2/360.0,3/360.0,4/360.0,6/360.0,12/360.0,24/360.0,},// OPNA
	//      [ 0, 1/240.0, 2/240.0, 4/240.0, 10/240.0, 20/240.0, 80/240.0, 140/240.0, ], // OPM
			{0,1/480.0,2/480.0,4/480.0,10/480.0,20/480.0,80/480.0,140/480.0,}, // OPM
	//      [ 0, 1/960.0, 2/960.0, 4/960.0, 10/960.0, 20/960.0, 80/960.0, 140/960.0, ], // OPM
		};
		
		//       3       6,      12      30       60       240      420     / 720
		//  1.000963
		//  lfofref[level * max * wave];
		//  pre = lfofref[level][pms * wave >> 8];
		int amt[2][4]=
		{
			{31,6,4,3},
			{31,2,1,0},
		};
		
		for (int type=0;type<2;type++){
			for (i=0;i<8;i++){
				Number pmb=pms[type][i];
				for (j=0;j<FM::FM_LFOENTS;j++){
					//Number v=Math::pow(2.0,pmb*(2*j-FM::FM_LFOENTS+1)/(FM::FM_LFOENTS-1));
					Number w=0.6*pmb*Math::sin(2*j*Math::PI/FM::FM_LFOENTS)+1;
					pmtable[type][i][j]=int (0x10000*(w-1));
				}
			}
			for (i=0;i<4;i++){
				for (j=0;j<FM::FM_LFOENTS;j++){
					amtable[type][i][j]=(((j*4)>>amt[type][i])*2)<<2;
				}
			}
		}
		
		tablemade=true;
	}
};

}
