#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <list>
#include <cstdio>
#include <locale>
#include <sstream>


using namespace std;

#include "MSequencer.h"
#include "FlMML_Array.h"
#include "FlMML_Number.h"
#include "FlMML_Regex.h"
#include "MWarning.h"
#include "FlMML_Math.h"
#include "MTrack.h"

namespace FlMML {

template<typename T, typename P> 
T remove_if2(T beg, T end, P pred) 
{ 
    T dest = beg; 
    for (T itr = beg;itr != end; ++itr) 
        if (!pred(*itr)) 
            *(dest++) = *itr; 
    return dest; 
} 

struct toUpper : public string {
    toUpper(const string& rstr) {
        {for (int iIndex = 0; iIndex < rstr.size(); iIndex++) {
            *this += toupper(rstr[iIndex], locale());
        }}
    }
};

struct toLower : public string {
    toLower(const string& rstr) {
        {for (int iIndex = 0; iIndex < rstr.size(); iIndex++) {
            *this += tolower(rstr[iIndex], locale());
        }}
    }
};

class ARGS {
public:
	string str;
	int index;
    friend bool operator <(const ARGS& a, const ARGS& b)
    {
		return a.str.length() < b.str.length();
	};
};

class MACRO {
public:
	string str;
	string code;
	vector<ARGS> args;
};

class MML {
protected:
public:
	MSequencer* m_sequencer;
	Array<MTrack> m_tracks;
	string m_string;
	int m_trackNo;
	int m_octave;
	bool m_relativeDir;
	int m_velocity;		// default velocity
	bool m_velDetail;
	bool m_velDir;
	int m_length;		// default length
	Number m_tempo;
	string::size_type m_letter;
	int m_keyoff;
	int m_gate;
	int m_maxGate;
	int m_form;
	int m_noteShift;
	string m_warning;
	int m_maxPipe;
	int m_maxSyncSource;
	int m_beforeNote;
	int m_portamento;
	bool m_usingPoly;
	int m_polyVoice;
	bool m_polyForce;
	bool m_infinityLoop;
	bool m_loaded;
	string m_metaTitle;
	string m_metaArtist;
	string m_metaCoding;
	string m_metaComment;
	MEngineData m_waveData;
	static const int MAX_PIPE=3;
	static const int MAX_SYNCSOURCE=3;
	static const int MAX_POLYVOICE=64;
public:
	MML() {
		m_sequencer=NULL;//new MSequencer();
		m_relativeDir = false;
		m_trackNo = 0;
		m_octave = 0;
		m_relativeDir = false;
		m_velocity = 0;
		m_velDetail = false;
		m_velDir = false;
		m_length = 0;
		m_tempo = 0.0f;
		m_letter = 0;
		m_keyoff = 0;
		m_gate = 0;
		m_maxGate = 0;
		m_form = 0;
		m_noteShift = 0;
		m_maxPipe = 0;
		m_maxSyncSource = 0;
		m_beforeNote = 0;
		m_portamento = 0;
		m_usingPoly = false;
		m_polyVoice = 0;
		m_polyForce = false;
		m_infinityLoop = false;
		m_loaded = false;

		//MChannel::boot(MSequencer::BUFFER_SIZE*32);
		MOscillator::boot();
		MEnvelope::boot();
	}
	virtual ~MML() {
		if (m_sequencer) m_sequencer->disconnectAll();
		m_tracks.clear();
		//if (m_tracks) delete m_tracks;
	}
	
	string getWarnings() {
		return m_warning;
	}
protected:
	void warning(int warnId,string str) {
		m_warning+=MWarning::getString(warnId,str)+"\n";
	}
	
	int len2tick(int len) {
		if (len==0)return m_length;
		return 384/len;
	}
	
	void note(int noteNo) {
		
		noteNo+=m_noteShift+getKeySig();
		if (getChar()=='*'){	// ポルタメント記号
			m_beforeNote=noteNo+m_octave*12;
			m_portamento=1;
			next();
		}
		else {
			int lenMode;
			int len;
			int tick=0;
			int tickTemp;
			int tie=0;
			int keyon=(m_keyoff==0)?0:1;
			m_keyoff=1;
			while(1){
				if (getChar()!='%'){
					lenMode=0;
				}
				else {
					lenMode=1;
					next();
				}
				len=getUInt(0);
				if (tie==1&&len==0){
					m_keyoff=0;
					break;
				}
				tickTemp=(lenMode?len:len2tick(len));
				tick+=getDot(tickTemp);
				tie=0;
				if (getChar()=='&'){
					tie=1;
					next();
				}
				else {
					break;
				}
			}
			if (m_portamento==1){ // ポルタメントなら
				m_tracks[m_trackNo]->recPortamento(m_beforeNote-(noteNo+m_octave*12),tick);
			}
//total += tick;
			m_tracks[m_trackNo]->recNote(noteNo+m_octave*12,tick,m_velocity,keyon,m_keyoff);
			if (m_portamento==1){ // ポルタメントなら
				m_tracks[m_trackNo]->recPortamento(0,0);
				m_portamento=0;
			}
		}
	}
	
	void rest() {
		
		int lenMode=0;
		if (getChar()=='%'){
			lenMode=1;
			next();
		}
		int len;
		len=getUInt(0);
		int tick=lenMode?len:len2tick(len);
		tick=getDot(tick);
//total += tick;
		m_tracks[m_trackNo]->recRest(tick);
	}
	
