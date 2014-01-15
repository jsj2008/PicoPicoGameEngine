//
//  QBStreamSound.h
//  PicoGame
//
//  Created by 山口 博光 on 2013/12/23.
//
//

#ifndef __PicoGame__QBStreamSound__
#define __PicoGame__QBStreamSound__

#include <iostream>
#include <vorbis/vorbisfile.h>

#define QBSTREAM_RING_SIZE (1024*64)

class QBStreamSound {
public:
  QBStreamSound();
  virtual ~QBStreamSound();
  
	char streamfile[1024];
	bool streamopen;
	bool streamopenov;
	signed short streambuffer[QBSTREAM_RING_SIZE];
	int streamp1;
	int streamp2;
	ogg_int64_t streampoint;
  int streamstep;
	bool streamplay;
	bool streamplayrun;
	bool streamdone;
	bool streamend;
	bool streamloop;
  bool streampause;
	unsigned char* streamdata;
	unsigned long streamsize;
	ogg_int64_t streamptr;
	ogg_int64_t looppoint;
	OggVorbis_File streamovf;
  vorbis_info info;
	float volume;

  void reset();
  static bool test(const char* filename);
  void play(const char* filename);
  void play(const char* filename,ogg_int64_t looppoint);
  void play(unsigned char* data,unsigned long size);
  void play(unsigned char* data,unsigned long size,ogg_int64_t looppoint);
  void stop();
  void pause();
  void resume();
  void rewind();
  void willplay();
  bool isPlaying();
  void setVolume(float volume=1.0);
  float getVolume();

  int streamOpen();
  int streamLoad(int p1,int p2,int* done);
  void streamLoad();
  void fill_sound_buffer(void* buffer,int size,float masterVolume=1.0f);
};

#endif /* defined(__PicoGame__QBStreamSound__) */
