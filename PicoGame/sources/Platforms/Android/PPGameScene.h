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

class PPGameScene : public cocos2d::CCLayer
{
public:
	PPGameScene();
	virtual ~PPGameScene();

	virtual bool init();  
	static cocos2d::CCScene* scene();
	LAYER_NODE_FUNC(PPGameScene);

	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void didAccelerate(cocos2d::CCAcceleration* pAccelerationValue);

	cocos2d::CCSize winSize() {
		return cocos2d::CCDirector::sharedDirector()->getWinSize();
	}

	virtual void onEnterTransitionDidFinish();
	virtual void gameIdle(cocos2d::ccTime dt);
	virtual void draw();
//	virtual void closeGame();
	
	QBGame* game;
	virtual int spriteLimit();
	virtual QBGame* newGame();
	virtual PPGameTextureInfo* textures();

	void textureIdle(PPGameSprite* g);
	
	PPGameSprite* g;

	cocos2d::CCSet* touchesSet;

	unsigned long getKey();
};

#endif // __PPGAME_SCENE_H__

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