	void atmark() {
		char c=getChar();
		int o=1;int a=0;int d=64;int s=32;int r=0;int sens=0;int mode=0;
		int w=0;int f=0;
		int pmd;int amd;int pms;int ams;
		switch(c){
			case 'v': // Volume
				m_velDetail=true;
				next();
				m_velocity=getUInt(m_velocity);
				if (m_velocity>127)m_velocity=127;
				break;
			case 'x': // Expression
				next();
				o=getUInt(127);
				if (o>127)o=127;
				m_tracks[m_trackNo]->recExpression(o);
				break;
			case 'e': // Envelope
			{
				string::size_type releasePos;
				Vector<int> t;
				Vector<int> l;
				next();
				o=getUInt(o);
				if (getChar()==',')next();
				a=getUInt(a);
				releasePos=m_letter;
				while(true){
					if (getChar()==','){
						next();
					}else {
						break;
					}
					releasePos=m_letter-1;
					d=getUInt(d);
					if (getChar()==','){
						next();
					}else {
						m_letter=releasePos;
						break;
					}
					s=getUInt(s);
					t.push(d);
					l.push(s);
				}
				if (t.size()==0){
					t.push(d);
					l.push(s);
				}
				if (getChar()==',')next();
				r=getUInt(r);
				
				m_tracks[m_trackNo]->recEnvelope(o,a,&t,&l,r);
			}
				break;
			case 'm':
				next();
				if (getChar()=='h'){
					next();
					w=0;f=0;pmd=0;amd=0;pms=0;ams=0;s=1;
					do{
						w=getUInt(w);
						if (getChar()!=',')break;
						next();
						f=getUInt(f);
						if (getChar()!=',')break;
						next();
						pmd=getUInt(pmd);
						if (getChar()!=',')break;
						next();
						amd=getUInt(amd);
						if (getChar()!=',')break;
						next();
						pms=getUInt(pms);
						if (getChar()!=',')break;
						next();
						ams=getUInt(ams);
						if (getChar()!=',')break;
						next();
						s=getUInt(s);
					}
					while(false);
					m_tracks[m_trackNo]->recHwLfo(w,f,pmd,amd,pms,ams,s);
				}
				break;
			case 'n': // Noise frequency
				next();
				if (getChar()=='s'){
					next();
					m_noteShift+=getSInt(0);
				}
				else {
					o=getUInt(0);
					if (o<0||o>127)o=0;
					m_tracks[m_trackNo]->recNoiseFreq(o);
				}
				break;
			case 'w': // pulse Width modulation
				next();
				o=getSInt(50);
				if (o<0){
					if (o>-1)o=-1;
					if (o<-99)o=-99;
				}
				else {
					if (o<1)o=1;
					if (o>99)o=99;
				}
				m_tracks[m_trackNo]->recPWM(o);
				break;
			case 'p': // Pan
				next();
				if (getChar()=='l'){
					next();
					o=getUInt(m_polyVoice);
					o=Math::max(0,Math::min(m_polyVoice,o));
					m_tracks[m_trackNo]->recPoly(o);
				}
				else {
					o=getUInt(64);
					if (o<1)o=1;
					if (o>127)o=127;
					m_tracks[m_trackNo]->recPan(o);
				}
				break;
			case '\'': // formant filter
                next();
				{
					string::size_type o = m_string.find_first_of('\'',m_letter);
					if (o!=string::npos){
						string vstr=m_string.substr(m_letter,o-m_letter);
						int vowel=0;
						switch(vstr.c_str()[0]){
							case 'a':vowel=MFormant::VOWEL_A;break;
							case 'e':vowel=MFormant::VOWEL_E;break;
							case 'i':vowel=MFormant::VOWEL_I;break;
							case 'o':vowel=MFormant::VOWEL_O;break;
							case 'u':vowel=MFormant::VOWEL_U;break;
							default :vowel=-1;break;
						}
						m_tracks[m_trackNo]->recFormant(vowel);
						m_letter=o+1;
					}
				}
				break;
			case 'd': // Detune
				next();
				o=getSInt(0);
				m_tracks[m_trackNo]->recDetune(o);
				break;
			case 'l':// Low frequency oscillator (LFO)
			{
				int dp=0;int wd=0;int fm=1;int sf=0;int rv=1;int dl=0;int tm=0;int sw=0;
				next();
				dp=getUInt(dp);
				if (getChar()==',')next();
				wd=getUInt(wd);
				if (getChar()==','){
					next();
					if (getChar()=='-'){rv=-1;next();}
					fm=(getUInt(fm)+1)*rv;
					if (getChar()=='-'){
						next();
						sf=getUInt(0);
					}
					if (getChar()==','){
						next();
						dl=getUInt(dl);
						if (getChar()==','){
							next();
							tm=getUInt(tm);
							if (getChar()==','){
								next();
								sw=getUInt(sw);
							}
						}
					}
				}
				
				m_tracks[m_trackNo]->recLFO(dp,wd,fm,sf,dl,tm,sw);
			}
				break;
			case 'f': // Filter
			{
				int swt=0;int amt=0;int frq=0;int res=0;
				next();
				swt=getSInt(swt);
				if (getChar()==','){
					next();
					amt=getSInt(amt);
					if (getChar()==','){
						next();
						frq=getUInt(frq);
						if (getChar()==','){
							next();
							res=getUInt(res);
						}
					}
				}
				m_tracks[m_trackNo]->recLPF(swt,amt,frq,res);
			}
				break;
			case 'q': // gate time 2
				next();
				m_tracks[m_trackNo]->recGate2(getUInt(2)*2);
				break;
			case 'i': // Input
			{
				sens=0;
				next();
				sens=getUInt(sens);
				if (getChar()==','){
					next();
					a=getUInt(a);
					if (a>m_maxPipe)a=m_maxPipe;
				}
				m_tracks[m_trackNo]->recInput(sens,a);
			}
				// @i[n],[m]   m:pipe no
				// if (n == 0) off
				// else sensitivity = n (max:8)
				break;
			case 'o': // Output
			{
				mode=0;
				next();
				mode=getUInt(mode);
				if (getChar()==','){
					next();
					a=getUInt(a);
					if (a>m_maxPipe){
						m_maxPipe=a;
						if (m_maxPipe>=MAX_PIPE)m_maxPipe=a=MAX_PIPE;
					}
				}
				m_tracks[m_trackNo]->recOutput(mode,a);
			}
				// @o[n],[m]   m:pipe no
				// if (n == 0) off
				// if (n == 1) overwrite
				// if (n == 2) add
				break;
			case 'r': // Ring
			{
				sens=0;
				next();
				sens=getUInt(sens);
				if (getChar()==','){
					next();
					a=getUInt(a);
					if (a>m_maxPipe)a=m_maxPipe;
				}
				m_tracks[m_trackNo]->recRing(sens,a);
			}
				break;
			case 's': // Sync
			{
				mode=0;
				next();
				mode=getUInt(mode);
				if (getChar()==','){
					next();
					a=getUInt(a);
					if (mode==1){
						// Sync out
						if (a>m_maxSyncSource){
							m_maxSyncSource=a;
							if (m_maxSyncSource>=MAX_SYNCSOURCE)m_maxSyncSource=a=MAX_SYNCSOURCE;
						}
					}else if (mode==2){
						// Sync in
						if (a>m_maxSyncSource)a=m_maxSyncSource;
					}
				}
				m_tracks[m_trackNo]->recSync(mode,a);
			}
				break;
			case 'u':	// midi風なポルタメント
				next();
				int rate;
				mode=getUInt(0);
				switch(mode){
					case 0:
					case 1:
						m_tracks[m_trackNo]->recMidiPort(mode);
						break;
					case 2:
						rate=0;
						if (getChar()==','){
							next();
							rate=getUInt(0);
							if (rate<0)rate=0;
							if (rate>127)rate=127;
						}
						m_tracks[m_trackNo]->recMidiPortRate(rate*1);
						break;
					case 3:
						if (getChar()==','){
							next();
							int oct;
							int baseNote=-1;
							if (getChar()!='o'){
								oct=m_octave;
							}
							else {
								next();
								oct=getUInt(0);
							}
							c=getChar();
							switch(c){
								case 'c':baseNote=0;break;
								case 'd':baseNote=2;break;
								case 'e':baseNote=4;break;
								case 'f':baseNote=5;break;
								case 'g':baseNote=7;break;
								case 'a':baseNote=9;break;
								case 'b':baseNote=11;break;
							}
							if (baseNote>=0){
								next();
								baseNote+=m_noteShift+getKeySig();
								baseNote+=oct*12;
							}
							else {
								baseNote=getUInt(60);
							}
							if (baseNote<0)baseNote=0;
							if (baseNote>127)baseNote=127;
							m_tracks[m_trackNo]->recPortBase(baseNote);
						}
						break;
				}
				break;
			default :
				m_form=getUInt(m_form);
				a=0;
				if (getChar()=='-'){
					next();
					a=getUInt(0);
				}
				m_tracks[m_trackNo]->recForm(m_form,a);
				break;
		}
	}
	
