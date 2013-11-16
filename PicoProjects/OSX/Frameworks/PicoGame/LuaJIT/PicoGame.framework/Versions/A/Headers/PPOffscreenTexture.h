/*-----------------------------------------------------------------------------------------------
	名前	PPOffscreenTexture.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPOffscreenTexture_h__
#define __PPOffscreenTexture_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
#include "PPGameSprite.h"

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;

class PPOffscreenTexture : public PPObject {
public:
	PPOffscreenTexture(PPWorld* world);
	virtual ~PPOffscreenTexture();
	
	virtual void init();
	virtual void start();
	virtual void create(PPSize size,PPGameTextureOption option);
	
	virtual void stepIdle();

	virtual void drawSelf(PPPoint pos);
	virtual PPSize size();

	virtual void openFrameBuffer();
	virtual void closeFrameBuffer();

	//virtual void render();

	virtual void lock(bool sync=true);
	virtual void unlock(bool sync=true);
	
	virtual void line(PPPoint pos1,PPPoint pos2,PPColor col);
	virtual void box(PPRect rect,PPColor col);
	virtual void fill(PPRect rect,PPColor col);
	virtual void paint(PPPoint pos,PPColor col,PPColor borderColor);
	virtual void paint(PPPoint pos,PPColor col);
	virtual void circle(PPPoint pos,float r,PPColor col,float start,float end);
	virtual void hflip();
	virtual void vflip();
	
	virtual void pset(PPPoint pos,PPColor col);
	virtual PPColor pget(PPPoint pos,PPColor col=PPColor::black());

	unsigned char* pixel(PPPoint pos);
	unsigned char* pixel();

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

	static std::string className;
	static PPObject* registClass(PPLuaScript* script,const char* name=NULL,const char* superclass=NULL);
	static PPObject* registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass=NULL);
	
	PPSize imageSize;
	PPSize textureSize;
	unsigned char* _pixel;
	//bool updated;
	GLint viewport[4];
	GLuint colorbuffer_name;
	GLuint framebuffer_name;
	//GLint oldframebuffer;
	//PPGameState oldstatus;
	bool firstFrame;
	bool update;
	
	PPGameTextureOption option;

	//void textureUpdate();
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
