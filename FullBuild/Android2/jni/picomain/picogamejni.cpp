#include <jni.h>
#include <android/log.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "QBGame.h"
#include "PPGameButton.h"
#include "PPGameSprite.h"
#include "PPGameStart.h"
#include "QBSoundAndroid.h"
#include "PPSensorAndroid.h"

#include "JniHelper.h"

#define  LOG_TAG    "picogamejni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define PPGAME_MAX_POLY (10000*10)

static QBGame* game=NULL;
static PPGameSprite* g=NULL;
static jint win_width=0;
static jint win_height=0;
static bool accelerometer=false;

extern "C" {
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_init(JNIEnv * env, jobject obj,  jint width, jint height);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_step(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_reload(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_density(JNIEnv * env, jobject obj, jfloat density);
  JNIEXPORT jint JNICALL Java_com_picopicoengine_PicoGameSound_exitSound( JNIEnv*  env,jobject obj);
  JNIEXPORT jint JNICALL Java_com_picopicoengine_PicoGameSound_fillSoundBuffer(JNIEnv* env,jobject obj,jshortArray buffer,jint size);

  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesBegin(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesEnd(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesMove(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesCancel(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
  JNIEXPORT jboolean JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeKeyDown(JNIEnv*  env, jobject thiz, jint keyCode);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeOnResume(JNIEnv*  env, jobject thiz);
  JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_setDensity(JNIEnv*  env, jobject thiz, jfloat density);
}

static void enableAccelerometerJNI();
static void disableAccelerometerJNI();

class PPJNITouch {
public:
	PPJNITouch() {
		x=0;
		y=0;
		touch=false;
	}

	float length(float ix,float iy) {
		return (x-ix)*(x-ix)+(y-iy)*(y-iy);
	}

	float x,y;
	bool touch;
};

#define MAX_JNI_TOUCH 10
static PPJNITouch touch[MAX_JNI_TOUCH];

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
  if (game==NULL) {
    game=PPGameStart();
    PPSensor::instance = new PPSensorAndroid();
  }
  win_width = width;
  win_height = height;
LOGI("PicoGameJNI_init %d,%d",win_width,win_height);
  glViewport(0, 0, win_width, win_height);
}

static void gameIdle()
{
   if (PPSensor::sharedManager()) {
     PPSensorAndroid* a = (PPSensorAndroid*)PPSensor::sharedManager();
     if (a->_accelerometerEnabled) {
       if (!accelerometer) {
      	 accelerometer=true;
      	 enableAccelerometerJNI();
       }
     } else {
       if (accelerometer) {
    	 accelerometer=false;
    	 disableAccelerometerJNI();
       }
     }
   }
  //game->Clear();

  if (g) {
    game->setTouchCount(0);

    // if (touchesSet != NULL) {
    //   CCTouch *pTouch;
    //   CCSetIterator setIter;
    //   for (setIter = touchesSet->begin(); setIter != touchesSet->end(); ++setIter) {
    //     pTouch = (CCTouch *)(*setIter);
    //     CCPoint location = pTouch->locationInView(pTouch->view());
    //     game->setTouchPosition(location.x,location.y);
    //   }
    // }

    for (int i=0;i<MAX_JNI_TOUCH;i++) {
    	if (touch[i].touch) {
    		game->setTouchPosition(touch[i].x,touch[i].y);
    	}
    }

    g->InitBuffer(PPGAME_MAX_POLY);
    g->st.screenWidth = win_width;
    g->st.screenHeight = win_height;
    game->screenWidth = win_width;
    game->screenHeight = win_height;

    // unsigned long key=getKey();
    unsigned long key=0;
    game->gameIdle(key);

//    if (game->exitGame) {
//      game->exitGame = false;
//      closeGame();
//    }
  }
}

static void textureIdle(PPGameSprite* g)
{
  if (game->TextureInfo()) {
    if (!g->initLoadTex) {
      if (g->textureManager->setTextureList(game->TextureInfo()) == 0) {
        g->initLoadTex = true;
      }
    }
  }
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_step(JNIEnv * env, jobject obj)
{
  int animationFrameInterval=1;
  GLfloat pm[16];
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glGetFloatv(GL_PROJECTION_MATRIX,pm);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  if (g) {
    animationFrameInterval=g->animationFrameInterval;
//    if (game) game->scale_factor=scale_factor;
    g->setWorld(game);
//    g->ClearScreen2D(0.0f, 0.0f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    game->DrawGL();
    textureIdle(g);
    g->SetViewPort();
    g->InitOT();
    gameIdle();
//    for (int i=0;i<polyCount;i++) {
//      g->DrawPoly(&poly[i]);
//    }
    g->DrawOT();
    g->ResetOT();
    textureIdle(g);
    QBGame::blinkCounter++;
  } else {
    g = new PPGameSprite();
  }

  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(pm);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);

  if (g) {
    if (animationFrameInterval!=g->animationFrameInterval) {
      // CCDirector *pDirector = CCDirector::sharedDirector();
      // LOGD("setAnimationInterval %d",g->animationFrameInterval);
      // pDirector->setAnimationInterval(g->animationFrameInterval/60.0);
    }
  }
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_reload(JNIEnv * env, jobject obj)
{
	if (g) {
		g->textureManager->reloadAllTexture();
	}
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameJNI_density(JNIEnv * env, jobject obj, jfloat density)
{
}

static int FILLSoundBuffer(short* buffer,int size)
{
  QBSound* snd = QBSound::sharedSound();
  if (snd == NULL) {
    snd = new QBSoundAndroid(5);
    if (snd == NULL) return 0;
    if (snd) {
      snd->Init();
      snd->Reset();
      snd->setMasterVolume(0.5);
    }
  }
  return  snd->fill_sound_buffer(buffer,size);
}

JNIEXPORT jint JNICALL Java_com_picopicoengine_PicoGameSound_exitSound( JNIEnv*  env,jobject obj)
{
  QBSound::exitSound();
  return 0;
}

JNIEXPORT jint JNICALL Java_com_picopicoengine_PicoGameSound_fillSoundBuffer(JNIEnv* env,jobject obj,jshortArray buffer,jint size)
{
  jshort *const arrSrc=(jshort*)env->GetPrimitiveArrayCritical(buffer,0);
  FILLSoundBuffer(arrSrc,size*sizeof(jshort));
  env->ReleasePrimitiveArrayCritical(buffer,arrSrc,0);
  return 0;
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesBegin(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y)
{
	for (int i=0;i<MAX_JNI_TOUCH;i++) {
		if (!touch[i].touch) {
			touch[i].touch=true;
			touch[i].x=x;
			touch[i].y=y;
			break;
		}
	}
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesEnd(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y)
{
	float len=FLT_MAX;
	int j=-1;
	for (int i=0;i<MAX_JNI_TOUCH;i++) {
		if (touch[i].touch) {
			float l=touch[i].length(x,y);
			if (l<len) {
				j=0;
			}
		}
	}
	if (j>=0) {
		touch[j].touch=false;
	}
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesMove(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
{
  int size = env->GetArrayLength(ids);
  jint id[size];
  jfloat x[size];
  jfloat y[size];
  env->GetIntArrayRegion(ids, 0, size, id);
  env->GetFloatArrayRegion(xs, 0, size, x);
  env->GetFloatArrayRegion(ys, 0, size, y);
  for (int i=0;i<MAX_JNI_TOUCH;i++) {
	  touch[i].touch=false;
  }
  for( int i = 0 ; i < size ; i++ ) {
	  touch[i].touch=true;
	  touch[i].x=x[i];
	  touch[i].y=y[i];
  }
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeTouchesCancel(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
{
  int size = env->GetArrayLength(ids);
  jint id[size];
  jfloat x[size];
  jfloat y[size];
  env->GetIntArrayRegion(ids, 0, size, id);
  env->GetFloatArrayRegion(xs, 0, size, x);
  env->GetFloatArrayRegion(ys, 0, size, y);
  for( int k = 0 ; k < size ; k++ ) {
	float len=FLT_MAX;
	int j=-1;
	for (int i=0;i<MAX_JNI_TOUCH;i++) {
		if (touch[i].touch) {
			float l=touch[i].length(x[k],y[k]);
			if (l<len) {
				j=0;
			}
		}
	}
	if (j>=0) {
		touch[j].touch=false;
	}
  }
}

JNIEXPORT jboolean JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeKeyDown(JNIEnv*  env, jobject thiz, jint keyCode)
{
	return true;
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeOnPause(JNIEnv*  env, jobject thiz)
{
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_nativeOnResume(JNIEnv*  env, jobject thiz)
{
}

JNIEXPORT void JNICALL Java_com_picopicoengine_PicoGameRenderer_setDensity(JNIEnv*  env, jobject thiz, jfloat density)
{
}

#define TG3_GRAVITY_EARTH                    (9.80665f)

void Java_org_cocos2dx_lib_Cocos2dxAccelerometer_onSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp)
{
	if (PPSensor::sharedManager()) {
		PPSensorAndroid* a = (PPSensorAndroid*)PPSensor::sharedManager();
		a->accelerometer.x = -((double)x/TG3_GRAVITY_EARTH);
		a->accelerometer.y = -((double)y/TG3_GRAVITY_EARTH);
		a->accelerometer.z = -((double)z/TG3_GRAVITY_EARTH);
	}
}

static void enableAccelerometerJNI()
{
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t,
		                         "com/picopicogame/PicoGameActivity",
								 "enableAccelerometer",
								 "()V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
	}
}

static void disableAccelerometerJNI()
{
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t,
		"com/picopicogame/PicoGameActivity",
		"disableAccelerometer",
		"()V"))
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
	}
}
