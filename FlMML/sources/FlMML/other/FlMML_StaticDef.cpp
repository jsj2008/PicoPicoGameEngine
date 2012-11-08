#include <vector>
#include "FlMML_StaticDef.h"
#include "MChannel.h"

#include "MML.h"
#include "MEnvelope.h"
#include "MOscillator.h"
#include "MOscSine.h"
#include "MOscSaw.h"
#include "MOscTriangle.h"
#include "MOscNoise.h"
#include "MOscFcTri.h"
#include "MOscFcNoise.h"
#include "MOscFcDpcm.h"
#include "MOscGbWave.h"
#include "MOscWave.h"
#include "MOscPicoWave.h"
#include "MOscGbLNoise.h"
#include "MOscGbSNoise.h"
#include "Math.h"
#include "MFormant.h"
#include "MWarning.h"

namespace FlMML {


bool MChannelData::s_init=0;
Number MChannelData::s_frequencyMap[FREQUENCY_LEN];
Number* MChannelData::s_volumeMap[3];


int MEnvelope::s_init=0;
Number MEnvelope::s_volumeMap[3][MEnvelope::s_volumeLen];


int MOscillator::s_init=0;


int MOscSine::s_init=0;
Number MOscSine::s_table[MOscSine::MAX_WAVE][TABLE_LEN];


int MOscSaw::s_init=0;
Number MOscSaw::s_table[MOscSaw::MAX_WAVE][TABLE_LEN];


int MOscTriangle::s_init=0;
Number MOscTriangle::s_table[MOscTriangle::MAX_WAVE][TABLE_LEN];


int MOscNoise::s_init=0;
Number MOscNoise::s_table[TABLE_LEN];


int MOscFcTri::s_init=0;
Number MOscFcTri::s_table[MOscFcTri::MAX_WAVE][FC_TRI_TABLE_LEN];


int MOscFcNoise::s_interval[] = {0x004,0x008,0x010,0x020,0x040,0x060,0x080,0x0a0,
	0x0ca,0x0fe,0x17c,0x1fc,0x2fa,0x3f8,0x7f2,0xfe4};


int MOscFcDpcm::s_interval[] = {428,380,340,320,286,254,226,214,190,160,142,128,106,85,72,54};


unsigned char MOscPicoWave::noise_lut[NOISE_LONG];
int MOscPicoWave::s_init=0;
//float MOscPicoWave::freqtable[11] = {
//	10,
//	23,
//	55,
//	110,
//	220,
//	440,
//	880,
//	1760,
//	3520,
//	7040,
//	14080,
//};

#include "OPM.h"

const int OPM::sltable[]={
	0,4,8,12,16,20,24,28,
	32,36,40,44,48,52,56,124,
};

const int OPM::slottable[]={
	0,2,1,3
};

bool OPM::s_init=false;

int OPM::amtable[4][OPM::OPM_LFOENTS];//=JaggArray::I2(4,OPM_LFOENTS);
int OPM::pmtable[4][OPM::OPM_LFOENTS];//=JaggArray::I2(4,OPM_LFOENTS);


#include "FM.h"

//int*** FM::pmtable=JaggArray::I3(2,8,FM::FM_LFOENTS);
//int*** FM::amtable=JaggArray::I3(2,8,FM::FM_LFOENTS);
int FM::pmtable[2][8][FM::FM_LFOENTS];
int FM::amtable[2][4][FM::FM_LFOENTS];
bool FM::tablemade=false;

#include "Chip.h"

const Number Chip::dt2lv[] = {1.0, 1.414, 1.581, 1.732};

#include "Channel4.h"

const int Channel4::fbtable[] = {31,7,6,5,4,3,2,1};

const int Channel4::kftable[]={
	65536,65595,65654,65713,65773,65832,65891,65951,
	66010,66070,66130,66189,66249,66309,66369,66429,
	66489,66549,66609,66669,66729,66789,66850,66910,
	66971,67031,67092,67152,67213,67273,67334,67395,
	67456,67517,67578,67639,67700,67761,67822,67883,
	67945,68006,68067,68129,68190,68252,68314,68375,
	68437,68499,68561,68623,68685,68747,68809,68871,
	68933,68995,69057,69120,69182,69245,69307,69370,
};

const int Channel4::kctable[]={
	5197,5506,5833,6180,6180,6547,6937,7349,
	7349,7786,8249,8740,8740,9259,9810,10394,
};

const int Channel4::iotable[8][6]={
	{0,1,1,2,2,3},{1,0,0,1,1,2},
	{1,1,1,0,0,2},{0,1,2,1,1,2},
	{0,1,2,2,2,1},{0,1,0,1,0,1},
	{0,1,2,1,2,1},{1,0,1,0,1,0},
};

#include "Operator.h"

const int Operator::notetable[]={
	0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  3,  3,  3,  3,  3,  3, 
	4,  4,  4,  4,  4,  4,  4,  5,  6,  7,  7,  7,  7,  7,  7,  7, 
	8,  8,  8,  8,  8,  8,  8,  9, 10, 11, 11, 11, 11, 11, 11, 11, 
	12, 12, 12, 12, 12, 12, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 
	16, 16, 16, 16, 16, 16, 16, 17, 18, 19, 19, 19, 19, 19, 19, 19, 
	20, 20, 20, 20, 20, 20, 20, 21, 22, 23, 23, 23, 23, 23, 23, 23, 
	24, 24, 24, 24, 24, 24, 24, 25, 26, 27, 27, 27, 27, 27, 27, 27, 
	28, 28, 28, 28, 28, 28, 28, 29, 30, 31, 31, 31, 31, 31, 31, 31, 
};

const int Operator::dttable[]={
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,
	4,  6,  6,  6,  8,  8,  8, 10, 10, 12, 12, 14, 16, 16, 16, 16,
	2,  2,  2,  2,  4,  4,  4,  4,  4,  6,  6,  6,  8,  8,  8, 10,
	10, 12, 12, 14, 16, 16, 18, 20, 22, 24, 26, 28, 32, 32, 32, 32,
	4,  4,  4,  4,  4,  6,  6,  6,  8,  8,  8, 10, 10, 12, 12, 14,
	16, 16, 18, 20, 22, 24, 26, 28, 32, 34, 38, 40, 44, 44, 44, 44,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -2, -2, -2, -2, -2, -2, -2, -2, -4, -4, -4, -4,
	-4, -6, -6, -6, -8, -8, -8,-10,-10,-12,-12,-14,-16,-16,-16,-16,
	-2, -2, -2, -2, -4, -4, -4, -4, -4, -6, -6, -6, -8, -8, -8,-10,
	-10,-12,-12,-14,-16,-16,-18,-20,-22,-24,-26,-28,-32,-32,-32,-32,
	-4, -4, -4, -4, -4, -6, -6, -6, -8, -8, -8,-10,-10,-12,-12,-14,
	-16,-16,-18,-20,-22,-24,-26,-28,-32,-34,-38,-40,-44,-44,-44,-44,
};

const int Operator::decaytable1[][8]={
	{ 0, 0, 0, 0, 0, 0, 0, 0},  { 0, 0, 0, 0, 0, 0, 0, 0},
	{ 1, 1, 1, 1, 1, 1, 1, 1},  { 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 1, 1, 1, 1, 1},  { 1, 1, 1, 1, 1, 1, 1, 1},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 0, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 0, 1, 0, 1, 0, 1, 0},  { 1, 1, 1, 0, 1, 0, 1, 0},
	{ 1, 1, 1, 0, 1, 1, 1, 0},  { 1, 1, 1, 1, 1, 1, 1, 0},
	{ 1, 1, 1, 1, 1, 1, 1, 1},  { 2, 1, 1, 1, 2, 1, 1, 1},
	{ 2, 1, 2, 1, 2, 1, 2, 1},  { 2, 2, 2, 1, 2, 2, 2, 1},
	{ 2, 2, 2, 2, 2, 2, 2, 2},  { 4, 2, 2, 2, 4, 2, 2, 2},
	{ 4, 2, 4, 2, 4, 2, 4, 2},  { 4, 4, 4, 2, 4, 4, 4, 2},
	{ 4, 4, 4, 4, 4, 4, 4, 4},  { 8, 4, 4, 4, 8, 4, 4, 4},
	{ 8, 4, 8, 4, 8, 4, 8, 4},  { 8, 8, 8, 4, 8, 8, 8, 4},
	{16,16,16,16,16,16,16,16},  {16,16,16,16,16,16,16,16},
	{16,16,16,16,16,16,16,16},  {16,16,16,16,16,16,16,16},
};

