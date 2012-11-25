/*-----------------------------------------------------------------------------------------------
	名前	PPVertualKey.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPVertualKey_h__
#define __PPVertualKey_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;

class PPVertualKey : public PPObject {
public:
	PPVertualKey(PPWorld* world);
	virtual ~PPVertualKey();
	
	//static PPVertualKey* instance;
	
	virtual void start();
	
	virtual void stepIdle();
	virtual void stepTouch();
	
	PPPoint startTouch;
	PPPoint preTouch;
	bool fixed;
//	int touchState;
	
	bool vKeyTouch;			//エリア内でタッチ中
	bool vKeyTouch2;		//タッチ中
	bool vKeyTouch3;		//エリア内でタッチ開始した
	PPPoint vKeyDelta;
	PPRect vKeyArea;
	bool doIdle;

	virtual int nearTouch(PPPoint pos,PPPoint* outPos);

	virtual int calcDir(int div,PPRect area);
	virtual int calcDir(int div);

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
