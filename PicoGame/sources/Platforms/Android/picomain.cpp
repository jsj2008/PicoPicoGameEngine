/*-----------------------------------------------------------------------------------------------
	名前	PPGameStart.c
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "AppDelegate.h"
#include "cocos2d.h"
#include <jni.h>
#include <android/log.h>
#include "QBGame.h"
#include "QBSoundAndroid.h"
#include "PPSensorAndroid.h"
#include "PPGameScene.h"

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

extern "C"
{

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    if (!cocos2d::CCDirector::sharedDirector()->getOpenGLView())
    {
		cocos2d::CCEGLView *view = &cocos2d::CCEGLView::sharedOpenGLView();
        view->setFrameWidthAndHeight(w, h);
        // if you want to run in WVGA with HVGA resource, set it
        // view->create(480, 320);
        cocos2d::CCDirector::sharedDirector()->setOpenGLView(view);

        AppDelegate *pAppDelegate = new AppDelegate();
        cocos2d::CCApplication::sharedApplication().run();
		
		PPSensor::instance = new PPSensorAndroid();
    }
    else
    {
        cocos2d::CCTextureCache::reloadAllTextures();
        cocos2d::CCDirector::sharedDirector()->setGLDefaultValues();
		cocos2d::CCScene* mainScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
		if (mainScene) {
			cocos2d::CCNode* baseScene = mainScene->getChildByTag(8888);
			if (baseScene) {
				PPGameScene* gameScene = (PPGameScene*)baseScene->getChildByTag(9999);
				if (gameScene) {
					gameScene->g->textureManager->reloadAllTexture();
//LOGD("reloadAllTexture");
				} else {
//LOGD("gameScene is NULL");
				}
			} else {
//LOGD("baseScene is NULL");
			}
		} else {
//LOGD("mainScene is NULL");
		}
//		cocos2d::CCEGLView *view = &cocos2d::CCEGLView::sharedOpenGLView();
//		getChildByTag
//      PPGameTexture::ReloadAllTexture();
    }
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_setDensity(JNIEnv*  env, jobject thiz, jfloat density)
{
//	PPWorld::setDensity(density);
	// cocos2d::CCScene* mainScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
	// if (mainScene) {
	// 	cocos2d::CCNode* baseScene = mainScene->getChildByTag(8888);
	// 	if (baseScene) {
	// 		PPGameScene* gameScene = (PPGameScene*)baseScene->getChildByTag(9999);
	// 		if (gameScene) {
	// 			gameScene->game->scale_factor=density;
	// 		} else {
	// 		}
	// 	} else {
	// 	}
	// } else {
	// }
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
//	if (snd == NULL) {
//		QBSound_Start(5);
//		snd = QBSound_Instance();
//	}
	return	snd->fill_sound_buffer(buffer,size);
}

jint Java_org_cocos2dx_lib_QBSound_exitSound( JNIEnv*  env,jobject obj)
{
	QBSound::exitSound();
 	return 0;
}

jint Java_org_cocos2dx_lib_QBSound_fillSoundBuffer(JNIEnv* env,jobject obj,jshortArray buffer,jint size)
{
//#if 1
 	jshort *const arrSrc=(jshort*)env->GetPrimitiveArrayCritical(buffer,NULL);
	FILLSoundBuffer(arrSrc,size*sizeof(jshort));
	env->ReleasePrimitiveArrayCritical(buffer,arrSrc,NULL); 
//#else
//	jboolean b;
//	int i, nSize;
// 	jshort* arrSrc=env->GetShortArrayElements(buffer,&b);
// 	nSize = env->GetArrayLength(buffer);
//	FILLSoundBuffer(arrSrc,size*2);
//#endif
 	return 0;
}

}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
