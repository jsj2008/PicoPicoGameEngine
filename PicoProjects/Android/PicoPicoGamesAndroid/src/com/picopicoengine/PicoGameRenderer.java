package com.picopicoengine;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;

public class PicoGameRenderer implements GLSurfaceView.Renderer {

    public void onDrawFrame(GL10 gl) {
        PicoGameJNI.step();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        PicoGameJNI.init(width, height);
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	PicoGameJNI.reload();
    }

    public void handleActionDown(int id, float x, float y)
    {
    	nativeTouchesBegin(id, x, y);
    }
    
    public void handleActionUp(int id, float x, float y)
    {
    	nativeTouchesEnd(id, x, y);
    }
    
    public void handleActionCancel(int[] id, float[] x, float[] y)
    {
    	nativeTouchesCancel(id, x, y);
    }
    
    public void handleActionMove(int[] id, float[] x, float[] y)
    {
    	nativeTouchesMove(id, x, y);
    }
    
    public void handleKeyDown(int keyCode)
    {
    	nativeKeyDown(keyCode);
    }
   
    private static native void nativeTouchesBegin(int id, float x, float y);
    private static native void nativeTouchesEnd(int id, float x, float y);
    private static native void nativeTouchesMove(int[] id, float[] x, float[] y);
    private static native void nativeTouchesCancel(int[] id, float[] x, float[] y);
    private static native boolean nativeKeyDown(int keyCode);

    private static native void nativeOnPause();
    private static native void nativeOnResume();

    public static native void setDensity(float density);

    static {
	    System.loadLibrary("picogamejni");
    }
}
