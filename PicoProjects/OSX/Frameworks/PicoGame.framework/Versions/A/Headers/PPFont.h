/*-----------------------------------------------------------------------------------------------
	名前	PPFont.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPFont_h__
#define __PPFont_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPWorld.h"
#ifndef NO_TTFONT
#include "PPTTFont.h"
#endif
#include "PPObject.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPFontManager {
public:
	PPFontManager();
	virtual ~PPFontManager();
	
	PPFont** table();
	int tableCount();
	void addFont(PPFont* font);
	void removeFont(PPFont* font);
	PPFont* getFont(const char* name);
	void fontManagerIdle();
	void reloadFont();
	void deleteFont(int start);
	int registCount();

private:
	int getFreeTable();

	PPFont** fontTable;
	int _tableCount;
};

class PPFont : public PPObject {
public:

	PPFont(PPWorld* world,const char* ttf_filename,int size=16,int width=512,int height=512,int tilenum=1);
	PPFont(PPWorld* world);
	virtual ~PPFont();

	virtual void scale(float scalex,float scaley);
	virtual int print(int x,int y,const char* str,bool lineWrap=false);
	virtual int length(const char* str);
	virtual float height();

	virtual void setTexture(int texture);
	virtual void idle();
	virtual void cache(const char* str);
	virtual void cacheAlphabetAndNumeric();
	
	virtual void debugDraw(PPPoint _pos);

#ifndef NO_TTFONT
	PPTTFont* ttfont;
#endif
	
	virtual void setName(const char* name) {
		if (_name) delete _name;
		_name = new std::string(name);
	}
	virtual const char* name() {return _name->c_str();}
	
	int index;
	bool lineWrap;

	PPPoint localScale;

	PPPoint cur;
	
protected:
	virtual int __print(PPWorld* _target,int x,int y,float scale,const char* str);
	
	std::string* _name;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
