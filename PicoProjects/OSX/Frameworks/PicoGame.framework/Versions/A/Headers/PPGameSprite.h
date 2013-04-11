/*-----------------------------------------------------------------------------------------------
	名前	PPGameSprite.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "PPGameTexture.h"
#include "PPWorld.h"

class PPLuaScript;

#define PPGAME_VBO_NUM (4000*10)

#define PPFLIP_H 1
#define PPFLIP_V 2
#define PPFLIP_R90 (1<<2)
#define PPFLIP_R180 (2<<2)
#define PPFLIP_R270 (3<<2)
#define PPFLIP_RMASK (3<<2)

#define PPGAME_MAX_BLEND 1000

#ifdef _WIN32
#define ppGenRenderbuffers glGenRenderbuffersEXT
#define ppBindRenderbuffer glBindRenderbufferEXT
#define ppRenderbufferStorage glRenderbufferStorageEXT
#define ppGenFramebuffers glGenFramebuffersEXT
#define ppBindFramebuffer glBindFramebufferEXT
#define ppFramebufferTexture2D glFramebufferTexture2DEXT
#define ppFramebufferRenderbuffer glFramebufferRenderbufferEXT
#define ppDeleteRenderbuffers glDeleteRenderbuffersEXT
#define ppDeleteFramebuffers glDeleteFramebuffersEXT
#define PP_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#define PP_RENDERBUFFER GL_RENDERBUFFER_EXT
#define PP_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#define PP_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
#define PP_DEPTH_COMPONENT GL_DEPTH_COMPONENT
#define PP_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_EXT
#elif TARGET_OS_IPHONE
#define ppGenRenderbuffers glGenRenderbuffersOES
#define ppBindRenderbuffer glBindRenderbufferOES
#define ppRenderbufferStorage glRenderbufferStorageOES
#define ppGenFramebuffers glGenFramebuffersOES
#define ppBindFramebuffer glBindFramebufferOES
#define ppFramebufferTexture2D glFramebufferTexture2DOES
#define ppFramebufferRenderbuffer glFramebufferRenderbufferOES
#define ppDeleteRenderbuffers glDeleteRenderbuffersOES
#define ppDeleteFramebuffers glDeleteFramebuffersOES
#define PP_FRAMEBUFFER GL_FRAMEBUFFER_OES
#define PP_RENDERBUFFER GL_RENDERBUFFER_OES
#define PP_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
#define PP_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_OES
#define PP_DEPTH_COMPONENT GL_DEPTH_COMPONENT16_OES
#define PP_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_OES
#else
#ifdef _ANDROID
#define ppGenRenderbuffers glGenRenderbuffersOES
#define ppBindRenderbuffer glBindRenderbufferOES
#define ppRenderbufferStorage glRenderbufferStorageOES
#define ppGenFramebuffers glGenFramebuffersOES
#define ppBindFramebuffer glBindFramebufferOES
#define ppFramebufferTexture2D glFramebufferTexture2DOES
#define ppFramebufferRenderbuffer glFramebufferRenderbufferOES
#define ppDeleteRenderbuffers glDeleteRenderbuffersOES
#define ppDeleteFramebuffers glDeleteFramebuffersOES
#define PP_FRAMEBUFFER GL_FRAMEBUFFER_OES
#define PP_RENDERBUFFER GL_RENDERBUFFER_OES
#define PP_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
#define PP_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_OES
#define PP_DEPTH_COMPONENT GL_DEPTH_COMPONENT16_OES
#define PP_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_OES
#else
#define ppGenRenderbuffers glGenRenderbuffersEXT
#define ppBindRenderbuffer glBindRenderbufferEXT
#define ppRenderbufferStorage glRenderbufferStorageEXT
#define ppGenFramebuffers glGenFramebuffersEXT
#define ppBindFramebuffer glBindFramebufferEXT
#define ppFramebufferTexture2D glFramebufferTexture2DEXT
#define ppFramebufferRenderbuffer glFramebufferRenderbufferEXT
#define ppDeleteRenderbuffers glDeleteRenderbuffersEXT
#define ppDeleteFramebuffers glDeleteFramebuffersEXT
#define PP_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#define PP_RENDERBUFFER GL_RENDERBUFFER_EXT
#define PP_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#define PP_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
#define PP_DEPTH_COMPONENT GL_DEPTH_COMPONENT
#define PP_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_EXT
#endif
#endif

enum {
	PPGameBlend_None,
	PPGameBlend_Light,
	PPGameBlend_BlackMask,
	PPGameBlend_Flash,
	PPGameBlend_Red,
	PPGameBlend_NoMask,
	PPGameBlend_Color,
	PPGameBlend_EdgeSmooth,
};

typedef struct _PPGamePolyStack {
	int ptr;
	int type;
	float alphaValue;
	int blendType;
} PPGamePolyStack;

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameState {
public:
	static const unsigned long UseLocalScale=0x1;

	void init() {
		_alpha = 1.0;
		_blendType = PPGameBlend_None;
		screenWidth = windowSize.width;
		screenHeight = windowSize.height;
		b_ptr = 0;
		b_stack_ptr = 0;
		b_start_ptr = 0;
		s_type = 1000;
		s_texId = 1000;
		s_alphaValue = 1.0;
		s_blendType = 1;
		offset = PPPointZero;
		offset2 = PPPointZero;
		origin = PPPointZero;
		scale = PPPoint(1,1);
		flags = 0;
		localScale = PPPoint(1,1);
		viewPort = PPRect(0,0,windowSize.width,windowSize.height);
		rotate = 0;
		renderbuffer = 0;
		framebuffer = 0;
	}

	unsigned long flags;

	float _alpha;
	int _blendType;
	PPPoint offset;
	PPPoint origin;
	PPPoint scale;
	PPPoint localScale;
	PPPoint offset2;
	float rotate;
	double screenWidth;
	double screenHeight;
	PPSize windowSize;
	int s_type;
	int s_texId;
	float s_alphaValue;
	int s_blendType;
	int b_ptr;
	int b_stack_ptr;
	int b_start_ptr;
	int blendCount;
	GLuint renderbuffer;
	GLuint framebuffer;
	PPRect viewPort;

	GLint glviewport[4];
	GLint gloldframebuffer;

	int start_ptr;
	int start_stack_ptr;

};

class PPGameSprite {
public:
	enum {
		MAX_ST_STACK = 1
	};

//	static PPGameSprite* select(PPGameSprite* projector);
//	static PPGameSprite* instance();
//	static void deleteInstance();

	PPGameSprite();
	virtual ~PPGameSprite();

	void setWorld(PPWorld* world) {
		world->projector = this;
		target = world;
	}
	
	PPWorld* world() {
		return target;
//		return PPWorld::sharedWorld();
	}

	int Init();
	int Exit();
	int InitOT();

	virtual int DrawOT();
	virtual void RestartOT();
	virtual void ResetOT();

	void CalcPolyPoint(PPGamePoly* poly,PPRect* size,PPPoint* delta,PPPoint* p);
	int DrawPoly(PPGamePoly* poly);

	virtual int BlendOn(float alpha=1.0,int type=0);
	int BlendOff();
	int PushStack(int type,float alphaValue=1.0,int blendType=0);
	int ClearScreen2D(GLfloat r,GLfloat g,GLfloat b);
	virtual void SetViewPort();
	void resetBlendType();
	int getBlendType(PPGamePoly* poly);
	void viewPort(PPRect rect);
	PPRect viewPort();
	void PreCalc(PPPoint *pos,PPPoint *outpos);
	int CaptureScreen(int x,int y,int w,int h,int rowbyte,unsigned char* pixel);
	void Clear();

//	float _alpha;
//	int _blendType;
//	
//	PPPoint offset;
//	PPPoint origin;
//	PPPoint scale;
//	float rotate;
	
//	double screenWidth;
//	double screenHeight;
	
//	int s_type;
//	int s_texId;
//	float s_alphaValue;
//	int s_blendType;
	
	bool initLoadTex;
	bool reloadTex;
	
	PPGameTextureManager* textureManager;

	void InitBuffer(int size) {
		if (size != b_size) {
			if (b_vert) free(b_vert);
			if (b_uv) free(b_uv);
			if (b_color) free(b_color);
			
			b_size = size;
			b_vert = (GLfloat*)malloc(6*2*size*sizeof(GLfloat));
			b_uv = (GLfloat*)malloc(6*2*size*sizeof(GLfloat));
			b_color = (GLubyte*)malloc(6*4*size*sizeof(GLubyte));
		}
	}

	int b_size;
	GLfloat* b_vert;
	GLfloat* b_uv;
	GLubyte* b_color;

	PPGamePolyStack b_stack[PPGAME_VBO_NUM];
//	int b_stack_ptr;
//	int b_start_ptr;
//	int b_ptr;
	
	PPGameBlend blendTable[PPGAME_MAX_BLEND];
//	int blendCount;

	PPGameState st;
	PPGameState oldst[MAX_ST_STACK];
	int oldstptr;
	int animationFrameInterval;
	
	void unlockoffscreen() {
		if (oldstptr > 0) {
			oldstptr = 0;
			st = oldst[oldstptr];

			ppBindFramebuffer(PP_FRAMEBUFFER,oldst[oldstptr].gloldframebuffer);

			glViewport(st.glviewport[0],st.glviewport[1],st.glviewport[2],st.glviewport[3]);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

private:
	int Line(PPGamePoly* poly);
	int Fill(PPGamePoly* poly);
	int Box(PPGamePoly* poly);
	int ViewPort(PPGamePoly* poly);
	int Draw(PPGamePoly* poly);
	int DrawCore(PPGamePoly* poly,PPPoint pos,PPSize size,PPPoint delta);
	
	PPWorld* target;
};

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
