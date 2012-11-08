#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "MML.h"

namespace FlMML {

class FlMMLPlayer {
private:
	MML m_mml;
public:
	FlMMLPlayer() {
	}
	
	virtual ~FlMMLPlayer() {
		if (m_mml.m_sequencer) {
			m_mml.m_sequencer->disconnectAll();
			m_mml.m_sequencer = NULL;
		}
	}
	
	void play(string mml="") {
		m_mml.play(mml);
	}
	
	void resume() {
		m_mml.resume();
	}
	
	void preload(string mml="") {
		m_mml.preload(mml);
	}
	
	void stop() {
		m_mml.stop();
	}
	
	void pause() {
		m_mml.pause();
	}
	
	bool isPlaying() {
		return m_mml.isPlaying();
	}
	
	bool isPaused() {
		return m_mml.isPaused();
	}
	
	void setMasterVolume(int vol) {
		m_mml.setMasterVolume(vol);
	}
	
	string getWarnings() {
		return m_mml.getWarnings();
	}
	
	unsigned int getTotalMSec() {
		return m_mml.getTotalMSec();
	}
	string getTotalTimeStr() {
		return m_mml.getTotalTimeStr();
	}
	unsigned int getNowMSec() {
		return m_mml.getNowMSec();
	}
	string getNowTimeStr() {
		return m_mml.getNowTimeStr();
	}
	int getVoiceCount() {
		return m_mml.getVoiceCount();
	}
	string getMetaTitle() {
		return m_mml.getMetaTitle();
	}
	string getMetaComment() {
		return m_mml.getMetaComment();
	}
	string getMetaArtist() {
		return m_mml.getMetaArtist();
	}
	string getMetaCoding() {
		return m_mml.getMetaCoding();
	}
	void setWaveOutput(string path) {
		m_mml.setWaveOutput(path);
	}
	void setSequencer(MSequencer* sequencer) {
		m_mml.m_sequencer = sequencer;
		if (sequencer) sequencer->engine = this;
	}
	int getLoopCount() {
		return m_mml.getLoopCount();
	}
};

}