	void firstLetter() {
		char c=getCharNext();
		char c0;
		//int i;
		switch(c){
			case 'c':note(0);break;
			case 'd':note(2);break;
			case 'e':note(4);break;
			case 'f':note(5);break;
			case 'g':note(7);break;
			case 'a':note(9);break;
			case 'b':note(11);break;
			case 'r':rest();break;
			case 'o': // Octave
				m_octave=getUInt(m_octave);
				if (m_octave<-2)m_octave=-2;
				if (m_octave>8)m_octave=8;
				break;
			case 'v': // Volume
				m_velDetail=false;
				m_velocity=getUInt((m_velocity-7)/8)*8+7;
				if (m_velocity<0)m_velocity=0;
				if (m_velocity>127)m_velocity=127;
				break;
			case '(': // vol up/down
			case ')':
				{
					int i = getUInt(1);
					if ((c=='('&&m_velDir)||
						(c==')'&&!m_velDir)){
						m_velocity+=(m_velDetail)?(1*i):(8*i);
						if (m_velocity>127)m_velocity=127;
					}
					else { // down
						m_velocity-=(m_velDetail)?(1*i):(8*i);
						if (m_velocity<0)m_velocity=0;
					}
				}
				break;
			case 'l': // Length
				m_length=len2tick(getUInt(0));
				m_length=getDot(m_length);
				break;
			case 't': // Tempo
				m_tempo=getUNumber(m_tempo);
				if (m_tempo<1)m_tempo=1;
				m_tracks[MTrack::TEMPO_TRACK]->recTempo(m_tracks[m_trackNo]->getRecGlobalTick(),m_tempo);
				break;
			case 'q': // gate time (rate)
				m_gate=getUInt(m_gate);
				m_tracks[m_trackNo]->recGate(Number (m_gate)/Number (m_maxGate));
				break;
			case '<': // octave shift
				if (m_relativeDir)m_octave++;else m_octave--;
				break;
			case '>': // octave shift
				if (m_relativeDir)m_octave--;else m_octave++;
				break;
			case ';': // end of track
				m_keyoff=1;
				if (m_tracks[m_trackNo]->getNumEvents()>0){
					m_trackNo++;
					m_tracks.assign(m_trackNo,createTrack());
				}
				break;
			case '@':
				atmark();
				break;
			case 'x':
				m_tracks[m_trackNo]->recVolMode(getUInt(1));
				break;
			case 'n':
				c0=getChar();
				if (c0=='s'){ // Note Shift (absolute)
					next();
					m_noteShift=getSInt(m_noteShift);
				}
				else {
					char s[3]={0};
					s[0] = c;
					s[1] = c0;
					warning(MWarning::UNKNOWN_COMMAND,s);
				}
				break;
			case '[':
				m_tracks[m_trackNo]->recChordStart();
				break;
			case ']':
				m_tracks[m_trackNo]->recChordEnd();
				break;
			case '!':
				m_tracks[m_trackNo]->recLoopEnd();
				break;
			default :
			{
				int cc=c;//.at(0);
				if (cc<128) {
					char s[2]={0};
					s[0] = c;
					warning(MWarning::UNKNOWN_COMMAND,s);
				}
			}
				break;
		}
	}
	
	char getCharNext() {
		return (m_letter<m_string.length())?m_string.at(m_letter++):0;
	}
	
	char getChar() {
		return (m_letter<m_string.length())?m_string.at(m_letter):0;
	}
	
	void next(int i=1) {
		m_letter+=1;
	}
	
	int getKeySig() {
		int k=0;
		int f=1;
		while(f){
			char c=getChar();
			switch(c){
				case '+':case '#':k++;next();break;
				case '-':k--;next();break;
				default :f=0;break;
			}
		}
		return k;
	}
	
