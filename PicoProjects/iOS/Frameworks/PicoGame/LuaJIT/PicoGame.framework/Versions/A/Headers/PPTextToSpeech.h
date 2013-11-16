//
//  PPTextToSpeech.h
//  PicoGame
//
//  Created by 山口 博光 on 2013/09/12.
//
//

#ifndef __PPTextToSpeech_H__
#define __PPTextToSpeech_H__

class PPTextToSpeech;

class PPTextToSpeech {
public:
	static PPTextToSpeech* sharedSpeech();

  PPTextToSpeech();
  virtual ~PPTextToSpeech();
  
  bool reset();
  
  void utter(const char* string);
  void stop();

  void voice(const char* voice);
  void rate(float rate);
  void pitch(float pitch);
  void volume(float volume);

  const char* getVoice();
  float getRate();
  float getPitch();
  float getVolume();
  bool isPlaying();
};

#endif
