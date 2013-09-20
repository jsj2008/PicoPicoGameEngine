/*-----------------------------------------------------------------------------------------------
	名前	PPFont.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPFont.h"
#include "PPGameSprite.h"

PPFontManager::PPFontManager() : fontTable(NULL),_tableCount(0)
{
}

PPFontManager::~PPFontManager()
{
	if (fontTable) {
		for (int i=0;i<_tableCount;i++) {
			if (fontTable[i]) {
				fontTable[i]->index = -1;
				delete fontTable[i];
			}
		}
		delete fontTable;
	}
}

PPFont** PPFontManager::table()
{
	if (fontTable == NULL) {
		_tableCount = 10;
		fontTable = new PPFont*[_tableCount];
		for (int i=0;i<_tableCount;i++) fontTable[i] = NULL;
	}
	return fontTable;
}

int PPFontManager::tableCount()
{
	return _tableCount;
}

int PPFontManager::getFreeTable()
{
	for (int j=0;j<2;j++) {
		for (int i=0;i<_tableCount;i++) {
			if (fontTable[i] == NULL) return i;
		}
		{
			PPFont** table = new PPFont*[_tableCount+10];
			for (int i=0;i<_tableCount+10;i++) table[i] = NULL;
			for (int i=0;i<_tableCount;i++) table[i] = fontTable[i];
			delete fontTable;
			fontTable = table;
			_tableCount += 10;
		}
	}
	return -1;
}

PPFont* PPFontManager::getFont(const char* name)
{
	table();
	for (int i=0;i<_tableCount;i++) {
		if (fontTable[i]) {
			if (strcmp(fontTable[i]->name(),name) == 0) return fontTable[i];
		}
	}
	return NULL;
}

void PPFontManager::addFont(PPFont* font)
{
	table();
	int index = getFreeTable();
	fontTable[index] = font;
	font->index = index;
}

void PPFontManager::removeFont(PPFont* font)
{
	table();
	if (font->index>=0) fontTable[font->index] = NULL;
}

void PPFontManager::fontManagerIdle()
{
	for (int i=0;i<_tableCount;i++) {
		if (fontTable[i]) {
			fontTable[i]->idle();
		}
	}
}

void PPFontManager::reloadFont()
{
#ifndef NO_TTFONT
	for (int i=0;i<_tableCount;i++) {
		if (fontTable[i]) {
			if (fontTable[i]->ttfont) {
				fontTable[i]->ttfont->_updated = true;
			}
		}
	}
#endif
}

void PPFontManager::deleteFont(int start)
{
	for (int i=start;i<_tableCount;i++) {
		if (fontTable[i]) {
			delete fontTable[i];
			fontTable[i] = NULL;
		}
	}
}

int PPFontManager::registCount()
{
	int c=0;
	for (int i=0;i<_tableCount;i++) {
		if (fontTable[i] != NULL) {
			c++;
		}
	}
	return c;//_tableCount;
}

PPFont::PPFont(PPWorld* world,const char* name,int size,int width,int height,int tilenum) : PPObject(world),_name(NULL),index(-1)
{
#ifndef NO_TTFONT
	ttfont = new PPTTFont(world,name,size,width,height,tilenum);
#endif
	poly._texture = -1;
	localScale = PPPoint(1,1);
	lineWrap = false;
	world->projector->textureManager->fontManager->addFont(this);
	setName(name);
}

PPFont::PPFont(PPWorld* world) : PPObject(world),_name(NULL),index(-1)
{
#ifndef NO_TTFONT
	ttfont = NULL;
#endif
	localScale = PPPoint(1,1);
	lineWrap = false;
	world->projector->textureManager->fontManager->addFont(this);
}

PPFont::~PPFont()
{
	if (index>=0) world()->projector->textureManager->fontManager->removeFont(this);
#ifndef NO_TTFONT
	if (ttfont) delete ttfont;
#endif
	if (_name) delete _name;
}

//void PPFont::display(PPWorld* _target,PP_DRAW_SELECTOR _selector)
//{
//	target = _target;
//	selector = _selector;
//}

void PPFont::setTexture(int tex)
{
#ifndef NO_TTFONT
	if (ttfont) ttfont->texture = tex;
#endif
	poly.initTexture(tex);
}

void PPFont::idle()
{
#ifndef NO_TTFONT
	if (ttfont) {
		if (poly._texture < 0) {
			PPGameTexture* tex = new PPGameTexture();
			setTexture(world()->projector->textureManager->setTexture(tex));
		}
		ttfont->idle();
	}
#endif
}

void PPFont::cache(const char* str)
{
#ifndef NO_TTFONT
	if (ttfont) ttfont->cache(str);
#endif
}

void PPFont::cacheAlphabetAndNumeric()
{
#ifndef NO_TTFONT
	if (ttfont) ttfont->cacheAlphabetAndNumeric();
#endif
}

float PPFont::__print(PPWorld* _target,float x,float y,float scale,const char* str)
{
	return x;
}

void PPFont::scale(float _scalex,float _scaley)
{
	localScale = PPPoint(_scalex,_scaley);
//	return scale;
}

float PPFont::print(float x,float y,const char* str,bool _lineWrap)
{
	lineWrap = _lineWrap;
	return __print(world(),x,y,1,str);
}

int PPFont::length(const char* str)
{
	bool _lineWrap = lineWrap;
	lineWrap = false;
	int len = __print(NULL,0,0,1,str);
	lineWrap = _lineWrap;
	return len;
}

void PPFont::debugDraw(PPPoint _pos)
{
#ifndef NO_TTFONT
	if (world() && selector) {
		if (ttfont) {
			ttfont->textureUpdate();
			for (int y=0;y<ttfont->base->gheight(ttfont);y++) {
				for (int x=0;x<ttfont->base->gwidth(ttfont);x++) {
					int t = x+y*ttfont->base->gwidth(ttfont);
					PPPoint p = _pos;
					PPPoint o = PPPoint(x*ttfont->tileWidth(),y*ttfont->tileHeight());
					p += o;
					(world()->*selector)(p,t,this);
				}
			}
		}
	}
#endif
}

float PPFont::height()
{
#ifndef NO_TTFONT
	if (ttfont) return ttfont->tileHeight()*localScale.y;
#endif
	return 0;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
