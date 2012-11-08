//
//  MSequencerAU.h
//  FlMMLApp
//
//  Created by 山口 博光 on 11/03/06.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <pthread.h>
#include <AudioToolbox/AudioToolbox.h>
//#include <AudioUnit/AUComponent.h>
#include "MSequencer.h"

namespace FlMML {

class MSequencerAU : public MSequencer {
public:
	MSequencerAU(/*int multiple=32*/);
	virtual ~MSequencerAU();
	
	virtual void start();
	virtual void play();
	virtual void stop();
	virtual void pause();
	virtual void setMasterVolume(int vol);
	virtual bool isPlaying();
	virtual bool isPaused();
	virtual void disconnectAll();
	virtual unsigned int getTimer();
	virtual unsigned int getNowMSec();
	
protected:
	void fillSoundBuffer(void* buffer,int size);
	
	static OSStatus callback(
							 void *inRefCon,
							 AudioUnitRenderActionFlags* ioActionFlags,
							 const AudioTimeStamp* inTimeStamp,
							 UInt32 inBusNumber,
							 UInt32 inNumberFrames,
							 AudioBufferList* ioData);
	
	AudioStreamBasicDescription mAudioFormat;
	AUNode mOutputNode;
	//AUNode mMixerNode;
	//AudioUnit mMixerAudioUnit;
	AUGraph mGraph;
	pthread_mutex_t mMutex;
	unsigned long mCount;
};

}
