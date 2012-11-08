/*-----------------------------------------------------------------------------------------------
	名前	QBSoundWin.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBSoundWin.h"
#include <math.h>

//static QBSoundWin* snd = NULL;

class QBSoundLocker {
public:
	QBSoundLocker(CRITICAL_SECTION *_locker) {
		locker = _locker;
		EnterCriticalSection(locker);
	}
	virtual ~QBSoundLocker() {
		LeaveCriticalSection(locker);
	}
	
	CRITICAL_SECTION *locker;
};

int QBSoundWin::BufferSize = BUFFSIZE;
int QBSoundWin::BufferSizeVista = BUFFSIZEVISTA;

static bool _vistaFlag=false;

static void CALLBACK waveOutProc(
	HWAVEOUT hwo,
	UINT uMsg,
	DWORD dwInstance,
	DWORD dwParam1,
	DWORD dwParam2
)
{
	QBSoundWin* snd = (QBSoundWin*)QBSound::sharedSound();
	switch (uMsg) {
	case WOM_OPEN:
		break;
	case WOM_DONE:
//printf("waveOutProc %d\n",(int)snd->stopWave);
		if (!snd->stopWave) {
//printf("waveOutProc2\n");
			snd->playSound();
			if (_vistaFlag) {
				snd->fill_sound_buffer(snd->m_pWaveBuff[snd->m_nCurBuff],QBSoundWin::BufferSizeVista);
			} else {
				snd->fill_sound_buffer(snd->m_pWaveBuff[snd->m_nCurBuff],QBSoundWin::BufferSize);
			}
		}
		break;
	case WOM_CLOSE:
		break;
	}
}

MMRESULT QBSoundWin::openWave()
{
	UINT m_uDeviceID = WAVE_MAPPER;

printf("openWave\n");

	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;

	m_wfxWaveForm.wFormatTag = WAVE_FORMAT_PCM;
	m_wfxWaveForm.nChannels = 2;
	m_wfxWaveForm.nSamplesPerSec = 44100;
	//m_wfxWaveForm.nAvgBytesPerSec = 44100;
	//m_wfxWaveForm.nBlockAlign = 42;
	m_wfxWaveForm.wBitsPerSample = 16;
	m_wfxWaveForm.nBlockAlign = m_wfxWaveForm.nChannels * m_wfxWaveForm.wBitsPerSample/8;
	m_wfxWaveForm.nAvgBytesPerSec = m_wfxWaveForm.nSamplesPerSec * m_wfxWaveForm.nBlockAlign;
	m_wfxWaveForm.cbSize = 0;

	MMRESULT ret;
	ret = waveOutOpen(&m_hWavePlay,m_uDeviceID,&m_wfxWaveForm,(DWORD)waveOutProc,(DWORD)this,CALLBACK_FUNCTION);
	if (ret != MMSYSERR_NOERROR) {
printf("openWave MMSYSERR_ERROR\n");
		return ret;
	}
	
	waveOutSetVolume(m_hWavePlay,0xFFFFFFFF);
	
	{
		long i;
		for (i=0;i<NumWaveBuff;++i) {
			if (_vistaFlag) {
				m_pWaveBuff[i] = (LPSTR)calloc(1,BufferSizeVista);
			} else {
				m_pWaveBuff[i] = (LPSTR)calloc(1,BufferSize);
			}
			m_pWaveHdr[i] = (LPWAVEHDR)calloc(1,sizeof(WAVEHDR));
		}
		for (i=0;i<NumWaveBuff;++i) {
			m_pWaveHdr[i]->lpData = m_pWaveBuff[i];
			if (_vistaFlag) {
				m_pWaveHdr[i]->dwBufferLength = BufferSizeVista;
			} else {
				m_pWaveHdr[i]->dwBufferLength = BufferSize;
			}
			m_pWaveHdr[i]->dwUser = m_dwUser;
			m_pWaveHdr[i]->dwFlags = 0;
			m_pWaveHdr[i]->dwLoops = 0;
		}
		for (i=0;i<NumWaveBuff;++i) {
			prepareWave(i);
		}
	}
	
	return ret;
}

MMRESULT QBSoundWin::prepareWave(long cur)
{
	MMRESULT ret;
	LPWAVEHDR pWaveHdr = m_pWaveHdr[cur];
	ret = waveOutPrepareHeader(m_hWavePlay,pWaveHdr,sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR) {
printf("prepareWave MMSYSERR_ERROR\n");
	}
	return ret;
}

MMRESULT QBSoundWin::unprepareWave(long cur)
{
	MMRESULT ret;
	LPWAVEHDR pWaveHdr = m_pWaveHdr[cur];
	if (!(pWaveHdr->dwFlags & WHDR_PREPARED)) {
printf("unprepareWave MMSYSERR_ERROR no Prepared\n");
		return 0;	//準備されたヘッダーではない
	}
	if (!(pWaveHdr->dwFlags & WHDR_INQUEUE)) {
printf("unprepareWave MMSYSERR_ERROR Playing\n");
		return 0;	//再生が終了していない
	}
	ret = waveOutUnprepareHeader(m_hWavePlay,pWaveHdr,sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR) {
printf("unprepareWave MMSYSERR_ERROR\n");
	}
	return ret;
}

MMRESULT QBSoundWin::writeWave(int cur)
{
	MMRESULT ret;
	LPWAVEHDR pWaveHdr = m_pWaveHdr[cur];
	//pWaveHdr->dwFlags = 0;
	ret = waveOutWrite(m_hWavePlay,pWaveHdr,sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR) {
printf("writeWave MMSYSERR_ERROR %d %d\n",ret,m_nCurBuff);
	}
	return ret;
}

MMRESULT QBSoundWin::resetWave()
{
	MMRESULT ret;
	ret = waveOutReset(m_hWavePlay);
	if (ret != MMSYSERR_NOERROR) {
printf("resetWave MMSYSERR_ERROR\n");
	}
	return ret;
}

MMRESULT QBSoundWin::closeWave()
{
	MMRESULT ret;
	ret = waveOutClose(m_hWavePlay);
	if (ret != MMSYSERR_NOERROR) {
printf("closeWave MMSYSERR_ERROR\n");
	}

	{
		long i;
		for (i=0;i<NumWaveBuff;++i) {
			if (m_pWaveBuff[i]) free(m_pWaveBuff[i]);
			if (m_pWaveHdr[i]) free(m_pWaveHdr[i]);
		}
	}

	return ret;
}

//static float theta=0;

void QBSoundWin::playSound()
{
	m_nCurPBuff ++;
	if (m_nCurPBuff >= NumWaveBuff) {
		m_nCurPBuff = 0;
	}
	writeWave(m_nCurPBuff);
	m_nCurBuff ++;
	if (m_nCurBuff >= NumWaveBuff) {
		m_nCurBuff = 0;
	}
}

QBSoundWin::QBSoundWin(int maxChannel) : QBSound(maxChannel)
{
	memset(m_pWaveBuff,0,sizeof(m_pWaveBuff));
	memset(m_pWaveHdr,0,sizeof(m_pWaveHdr));
	m_nCurBuff=0;
	stopWave = false;
	theta = 0;
}

int QBSoundWin::Init()
{
	_vistaFlag = false;
	while (1) {
		OSVERSIONINFOEX verInfo;
		verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if( !::GetVersionEx( reinterpret_cast<OSVERSIONINFO*>( &verInfo ) ) ){
			verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if( !::GetVersionEx( reinterpret_cast<OSVERSIONINFO*>( &verInfo ) ) ){
				break;
			}
		}
		if (verInfo.dwMajorVersion >= 6) {
			_vistaFlag = true;
		}
		break;
//printf("info.dwMajorVersion %d\n",verInfo.dwMajorVersion);
	}
	QBSound::Init();
	stopWave = false;
	openWave();
	//for (int i=0;i<NumWaveBuff;i++) {
	//	fill_sound_buffer(m_pWaveBuff[i],BufferSize);
	//}
	m_nCurPBuff = 0;//NumWaveBuff/2;
	m_nCurBuff = 2;
	playSound();
	playSound();
	//writeWave(m_nCurPBuff);
//	DWORD dwID;
	InitializeCriticalSection(&mMutex);
	//DeleteCriticalSection(&mCriticalSection);
//	CreateThread(NULL , 0 , LoaderThreadProc , (LPVOID)this , 0 , &dwID);
	return 0;
}

int QBSoundWin::play(const char *note,int track,bool loop)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::play(note,track,loop);
}

////int QBSoundWin::playpcm(signed short* dataPtr,unsigned long dataSize,int dataChannel,int track,bool loop)
////{
////	int r=0;
////	EnterCriticalSection(&mCriticalSection);
////	r = QBSound::playpcm(dataPtr,dataSize,dataChannel,track,loop);
////	LeaveCriticalSection(&mCriticalSection);
////	return r;
////}
//
////int QBSoundWin::playpcm(const char* filename,int track,bool loop,ogg_int64_t looppoint)
////{
////	int r=0;
////	EnterCriticalSection(&mCriticalSection);
////	r = QBSound::playpcm(filename,track,loop,looppoint);
////	LeaveCriticalSection(&mCriticalSection);
////	return r;
////}
//
////int QBSoundWin::playmempcm(unsigned char* data,unsigned long size,int track,bool loop,ogg_int64_t looppoint)
////{
////	int r=0;
////	EnterCriticalSection(&mCriticalSection);
////	r = QBSound::playmempcm(data,size,track,loop,looppoint);
////	LeaveCriticalSection(&mCriticalSection);
////	return r;
////}
//
//int QBSoundWin::fill_sound_buffer(void* buffer,int size)
//{
//	int r;
//	EnterCriticalSection(&mCriticalSection);
//	r = QBSound::fill_sound_buffer(buffer,size);
//	LeaveCriticalSection(&mCriticalSection);
//	return r;
//}
//
//int QBSoundWin::stop(int track)
//{
//	int r=0;
//	EnterCriticalSection(&mCriticalSection);
//	r = QBSound::stop(track);
//	LeaveCriticalSection(&mCriticalSection);
//	return r;
//}
//
//int QBSoundWin::isPlaying(int track)
//{
//	int r=0;
//	EnterCriticalSection(&mCriticalSection);
//	r = QBSound::isPlaying(track);
//	LeaveCriticalSection(&mCriticalSection);
//	return r;
//}
//
////int QBSoundWin::fadeOut(int track,int speed)
////{
////	int r=0;
////	EnterCriticalSection(&mCriticalSection);
////	r = QBSound::fadeOut(track,speed);
////	LeaveCriticalSection(&mCriticalSection);
////	return r;
////}
//
////void QBSoundWin::streamLoad()
////{
////	int i;
////	for (i=0;i<mMaxTrack;i++) {
////		EnterCriticalSection(&mCriticalSection);
////		QBSound::streamOpen(i);
////		if (mTrack[i].streamplay && mTrack[i].streamdone==false) {
////			int p1 = mTrack[i].streamp1;
////			int p2 = mTrack[i].streamp2;
////			int done = 0;
////			LeaveCriticalSection(&mCriticalSection);
////			p2 = QBSound::streamLoad(i,p1,p2,&done);
////			EnterCriticalSection(&mCriticalSection);
////			if (done) {
////				if (!mTrack[i].streamloop) {
////					mTrack[i].streamdone = true;
////				}
////			}
////			mTrack[i].streamp2 = p2;
////			mTrack[i].streamplayrun = true;
////			LeaveCriticalSection(&mCriticalSection);
////		} else {
////			LeaveCriticalSection(&mCriticalSection);
////		}
////	}
////}
//
int QBSoundWin::Exit()
{
	return 0;
}

int QBSoundWin::Reset()
{
	QBSoundLocker locker(&mMutex);
	return QBSound::Reset();
}

//
//int QBSoundWin::setVolume(int track,float volume)
//{
//	int r=0;
//	EnterCriticalSection(&mCriticalSection);
//	r = QBSound::setVolume(track,volume);
//	LeaveCriticalSection(&mCriticalSection);
//	return r;
//}

int QBSoundWin::fill_sound_buffer(void* buffer,int size)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::fill_sound_buffer(buffer,size);
}

int QBSoundWin::stop(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::stop(track);
}

int QBSoundWin::isPlaying(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPlaying(track);
}

int QBSoundWin::setVolume(int track,float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setVolume(track,volume);
}

float QBSoundWin::getVolume(int track)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getVolume(track);
}

//int QBSoundWin::sysPause()
//{
//	mPause = 0.0;
//	return 0;
//}

//int QBSoundWin::sysPlay()
//{
//	mPause = 1.0;
//	return 0;
//}

int QBSoundWin::setMasterVolume(float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setMasterVolume(volume);
}

float QBSoundWin::getMasterVolume()
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getMasterVolume();
}

int QBSoundWin::playMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::playMML(mml,i);
}

int QBSoundWin::stopMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::stopMML(i);
}

int QBSoundWin::preloadMML(const char* mml,int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::preloadMML(mml,i);
}

int QBSoundWin::pauseMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::pauseMML(i);
}

int QBSoundWin::resumeMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::resumeMML(i);
}

bool QBSoundWin::isPlayingMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPlayingMML(i);
}

bool QBSoundWin::isPausedMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::isPausedMML(i);
}

int QBSoundWin::setVolumeMML(int i,float volume)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::setVolumeMML(i,volume);
}

float QBSoundWin::getVolumeMML(int i)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::getVolumeMML(i);
}

void QBSoundWin::noteOn(int i,int ch,int noteNo,int velocity)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::noteOn(i,ch,noteNo,velocity);
}

void QBSoundWin::noteOff(int i,int ch)
{
	QBSoundLocker locker(&mMutex);
	return QBSound::noteOff(i,ch);
}

void QBSoundWin::setWav9(int no,int initVol,bool loop,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav9(no,initVol,loop,data);
}

void QBSoundWin::setWav10(int no,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav10(no,data);
}

void QBSoundWin::setWav13(int no,const char* data)
{
	QBSoundLocker locker(&mMutex);
	QBSound::setWav13(no,data);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
