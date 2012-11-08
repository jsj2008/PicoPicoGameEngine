/*-----------------------------------------------------------------------------------------------
	名前	PPTTFont.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPTTFont_h__
#define __PPTTFont_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <freetype/ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include "PPWorld.h"
#include "PPGameTexture.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPTTFont;

class PPTTFontImage {
public:
	friend class PPTTFont;
	friend class PPTTFontTile;

	enum {
		RETAIN_COUNT=1
	};

	PPTTFontImage(PPWorld* world,int _width,int _height) {
		width = _width;
		height = _height;
		bytePerRow = _width*2;
		retainCount = 0;
		pixelCount = 0;
		index = -1;
		pixel = (unsigned char*)calloc(1,width*height*2);
		__world = world;
	}
	PPTTFontImage(PPTTFontImage* base,int x,int y,int gw,int gh) {
		pixel = &base->pixel[x*gw*2+y*gh*base->bytePerRow];
		width = gw;
		height = gh;
		bytePerRow = base->bytePerRow;
		pixelCount = 0;
		retainCount = 0;
		index = x+y*(base->width/gw);
		__world = base->world();
	}

	int gwidth(PPTTFont* font);
	int gheight(PPTTFont* font);

	virtual ~PPTTFontImage() {
		if (index < 0) {
			if (pixel) free(pixel);
		}
	}
	void clear(unsigned char t=0) {
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				if (t != 0) {
					setPixel(x,y,getPixel(x,y) | t);
				} else {
					setPixel(x,y,0);
				}
			}
		}
//		setPixel(0,0,1);
		pixelCount = 0;
	}
	void setPixel(int x,int y,unsigned char p) {
		if (x < 0 || y < 0 || x >= width || y >= height) {
//			printf("error setPixel\n");
//			exit(0);
			return;
		}
		if (p != 0) {
			pixelCount++;
		}
		pixel[x*2+0+y*bytePerRow] = p;
		pixel[x*2+1+y*bytePerRow] = p;
	}
	unsigned char getPixel(int x,int y) {
		if (x < 0 || y < 0 || x >= width || y >= height) {
//			printf("error getPixel\n");
//			exit(0);
			return 0;
		}
		return pixel[x*2+y*bytePerRow];
	}

	void show() {
		printf("width %d,height %d\n",width,height);
		int  y, x;
		for ( y = 0; y < height; y++ ) {
			for ( x = 0; x < width; x++ ) {
				putchar(getPixel(x,y) == 0 ? ' ' : getPixel(x,y) < 128 ? '+' : '*' );
			}
			putchar( '\n' );
		}
		printf("\n");
	}

	unsigned char* pixel;
	int width;
	int height;
	int bytePerRow;
	
	PPWorld* world() {
		return __world;
	}

private:
	int retainCount;
	int index;
	int pixelCount;
	PPWorld* __world;
};

class PPTTFontTile;

class PPTTFont {
public:
	friend class PPTTFontTile;
	friend class PPFontManager;
	friend class PPFont;

	PPTTFont(PPWorld* world,const char* name,int size=16,int width=512,int height=512,int tilenum=1);
	virtual ~PPTTFont();
	
	virtual int load(const char* name,int size=16);
	virtual void cache(const char* str);
	virtual void cacheAlphabetAndNumeric();
	virtual PPTTFontTile* image(const char* string);
	virtual PPTTFontTile* length(const char* string);

	virtual void idle();
	virtual void showTileTable();
	
	virtual int tileWidth() {
		return gridX();
	}
	virtual int tileHeight() {
		return gridY();
	}
	virtual int advanceX(int t);
	
	virtual bool isUpdated() {
		return _updated;
	}
	virtual void updated() {
		_updated = false;
	}

	virtual void textureUpdate();
	
	int texture;
	PPGameTextureOption option;
	
	PPTTFontImage* base;
private:
	PPTTFontImage* getFreeTile();

	int drawbitmap(PPTTFontTile* tile,FT_Bitmap* bitmap,FT_Int x,FT_Int y);
	
	int gridX() {return _gridX;}
	int gridY() {return _gridY;}
	
	int _gridX;
	int _gridY;

	FT_Library library;
	FT_Face face;

//	int width;
//	int height;
//	int target_height;
	int baseline;
	int newFontCount;
	
	int tileCount;
	PPTTFontImage** tile;
	
	int maxTile;
	PPTTFontTile** fontTile;

	PPTTFontImage* getTile(int index) {
		return tile[index];
	}
	
	bool _updated;
};

class PPTTFontTile {
public:
	enum {
		RETAIN_COUNT=2
	};

	PPTTFontTile(PPTTFont* _font,const char* string,int _width,int _height) {
		font = _font;
		width = _width;
		height = _height;
		tile = new int[gwidth()*gheight()];
		str = new std::string(string);
		retainCount = 0;
//		index = 0;
		noRelease = false;
		advanceX = 0;
		clear();
	}
	virtual ~PPTTFontTile() {
		delete tile;
		delete str;
	}
	
	void clear() {
		for (int y=0;y<gheight();y++) {
			for (int x=0;x<gwidth();x++) {
				tile[x+y*gwidth()] = 0;
			}
		}
	}
	int gwidth() {
		return (width +font->gridX()-1)/font->gridX();
	}
	int gheight() {
		return (height+font->gridY()-1)/font->gridY();
	}
	
	void setTile(int x,int y,int index) {
		tile[x+y*gwidth()] = index;
	}
	int getTile(int x,int y) {
		return tile[x+y*gwidth()];
	}
	
	void setPixel(int x,int y,unsigned char c) {
		if (x >= 0 && y >= 0 && x < width && y < height) {
			int gx = x/font->gridX();
			int gy = y/font->gridY();
			int t = getTile(gx,gy);
			if (t > 0) {
				font->tile[t]->setPixel(x % font->gridX(),y % font->gridY(),c);
			}
		}
	}
	unsigned char getPixel(int x,int y) {
		if (x >= 0 && y >= 0 && x < width && y < height) {
			int gx = x/font->gridX();
			int gy = y/font->gridY();
			int t = getTile(gx,gy);
			if (t == 0) return 0;
			return font->tile[t]->getPixel(x % font->gridX(),y % font->gridY());
		}
		return 0;
	}
	
	void show() {
		printf("width %d,height %d,string %s\n",width,height,str->c_str());
		int  y, x;
		for ( y = 0; y < gheight(); y++ ) {
			for ( x = 0; x < gwidth(); x++ ) {
				printf("%2d,",tile[x+y*gwidth()]);
//				font->tile[tile[x+y*gwidth()]]->show();
			}
			printf("\n");
		}
		for ( y = 0; y < height; y++ ) {
			for ( x = 0; x < width; x++ ) {
				putchar(getPixel(x,y) == 0 ? ' ' : getPixel(x,y) < 128 ? '+' : '*' );
			}
			putchar( '\n' );
		}
		printf("\n");
	}
	void changeHeight(int _height) {
		
	}
	void tileRetain() {
		for (int y=0;y<gheight();y++) {
			for (int x=0;x<gwidth();x++) {
				int t = tile[x+y*gwidth()];
				font->tile[t]->retainCount = PPTTFontImage::RETAIN_COUNT;
			}
		}
	}
	void tileRelease() {
		for (int y=0;y<gheight();y++) {
			for (int x=0;x<gwidth();x++) {
				int t = tile[x+y*gwidth()];
				font->tile[t]->retainCount = 0;
			}
		}
	}
	void retain() {
		retainCount = RETAIN_COUNT;
	}
	void checkEmptyBlock() {
		for (int y=0;y<gheight();y++) {
			for (int x=0;x<gwidth();x++) {
				int t = tile[x+y*gwidth()];
				if (font->tile[t]->pixelCount == 0) {
					font->tile[t]->retainCount = 0;
					tile[x+y*gwidth()] = 0;
				}
			}
		}
	}
	
	PPTTFont* font;
	
	int* tile;
	int width;
	int height;
	int retainCount;
//	int index;
	int advanceX;
	int bearingX;
	bool noRelease;
	
	std::string* str;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
