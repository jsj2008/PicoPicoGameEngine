/*-----------------------------------------------------------------------------------------------
	名前	PPGameScene.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameScene.h"
#include "PPGameStart.h"
#include "PPSensorAndroid.h"
#include <jni.h>

// #if 0
// #include <android/log.h>
// #define  LOG_TAG    "PPGameScene"
// #define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
// #else
// #define  LOGD(...) 
// #endif

using namespace cocos2d;

#define PPGAME_MAX_POLY (10000*10)

static int staticKey = 0;
float PPGameScene::scale_factor=1;

extern "C" {
	void Java_org_cocos2dx_lib_Cocos2dxActivity_setHardKey(JNIEnv*  env, jobject thiz, jint key);
};

void Java_org_cocos2dx_lib_Cocos2dxActivity_setHardKey(JNIEnv*  env, jobject thiz, jint key)
{
	staticKey = key;
}

CCScene* PPGameScene::scene()
{
	CCScene *scene = CCScene::node();
	PPGameScene *layer = PPGameScene::node();
	scene->addChild(layer);
	return scene;
}

int PPGameScene::spriteLimit()
{
	return PPGAME_MAX_POLY;
}

QBGame* PPGameScene::newGame()
{
	return PPGameStart();
}

PPGameScene::PPGameScene()
{
	touchesSet = NULL;
	game = NULL;
//	poly = NULL;
	g = NULL;
//	polyCount = 0;
}

PPGameScene::~PPGameScene()
{
	if (game) delete game;
//	if (poly) delete poly;
	if (g) delete g;
	game = NULL;
//	poly = NULL;
	g = NULL;
	CC_SAFE_RELEASE(touchesSet);
}

// on "init" you need to initialize your instance
bool PPGameScene::init()
{
	if ( !CCLayer::init() )
	{
		return false;
	}

	if (game == NULL) game = newGame();

	setIsTouchEnabled(true);
	setIsKeypadEnabled(true);
	if (touchesSet == NULL) touchesSet = new CCSet();

	return true;
}

void PPGameScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator setIter;
	for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter) {
		touchesSet->addObject(*setIter);
	}
}

void PPGameScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
}

void PPGameScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator setIter;
	for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter) {
		touchesSet->removeObject(*setIter);
	}
}

void PPGameScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator setIter;
	for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter) {
		touchesSet->removeObject(*setIter);
	}
}

void PPGameScene::didAccelerate(CCAcceleration* pAccelerationValue)
{
	if (PPSensor::sharedManager()) {
		PPSensorAndroid* a = (PPSensorAndroid*)PPSensor::sharedManager();
		a->accelerometer.x = pAccelerationValue->x;
		a->accelerometer.y = pAccelerationValue->y;
		a->accelerometer.z = pAccelerationValue->z;
	}
}

PPGameTextureInfo* PPGameScene::textures()
{
	return game->TextureInfo();
}

void PPGameScene::textureIdle(PPGameSprite* g)
{
	if (textures()) {
		if (!g->initLoadTex) {
			if (g->textureManager->setTextureList(textures()) == 0) {
				g->initLoadTex = true;
			}
		}
	}
}

unsigned long PPGameScene::getKey()
{
	unsigned long key = 0;
	if (staticKey & 0x01) key |= PAD_UP;
	if (staticKey & 0x02) key |= PAD_DOWN;
	if (staticKey & 0x04) key |= PAD_LEFT;
	if (staticKey & 0x08) key |= PAD_RIGHT;
	if (staticKey & 0x10) key |= PAD_A;
	if (staticKey & 0x20) key |= PAD_B;
	return key;
}

//void PPGameScene::closeGame()
//{
//}

void PPGameScene::draw()
{
	int animationFrameInterval=1;
	GLfloat pm[16];
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glGetFloatv(GL_PROJECTION_MATRIX,pm);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	if (g) {
		animationFrameInterval=g->animationFrameInterval;
		if (game) game->scale_factor=scale_factor;
		g->setWorld(game);
//		g->ClearScreen2D(0.0f, 0.0f, 0.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		game->DrawGL();
		textureIdle(g);
		g->SetViewPort();
		g->InitOT();
		gameIdle(0);
//		for (int i=0;i<polyCount;i++) {
//			g->DrawPoly(&poly[i]);
//		}
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
			CCDirector *pDirector = CCDirector::sharedDirector();
			LOGD("setAnimationInterval %d",g->animationFrameInterval);
			pDirector->setAnimationInterval(g->animationFrameInterval/60.0);
		}
	}
}

void PPGameScene::onEnterTransitionDidFinish()
{
	setIsTouchEnabled(true);
}

void PPGameScene::gameIdle(ccTime dt)
{
	if (PPSensor::sharedManager()) {
		PPSensorAndroid* a = (PPSensorAndroid*)PPSensor::sharedManager();
		if (a->_accelerometerEnabled) {
			if (!getIsAccelerometerEnabled()) {
				setIsAccelerometerEnabled(true);
			}
		} else {
			if (getIsAccelerometerEnabled()) {
				setIsAccelerometerEnabled(false);
			}
		}
	}
	//game->Clear();
	if (g) {
		game->setTouchCount(0);
		if (touchesSet != NULL) {
			CCTouch *pTouch;
			CCSetIterator setIter;
			for (setIter = touchesSet->begin(); setIter != touchesSet->end(); ++setIter) {
				pTouch = (CCTouch *)(*setIter);
				CCPoint location = pTouch->locationInView(pTouch->view());
				game->setTouchPosition(location.x,location.y);
			}
		}
		g->InitBuffer(spriteLimit());
		g->st.screenWidth = winSize().width;
		g->st.screenHeight = winSize().height;
		game->screenWidth = winSize().width;
		game->screenHeight = winSize().height;
		unsigned long key=getKey();
		game->gameIdle(key);
//		if (game->exitGame) {
//			game->exitGame = false;
//			closeGame();
//		}
	}
}

void PPGameScene::keyBackClicked() {
//	CCDirector::sharedDirector()->end();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
