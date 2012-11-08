/*-----------------------------------------------------------------------------------------------
	名前	QBSound.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBSound.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>
#include "PPFlMMLSeq.h"
#include "PPLuaScript.h"
#include "PPFlMMLObject.h"
#include "PPSEMMLObject.h"
#include <FlMML/FlMML_Math.h>
#include <FlMML/FlMML.h>
//#include <regex.h>

#define RING_SIZE (1024*64)

static QBSound* snd=NULL;

static FlMML::FlMMLPlayer** flmmlEngine = NULL;
static FlMML::PPFlMMLSeq** flmmlSequencer = NULL;
static PPFlMMLObject** flmmlObject = NULL;
static PPSEMMLObject** semmlObject = NULL;

QBSound* QBSound::sharedSound()
{
	return snd;
}

void QBSound::exitSound()
{
//printf("QBSound::exitSound()\n");
	if (snd) {
		snd->Exit();
		delete snd;
	}
	snd = NULL;
}

QBSound::QBSound(int maxChannel) : mTrack(NULL),mMaxTrack(maxChannel),mMasterVolume(1.0),mFlMMLNum(3)
{
//regex_t preg;
//if (regcomp(&preg, "([[:digit:]]+), ([[:digit:]]+), ([[:digit:]]+)", REG_EXTENDED|REG_NEWLINE) != 0) {
//printf("regex compile failed.\n");
//exit(1);
//}
//printf("QBSound::QBSound()\n");

	snd = this;

	if (flmmlEngine == NULL) {
		flmmlEngine = new FlMML::FlMMLPlayer*[mFlMMLNum];
		flmmlSequencer = new FlMML::PPFlMMLSeq*[mFlMMLNum];
		flmmlObject = new PPFlMMLObject*[mFlMMLNum];
		for (int i=0;i<mFlMMLNum;i++) {
			flmmlEngine[i] = new FlMML::FlMMLPlayer();
			flmmlSequencer[i] = new FlMML::PPFlMMLSeq();
			flmmlSequencer[i]->index = i;
			flmmlEngine[i]->setSequencer(flmmlSequencer[i]);
			flmmlSequencer[i]->start();
			flmmlObject[i] = new PPFlMMLObject(NULL);
			flmmlObject[i]->index = i;
		}
	}
}

int QBSound::Init()
{
//printf("QBSound::Init()\n");
	init_track(mMaxTrack);
	return 0;
}

int QBSound::Exit()
{
//printf("QBSound::Exit()\n");
	return 0;
}

static void reset_track(PSGTrack* track)
{
	track->notect  = 0;
	track->noteoff = 0;
	
//	track->channel->reset();
	
	track->m_velocity = 127;
	track->m_noteShift = 0;
	track->m_polyVoice = 0;
	track->m_maxPipe = 0;
	track->m_maxSyncSource = 0;
	track->m_gate = 15.0;
	track->m_maxGate = 16.0;
	track->m_gate2 = 0;
//	track->m_bpm=0;
	track->m_spt=0;
	track->m_relativeDir=true;
	track->m_velDir=true;
	track->m_velDetail=true;
	track->m_octave = 4;
	track->m_portamento = 0;
	
	track->nlen = 16;
	track->tempo = 300;

	track->m_spt = 44100.0/(track->tempo*96.0/60.0);
}

int QBSound::Reset()
{
	int i;
	for (i=0;i<mMaxTrack;i++) {
		mTrack[i].channel->reset();
		reset_track(&mTrack[i]);
	}
//printf("QBSound::Reset()\n");
	return 0;
}

int QBSound::init_track(int maxChannel)
{
	mTrack = (PSGTrack*)calloc(maxChannel,sizeof(PSGTrack));
	if (mTrack) {
		int i;
		for (i=0;i<mMaxTrack;i++) {
			mTrack[i].note    = (char*)calloc(1,sizeof(char)*RING_SIZE);
			mTrack[i].noteptr = mTrack[i].note;
			mTrack[i].ptr = mTrack[i].note;
			mTrack[i].loopptr = NULL;
			mTrack[i].end = true;
			mTrack[i].channel = new FlMML::MChannel(&waveData);
//			mTrack[i].channel->reset();
			mTrack[i]._note = -1;
			mTrack[i].volume = 1.0;
			reset_track(&mTrack[i]);
		}
		
		semmlObject = new PPSEMMLObject*[maxChannel];
		for (i=0;i<mMaxTrack;i++) {
			semmlObject[i] = new PPSEMMLObject(NULL);
			semmlObject[i]->index = i;
		}
		
		return 0;
	}
	return -1;
}

void QBSound::stopAll()
{
	for (int i=0;i<mMaxTrack;i++) {
		QBSound::stop(i);
	}
	for (int i=0;i<mFlMMLNum;i++) {
		QBSound::stopMML(i);
	}
}

int QBSound::play(const char *_note,int trackindex,bool loop)
{
	startAUGraph();
	int i=trackindex;
//	char* nptr = mTrack[i].ptr;

#if 1	
	std::string m_string=_note;
	m_string=FlMML::MML::removeWhitespace(m_string);
	const char* note = m_string.c_str();
#else
	MML mml;
	mml.processComment(_note);
	mml.processMacro();
	mml.m_string=mml.removeWhitespace(mml.m_string);
	mml.processRepeat();
	mml.processGroupNotes();

	std::string notestr = mml.m_string;
	std::string head = "";
	if (mml.m_relativeDir) {
		head = "@@1";
	} else {
		head = "@@0";
	}
	if (mml.m_velDir) {
		head = head+",1";
	} else {
		head = head+",0";
	}
	notestr = head+notestr;
	const char* note = notestr.c_str();
#endif

	int l=(int)strlen(note);
	int k=0;
	for (k=0;k<=l;k++) {
		*(mTrack[i].ptr) = note[k];
		if (note[k] > 0) {
			mTrack[i].ptr ++;
			if (mTrack[i].ptr >= mTrack[i].note+RING_SIZE) {
				mTrack[i].ptr = mTrack[i].note;
			}
		}
	}
//	if (loop) {
//		mTrack[i].loopptr = nptr;
//	} else {
//		mTrack[i].loopptr = NULL;
//	}
	mTrack[i].end     = false;
	return 0;
}

int QBSound::setMasterVolume(float volume)
{
	mMasterVolume = volume;
	if (mMasterVolume < 0) mMasterVolume = 0;
	if (mMasterVolume > 1) mMasterVolume = 1;
	return 0;
}

float QBSound::getMasterVolume()
{
	return mMasterVolume;
}

int QBSound::stop(int trackindex)
{
	if (trackindex < 0 || trackindex >= mMaxTrack) {
		int i;
		for (i=0;i<mMaxTrack;i++) {
			*(mTrack[i].noteptr) = 0;
			mTrack[i].ptr = mTrack[i].noteptr;
			mTrack[i].loopptr = NULL;
			mTrack[i].end = true;
//			mTrack[i].channel->reset();
			mTrack[i].notect = 0;
		}
	} else {
		int i = trackindex;
		*(mTrack[i].noteptr) = 0;
		mTrack[i].ptr = mTrack[i].noteptr;
		mTrack[i].loopptr = NULL;
		mTrack[i].end = true;
//		mTrack[i].channel->reset();
		mTrack[i].notect = 0;
	}
	return 0;
}

int QBSound::isPlaying(int track)
{
	if (track < 0 || track >= mMaxTrack) {
		int i;
		for (i=0;i<mMaxTrack;i++) {
			if (mTrack[i].end == false) {
				return 1;
			}
		}
	} else {
		if (mTrack[track].end == false) {
			return 1;
		}
	}
	return 0;
}

int QBSound::fill_sound_buffer(void* buffer,int size)
{
#ifdef __FLOAT_BUFFER__
	//semml
	{
		float* b = (float*)buffer;
		VNumber vbuffer[2];
//		vbuffer.reserve(2);
		for (int i=0;i<size/sizeof(float);i+=2) {
			for (int j=0;j<2;j++) {
				vbuffer[j] = 0;
			}
			for (int j=0;j<mMaxTrack;j++) {
				analyze_note(&mTrack[j]);
				mTrack[j].channel->getSamples(vbuffer,1,0,1);
			}
			for (int j=0;j<2;j++) {
				Number n = vbuffer[j]*mMasterVolume*mTrack[j].volume;
				if (n < -1.0f) n = -1.0f;
				if (n >  1.0f) n =  1.0f;
				b[i+j] = n;
			}
		}
	}

	//flmml
	if (flmmlSequencer) {
		for (int i=0;i<mFlMMLNum;i++) {
			flmmlEngine[i]->setMasterVolume(mMasterVolume*flmmlSequencer[i]->volume*127);
			flmmlSequencer[i]->fillSoundBuffer(buffer,size);
		}
	}
#else
	//semml
	{
		signed short* b = (signed short*)buffer;
		VNumber vbuffer[2];
//		vbuffer.reserve(2);
		for (int i=0;i<size/sizeof(signed short);i+=2) {
			for (int j=0;j<2;j++) {
				vbuffer[j] = 0;
			}
			for (int j=0;j<mMaxTrack;j++) {
				analyze_note(&mTrack[j]);
				mTrack[j].channel->getSamples(vbuffer,1,0,1);
			}
			for (int j=0;j<2;j++) {
				FlMML::Number n = vbuffer[j]*mMasterVolume*mTrack[j].volume;
				if (n < -1.0f) n = -1.0f;
				if (n >  1.0f) n =  1.0f;
				b[i+j] = (signed short)(n*0x7fff);
			}
		}
	}

	//flmml
	if (flmmlSequencer) {
		for (int i=0;i<mFlMMLNum;i++) {
			flmmlEngine[i]->setMasterVolume(mMasterVolume*flmmlSequencer[i]->volume*127);
			flmmlSequencer[i]->fillSoundBuffer(buffer,size);
		}
	}
#endif	
	return 0;
}

static int getNum(PSGTrack *track,int def=0)
{
	char a;
	int retval = 0;
	a = *track->noteptr;
	if (!(a >= '0' && a <= '9')) {
		return def;
	}
Loop:
	a = *track->noteptr;
	if (a >= '0' && a <= '9') {
		track->noteptr ++;
		if (track->noteptr >= track->note+RING_SIZE) {
			track->noteptr = track->note;
		}
		retval *= 10;
		retval += a - '0';
		goto Loop;
	}
	return retval;
}

static char lowercase(char a)
{
	if (a >= 'A' && a <= 'Z') {
		return a-'A'+'a';
	}
	return a;
}

static char getChar(PSGTrack* track)
{
	return lowercase(*track->noteptr);
}

static void nextPtr(PSGTrack* track,int delta=0)
{
	if (getChar(track) == 0) return;
	track->noteptr += delta;
	if (track->noteptr >= track->note+RING_SIZE) {
		track->noteptr = track->note;
	}
}

static int getKeySig(PSGTrack* track)
{
	int k=0;
	int f=1;
	while(f){
		char c=getChar(track);
		switch(c){
			case '+':case '#':k++;nextPtr(track,1);break;
			case '-':k--;nextPtr(track,1);break;
			default :f=0;break;
		}
	}
	return k;
}

static void atmark(PSGTrack* track)
{
	nextPtr(track,1);
	char letter = getChar(track);
	switch (letter) {
	case '@':
		track->channel->reset();
		reset_track(track);
#if 0
		if (getNum(track,true)) {
			track->m_relativeDir = true;
		} else {
			track->m_relativeDir = false;
		}
		if (getChar(track) == ',') {
			nextPtr(track,1);
		}
		if (getNum(track,true)) {
			track->m_velDir = true;
		} else {
			track->m_velDir = false;
		}
#endif
		break;
	case 'v':	// Volume
		track->m_velDetail=true;
		track->m_velocity = getNum(track,track->m_velocity);
		if (track->m_velocity>127) track->m_velocity=127;
		break;
	case 'x':	// Expression
		{
			nextPtr(track,1);
			int o=getNum(track,127);
			if (o>127)o=127;
			track->channel->setExpression(o);
		}
		break;
	case 'e':	// Envelope
		{
			int arraynum = 10;
			int* array = (int*)calloc(arraynum,sizeof(int));
			nextPtr(track,1);
			int type = getNum(track);
			if (getChar(track) == ',') {
				nextPtr(track,1);
			}
			int attack = getNum(track);
			int count=0;
			while(true) {
				if (getChar(track) == ',') {
					nextPtr(track,1);
				} else {
					break;
				}
				int value = getNum(track);
				if (arraynum >= count) {
					arraynum += 10;
					array = (int*)realloc(array,arraynum*sizeof(int));
				}
				array[count] = value;
				count ++;
			}

			if (count > 0) {
				if (type == 1) {
					track->channel->setEnvelope1Atk(attack);
					int i=0;
					for (i=0;i<(count-1)/2;i++) {
						track->channel->setEnvelope1Point(array[i*2+0],array[i*2+1]);
					}
					track->channel->setEnvelope1Rel(array[i*2]);
				}
				if (type == 2) {
					track->channel->setEnvelope2Atk(attack);
					int i=0;
					for (i=0;i<(count-1)/2;i++) {
						track->channel->setEnvelope2Point(array[i*2+0],array[i*2+1]);
					}
					track->channel->setEnvelope2Rel(array[i*2]);
				}
			}

			free(array);
		}
		break;
	case 'm':
		nextPtr(track,1);
		if (getChar(track)=='h'){
			int w=0;int f=0;int pmd=0;int amd=0;int pms=0;int ams=0;int s=1;
			do{
				w=getNum(track,w);
				if (getChar(track)!=',')break;
				nextPtr(track,1);
				f=getNum(track,f);
				if (getChar(track)!=',')break;
				nextPtr(track,1);
				pmd=getNum(track,pmd);
				if (getChar(track)!=',')break;
				nextPtr(track,1);
				amd=getNum(track,amd);
				if (getChar(track)!=',')break;
				nextPtr(track,1);
				pms=getNum(track,pms);
				if (getChar(track)!=',')break;
				nextPtr(track,1);
				ams=getNum(track,ams);
				if (getChar(track)!=',')break;
				nextPtr(track,1);
				s=getNum(track,s);
			}
			while(false);
		}
		break;
	case 'n':
		nextPtr(track,1);
		if (getChar(track)=='s'){
			// Note Shift
			nextPtr(track,1);
			track->m_noteShift+=getNum(track);
		}
		else {
			// Noise frequency
			int o=getNum(track);
			if (o<0||o>127)o=0;
			track->channel->setNoiseFreq(o);
		}
		break;
	case 'w':	// pulse Width modulation
		{
			nextPtr(track,1);
			int o=getNum(track);
			if (o<0){
				if (o>-1)o=-1;
				if (o<-99)o=-99;
			}
			else {
				if (o<1)o=1;
				if (o>99)o=99;
			}
			track->channel->setPWM(o);
		}
		break;
	case 'p':	// Pan
		{
			nextPtr(track,1);
			if (getChar(track)=='l'){
				nextPtr(track,1);
				int o=getNum(track,track->m_polyVoice);
				o=FlMML::Math::max(0,FlMML::Math::min(track->m_polyVoice,o));
				track->channel->setVoiceLimit(o);
			}
			else {
				int o=getNum(track,64);
				if (o<1)o=1;
				if (o>127)o=127;
				track->channel->setPan(o);
			}
		}
		break;
	case '\'':	// formant filter
		{
			nextPtr(track,1);
			char letter = getChar(track);
			nextPtr(track,1);
			int vowel=0;
			switch(letter){
				case 'a':vowel=FlMML::MFormant::VOWEL_A;break;
				case 'e':vowel=FlMML::MFormant::VOWEL_E;break;
				case 'i':vowel=FlMML::MFormant::VOWEL_I;break;
				case 'o':vowel=FlMML::MFormant::VOWEL_O;break;
				case 'u':vowel=FlMML::MFormant::VOWEL_U;break;
				default :vowel=-1;break;
			}
			track->channel->setFormant(vowel);
			if (letter != '\'') {
				nextPtr(track,1);
			}
		}
		break;
	case 'd':	// Detune
		{
			nextPtr(track,1);
			int o=getNum(track,0);
			track->channel->setDetune(o);
		}
		break;
	case 'l':	// Low frequency oscillator (LFO)
		{
			int dp=0;int wd=0;int fm=1;int sf=0;int rv=1;int dl=0;int tm=0;int sw=0;
			nextPtr(track,1);
			dp=getNum(track,dp);
			if (getChar(track)==',')nextPtr(track,1);
			wd=getNum(track,wd);
			if (getChar(track)==','){
				nextPtr(track,1);
				if (getChar(track)=='-'){rv=-1;nextPtr(track,1);}
				fm=(getNum(track,fm)+1)*rv;
				if (getChar(track)=='-'){
					nextPtr(track,1);
					sf=getNum(track,0);
				}
				if (getChar(track)==','){
					nextPtr(track,1);
					dl=getNum(track,dl);
					if (getChar(track)==','){
						nextPtr(track,1);
						tm=getNum(track,tm);
						if (getChar(track)==','){
							nextPtr(track,1);
							sw=getNum(track,sw);
						}
					}
				}
			}
			track->channel->setLFOFMSF(fm,sf);
			track->channel->setLFODPWD(dp,wd);
			track->channel->setLFODLTM(dl,tm);
			track->channel->setLFOTarget(sw);
		}
		break;
	case 'f':	// Filter
		{
			int swt=0;int amt=0;int frq=0;int res=0;
			nextPtr(track,1);
			swt=getNum(track,swt);
			if (getChar(track)==','){
				nextPtr(track,1);
				amt=getNum(track,amt);
				if (getChar(track)==','){
					nextPtr(track,1);
					frq=getNum(track,frq);
					if (getChar(track)==','){
						nextPtr(track,1);
						res=getNum(track,res);
					}
				}
			}
			track->channel->setLpfSwtAmt(swt,amt);
			track->channel->setLpfFrqRes(frq,res);
		}
		break;
	case 'q':	// gate time 2
		nextPtr(track,1);
		track->m_gate2 = getNum(track,2);
		break;
	case 'i':	// Input
		{
			int a=0;
			int sens=0;
			nextPtr(track,1);
			sens=getNum(track,sens);
			if (getChar(track)==','){
				nextPtr(track,1);
				a=getNum(track,a);
				if (a>track->m_maxPipe)a=track->m_maxPipe;
			}
			track->channel->setInput(sens,a);
		}
		break;
	case 'o':	// Output
		{
			int a=0;
			int mode=0;
			nextPtr(track,1);
			mode=getNum(track,mode);
			if (getChar(track)==','){
				nextPtr(track,1);
				a=getNum(track,a);
				if (a>track->m_maxPipe){
					track->m_maxPipe=a;
					if (track->m_maxPipe>=FlMML::MML::MAX_PIPE)track->m_maxPipe=a=FlMML::MML::MAX_PIPE;
				}
			}
			track->channel->setOutput(mode,a);
		}
		break;
	case 'r':	// Ring
		{
			int a=0;
			int sens=0;
			nextPtr(track,1);
			sens=getNum(track,sens);
			if (getChar(track)==','){
				nextPtr(track,1);
				a=getNum(track,a);
				if (a>track->m_maxPipe)a=track->m_maxPipe;
			}
			track->channel->setRing(sens,a);
		}
		break;
	case 's':	// Sync
		{
			int a=0;
			int mode=0;
			nextPtr(track,1);
			mode=getNum(track,mode);
			if (getChar(track)==','){
				nextPtr(track,1);
				a=getNum(track,a);
				if (mode==1){
					// Sync out
					if (a>track->m_maxSyncSource){
						track->m_maxSyncSource=a;
						if (track->m_maxSyncSource>=FlMML::MML::MAX_SYNCSOURCE)track->m_maxSyncSource=a=FlMML::MML::MAX_SYNCSOURCE;
					}
				}else if (mode==2){
					// Sync in
					if (a>track->m_maxSyncSource)a=track->m_maxSyncSource;
				}
			}
			track->channel->setSync(mode,a);
		}
		break;
	case 'u':	// midi風なポルタメント
		{
			nextPtr(track,1);
			float rate;
			int mode=getNum(track,0);
			switch(mode){
				case 0:
				case 1:
					track->channel->setMidiPort(mode);
					break;
				case 2:
					rate=0;
					if (getChar(track)==','){
						nextPtr(track,1);
						rate=getNum(track,0);
						if (rate<0)rate=0;
						if (rate>127)rate=127;
					}
					track->channel->setMidiPortRate((8-(rate*7.99/128))/rate);
					break;
				case 3:
					if (getChar(track)==','){
						nextPtr(track,1);
						int oct;
						int baseNote=-1;
						if (getChar(track)!='o'){
							oct=track->m_octave;
						}
						else {
							nextPtr(track,1);
							oct=getNum(track,0);
						}
						char c=getChar(track);
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
							nextPtr(track,1);
							baseNote+=track->m_noteShift+getKeySig(track);
							baseNote+=oct*12;
						}
						else {
							baseNote=getNum(track,60);
						}
						if (baseNote<0)baseNote=0;
						if (baseNote>127)baseNote=127;
						track->channel->setPortBase(baseNote);
					}
					break;
			}
		}
		break;
	default:
		if (letter >= '0' && letter <= '9') 
		{
			int type = getNum(track);
			int sub = 0;
			if (getChar(track)=='-'){
				nextPtr(track,1);
				sub = getNum(track);
			}
			track->channel->setForm(type,sub);
		}
		break;
	}
}

void QBSound::analyze_note(PSGTrack *track)
{
	if (track->noteptr == NULL) goto End;
	if (track->note    == NULL) goto End;

	if (track->notect == 0) {
		if (*track->noteptr == 0) {
			if (track->loopptr == NULL) {
				goto End;
			} else {
				track->noteptr = track->loopptr;
			}
		}
		while (1) {
Loop:
			nextPtr(track);
			if (*track->noteptr == 0) {
				if (track->loopptr == NULL) {
					goto End;
				} else {
					track->noteptr = track->loopptr;
				}
			}
			char letter = getChar(track);
			switch (letter) {
			case '!':		//ループマーク
				nextPtr(track,1);
				goto Loop;
			case '<':		//オクターブ+//
				if (track->m_relativeDir)track->m_octave++;else track->m_octave--;
				if (track->m_octave<-2)track->m_octave=-2;
				if (track->m_octave> 8)track->m_octave= 8;
				nextPtr(track,1);
				goto Loop;
			case '>':		//オクターブ-//
				if (track->m_relativeDir)track->m_octave--;else track->m_octave++;
				if (track->m_octave<-2)track->m_octave=-2;
				if (track->m_octave> 8)track->m_octave= 8;
				nextPtr(track,1);
				goto Loop;
			case '(': // vol up/down
			case ')':
				if ((letter=='('&&track->m_velDir)||
					(letter==')'&&!track->m_velDir)){
					track->m_velocity+=(track->m_velDetail)?1:8;
					if (track->m_velocity>127)track->m_velocity=127;
				}
				else { // down
					track->m_velocity-=(track->m_velDetail)?1:8;
					if (track->m_velocity<0)track->m_velocity=0;
				}
				nextPtr(track,1);
				goto Loop;
			case '+':		//ボリューム+//
				track->m_velocity += 1;
				if (track->m_velocity > 127) track->m_velocity = 127;
				nextPtr(track,1);
				goto Loop;
			case '-':		//ボリューム-//
				track->m_velocity -= 1;
				if (track->m_velocity < 0) track->m_velocity = 0;
				nextPtr(track,1);
				goto Loop;
			case 'n':
				if (getChar(track) == 's') {
					int sign = 1;
					nextPtr(track,1);
					if (getChar(track) == '-') {
						nextPtr(track,1);
						sign = -1;
					}
					track->m_noteShift=getNum(track,track->m_noteShift)*sign;
				}
				goto Loop;
			case 'o':		//オクターブ//
				{
					nextPtr(track,1);
					char a = *track->noteptr;
					if (a >= '0' && a <= '9') {
						track->m_octave = getNum(track);
						if (track->m_octave > 8) {
							track->m_octave = 8;
						}
					}
				}
				goto Loop;
			case '@':		//音色//
				atmark(track);
				goto Loop;
			case 'q':		//ゲートタイム
				nextPtr(track,1);
				track->m_gate = getNum(track,track->m_gate);
				goto Loop;
			case 'v':		//ボリューム//
				track->m_velDetail=false;
				{
					nextPtr(track,1);
					char a = *track->noteptr;
					if (a >= '0' && a <= '9') {
						int v = getNum(track,(track->m_velocity-7)/8)*8+7;
						if (v < 0) v = 0;
						if (v > 127) v = 127;
						track->m_velocity = v;
					}
				}
				goto Loop;
			case 'l':		//音長//
				{
					nextPtr(track,1);
					char a = *track->noteptr;
					if (a >= '0' && a <= '9') {
						track->nlen = getNum(track);
					}
				}
				goto Loop;
			case 't':
				{
#if 0
					char* p = track->noteptr;
					p ++;
					if (p >= track->note+RING_SIZE) {
						p = track->note;
					}
					char a = *p;
#endif
					nextPtr(track,1);
					char a = *track->noteptr;
					if (a >= '0' && a <= '9') {
						track->tempo = getNum(track,track->tempo);
//						track->m_bpm = track->tempo;
						track->m_spt = 44100.0/(track->tempo*96.0/60.0);
					}
				}
				goto Loop;
			default:
				if ((letter >= 'a' && letter <= 'g') || letter == 'r' || letter == 'p') {
					int note = 0;
					int nlen = 0;
					int noteFlag = 1;
					switch (letter) {
					case 'c':
						note = 0;
						break;
					case 'd':
						note = 2;
						break;
					case 'e':
						note = 4;
						break;
					case 'f':
						note = 5;
						break;
					case 'g':
						note = 7;
						break;
					case 'a':
						note = 9;
						break;
					case 'b':
						note = 11;
						break;
					case 'r':
					case 'p':
						noteFlag = 0;
						break;
					}
					nextPtr(track,1);
					//シャープ//
					if (noteFlag) {
						if (*track->noteptr == '#' || *track->noteptr == '+') {
							nextPtr(track,1);
							note ++;
						}
					}
					//音長//
					{
						if (*track->noteptr == '%') {
							nextPtr(track,1);
							nlen = getNum(track,384/4);
							track->notect = nlen*track->m_spt;
						} else {
							char a = *track->noteptr;
							if (a >= '0' && a <= '9') {
								nlen = getNum(track);
							} else {
								nlen = track->nlen;
							}
							if (nlen <= 0) nlen = 4;
							track->notect = (384/nlen)*track->m_spt;
						}
						{
							float zenonpu = 384*track->m_spt;
							track->noteoff = track->notect-((track->notect*track->m_gate/track->m_maxGate)-track->m_gate2*zenonpu/192);
						}
					}
					//ポルタメント//
					{
						if (*track->noteptr == '*') {
							nextPtr(track,1);
							track->m_beforeNote=track->m_octave*12+note+track->m_noteShift;
							track->m_portamento=1;
							goto Loop;
						}
					}
					//発音//
					if (noteFlag) {
						track->_note = track->m_octave*12+note+track->m_noteShift;
						if (track->m_portamento) {
							track->channel->setPortamento((track->m_beforeNote-track->_note)*100,track->notect);
							track->channel->noteOn(track->_note,track->m_velocity);
						} else {
							track->channel->noteOn(track->_note,track->m_velocity);
						}
					} else {
						if (track->_note >= 0) {
							if (track->m_portamento) {
								track->channel->setPortamento(0,0);
								track->m_portamento=0;
							}
							track->channel->noteOff(track->_note);
						}
					}
					break;
				} else {
					nextPtr(track,1);
					goto Loop;
				}
			}
			break;
		}
	}
	if (track->notect > 0) {
		track->notect --;
		if (track->notect == track->noteoff) {
			if (track->_note >= 0) {
				if (track->m_portamento) {
					track->channel->setPortamento(0,0);
					track->m_portamento=0;
				}
				track->channel->noteOff(track->_note);
				track->_note = -1;
			}
		}
	}
	return;
End:
	track->end = true;
	if (track->_note >= 0) {
		if (track->m_portamento) {
			track->channel->setPortamento(0,0);
			track->m_portamento=0;
		}
		track->channel->noteOff(track->_note);
		track->_note = -1;
	}
	return;
}

int QBSound::setVolume(int track,float volume)
{
	mTrack[track].volume = volume;
	if (mTrack[track].volume < 0) mTrack[track].volume = 0;
	if (mTrack[track].volume > 1) mTrack[track].volume = 1;
	return 0;
}

float QBSound::getVolume(int track)
{
	return mTrack[track].volume;
}

int QBSound::playMML(const char* mml,int i)
{
	startAUGraph();
	flmmlEngine[i]->play(mml);
	return 0;
}

int QBSound::stopMML(int i)
{
	flmmlEngine[i]->stop();
	return 0;
}

int QBSound::preloadMML(const char* mml,int i)
{
	flmmlEngine[i]->preload(mml);
	return 0;
}

int QBSound::pauseMML(int i)
{
	flmmlEngine[i]->pause();
	return 0;
}

int QBSound::resumeMML(int i)
{
	flmmlEngine[i]->resume();
	return 0;
}

bool QBSound::isPlayingMML(int i)
{
	return flmmlEngine[i]->isPlaying();
}

bool QBSound::isPausedMML(int i)
{
	return flmmlEngine[i]->isPaused();
}

int QBSound::setVolumeMML(int i,float volume)
{
	flmmlSequencer[i]->volume = volume;
	if (flmmlSequencer[i]->volume < 0) flmmlSequencer[i]->volume = 0;
	if (flmmlSequencer[i]->volume > 1) flmmlSequencer[i]->volume = 1;
	return 0;
}

void QBSound::noteOn(int i,int ch,int noteNo,float velocity)
{
	startAUGraph();
	if (velocity < 0) velocity = 0;
	if (velocity > 1) velocity = 1;
	flmmlSequencer[i]->noteOn(ch,noteNo,velocity*127);
//printf("ch %d,noteNo %d,velocity %f\n",ch,noteNo,velocity);
}

void QBSound::noteOff(int i,int ch)
{
	flmmlSequencer[i]->noteOff(ch);
}

float QBSound::getVolumeMML(int i)
{
	return flmmlSequencer[i]->volume;
}

void QBSound::openSEMMLLibrary(PPLuaScript* script,const char* name)
{
	script->makeObjectTable((PPObject**)semmlObject,mMaxTrack,name);
	if (semmlObject) {
		for (int i=0;i<mMaxTrack;i++) {
			semmlObject[i]->start();
		}
	}
}

void QBSound::idleSEMML()
{
	if (semmlObject) {
		for (int i=0;i<mMaxTrack;i++) {
			semmlObject[i]->idle(false);
		}
	}
}

void QBSound::openFlMMLLibrary(PPLuaScript* script,const char* name)
{
	script->makeObjectTable((PPObject**)flmmlObject,mFlMMLNum,name);
}

void QBSound::setWav9(int waveNo,int intVol,bool loopFg,const char* data)
{
	waveData.dpcmData.setWave(waveNo,intVol,loopFg,std::string(data));
}

void QBSound::setWav10(int waveNo,const char* data)
{
#if 1
	waveData.gbWaveData.setWave(waveNo,data);
#else
	std::string s = std::string(data)+"00000000000000000000000000000000";
	s = MML::removeWhitespace(s);
	s = toLower(s);
	waveData.gbWaveData.setWave(waveNo,s);
#endif
}

void QBSound::setWav13(int waveNo,const char* data)
{
#if 1
	waveData.waveData.setWave(waveNo,data);
#else
	std::string s = std::string(data);
	s=MML::removeWhitespace(s);
	s = toLower(s);
	waveData.waveData.setWave(waveNo,s);
#endif
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
