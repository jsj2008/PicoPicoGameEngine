#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "FlMML_Number.h"
#include "FlMML_StaticDef.h"
#include "MEnvelope.h"
#include "FlMML_Math.h"
#include "FlMML_Vector.h"

namespace FlMML {

/**
 * This class was created based on "Paul Kellett" that programmed by Paul Kellett
   and "Moog VCF, variation 1" that programmed by paul.kellett@maxim.abel.co.uk
   See following URL; http://www.musicdsp.org/showArchiveComment.php?ArchiveID=29
					  http://www.musicdsp.org/showArchiveComment.php?ArchiveID=25
   Thanks to their great works!
*/
class MFilter {
private:
	Number m_t1;
	Number m_t2;
	Number m_b0;
	Number m_b1;
	Number m_b2;
	Number m_b3;
	Number m_b4;
	int sw;
	MChannelData* m_channeldata;
public:
	MFilter(MChannelData* data) {
		m_channeldata = data;
		setSwitch(0);
	}
	void reset() {
		m_t1=m_t2=m_b0=m_b1=m_b2=m_b3=m_b4=0.0;
	}
	void setSwitch(int s) {
		reset();
		sw=s;
	}
	
	bool checkToSilence() {
		switch(sw){
			case 0:
				return false;
			case 1:
			case -1:
				return (-0.000001<=m_b0&&m_b0<=0.000001&&-0.000001<=m_b1&&m_b1<=0.000001);
			case 2:
			case -2:
				return (
						-0.000001<=m_t1&&m_t1<=0.000001&&
						-0.000001<=m_t2&&m_t2<=0.000001&&
						-0.000001<=m_b0&&m_b0<=0.000001&&
						-0.000001<=m_b1&&m_b1<=0.000001&&
						-0.000001<=m_b2&&m_b2<=0.000001&&
						-0.000001<=m_b3&&m_b3<=0.000001&&
						-0.000001<=m_b4&&m_b4<=0.000001
						);
		}
		return false;
	}
	void run(VNumber* samples,int start,int end,MEnvelope& envelope,Number frq,Number amt,Number res,Number key) {
		switch(sw){
			case -2:hpf2(samples,start,end,envelope,frq,amt,res,key);break;
			case -1:hpf1(samples,start,end,envelope,frq,amt,res,key);break;
			case 0:return ;
			case 1:lpf1(samples,start,end,envelope,frq,amt,res,key);break;
			case 2:lpf2(samples,start,end,envelope,frq,amt,res,key);break;
		}
	}
	void lpf1(VNumber* samples,int start,int end,MEnvelope& envelope,Number frq,Number amt,Number res,Number key) {
		Number b0=m_b0;Number b1=m_b1;
		int i;
		Number fb;
		Number cut;
		Number k=key*(2.0*Math::PI/(MSequencer_RATE44100*440.0));
		if (amt>0.0001||amt<-0.0001){
			for (i=start;i<end;i++){
				cut=m_channeldata->getFrequency(frq+amt*envelope.getNextAmplitudeLinear())*k;
				if (cut<(1.0/127.0))cut=0.0;
				if (cut>(1.0-0.0001))cut=1.0-0.0001;
				fb=res+res/(1.0-cut);
 				// for each sample...
				b0=b0+cut*((samples)[i]-b0+fb*(b0-b1));
				(samples)[i]=b1=b1+cut*(b0-b1);
			}
		}
		else {
			cut=m_channeldata->getFrequency(frq)*k;
			if (cut<(1.0/127.0))cut=0.0;
			if (cut>(1.0-0.0001))cut=1.0-0.0001;
			fb=res+res/(1.0-cut);
			for (i=start;i<end;i++){
 				// for each sample...
 				b0=b0+cut*((samples)[i]-b0+fb*(b0-b1));
				(samples)[i]=b1=b1+cut*(b0-b1);
			}
		}
		m_b0=b0;
		m_b1=b1;
	}
	void lpf2(VNumber* samples,int start,int end,MEnvelope& envelope,Number frq,Number amt,Number res,Number key) {
		Number t1=m_t1;Number t2=m_t2;Number b0=m_b0;Number b1=m_b1;Number b2=m_b2;Number b3=m_b3;Number b4=m_b4;
		Number k=key*(2.0*Math::PI/(MSequencer_RATE44100*440.0));
		for (int i=start;i<end;i++){
			Number cut=m_channeldata->getFrequency(frq+amt*envelope.getNextAmplitudeLinear())*k;
			if (cut<(1.0/127.0))cut=0.0;
			if (cut>1.0)cut=1.0;
			// Set coefficients given frequency & resonance [0.0...1.0]
			Number q=1.0-cut;
			Number p=cut+0.8*cut*q;
			Number f=p+p-1.0;
			q=res*(1.0+0.5*q*(1.0-q+5.6*q*q));
			// Filter (input [-1.0...+1.0])
			Number input=(samples)[i];
			input-=q*b4;				//feedback
			t1=b1;b1=(input+b0)*p-b1*f;
			t2=b2;b2=(b1+t1)*p-b2*f;
			t1=b3;b3=(b2+t2)*p-b3*f;
			b4=(b3+t1)*p-b4*f;
			b4=b4-b4*b4*b4*0.166667;	//clipping
			b0=input;
			(samples)[i]=b4;
		}
		m_t1=t1;
		m_t2=t2;
		m_b0=b0;
		m_b1=b1;
		m_b2=b2;
		m_b3=b3;
		m_b4=b4;
	}
	void hpf1(VNumber* samples,int start,int end,MEnvelope& envelope,Number frq,Number amt,Number res,Number key) {
		Number b0=m_b0;Number b1=m_b1;
		int i;
		Number fb;
		Number cut;
		Number k=key*(2.0*Math::PI/(MSequencer_RATE44100*440.0));
		Number input;
		if (amt>0.0001||amt<-0.0001){
			for (i=start;i<end;i++){
				cut=m_channeldata->getFrequency(frq+amt*envelope.getNextAmplitudeLinear())*k;
				if (cut<(1.0/127.0))cut=0.0;
				if (cut>(1.0-0.0001))cut=1.0-0.0001;
				fb=res+res/(1.0-cut);
 				// for each sample...
				input=(samples)[i];
				b0=b0+cut*(input-b0+fb*(b0-b1));
				b1=b1+cut*(b0-b1);
				(samples)[i]=input-b0;
			}
		}
		else {
			cut=m_channeldata->getFrequency(frq)*k;
			if (cut<(1.0/127.0))cut=0.0;
			if (cut>(1.0-0.0001))cut=1.0-0.0001;
			fb=res+res/(1.0-cut);
			for (i=start;i<end;i++){
 				// for each sample...
				input=(samples)[i];
				b0=b0+cut*(input-b0+fb*(b0-b1));
				b1=b1+cut*(b0-b1);
				(samples)[i]=input-b0;
			}
		}
		m_b0=b0;
		m_b1=b1;
	}
	void hpf2(VNumber* samples,int start,int end,MEnvelope& envelope,Number frq,Number amt,Number res,Number key) {
		Number t1=m_t1;Number t2=m_t2;Number b0=m_b0;Number b1=m_b1;Number b2=m_b2;Number b3=m_b3;Number b4=m_b4;
		Number k=key*(2.0*Math::PI/(MSequencer_RATE44100*440.0));
		for (int i=start;i<end;i++){
			Number cut=m_channeldata->getFrequency(frq+amt*envelope.getNextAmplitudeLinear())*k;
			if (cut<(1.0/127.0))cut=0.0;
			if (cut>1.0)cut=1.0;
			// Set coefficients given frequency & resonance [0.0...1.0]
			Number q=1.0-cut;
			Number p=cut+0.8*cut*q;
			Number f=p+p-1.0;
			q=res*(1.0+0.5*q*(1.0-q+5.6*q*q));
			// Filter (input [-1.0...+1.0])
			Number input=(samples)[i];
			input-=q*b4;				//feedback
			t1=b1;b1=(input+b0)*p-b1*f;
			t2=b2;b2=(b1+t1)*p-b2*f;
			t1=b3;b3=(b2+t2)*p-b3*f;
			b4=(b3+t1)*p-b4*f;
			b4=b4-b4*b4*b4*0.166667;	//clipping
			b0=input;
			(samples)[i]=input-b4;
		}
		m_t1=t1;
		m_t2=t2;
		m_b0=b0;
		m_b1=b1;
		m_b2=b2;
		m_b3=b3;
		m_b4=b4;
	}
};

}
