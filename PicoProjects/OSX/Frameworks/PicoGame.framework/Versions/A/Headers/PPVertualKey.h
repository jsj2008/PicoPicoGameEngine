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
	bool fixed;
	int touchState;
	
	bool vKeyTouch;
	bool vKeyTouch2;
	PPPoint vKeyDelta;
	PPRect vKeyArea;

	virtual int calcDir(int div) {
		if (!vKeyTouch) return -1;
		float length = vKeyDelta.length();
		PPPoint d = vKeyDelta;
		float q = atan2(-d.y/length,d.x/length)*360/(2*M_PI);
		q += (360/div)/2;
		while (q < 0) q += 360;
		while (q >= 360) q -= 360;
		return ((int)q)/(360/div);
	}

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
