//
//  QBStreamSound.cpp
//  PicoGame
//
//  Created by 山口 博光 on 2013/12/23.
//
//

#include "QBStreamSound.h"
#include <string.h>

#pragma mark -

static int freeSize(int p1,int p2)
{
	if (p2 > p1) return QBSTREAM_RING_SIZE-p2+p1;
	if (p1 > p2) return p1-p2;
	return QBSTREAM_RING_SIZE;
}

static size_t qbsound_read(void* buffer,size_t size,size_t count,FILE* stream)
{
	QBStreamSound* t=(QBStreamSound*)stream;
	if (buffer == NULL) return 0;
	if (t == NULL) return 0;
	
	long long s = size*count;
	if (t->streamsize-t->streamptr < s) {
		s = t->streamsize-t->streamptr;
	}
	
	memcpy(buffer,&t->streamdata[t->streamptr],(size_t)s);
	t->streamptr += s;

//printf("qbsound_read %ld,%ld,%ld\n",s,size,count);
	
	return (size_t)s;
}

static int qbsound_seek(FILE *stream,ogg_int64_t off,int whence)
{
	QBStreamSound* t=(QBStreamSound*)stream;
	if (t == NULL) return -1;
	switch(whence) {
	case SEEK_CUR:
//printf("seek cur %lld\n",off);
		t->streamptr += off;
		break;
	case SEEK_END:
//printf("seek end\n");
		t->streamptr = t->streamsize + off;
		break;
	case SEEK_SET:
//printf("seek set\n");
		t->streamptr = off;
		break;
	default:
		return -1;
	}
	if (t->streamptr < 0) {
		t->streamptr = 0;
		return -1;
	} else
	if (t->streamptr > t->streamsize) {
		t->streamptr = t->streamsize;
		return -1;
	}
	return 0;
}

static long qbsound_tell(FILE *stream)
{
//printf("tell\n");
	QBStreamSound* t=(QBStreamSound*)stream;
	if (t == NULL) return 0;
	return t->streamptr;
}

static int qbsound_close(FILE *stream)
{
//printf("close\n");
	QBStreamSound* t=(QBStreamSound*)stream;
	if (t == NULL) return -1;
	t->streamptr = 0;
	return 0;
}

#pragma mark -

QBStreamSound::QBStreamSound()
{
//printf("malloc qbstreamsound\n");
  streamopenov = false;
  reset();
}

QBStreamSound::~QBStreamSound()
{
//printf("delete qbstreamsound\n");
}

int QBStreamSound::streamOpen()
{
  QBStreamSound* track=this;
	if (track->streamopen) {
		track->streamopen = false;
		int err;
		if (track->streamopenov) {
			ov_clear(&track->streamovf);
			track->streamopenov = false;
		}
		if (track->streamdata) {
			ov_callbacks callbacks = {
				(size_t (*)(void *, size_t, size_t, void *)) qbsound_read,
				(int (*)(void *, ogg_int64_t, int))          qbsound_seek,
				(int (*)(void *))                            qbsound_close,
				(long (*)(void *))                           qbsound_tell
			};
			track->streamptr = 0;
      track->streamp1 = 0;
      track->streamp2 = 0;
 			err = ov_open_callbacks(track,&track->streamovf,NULL,0,callbacks);
      if (err == 0) {
        vorbis_info* info;
        info = ov_info(&track->streamovf,-1);
        track->info = *info;
        track->streamplay = true;
        track->streamplayrun = false;
        track->streamdone = false;
        track->streamstep = 0;
      } else {
printf("err %d\n",err);
      }
		} else {
			err = ov_fopen((char*)track->streamfile,&track->streamovf);
			if (err == 0) {
				vorbis_info* info;
				info = ov_info(&track->streamovf,-1);
        track->streamstep = 0;
				track->streamp1 = 0;
				track->streamp2 = 0;
				track->info = *info;
				track->streamplay = true;
				track->streamplayrun = false;
				track->streamdone = false;
				track->streamopenov = true;
			}
		}
	}
	return 0;
}

