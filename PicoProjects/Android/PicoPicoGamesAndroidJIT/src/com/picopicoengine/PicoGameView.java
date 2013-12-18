package com.picopicoengine;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.MotionEvent;

public class PicoGameView extends GLSurfaceView {
	
  private PicoGameRenderer mRenderer;

  public PicoGameView(Context context) {
    super(context);
    initView();
  }

  public PicoGameView(Context context, AttributeSet attrs) {
    super(context, attrs);
    initView();
  }

  protected void initView() {
	mRenderer = new PicoGameRenderer();
    setRenderer(mRenderer);
  }

  public boolean onTouchEvent(final MotionEvent event) {
  	// these data are used in ACTION_MOVE and ACTION_CANCEL
  	final int pointerNumber = event.getPointerCount();
  	final int[] ids = new int[pointerNumber];
  	final float[] xs = new float[pointerNumber];
  	final float[] ys = new float[pointerNumber];

  	for (int i = 0; i < pointerNumber; i++) {
          ids[i] = event.getPointerId(i);
          xs[i] = event.getX(i);
          ys[i] = event.getY(i);
      }
      
      switch (event.getAction() & MotionEvent.ACTION_MASK) {
      case MotionEvent.ACTION_POINTER_DOWN:
      	final int idPointerDown = event.getAction() >> MotionEvent.ACTION_POINTER_ID_SHIFT;
          final float xPointerDown = event.getX(idPointerDown);
          final float yPointerDown = event.getY(idPointerDown);

          queueEvent(new Runnable() {
              @Override
              public void run() {
                  mRenderer.handleActionDown(idPointerDown, xPointerDown, yPointerDown);
              }
          });
          break;
          
      case MotionEvent.ACTION_DOWN:
      	// there are only one finger on the screen
      	final int idDown = event.getPointerId(0);
          final float xDown = xs[0];
          final float yDown = ys[0];
          
          queueEvent(new Runnable() {
              @Override
              public void run() {
                  mRenderer.handleActionDown(idDown, xDown, yDown);
              }
          });
          break;

      case MotionEvent.ACTION_MOVE:
          queueEvent(new Runnable() {
              @Override
              public void run() {
                  mRenderer.handleActionMove(ids, xs, ys);
              }
          });
          break;

      case MotionEvent.ACTION_POINTER_UP:
      	final int idPointerUp = event.getAction() >> MotionEvent.ACTION_POINTER_ID_SHIFT;
          final float xPointerUp = event.getX(idPointerUp);
          final float yPointerUp = event.getY(idPointerUp);
          
          queueEvent(new Runnable() {
              @Override
              public void run() {
                  mRenderer.handleActionUp(idPointerUp, xPointerUp, yPointerUp);
              }
          });
          break;
          
      case MotionEvent.ACTION_UP:  
      	// there are only one finger on the screen
      	final int idUp = event.getPointerId(0);
          final float xUp = xs[0];
          final float yUp = ys[0];
          
          queueEvent(new Runnable() {
              @Override
              public void run() {
                  mRenderer.handleActionUp(idUp, xUp, yUp);
              }
          });
          break;

      case MotionEvent.ACTION_CANCEL:
          queueEvent(new Runnable() {
             @Override
              public void run() {
                  mRenderer.handleActionCancel(ids, xs, ys);
              }
          });
          break;
      }
    
      return true;
  }

}
