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

/*-----------------------------------------------------------------------------------------------
 クラス
 -----------------------------------------------------------------------------------------------*/

class PPBox2D;
class PPVertualKey;
class PPTMXMap;
class PPUIText;
class PPParticleEmitter;
class PPOffscreenTexture;
class PPUIScrollView;
class PPGameStick;

class PPScriptGame : public QBGame {
public:
	PPScriptGame();
	virtual ~PPScriptGame();
	virtual void reloadData();
	void stepInit();
	void initGraph();
	void stepIdle();
	void stepError();
	virtual void initScript();
	virtual void disableIO();
  
	PPLuaScript* script;
  
	PPVertualKey* vKey;
	PPObject* spriteObject;
	PPTMXMap* tmxObject;
	PPParticleEmitter* particleObject;
	PPOffscreenTexture* offscreenObject;
	PPGameStick* stick;
	PPUIScrollView* scrollViewObject;
	PPUIText* textObject;
  PPBox2D* box2d;
  
	int systemTextureID;
	
	std::string luaScriptPath;
private:
  bool disableIOFlag;
};

#endif

/*-----------------------------------------------------------------------------------------------
 このファイルはここまで
 -----------------------------------------------------------------------------------------------*/