int QBStreamSound::streamLoad(int p1,int p2,int* done)
{
  QBStreamSound* track=this;
	int readSum = 0;
	int size=0;
	signed long readSize;
	while (freeSize(p1,p2) > QBSTREAM_RING_SIZE/16) {
		size = QBSTREAM_RING_SIZE-p1;
		if (p1 > p2) size = p1-p2-1;
		if (p1 < p2) size = QBSTREAM_RING_SIZE-p2;
		if (size < 1) break;
		int bitstream = 0;
		readSize = ov_read( &track->streamovf,
							(char*)&track->streambuffer[p2],
							size*sizeof(short),
							0,           // リトルエンディアン
							2,           // WORDは16bit
							1,           // 符号有り
							&bitstream);
		if (readSize <= 0) {
      if (streamloop) {
        ov_pcm_seek(&track->streamovf,track->looppoint);
        track->streampoint = 0;
      } else {
        *done = 1;
        return p2;
      }
		}
		track->streampoint = ov_pcm_tell(&track->streamovf);
		p2 += readSize/sizeof(short);
		p2 %= QBSTREAM_RING_SIZE;
		readSum += readSize;
/*
		if (readSum >= 32*1024 || p1 == p2) {
			break;
		}
*/
	}
	return p2;
}

void QBStreamSound::streamLoad()
{
  streamOpen();
  if (streamplay && streamdone==false) {
    if (streampause==false) {
      int p1 = streamp1;
      int p2 = streamp2;
      int done=0;
      p2 = streamLoad(p1,p2,&done);
      if (done) {
        if (!streamloop) {
          streamend = true;
        }
      }
      streamp2 = p2;
      streamplayrun = true;
    }
  }
}

void QBStreamSound::reset()
{
  volume = 1;
	streamplay = false;
  streamopen = false;
	streamdata = NULL;
  streamend = false;
}

void QBStreamSound::play(const char* filename)
{
	strcpy(streamfile,filename);
	streamdata = NULL;
	streamopen = true;
	streamloop = false;
  streamdone = false;
  streampause = false;
  streamend = false;
}

void QBStreamSound::play(const char* filename,ogg_int64_t _looppoint)
{
	strcpy(streamfile,filename);
	streamdata = NULL;
	streamopen = true;
	streamloop = true;
  streampause = false;
  streamdone = false;
  streamend = false;
	looppoint = _looppoint;
}

void QBStreamSound::play(unsigned char* data,unsigned long size)
{
	streamdata = data;
	streamsize = size;
	streamopen = true;
	streamloop = false;
  streampause = false;
  streamdone = false;
  streamend = false;
}

void QBStreamSound::play(unsigned char* data,unsigned long size,ogg_int64_t _looppoint)
{
	streamdata = data;
	streamsize = size;
	streamopen = true;
	streamloop = true;
  streampause = false;
  streamdone = false;
  streamend = false;
	looppoint = _looppoint;
}

bool QBStreamSound::test(const char* filename)
{
  int ret;
  FILE *f = fopen(filename,"rb");
  if(!f) return false;
  OggVorbis_File vf;
  ret = ov_test(f,&vf,NULL,0);
  fclose(f);
  return (ret==0);
}

void QBStreamSound::stop()
{
	streamplay = false;
}

void QBStreamSound::pause()
{
  streampause = true;
}

void QBStreamSound::resume()
{
  streampause = false;
}

void QBStreamSound::rewind()
{
  if (streamplay && streamdone==false) {
    ov_time_seek(&streamovf,0);
  }
}

void QBStreamSound::willplay()
{
}

bool QBStreamSound::isPlaying()
{
  if (streamplay == true || streamopen == true) {
    return true;
  }
  return false;
}

void QBStreamSound::setVolume(float _volume)
{
  volume = _volume;
}

float QBStreamSound::getVolume()
{
  return volume;
}

void QBStreamSound::fill_sound_buffer(void* buffer,int size,float masterVolume)
{
#ifdef __FLOAT_BUFFER__
#else
  if (streampause) return;
  if (streamplay == true && streamplayrun == true) {
    signed short* b = (signed short*)buffer;
    for (int i=0;i<size/sizeof(signed short);i+=2) {
      signed short s1,s2;
      if (streamp1 != streamp2) {
        {
          if (info.channels == 1) {
            s1 = streambuffer[streamp1  ];//(0x7fff*8*128/64);
            s2 = s1;
          } else
          if (info.channels == 2) {
            s1 = streambuffer[streamp1  ];///(0x7fff*8*128/64);
            s2 = streambuffer[streamp1+1];///(0x7fff*8*128/64);
          } else {
            s1=s2=0;
          }
          b[i+0] += s1*volume*masterVolume;
          b[i+1] += s2*volume*masterVolume;
          streamstep += info.rate;
          if (streamstep>=44100) {
            streamp1 += info.channels;
            streamstep -= 44100;
          }
        }
        streamp1 %= QBSTREAM_RING_SIZE;
      } else 
      if (streamend) {
        streamdone = true;
        streamplay = false;
        streamplayrun = false;
      }
    }
  }
#endif
}

