#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "FlMML_Vector.h"
#include "FlMML_Number.h"
#include "FlMML_StaticDef.h"

namespace FlMML {

/*
 * チャンネルボイス・インターフェース
 */
class IChannel {
public:
	virtual ~IChannel() {}
	virtual void setExpression(int ex)=0;
	virtual void setVelocity(int velocity)=0;
	virtual void setNoteNo(int noteNo,bool tie=true)=0;
	virtual void setDetune(int detune)=0;
	virtual void noteOn(int noteNo,int velocity)=0;
	virtual void noteOff(int noteNo)=0;
	virtual void close()=0;
	virtual void setNoiseFreq(Number frequency)=0;
	virtual void setForm(int form,int subform)=0;
	virtual void setEnvelope1Atk(int attack)=0;
	virtual void setEnvelope1Point(int time,int level)=0;
	virtual void setEnvelope1Rel(int release)=0;
	virtual void setEnvelope2Atk(int attack)=0;
	virtual void setEnvelope2Point(int time,int level)=0;
	virtual void setEnvelope2Rel(int release)=0;
	virtual void setPWM(int pwm)=0;
	virtual void setPan(int pan)=0;
	virtual void setFormant(int vowel)=0;
	virtual void setLFOFMSF(int form,int subform)=0;
	virtual void setLFODPWD(int depth,Number freq)=0;
	virtual void setLFODLTM(int delay,int time)=0;
	virtual void setLFOTarget(int target)=0;
	virtual void setLpfSwtAmt(int swt,int amt)=0;
	virtual void setLpfFrqRes(int frq,int res)=0;
	virtual void setVolMode(int m)=0;
	virtual void setInput(int i,int p)=0;
	virtual void setOutput(int o,int p)=0;
	virtual void setRing(int s,int p)=0;
	virtual void setSync(int m,int p)=0;
	virtual void setPortamento(int depth,Number len)=0;
	virtual void setMidiPort(int mode)=0;
	virtual void setMidiPortRate(Number rate)=0;
	virtual void setPortBase(int base)=0;
	virtual void setSoundOff()=0;
	virtual int getVoiceCount()=0;
	virtual void setVoiceLimit(int voiceLimit)=0;
	virtual void setHwLfo(int data)=0;
	virtual void reset()=0;
	virtual void getSamples(VNumber* samples,int max,int start,int delta)=0;
	int m_trackNo;
//	int m_loopCount;
};

}