const int Operator::decaytable2[]={
	1,2,4,8,16,32,64,128,256,512,1024,2047,2047,2047,2047,2047
};

const int Operator::attacktable[][8]= {
	{-1,-1,-1,-1,-1,-1,-1,-1},  {-1,-1,-1,-1,-1,-1,-1,-1},
	{ 4, 4, 4, 4, 4, 4, 4, 4},  { 4, 4, 4, 4, 4, 4, 4, 4},
	{ 4, 4, 4, 4, 4, 4, 4, 4},  { 4, 4, 4, 4, 4, 4, 4, 4},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4,-1, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4,-1, 4,-1, 4,-1, 4,-1},  { 4, 4, 4,-1, 4,-1, 4,-1},
	{ 4, 4, 4,-1, 4, 4, 4,-1},  { 4, 4, 4, 4, 4, 4, 4,-1},
	{ 4, 4, 4, 4, 4, 4, 4, 4},  { 3, 4, 4, 4, 3, 4, 4, 4},
	{ 3, 4, 3, 4, 3, 4, 3, 4},  { 3, 3, 3, 4, 3, 3, 3, 4},
	{ 3, 3, 3, 3, 3, 3, 3, 3},  { 2, 3, 3, 3, 2, 3, 3, 3},
	{ 2, 3, 2, 3, 2, 3, 2, 3},  { 2, 2, 2, 3, 2, 2, 2, 3},
	{ 2, 2, 2, 2, 2, 2, 2, 2},  { 1, 2, 2, 2, 1, 2, 2, 2},
	{ 1, 2, 1, 2, 1, 2, 1, 2},  { 1, 1, 1, 2, 1, 1, 1, 2},
	{ 0, 0, 0, 0, 0, 0, 0, 0},  { 0, 0 ,0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0},  { 0, 0 ,0, 0, 0, 0, 0, 0},
};
const int Operator::ssgenvtable[][2][3][2]={
	{{{1,1},{1,1},{1,1}},
		{{0,1},{1,1},{1,1}}},
	{{{0,1},{2,0},{2,0}},
		{{0,1},{2,0},{2,0}}},
	{{{1,-1},{0,1},{1,-1}},
		{{0,1},{1,-1},{0,1}}},
	{{{1,-1},{0,0},{0,0}},
		{{0,1},{0,0},{0,0}}},
	{{{2,-1},{2,-1},{2,-1}},
		{{1,-1},{2,-1},{2,-1}}},
	{{{1,-1},{0,0},{0,0}},
		{{1,-1},{0,0},{0,0}}},
	{{{0,1},{1,-1},{0,1}},
		{{1,-1},{0,1},{1,-1}}},
	{{{0,1},{2,0},{2,0}},
		{{1,-1},{2,0},{2,0}}},
};

