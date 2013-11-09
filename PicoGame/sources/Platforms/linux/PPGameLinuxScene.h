/*-----------------------------------------------------------------------------------------------
	名前	PPGameLinuxScene.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGameLinuxScene_h__
#define __PPGameLinuxScene_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBGame.h"
#include "PPGameButton.h"
#include "PPGameSprite.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameLinuxScene {
public:
	PPGameLinuxScene();
	virtual ~PPGameLinuxScene();

	int touchLocation_x;
	int touchLocation_y;
	bool touchScreen;

	virtual bool init();
	virtual void gameIdle();
	virtual void draw();
//	virtual void closeGame();
	
	QBGame* game;
	virtual int spriteLimit();
	virtual QBGame* newGame();
	virtual PPGameTextureInfo* textures();

	void textureIdle(PPGameSprite* g);
	
	//int polyCount;
	//PPGamePoly* poly;
	PPGameSprite* g;
	
	PPSize windowSize;
	PPSize winSize() {
		return windowSize;
	}

	unsigned long hardkey;

	unsigned long getKey();
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
