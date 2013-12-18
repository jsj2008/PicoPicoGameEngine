/*-----------------------------------------------------------------------------------------------
	名前	PPGameScene.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGAME_SCENE_H__
#define __PPGAME_SCENE_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "cocos2d.h"
#include "QBGame.h"
#include "PPGameButton.h"
#include "PPGameSprite.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

NS_CC_BEGIN

class PPGameScene : public cocos2d::Layer
{
public:
	PPGameScene();
	virtual ~PPGameScene();

	virtual bool init();  
	static cocos2d::Scene* scene();
	CREATE_FUNC(PPGameScene);

  virtual void onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
  virtual void onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
  virtual void onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);
  virtual void onTouchesCancelled(const std::vector<Touch*>&touches, Event *unused_event);
  virtual void onAcceleration(Acceleration* acc, Event* unused_event);
  
	Size winSize() {
    int w,h;
    glfwGetFramebufferSize(EGLView::getInstance()->getWindow(),&w,&h);
		return Size(w,h);
	}

	virtual void onEnterTransitionDidFinish();
	virtual void gameIdle();
	virtual void draw();
//	virtual void keyBackClicked();
//	virtual void closeGame();
	
	QBGame* game;
	virtual int spriteLimit();
	virtual QBGame* newGame();
	virtual PPGameTextureInfo* textures();

	void textureIdle(PPGameSprite* g);
	
	PPGameSprite* g;

	cocos2d::Set* touchesSet;

	static float scale_factor;

	unsigned long getKey();
};

NS_CC_END

#endif // __PPGAME_SCENE_H__

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
