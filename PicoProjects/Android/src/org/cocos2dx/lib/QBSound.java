package org.cocos2dx.lib;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Process;

public class QBSound implements Runnable {
    //public static final int BUFFER_SIZE = 8192;
	
	public int minSize;
	public short[] audioData1;
	public short[] audioData2;
	public int bufferNo;
	public double q;
	public int stopSound;
	public boolean runSound;

    public QBSound() {
    	runSound = false;
        setMasterVolume(100);
    }

    public int getMasterVolume() {
        return mMasterVolume;
    }

    public void setMasterVolume(int value) {
        mMasterVolume = value;
    }
    
    public void exit() {
    	exitSound();
    }

    public void stop() {
    	stopSound = 1;
    	track.stop();
//    	track.release();
    }
    
    public void run() {
    	if (runSound) return;
    	runSound = true;
    	q = 0;
    	stopSound = 0;
    	bufferNo = 0;
    	minSize = AudioTrack.getMinBufferSize( 44100, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT );        
        
        track = new AudioTrack(AudioManager.STREAM_MUSIC,
        		44100, AudioFormat.CHANNEL_CONFIGURATION_STEREO,
                AudioFormat.ENCODING_PCM_16BIT,minSize,
                AudioTrack.MODE_STREAM);
        track.reloadStaticData();
    	track.play();
        Process.setThreadPriority(Process.THREAD_PRIORITY_AUDIO);
/*
        track.setPlaybackPositionUpdateListener(
                new AudioTrack.OnPlaybackPositionUpdateListener() {
                    public void onPeriodicNotification(AudioTrack track) {
                    }
                    // 再生完了時のコールバック
                    public void onMarkerReached(AudioTrack track) {
                    	if (stopSound!=0) return;
                    	switch (bufferNo) {
                    	case 0:
                            track.setNotificationMarkerPosition(audioData2.length/2);
                            track.write(audioData2, 0, audioData2.length);
                            track.play();
                    		fillSoundBuffer(audioData1,audioData1.length);
                    		bufferNo ++;
                    		break;
                    	case 1:
                            track.setNotificationMarkerPosition(audioData1.length/2);
                            track.write(audioData1, 0, audioData1.length);
                            track.play();
                    		fillSoundBuffer(audioData2,audioData2.length);
                    		bufferNo = 0;
                    		break;
                    	}
//                    	idle();
                    }
                }
            );
*/
    	audioData1 = new short[minSize/2];
    	audioData2 = new short[minSize/2];
		fillSoundBuffer(audioData1,audioData1.length);
		fillSoundBuffer(audioData2,audioData2.length);
        track.write(audioData1, 0, audioData1.length);
        track.setNotificationMarkerPosition(audioData1.length/2);
/*        
        while (stopSound==0) {
        	idle();
        }
*/
    }

    public void idle() {
    	if (stopSound!=0) return;
    	switch (bufferNo) {
    	case 0:
            track.setNotificationMarkerPosition(audioData2.length/2);
            track.write(audioData2, 0, audioData2.length);
            track.play();
    		fillSoundBuffer(audioData1,audioData1.length);
/*
    		{
    			int i;
    			for (i=0;i<audioData1.length;i+=2) {
    				short v = (short)(Math.sin(Math.toRadians(q))*15000);
    				audioData1[i+0] += v;
    				audioData1[i+1] += v;
    				q += 5;
        			if (q > 360) q -= 360;
    			}
    		}
*/
    		bufferNo ++;
    		break;
    	case 1:
            track.setNotificationMarkerPosition(audioData1.length/2);
            track.write(audioData1, 0, audioData1.length);
            track.play();
    		fillSoundBuffer(audioData2,audioData2.length);
/*
    		{
    			int i;
    			for (i=0;i<audioData2.length;i+=2) {
    				short v = (short)(Math.sin(Math.toRadians(q))*15000);
    				audioData2[i+0] += v;
    				audioData2[i+1] += v;
    				q += 5;
        			if (q > 360) q -= 360;
    			}
    		}
*/
    		bufferNo = 0;
    		break;
    	}
    }

    private int mMasterVolume;
    private AudioTrack track;

	private static native int fillSoundBuffer(short buffer[],int size);
	private static native int exitSound();

}