	int getUInt(int def) {
		int ret=0;
		string::size_type l=m_letter;
		int f=1;
		while(f){
			char c=getChar();
			switch(c){
				case '0':ret=ret*10+0;next();break;
				case '1':ret=ret*10+1;next();break;
				case '2':ret=ret*10+2;next();break;
				case '3':ret=ret*10+3;next();break;
				case '4':ret=ret*10+4;next();break;
				case '5':ret=ret*10+5;next();break;
				case '6':ret=ret*10+6;next();break;
				case '7':ret=ret*10+7;next();break;
				case '8':ret=ret*10+8;next();break;
				case '9':ret=ret*10+9;next();break;
				default :f=0;break;
			}
		}
		return (m_letter==l)?def:ret;
	}
	
	Number getUNumber(Number def) {
		Number ret=getUInt(int (def));
		Number l=1;
		if (getChar()=='.'){
			next();
			bool f=true;
			while(f){
				char c=getChar();
				l*=0.1;
				switch(c){
					case '0':ret=ret+0*l;next();break;
					case '1':ret=ret+1*l;next();break;
					case '2':ret=ret+2*l;next();break;
					case '3':ret=ret+3*l;next();break;
					case '4':ret=ret+4*l;next();break;
					case '5':ret=ret+5*l;next();break;
					case '6':ret=ret+6*l;next();break;
					case '7':ret=ret+7*l;next();break;
					case '8':ret=ret+8*l;next();break;
					case '9':ret=ret+9*l;next();break;
					default :f=false;break;
				}
			}
		}
		return ret;
	}
	
	int getSInt(int def) {
		char c=getChar();
		int s=1;
		if (c=='-'){s=-1;next();}
		else if (c=='+')next();
		return getUInt(def)*s;
	}
	
	int getDot(int tick) {
		char c=getChar();
		int intick=tick;
		while(c=='.'){
			next();
			intick/=2;
			tick+=intick;
			c=getChar();
		}
		return tick;
	}
public:
	MTrack* createTrack() {
		m_octave=4;
		m_velocity=100;
		m_noteShift=0;
		return new MTrack(m_trackNo,&m_waveData);
	}
protected:
	void begin() {
		m_letter=0;
	}
	
