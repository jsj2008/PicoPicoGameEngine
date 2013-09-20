//
//  PPTextToSpeech.m
//  PicoGame
//
//  Created by 山口 博光 on 2013/09/12.
//
//

#include "PPTextToSpeech.h"
#import <AVFoundation/AVSpeechSynthesis.h>

static PPTextToSpeech* speech=NULL;
static char* speechvoice=NULL;
static AVSpeechSynthesizer* synthesizer=NULL;
static float speechrate = 0.5;
static float speechpitch = 1;
static float speechvolume = 1;

PPTextToSpeech* PPTextToSpeech::sharedSpeech()
{
  if (speech == NULL) {
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.0) return NULL;
    speech = new PPTextToSpeech();
  }
  return speech;
}

PPTextToSpeech::PPTextToSpeech()
{
}

PPTextToSpeech::~PPTextToSpeech()
{
}

bool PPTextToSpeech::reset()
{
  if (speechvoice) free(speechvoice);
  speechvoice=NULL;
  speechrate = 0.5;
  speechpitch = 1;
  speechvolume = 1;
  return true;
}

void PPTextToSpeech::voice(const char* _voice)
{
  if (speechvoice) free(speechvoice);
  if (_voice) {
    speechvoice = (char*)malloc(strlen(_voice)+1);
    strcpy(speechvoice,_voice);
  } else {
    speechvoice = NULL;
  }
}

void PPTextToSpeech::rate(float rate)
{
  speechrate = rate;
}

void PPTextToSpeech::pitch(float pitch)
{
  speechpitch = pitch;
}

void PPTextToSpeech::volume(float volume)
{
  speechvolume = volume;
}

void PPTextToSpeech::utter(const char* string)
{
  if (synthesizer==NULL) {
    synthesizer = [[AVSpeechSynthesizer alloc] init];
  }
  AVSpeechUtterance* utterance = [[AVSpeechUtterance alloc] initWithString:[[NSString alloc] initWithUTF8String:string]];
  utterance.rate = speechrate;
  utterance.pitchMultiplier = speechpitch;
  utterance.volume = speechvolume;
  if (speechvoice) {
    utterance.voice = [AVSpeechSynthesisVoice voiceWithLanguage:[[NSString alloc] initWithUTF8String:speechvoice]];
  }
  [synthesizer speakUtterance:utterance];
}

void PPTextToSpeech::stop()
{
  if (synthesizer) {
    [synthesizer stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
  }
}

const char* PPTextToSpeech::getVoice()
{
  return speechvoice;
}

float PPTextToSpeech::getRate()
{
  return speechrate;
}

float PPTextToSpeech::getPitch()
{
  return speechpitch;
}

float PPTextToSpeech::getVolume()
{
  return speechvolume;
}

bool PPTextToSpeech::isPlaying()
{
  if (synthesizer) {
    return synthesizer.speaking;
  }
  return false;
}
