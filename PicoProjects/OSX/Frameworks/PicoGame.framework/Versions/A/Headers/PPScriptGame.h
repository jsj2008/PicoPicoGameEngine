/*-----------------------------------------------------------------------------------------------
	名前	PPScriptGame.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPScriptGame_H__
#define __PPScriptGame_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBGame.h"
#include "PPLuaScript.h"
#include "PPVertualKey.h"
#include "PPTMXMap.h"
#include "PPParticle.h"
#include "PPGameStick.h"
#include "PPOffscreenTexture.h"
#include "PPUIScrollView.h"
#include "PPUIText.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPScriptGame : public QBGame {
public:
	PPScriptGame();
	virtual ~PPScriptGame();
	virtual void reloadData();
	void stepInit();
	void initGraph();
	void stepIdle();
	void stepError();

	PPLuaScript* script;

	PPVertualKey* vKey;
	PPObject* spriteObject;
	PPTMXMap* tmxObject;
	PPParticleEmitter* particleObject;
	PPOffscreenTexture* offscreenObject;
	PPGameStick* stick;
	PPUIScrollView* scrollViewObject;
	PPUIText* textObject;

	int systemTextureID;
	
	std::string luaScriptPath;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