int Operator::sinetable[FM::FM_OPSINENTS];
int Operator::cltable[FM::FM_CLENTS];
bool Operator::tablehasmade = false;

#include "MOscOPM.h"

// YM2151 アプリケーションマニュアル Fig.2.4より
int MOscOPMData::kctable[] = {
	// C   C#  D   D#  E   F   F#  G   G#  A   A#  B  
	0xE,0x0,0x1,0x2,0x4,0x5,0x6,0x8,0x9,0xA,0xC,0xD, // 3.58MHz         
};

// スロットのアドレス
int MOscOPMData::slottable[] = {
	0, 2, 1, 3 
};

// キャリアとなるOP
int MOscOPMData::carrierop[] = {
	//   c2   m2   c1   m1
	0x40,                // AL 0
	0x40,                // AL 1
	0x40,                // AL 2
	0x40,                // AL 3
	0x40     |0x10,      // AL 4
	0x40|0x20|0x10,      // AL 5
	0x40|0x20|0x10,      // AL 6
	0x40|0x20|0x10|0x08, // AL 7
};     

int MOscOPMData::defTimbre[] = {
	/*  AL FB */
	4, 5,
	/*  AR DR SR RR SL TL KS ML D1 D2 AM　*/
	31, 5, 0, 0, 0,23, 1, 1, 3, 0, 0, 
	20,10, 3, 7, 8, 0, 1, 1, 3, 0, 0, 
	31, 3, 0, 0, 0,25, 1, 1, 7, 0, 0, 
	31,12, 3, 7,10, 2, 1, 1, 7, 0, 0, 
	//  OM,
	15,
	//  WF LFRQ PMD AMD
	0,  0,  0,  0, 
	//  PMS AMS
	0,  0,
	//  NE NFRQ
	0,  0,     
};