	void process() {
		begin();
		while(m_letter<m_string.length()){
			firstLetter();
		}
	}
	
public:
	void processRepeat() {
		m_string = toLower(m_string);
		begin();
		Vector<int> repeat;
		Vector<string::size_type> origin;
		Vector<string::size_type> start;
		Vector<string::size_type> last;
		repeat.reserve(100);
		origin.reserve(100);
		start.reserve(100);
		last.reserve(100);
		int nest=-1;
		while(m_letter<m_string.length()){
			char c=getCharNext();
			switch(c){
				case '/':
					if (getChar()==':'){
						next();
						++nest;
						origin[nest]=m_letter-2;
						repeat[nest]=getSInt(2);
						if (m_infinityLoop == true && repeat[nest] < 0) repeat[nest] = 9999;
						else
						if (repeat[nest] < 0) repeat[nest] = 2;
						else
						if (repeat[nest] > 1024) repeat[nest] = 1024;
						start[nest]=m_letter;
						last[nest]=string::npos;
					}
					else if (nest>=0){
						if (last.size() <= nest) last.reserve(nest+10);
						last[nest]=m_letter-1;
						m_string=m_string.substr(0,m_letter-1)+m_string.substr(m_letter);
						m_letter--;
					}
					else {
					}
					break;
				case ':':
					if (getChar()=='/'&&nest>=0){
						bool loopEnd = false;
						next();
						if (getChar()==';' || getChar()==0/* || getChar()==':'*/) {
							loopEnd = true;
						}
						string contents=m_string.substr(start[nest],m_letter-2-start[nest]);
						string newstr=m_string.substr(0,origin[nest]);
						if (repeat[nest]==9999) {
							newstr+="!";
							for (int i=0;i<1;i++){
								if (i<repeat[nest]-1 || last[nest]==string::npos) {
									newstr+=contents;
								} else {
									newstr+=m_string.substr(start[nest],last[nest]-start[nest]);
								}
								if (loopEnd && m_infinityLoop == false) {
									newstr+="!";
								}
							}
						} else {
							for (int i=0;i<repeat[nest];i++){
								if (i<repeat[nest]-1||last[nest] == string::npos) {
									newstr+=contents;
								} else {
									newstr+=m_string.substr(start[nest],last[nest]-start[nest]);
								}
								if (loopEnd && m_infinityLoop == false) {
									newstr+="!";
								}
							}
						}
						string::size_type l= newstr.length();
						newstr+=m_string.substr(m_letter);
						m_string=newstr;
						m_letter=l;
						nest--;
					}
					break;
				default :
					break;
			}
		}
		if (nest>=0)warning(MWarning::UNCLOSED_REPEAT,"");
	}

protected:
	bool replaceMacro(vector<MACRO>* macroTable) {
		vector<MACRO>::iterator itr = macroTable->begin();
		while (itr != macroTable->end()){
			MACRO* macro = &itr[0];
			if (m_string.substr(m_letter,macro->str.length())==macro->str){
				string::size_type start=m_letter;string::size_type last=m_letter+macro->str.length();string code=macro->code;
				m_letter+=macro->str.length();
				char c=getCharNext();
				while(isspace(c)/*||c=='　'*/){
					c=getCharNext();
				}
				//if (macro->args.size() > 0) 
				{
					// 引数
					vector<string> args;
					int q=0;
					if (c=='{' && macro->args.size() > 0){
						c=getCharNext();
						// 引数の中にマクロがあるか?
						while(q==1||(c!='}'&&c!=0)){
							if (c=='"')q=1-q;
							if (c=='$'){
								replaceMacro(macroTable);
							}
							c=getCharNext();
						}
						// 引数の取り出し
						last=m_letter;
						string argstr=m_string.substr(start+macro->str.length()+1,last-1-(start+macro->str.length()+1));
						//printf("==\n%s\n",argstr.c_str());
						string curarg="";bool quoted=false;
						for (int pos=0;pos<argstr.length();pos++){
							if (!quoted&&argstr.at(pos)=='"'){
								quoted=true;
							}else if (quoted&&(pos+1)<argstr.length()&&argstr.at(pos)=='\\'&&argstr.at(pos+1)=='"'){
								curarg+='"';
								pos++;
							}else if (quoted&&argstr.at(pos)=='"'){
								quoted=false;
							}else if (!quoted&&argstr.at(pos)==','){
								args.push_back(curarg);
								curarg="";
							}else {
								curarg+=argstr.at(pos);
							}
						}
						args.push_back(curarg);
						if (quoted){
							warning(MWarning::UNCLOSED_ARGQUOTE,"");
						}
					}
					
					// 引数への置換0
					int argsSize = (int)args.size();
					for(int j=argsSize-1;j>=0;j--){
						for(int i=0;i<code.length();i++)
							//				for(int i=0;i<code.length();i++)
							//					for(int j=0;j<args.size();j++){
						{
							if(j >= macro->args.size()){
								break;
							}
							if (macro->args[j].str.length() > 0) 
							{
								if(code.substr(i, macro->args[j].str.length() + 1) == ("%" + macro->args[j].str)){
									string rs = "%" + macro->args[j].str;
									string substr = code.substr(i);
									regex exp(rs);
									code = code.substr(0, i) + regex_replace(substr,exp,args[macro->args[j].index],regex_constants::match_not_dot_newline);
									i += args[macro->args[j].index].length() - 1;
									break;
								}
							}
						}
					}
				}
				m_string=m_string.substr(0,start-1)+code+m_string.substr(last);
				m_letter=start-1;
				
				return true;
			}
			itr++;
		}
		return false;
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

public:	
	void processMacro() {
		// INFINITY LOOP
		{
			regex exp("^#INFINITY[[:space:]]+LOOP[[:space:]]*$");
			if (regex_search(m_string,exp)){
				m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
				m_infinityLoop=true;
			}
		}
	
		// OCTAVE REVERSE
		{
			regex exp("^#OCTAVE[[:space:]]+REVERSE[[:space:]]*$");
			if (regex_search(m_string,exp)){
				m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
				m_relativeDir=false;
			}
		}
		
		// VELOCITY REVERSE
		{
			regex exp("^#VELOCITY[[:space:]]+REVERSE[[:space:]]*$");
			if (regex_search(m_string,exp)){
				m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
				m_velDir=false;
			}
		}
		
		// meta informations
		{
			m_metaTitle=findMetaDescN("TITLE");
			m_metaArtist=findMetaDescN("ARTIST");
			m_metaComment=findMetaDescN("COMMENT");
			m_metaCoding=findMetaDescN("CODING");
			findMetaDescN("PRAGMA");
		}

		// FM Desc
		{
			{
				regex exp("^#OPM@([[:digit:]]+)[ \\t]*\\{([^\\}]*)\\}");
				{
					string::const_iterator start=m_string.begin();
					string::const_iterator end=m_string.end();
					smatch result;
					while (regex_search(start, end, result, exp,regex_constants::match_not_dot_newline)) {
						int f1 = atoi(result.str(1).c_str());
						m_waveData.opmData.setTimber(f1,MOscOPM::TYPE_OPM,result.str(2));
						start = result[0].second;
					}
				}
				
				m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
			}
			{
				regex exp("^#OPN@([[:digit:]]+)[ \\t]*\\{([^\\}]*)\\}");
				{
					string::const_iterator start=m_string.begin();
					string::const_iterator end=m_string.end();
					smatch result;
					while (regex_search(start, end, result, exp,regex_constants::match_not_dot_newline)) {
						int f1 = atoi(result.str(1).c_str());
						m_waveData.opmData.setTimber(f1,MOscOPM::TYPE_OPN,result.str(2));
						start = result[0].second;
					}
				}
				
				m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
			}
			{
				Array<string>* fmg=findMetaDescV("FMGAIN");
				for (int i=0;i<fmg->size();i++){
					int v = atoi((*fmg)[i]->c_str());
					m_waveData.opmData.setCommonGain(20.0*v/127.0);
				}
				delete fmg;
			}
		}
		
		// POLY MODE
		{
			string usePoly=findMetaDescN("USING[[:space:]]+POLY");
			usePoly = removeWhitespace(usePoly);
			usePoly = toLower(usePoly);
			if (usePoly.length()>0){
				string::const_iterator start=usePoly.begin();
				string::const_iterator end=usePoly.end();
				m_usingPoly = false;
				m_polyForce = false;
				if (!m_usingPoly) {
					regex exp("^([[:digit:]]+)[[:space:]]*force.*$");
					smatch result;
					if (regex_search(start,end,result,exp)) {
						m_polyVoice=Math::min(Math::max(1,atoi(result.str(0).c_str())),MAX_POLYVOICE); // 1～MAX_POLYVOICE
						m_usingPoly = true;
						m_polyForce = true;
					}
				}
				if (!m_usingPoly) {
					regex exp("^([[:digit:]]+).*$");
					smatch result;
					if (regex_search(start,end,result,exp)) {
						m_polyVoice=Math::min(Math::max(1,atoi(result.str(0).c_str())),MAX_POLYVOICE); // 1～MAX_POLYVOICE
						m_usingPoly = true;
					}
				}
			}
		}

		// GB WAVE (ex. "#WAV10 0,0123456789abcdeffedcba9876543210")
		{
			m_waveData.gbWaveData.clear();
			regex exp("^#WAV10[[:space:]].*$");
			string::const_iterator start=m_string.begin();
			string::const_iterator end=m_string.end();
			smatch result;
			while (regex_search(start, end, result, exp,regex_constants::match_not_dot_newline)) {
				list<string> wav=split(result.str(0)," ");
				string wavs;
				{
					list<string>::iterator iter = wav.begin();
					if (iter != wav.end()) {
						iter++;
						while (iter != wav.end()) {
							wavs+=*iter;
							iter++;
						}
					}
				}
				list<string> arg=split(wavs,",");
				{
					list<string>::iterator iter = arg.begin();
					if (iter != arg.end()) {
						int waveNo = atoi(iter->c_str());
						iter++;
						if (iter != arg.end()) {
							if (waveNo<0)waveNo=0;
							if (waveNo>=MOscGbWaveData::MAX_WAVE)waveNo=MOscGbWaveData::MAX_WAVE-1;
							m_waveData.gbWaveData.setWave(waveNo,
												(toLower(*iter)+"00000000000000000000000000000000").substr(0,32));
						}
					}
				}
				start = result[0].second;
			}
			m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
		}
		{
			m_waveData.waveData.clear();
			regex exp("^#WAV13[[:space:]].*$");
			string::const_iterator start=m_string.begin();
			string::const_iterator end=m_string.end();
			smatch result;
			while (regex_search(start, end, result, exp,regex_constants::match_not_dot_newline)) {
				list<string> wav=split(result.str(0)," ");
				string wavs;
				{
					list<string>::iterator iter = wav.begin();
					if (iter != wav.end()) {
						iter++;
						while (iter != wav.end()) {
							wavs+=*iter;
							iter++;
						}
					}
				}
				list<string> arg=split(wavs,",");
				{
					list<string>::iterator iter = arg.begin();
					if (iter != arg.end()) {
						int waveNo = atoi(iter->c_str());
						iter++;
						if (iter != arg.end()) {
							if (waveNo<0)waveNo=0;
							if (waveNo>=MOscWaveData::MAX_WAVE)waveNo=MOscWaveData::MAX_WAVE-1;
							m_waveData.waveData.setWave(waveNo,toLower(*iter));
						}
					}
				}
				start = result[0].second;
			}
			m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
		}

		//2009.05.10 OffGao ADD START addDPCM
		// DPCM WAVE (ex. "#WAV9 0,0123456789abcdeffedcba9876543210")
		{
			m_waveData.dpcmData.clear();
			regex exp("^#WAV9[[:space:]].*$");
			string::const_iterator start=m_string.begin();
			string::const_iterator end=m_string.end();
			smatch result;
			while (regex_search(start, end, result, exp,regex_constants::match_not_dot_newline)) {
				list<string> wav=split(result.str(0)," ");
				string wavs;
				{
					list<string>::iterator iter = wav.begin();
					if (iter != wav.end()) {
						iter++;
						while (iter != wav.end()) {
							wavs+=*iter;
							iter++;
						}
					}
				}
				list<string> arg=split(wavs,",");
				{
					list<string>::iterator iter = arg.begin();
					if (iter != arg.end()) {
						int waveNo = atoi(iter->c_str());
						iter++;
						if (iter != arg.end()) {
							if (waveNo<0)waveNo=0;
							if (waveNo>=MOscFcDpcmData::MAX_WAVE)waveNo=MOscFcDpcmData::MAX_WAVE-1;
							int intVol=atoi(iter->c_str());
							if (intVol<0)intVol=0;
							if (intVol>127)intVol=127;
							iter++;
							if (iter != arg.end()) {
								int loopFg=atoi(iter->c_str());
								if (loopFg<0)loopFg=0;
								if (loopFg>1)loopFg=1;
								iter++;
								if (iter != arg.end()) {
									m_waveData.dpcmData.setWave(waveNo,intVol,loopFg,*iter);
								}
							}
						}
					}
				}
				start = result[0].second;
			}
			m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
		}
		//2009.05.10 OffGao ADD END addDPCM

		// macro
		{
			begin();
			bool top=true;
			//Array macroTable=new Array ();
			regex trimHead("^[[:space:]]*");
			regex trimFoot("[[:space:]]*$");
			vector<MACRO> macroTable;
			while(m_letter<m_string.length()){
				char c=getCharNext();
				switch(c){
					case '$':
						if (top){
							string::size_type last= m_string.find_first_of(";",m_letter);
							if (last>m_letter){
								string::size_type nameEnd=m_string.find_first_of("=",m_letter);
								if (nameEnd>m_letter&&nameEnd<last){
									string::size_type start=m_letter;
									string::size_type argspos= m_string.find_first_of("{");
									if (argspos == string::npos || argspos >= nameEnd){
										argspos=nameEnd;
									}
									string idPart = m_string.substr(start,argspos-start);
									regex exp("[a-zA-Z_][a-zA-Z_0-9#\\+\\(\\)]*");
									string::const_iterator startit=idPart.begin();
									string::const_iterator endit=idPart.end();
									smatch regexResult;
									if (regex_search(startit, endit, regexResult, exp)) {
										string str=regexResult.str(0);
										idPart = regex_replace(idPart,trimHead,"",regex_constants::match_not_dot_newline);
										idPart = regex_replace(idPart,trimFoot,"",regex_constants::match_not_dot_newline);
										if (idPart != str) {
											printf("warning '%s','%s'\n",idPart.c_str(),str.c_str());
										}
										if (str.length()>0){
											
											//										Array args=new Array ();
											vector<ARGS> vargs;
											if (argspos<nameEnd){
												string::size_type t = m_string.find_first_of("}",argspos);
												if (t > nameEnd) {
													printf("warning missing '}' '%s'\n",idPart.c_str());
													t = nameEnd;
												}
												string argstr=m_string.substr(argspos+1,t-(argspos+1));
												list<string> args=split(argstr,",");
												//											args=argstr.split(",");
												list<string>::iterator iter = args.begin();
												int i=0;
												while (iter != args.end()) {
													//											for (i=0;i<args.length;i++){
													string tt="";
													regex reg("[a-zA-Z_][a-zA-Z_0-9#\\+\\(\\)]*");
													string::const_iterator start=iter->begin();
													string::const_iterator end=iter->end();
													smatch result;
													if (regex_search(start, end, result, reg)) {
														tt = result.str(0);
														//break;
													}
													{
														ARGS t;
														t.str = tt;
														t.index = i;
														vargs.push_back(t);
														i++;
													}
													iter++;
												}
												sort(vargs.begin(),vargs.end());
											}
											m_letter=nameEnd+1;
											c=getCharNext();
											while(m_letter<last){
												if (c=='$'){
													if (!replaceMacro(&macroTable)){
														if (m_string.substr(m_letter,str.length())==str){
															m_letter--;
															m_string=remove(m_string,m_letter,m_letter+str.length());
															warning(MWarning::RECURSIVE_MACRO,str);
														}
													}
													last= m_string.find_first_of(";",m_letter);
												}
												c=getCharNext();
											}
											int pos=0;
											for (;pos<macroTable.size();pos++){
												if (macroTable[pos].str==str){
													//macroTable.splice(pos, 1);
													vector<MACRO>::iterator ipos = macroTable.begin();
													ipos += pos;
													macroTable.erase(ipos);
													pos--;
													continue;
												}
												if (macroTable[pos].str.length()<str.length()){
													break;
												}
											}
											{
												MACRO t;
												t.str = str;
												t.code = m_string.substr(nameEnd+1,last-(nameEnd+1));
												t.args = vargs;
												vector<MACRO>::iterator ipos = macroTable.begin();
												ipos += pos;
												macroTable.insert(ipos,1,t);
											}
											m_string=remove(m_string,start-1,last);
											m_letter=start-1;
											
										}
										break;
									}
								}else {
									// macro use
									replaceMacro(&macroTable);
									top=false;
								}
							}else {
								// macro use
								replaceMacro(&macroTable);
								top=false;
							}
						}else {
							// macro use
							replaceMacro(&macroTable);
							top=false;
						}
						break;
					case ';':
						top=true;
						break;
					default :
						if (!isspace(c)/*&&c!='　'*/){
							top=false;
						}
						break;
				}
			}
		}
	}
	
	
	// 指定されたメタ記述を引き抜いてくる
	Array<string>* findMetaDescV(string sectionName) {
		Array<string>* tt=new Array<string>;
		string format = "^#"+sectionName+"([[:space:]]*|[[:space:]]+(.*))$";
		string::const_iterator start=m_string.begin();
		string::const_iterator end=m_string.end();
		regex exp(format);
		smatch result;
		if (regex_search(start,end,result,exp)) {
			string* s = new string(result.str(2));
			tt->push(s);
		}
		m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
		return tt;
	}
	
	// 指定されたメタ記述を引き抜いてくる
	string findMetaDescN(string sectionName) {
		string tt="";
		string format = "^#"+sectionName+"([[:space:]]*|[[:space:]]+(.*))$";
		string::const_iterator start=m_string.begin();
		string::const_iterator end=m_string.end();
		regex exp(format);
		smatch result;
		if (regex_search(start,end,result,exp)) {
			tt = result.str(1);
		}
		m_string=regex_replace(m_string,exp,"",regex_constants::match_not_dot_newline);
		return tt;
	}
	
	void processComment(string str) {
		m_string=str;
		begin();
		string::size_type commentStart=string::npos;
		while(m_letter<m_string.length()){
			char c=getCharNext();
			switch(c){
				case '/':
					if (getChar()=='*'){
						if (commentStart==string::npos)commentStart=m_letter-1;
						next();
					}
					break;
				case '*':
					if (getChar()=='/'){
						if (commentStart!=string::npos){
							m_string=remove(m_string,commentStart,m_letter);
							m_letter=commentStart;
							commentStart=string::npos;
						}
						else {
							warning(MWarning::UNOPENED_COMMENT,"");
						}
					}
					break;
				default :
					break;
			}
		}
		if (commentStart!=string::npos)warning(MWarning::UNCLOSED_COMMENT,"");
		
		// 外部プログラム用のクォーテーション
		begin();
		commentStart=string::npos;
		while(m_letter<m_string.length()){
			if (getCharNext()=='`'){
				if (commentStart==string::npos){
					commentStart=m_letter-1;
				}
				else {
					m_string=remove(m_string,commentStart,m_letter-1);
					m_letter=commentStart;
					commentStart=string::npos;
				}
			}
		}
		
	}
	
	void processGroupNotes() {
		string::size_type GroupNotesStart=string::npos;
		string::size_type GroupNotesEnd;
		int noteCount=0;
		string::size_type repend;int len;int tick;int tick2;Number tickdiv;int noteTick;int noteOn;
		int lenMode;
		int defLen=96;
		string newstr;
		begin();
		while(m_letter<m_string.length()){
			char c=getCharNext();
			switch(c){
				case 'l':
					defLen=len2tick(getUInt(0));
					defLen=getDot(defLen);
					break;
				case '{':
					GroupNotesStart=m_letter-1;
					noteCount=0;
					break;
				case '}':
					repend=m_letter;
					if (GroupNotesStart==string::npos){
						warning(MWarning::UNOPENED_GROUPNOTES,"");
					}
					tick=0;
					while(1){
						if (getChar()!='%'){
							lenMode=0;
						}
						else {
							lenMode=1;
							next();
						}
						len=getUInt(0);
						if (len==0){
							if (tick==0)tick=defLen;
							break;
						}
						tick2=(lenMode?len:len2tick(len));
						tick2=getDot(tick2);
						tick+=tick2;
						if (getChar()!='&'){
							break;
						}
						next();
					}
					GroupNotesEnd=m_letter;
					m_letter=GroupNotesStart+1;
					newstr=m_string.substr(0,GroupNotesStart);
					tick2=0;
					tickdiv=Number (tick)/Number (noteCount);
					noteCount=1;
					noteOn=0;
					while(m_letter<repend){
						c=getCharNext();
						switch(c){
							case '+':
							case '#':
							case '-':
								break;
								
							default :
								if ((c>='a'&&c<='g')||c=='r'){
									if (noteOn==0){
										noteOn=1;
										break;
									}
								}
								if (noteOn==1){
									noteTick=Math::round(Number (noteCount)*tickdiv-Number (tick2));
									noteCount++;
									tick2+=noteTick;
									if (tick2>tick){
										noteTick-=(tick2-tick);
										tick2=tick;
									}
									newstr+="%";
									{
										stringstream oss;
										oss <<  noteTick; 
										newstr+=oss.str();
									}
								}
								noteOn=0;
								if ((c>='a'&&c<='g')||c=='r'){
									noteOn=1;
								}
								break;
						}
						if (c!='}'){
							newstr+=c;
						}
					}
					m_letter=(int)newstr.length();
					newstr+=m_string.substr(GroupNotesEnd);
					m_string=newstr;
					GroupNotesStart=string::npos;
					break;
				default :
					if ((c>='a'&&c<='g')||c=='r'){
						noteCount++;
					}
					break;
			}
		}
		if (GroupNotesStart!=string::npos)warning(MWarning::UNCLOSED_GROUPNOTES,"");
	}
public:
	struct Func33 { 
		bool operator()( const char a ) const { return isspace(a); } 
	};
	
	static string removeWhitespace(string str) {
		str.erase(remove_if(str.begin(),str.end(),Func33()),str.end());
		return str;
	}
	
	string remove(string str,string::size_type start,string::size_type end) {
		return str.substr(0,start)+str.substr(end+1);
	}
	
	void preload(string str) {
		if (m_sequencer == NULL) return;
		m_sequencer->disconnectAll();

		//漢字コードと'!'を削除
		{
			int i;
			for (i=0;i<str.length();i++) {
				if (str.at(i) < 0 || str.at(i) == '!') str.at(i) = ' ';
			}
		}
		
		m_tracks.clear();
		//m_tracks.reserve(8);
		m_trackNo = 0;
		m_tracks.push(createTrack());
		m_trackNo = 1;
		m_tracks.push(createTrack());
		m_warning = "";
		//m_warning=new string();
		
		m_trackNo=MTrack::FIRST_TRACK;
		m_octave=4;
		m_relativeDir=true;
		m_velocity=100;
		m_velDetail=true;
		m_velDir=true;
		m_length=len2tick(4);
		m_tempo=120;
		m_keyoff=1;
		m_gate=15;
		m_maxGate=16;
		m_form=MOscillator::PULSE;
		m_noteShift=0;
		m_maxPipe=0;
		m_maxSyncSource=0;
		m_beforeNote=0;
		m_portamento=0;
		m_usingPoly=false;
		m_polyVoice=1;
		m_polyForce=false;
		m_infinityLoop=false;
		
		m_metaTitle="";
		m_metaArtist="";
		m_metaCoding="";
		m_metaComment="";
		
		processComment(str);
		processMacro();
		m_string=removeWhitespace(m_string);
//cout << m_string << endl;
		processRepeat();
//cout << m_string << endl;
		processGroupNotes();
//cout << m_string << endl;
		process();
		
		// omit
		if (m_tracks[m_tracks.size()-1]->getNumEvents()==0) {
			m_tracks.pop();
		}
		
		// conduct
		m_tracks[MTrack::TEMPO_TRACK]->conduct(&m_tracks,m_infinityLoop);
		
		// post process
		for (int i=MTrack::TEMPO_TRACK;i<m_tracks.size();i++){
			if (i>MTrack::TEMPO_TRACK){
				if (m_usingPoly&&(m_polyForce||m_tracks[i]->findPoly())){
					m_tracks[i]->usingPoly(m_polyVoice);
				}
				if (m_infinityLoop) {
					m_tracks[i]->recEOT();
				} else {
					m_tracks[i]->recPlayEnd();
					m_tracks[i]->recRestMSec(2000);
					m_tracks[i]->recClose();
				}
			}
			m_tracks[i]->m_infinityLoop = m_infinityLoop;
		}
		
		m_loaded = true;
	}

	void play(string str="") {

		if (str == "" && m_loaded) {
			if (m_sequencer == NULL) return;
			m_sequencer->disconnectAll();

			for (int i=MTrack::TEMPO_TRACK;i<m_tracks.size();i++){
				m_sequencer->connect(m_tracks[i]);
			}
			for (int i=MTrack::TEMPO_TRACK;i<m_tracks.size();i++){
				m_tracks[i]->replay();
			}
		} else {
			preload(str);
			for (int i=MTrack::TEMPO_TRACK;i<m_tracks.size();i++){
				m_sequencer->connect(m_tracks[i]);
			}
		}

		m_waveData.channelData.createPipes(m_maxPipe+1);
		m_waveData.channelData.createSyncSources(m_maxSyncSource+1);
		m_sequencer->play();
	}
	
	void stop() {
		if (m_sequencer == NULL) return;
		m_sequencer->stop();
		m_sequencer->disconnectAll();
		preload("");
	}
	
	void pause() {
		if (m_sequencer == NULL) return;
		m_sequencer->pause();
	}
	
	void resume() {
		if (m_sequencer == NULL) return;
		m_sequencer->play();
	}
	
	void setMasterVolume(int vol) {
		if (m_sequencer == NULL) return;
		m_sequencer->setMasterVolume(vol);
	}
	
	unsigned int getGlobalTick() {
		if (m_sequencer == NULL) return 0;
		return m_sequencer->getGlobalTick();
	}
	
	bool isPlaying() {
		if (m_sequencer == NULL) return false;
		return m_sequencer->isPlaying();
	}
	
	bool isPaused() {
		if (m_sequencer == NULL) return false;
		return m_sequencer->isPaused();
	}

	int getLoopCount() {
		int t=1000;
		for (int i=MTrack::FIRST_TRACK;i<m_tracks.size();i++){
			if (m_tracks[i]->isEnd()) {
			} else {
				if (t > m_tracks[i]->loopCount()) t = m_tracks[i]->loopCount();
			}
		}
		return t;
	}
	
	unsigned int getTotalMSec() {
		return m_tracks[MTrack::TEMPO_TRACK]->getTotalMSec();
	}
	string getTotalTimeStr() {
		return m_tracks[MTrack::TEMPO_TRACK]->getTotalTimeStr();
	}
	unsigned int getNowMSec() {
		if (m_sequencer == NULL) return 0;
		return m_sequencer->getNowMSec();
	}
	string getNowTimeStr() {
		if (m_sequencer == NULL) return "";
		return m_sequencer->getNowTimeStr();
	}
	int getVoiceCount() {
		int i;
		int c=0;
		for (i=0;i<m_tracks.size();i++){
			c+=m_tracks[i]->getVoiceCount();
		}
		return c;
	}
	string getMetaTitle() {
		return m_metaTitle;
	}
	string getMetaComment() {
		return m_metaComment;
	}
	string getMetaArtist() {
		return m_metaArtist;
	}
	string getMetaCoding() {
		return m_metaCoding;
	}
	
	void setWaveOutput(string path) {
		if (m_sequencer == NULL) return;
		m_sequencer->m_filepath = path;
	}
};

}
