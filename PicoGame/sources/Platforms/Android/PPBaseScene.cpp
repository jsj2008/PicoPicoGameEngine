/*-----------------------------------------------------------------------------------------------
	名前	PPBaseScene.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPBaseScene.h"
#include "PPGameScene.h"

using namespace cocos2d;

CCScene* PPBaseScene::scene()
{
	CCScene *scene = CCScene::node();
	PPBaseScene *layer = PPBaseScene::node();
	layer->setTag(8888);
	scene->addChild(layer);

	return scene;
}

bool PPBaseScene::init()
{
	if ( !CCLayer::init() )
	{
		return false;
	}

	PPGameScene *layer = PPGameScene::node();
	layer->setTag(9999);
	addChild(layer);

//	CCSprite* sp = CCSprite::spriteWithFile("CloseNormal.png");
//	sp->setPosition(ccp(100,100));
//	addChild(sp);

	return true;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
