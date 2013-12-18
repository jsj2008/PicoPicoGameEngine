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
#include "AppMacros.h"
#include <PicoGame/QBSoundMac.h>
//#include <jni.h>

// #if 1
// #include <android/log.h>
// #define  LOG_TAG    "PPGameScene"
// #define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
// #else
// #define  LOGD(...) 
// #endif

const GLchar * picoPositionTextureColor_vert =
#include "picoShader_PositionTextureColor_vert.h"

using namespace cocos2d;

#define PPGAME_MAX_POLY (10000*10)

static int staticKey = 0;
float PPGameScene::scale_factor=1;

//extern "C" {
//	void Java_org_cocos2dx_lib_Cocos2dxActivity_setHardKey(JNIEnv*  env, jobject thiz, jint key);
//};
//
//void Java_org_cocos2dx_lib_Cocos2dxActivity_setHardKey(JNIEnv*  env, jobject thiz, jint key)
//{
//	staticKey = key;
//}

cocos2d::Scene* PPGameScene::scene()
{
	cocos2d::Scene* scene = Scene::create();
	PPGameScene *layer = PPGameScene::create();
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

  if (QBSound::sharedSound()==NULL) {
    QBSoundMac* snd = new QBSoundMac(5);
    if (snd) {
      snd->Init();
      snd->Reset();
      snd->setMasterVolume(0.5);
    }
  }
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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto label = LabelTTF::create("Hello World", "Arial", TITLE_FONT_SIZE);
    auto origin = Director::getInstance()->getVisibleOrigin();
  
    // position the label on the center of the screen
    label->setPosition(Point(origin.x + visibleSize.width/2,
                             origin.y + visibleSize.height - label->getContentSize().height));
 
 
    // add the label as a child to this layer
    this->addChild(label, 1);

//	setTouchEnabled(true);
	setKeypadEnabled(true);
	if (touchesSet == NULL) touchesSet = new Set();

  GLProgram *p = new GLProgram();
  p->reset();
  p->initWithVertexShaderByteArray(picoPositionTextureColor_vert, ccPositionTextureColorAlphaTest_frag);
  
  p->addAttribute(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
  p->addAttribute(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
  p->addAttribute(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);

  p->link();
  p->updateUniforms();

  ShaderCache::getInstance()->addProgram(p,"PicoGameShader");
 
  setShaderProgram(ShaderCache::getInstance()->getProgram("PicoGameShader"));

   auto listener = EventListenerTouchAllAtOnce::create();

   listener->onTouchesBegan = CC_CALLBACK_2(PPGameScene::onTouchesBegan, this);
   listener->onTouchesMoved = CC_CALLBACK_2(PPGameScene::onTouchesMoved, this);
   listener->onTouchesEnded = CC_CALLBACK_2(PPGameScene::onTouchesEnded, this);
   listener->onTouchesCancelled = CC_CALLBACK_2(PPGameScene::onTouchesCancelled, this);

   getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void PPGameScene::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
//	SetIterator setIter;
//	for (setIter = touches->begin(); setIter != touches->end(); ++setIter) {
  Touch* touch = touches[0];
	touchesSet->addObject(touch);

printf("touch\n");
fflush(stdout);
//	}
}

void PPGameScene::onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
{
}

void PPGameScene::onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
//	CCSetIterator setIter;
//	for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter) {
//		touchesSet->removeObject(*setIter);
//	}
  Touch* touch = touches[0];
  touchesSet->removeObject(touch);
}

void PPGameScene::onTouchesCancelled(const std::vector<Touch*>&touches, Event *unused_event)
{
//	CCSetIterator setIter;
//	for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter) {
//		touchesSet->removeObject(*setIter);
//	}
  Touch* touch = touches[0];
  touchesSet->removeObject(touch);
}

void PPGameScene::onAcceleration(Acceleration* acc, Event* unused_event)
{
	if (PPSensor::sharedManager()) {
		PPSensorAndroid* a = (PPSensorAndroid*)PPSensor::sharedManager();
		a->accelerometer.x = acc->x;
		a->accelerometer.y = acc->y;
		a->accelerometer.z = acc->z;
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

void PPGameScene::draw()
{
  kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPushMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
  kmGLPushMatrix();

  CC_NODE_DRAW_SETUP();

  GL::blendFunc( BlendFunc::ALPHA_PREMULTIPLIED.src, BlendFunc::ALPHA_PREMULTIPLIED.dst );
  GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glActiveTexture(GL_TEXTURE0);

	if (g) {
		if (game) game->scale_factor=scale_factor;
		g->setWorld(game);
		textureIdle(g);
		g->InitOT();
		gameIdle();
		g->DrawOT2();
 		g->ResetOT();
		textureIdle(g);
		QBGame::blinkCounter++;
	} else {
		g = new PPGameSprite();
	}

	glEnable(GL_TEXTURE_2D);
 
  Director::getInstance()->setViewport();

  CC_INCREMENT_GL_DRAWS(1);

  kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPopMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLPopMatrix();
}

void PPGameScene::onEnterTransitionDidFinish()
{
}

void PPGameScene::gameIdle()
{
	if (PPSensor::sharedManager()) {
		PPSensorAndroid* a = (PPSensorAndroid*)PPSensor::sharedManager();
		if (a->_accelerometerEnabled) {
			if (!isAccelerometerEnabled()) {
				setAccelerometerEnabled(true);
			}
		} else {
			if (isAccelerometerEnabled()) {
				setAccelerometerEnabled(false);
			}
		}
	}
	if (g && game) {
		game->setTouchCount(0);
		if (touchesSet != NULL) {
			Touch *pTouch;
			SetIterator setIter;
      int w,h;
      glfwGetWindowSize(EGLView::getInstance()->getWindow(),&w,&h);
			for (setIter = touchesSet->begin(); setIter != touchesSet->end(); ++setIter) {
				pTouch = (Touch *)(*setIter);
				Point location = pTouch->getLocationInView();
				game->setTouchPosition(location.x*winSize().width/w,location.y*winSize().height/h);
			}
		}
		g->InitBuffer(spriteLimit());
		g->st.screenWidth = winSize().width;
		g->st.screenHeight = winSize().height;
		game->screenWidth = winSize().width;
		game->screenHeight = winSize().height;
		unsigned long key=getKey();
		game->gameIdle(key);
	}
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
