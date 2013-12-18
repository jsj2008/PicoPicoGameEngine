package com.picopicoengine;

public class PicoGameJNI {

  static {
	    System.loadLibrary("picogamejni");
  }

  public static native void init(int width, int height);
  public static native void step();
  public static native void reload();

}
