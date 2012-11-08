#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace FlMML {

class MStatus {
public:
	static const int EOT=0;
	static const int NOP=1;
	static const int NOTE_ON=2;
	static const int NOTE_OFF=3;
	static const int TEMPO=4;
	static const int VOLUME=5;
	static const int NOTE=6;
	static const int FORM=7;
	static const int ENVELOPE1_ATK=8;
	static const int ENVELOPE1_ADD=9;
	static const int ENVELOPE1_REL=10;
	static const int NOISE_FREQ=11;
	static const int PWM=12;
	static const int PAN=13;
	static const int FORMANT=14;
	static const int DETUNE=15;
	static const int LFO_FMSF=16;
	static const int LFO_DPWD=17;
	static const int LFO_DLTM=18;
	static const int LFO_TARGET=19;
	static const int LPF_SWTAMT=20;
	static const int LPF_FRQRES=21;
	static const int CLOSE=22;
	static const int VOL_MODE=23;
	static const int ENVELOPE2_ATK=24;
	static const int ENVELOPE2_ADD=25;
	static const int ENVELOPE2_REL=26;
	static const int INPUT=27;
	static const int OUTPUT=28;
	static const int EXPRESSION=29;
	static const int RINGMODULATE=30;
	static const int SYNC=31;
	static const int PORTAMENTO=32;
	static const int MIDIPORT=33;
	static const int MIDIPORTRATE=34;
	static const int BASENOTE=35;
	static const int POLY=36;
	static const int SOUND_OFF=37;
	static const int RESET_ALL=38;
	static const int HW_LFO=39;
	static const int LOOP_END=40;
	static const int PLAY_END=41;
};

}