int MOscOPMData::zeroTimbre[] = {
	/*  AL FB */
	0, 0,
	/*  AR DR SR RR SL TL KS ML D1 D2 AM　*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//  OM,
	15,
	//  WF LFRQ PMD AMD
	0,  0,  0,  0, 
	//  PMS AMS
	0,  0,
	//  NE NFRQ
	0,  0,     
};

string MWarning::returnstring;
const string MWarning::s_string[]={
	"対応していないコマンド '%s' があります。",
	"終わりが見つからない繰り返しがあります。",
	"始まりが見つからないコメントがあります。",
	"終わりが見つからないコメントがあります。",
	"マクロが再帰的に呼び出されています。",
	"マクロ引数指定の \"\" が閉じられていません",
	"終りが見つからない連符があります",
	"始まりが見つからない連符があります",
};


int MOscGbLNoise::s_init = 0;
float MOscGbLNoise::s_table[MOscGbLNoise::GB_NOISE_TABLE_LEN];
int MOscGbLNoise::s_interval[]={
	0x000002, 0x000004, 0x000008, 0x00000c, 0x000010, 0x000014, 0x000018, 0x00001c,
	0x000020, 0x000028, 0x000030, 0x000038, 0x000040, 0x000050, 0x000060, 0x000070,
	0x000080, 0x0000a0, 0x0000c0, 0x0000e0, 0x000100, 0x000140, 0x000180, 0x0001c0,
	0x000200, 0x000280, 0x000300, 0x000380, 0x000400, 0x000500, 0x000600, 0x000700,
	0x000800, 0x000a00, 0x000c00, 0x000e00, 0x001000, 0x001400, 0x001800, 0x001c00,
	0x002000, 0x002800, 0x003000, 0x003800, 0x004000, 0x005000, 0x006000, 0x007000,
	0x008000, 0x00a000, 0x00c000, 0x00e000, 0x010000, 0x014000, 0x018000, 0x01c000,
	0x020000, 0x028000, 0x030000, 0x038000, 0x040000, 0x050000, 0x060000, 0x070000};


int MOscGbSNoise::s_init = 0;
float MOscGbSNoise::s_table[MOscGbSNoise::GB_NOISE_TABLE_LEN];
int MOscGbSNoise::s_interval[]={
	0x000002, 0x000004, 0x000008, 0x00000c, 0x000010, 0x000014, 0x000018, 0x00001c,
	0x000020, 0x000028, 0x000030, 0x000038, 0x000040, 0x000050, 0x000060, 0x000070,
	0x000080, 0x0000a0, 0x0000c0, 0x0000e0, 0x000100, 0x000140, 0x000180, 0x0001c0,
	0x000200, 0x000280, 0x000300, 0x000380, 0x000400, 0x000500, 0x000600, 0x000700,
	0x000800, 0x000a00, 0x000c00, 0x000e00, 0x001000, 0x001400, 0x001800, 0x001c00,
	0x002000, 0x002800, 0x003000, 0x003800, 0x004000, 0x005000, 0x006000, 0x007000,
	0x008000, 0x00a000, 0x00c000, 0x00e000, 0x010000, 0x014000, 0x018000, 0x01c000,
	0x020000, 0x028000, 0x030000, 0x038000, 0x040000, 0x050000, 0x060000, 0x070000};


const double Math::PI=M_PI;
const double Math::LOG2E=M_LOG2E;


const double MFormant::m_ca0=0.00000811044;
const double MFormant::m_ca1=8.943665402;
const double MFormant::m_ca2=-36.83889529;
const double MFormant::m_ca3=92.01697887;
const double MFormant::m_ca4=-154.337906;
const double MFormant::m_ca5=181.6233289;
const double MFormant::m_ca6=-151.8651235;
const double MFormant::m_ca7=89.09614114;
const double MFormant::m_ca8=-35.10298511;
const double MFormant::m_ca9=8.388101016;
const double MFormant::m_caA=-0.923313471;

const double MFormant::m_ce0=0.00000436215;
const double MFormant::m_ce1=8.90438318;
const double MFormant::m_ce2=-36.55179099;
const double MFormant::m_ce3=91.05750846;
const double MFormant::m_ce4=-152.422234;
const double MFormant::m_ce5=179.1170248;
const double MFormant::m_ce6=-149.6496211;
const double MFormant::m_ce7=87.78352223;
const double MFormant::m_ce8=-34.60687431;
const double MFormant::m_ce9=8.282228154;
const double MFormant::m_ceA=-0.914150747;

const double MFormant::m_ci0=0.00000333819;
const double MFormant::m_ci1=8.893102966;
const double MFormant::m_ci2=-36.49532826;
const double MFormant::m_ci3=90.96543286;
const double MFormant::m_ci4=-152.4545478;
const double MFormant::m_ci5=179.4835618;
const double MFormant::m_ci6=-150.315433;
const double MFormant::m_ci7=88.43409371;
const double MFormant::m_ci8=-34.98612086;
const double MFormant::m_ci9=8.407803364;
const double MFormant::m_ciA=-0.932568035;

const double MFormant::m_co0=0.00000113572;
const double MFormant::m_co1=8.994734087;
const double MFormant::m_co2=-37.2084849;
const double MFormant::m_co3=93.22900521;
const double MFormant::m_co4=-156.6929844;
const double MFormant::m_co5=184.596544;
const double MFormant::m_co6=-154.3755513;
const double MFormant::m_co7=90.49663749;
const double MFormant::m_co8=-35.58964535;
const double MFormant::m_co9=8.478996281;
const double MFormant::m_coA=-0.929252233;

const double MFormant::m_cu0=4.09431e-7;
const double MFormant::m_cu1=8.997322763;
const double MFormant::m_cu2=-37.20218544;
const double MFormant::m_cu3=93.11385476;
const double MFormant::m_cu4=-156.2530937;
const double MFormant::m_cu5=183.7080141;
const double MFormant::m_cu6=-153.2631681;
const double MFormant::m_cu7=89.59539726;
const double MFormant::m_cu8=-35.12454591;
const double MFormant::m_cu9=8.338655623;
const double MFormant::m_cuA=-0.910251753;

}
