package com.picopicoengine;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.os.Bundle;

public class PicoGameActivity extends Cocos2dxActivity {
    private static Thread   soundThread;
    private static PicoGameSound  sound;
    private static boolean mThredRun=false;

	PicoGameView mView;

    private static void startSoundThread() {
        if (!mThredRun) {
            mThredRun = true;
            soundThread = new Thread() {
                public void run( )
                {               
                    sound.run();
    
                    while( mThredRun )
                    {
                        sound.idle();
                    }
    
                };
            };
            soundThread.start();
        }
    }

    private static void stopSoundThread() {
        if (mThredRun) {
            mThredRun = false;
            try {
                soundThread.join();
            } catch (Exception e) {
            }
        }
    }

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        String packageName = getApplication().getPackageName();
        setPackageName(packageName);

        sound = new PicoGameSound();
        startSoundThread();

        mView = new PicoGameView(getApplication());
        setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();

        stopSoundThread();

        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();

        startSoundThread();

        mView.onResume();
    }

    static {
	    System.loadLibrary("cocosdenshion");
	    System.loadLibrary("picogamejni");
    }
}
