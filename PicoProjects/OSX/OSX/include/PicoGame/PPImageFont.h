/*-----------------------------------------------------------------------------------------------
	名前	PPImageFont.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPImageFont_h__
#define __PPImageFont_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPFont.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPImageFont : public PPFont {
public:
	PPImageFont(PPWorld* world,int _texture,int _fontWidth,int _fontHeight) : PPFont(world) {
		poly.reset();
		poly._texture = _texture;
		fontWidth = poly.texTileSize.width = _fontWidth;
		fontHeight = poly.texTileSize.height = _fontHeight;
	}
	virtual float height();
	
	virtual void putFont(PPPoint pos,int gid);
	
protected:
	virtual float __print(PPWorld* _target,float x,float y,float scale,const char* str);
	int fontWidth;
	int fontHeight;
	
	void moveCursor(float dx,float dy);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
