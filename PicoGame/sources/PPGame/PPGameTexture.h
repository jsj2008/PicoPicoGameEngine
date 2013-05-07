/*-----------------------------------------------------------------------------------------------
	名前	PPGameTexture.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGameTexture_h__
#define __PPGameTexture_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#ifdef _WIN32
#include <windows.h>
#include <GL/glew.h>
#elif TARGET_OS_IPHONE
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#ifdef _ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#endif

#include "PPGameDef.h"
#include "PPGamePoly.h"
#include "PPTextureArray.h"
#include "PPGameGeometry.h"

extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
}

#define PPGAME_MAX_TEXTURE 128

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPGameTextureManager;

class PPGameTexture {
public:
	friend class PPGameSprite;
	friend class PPGameTextureManager;

	PPGameTexture();
	virtual ~PPGameTexture();
	
	int index;
	
	PPGameTextureOption option;

	std::string name;
	PPGameImage* group;
	
	PPGameTextureManager* manager;
	
	unsigned char* pixel;
	int width;
	int height;
	
	bool notexturefile;
	
	float width100;
	float height100;
	float invwidth;
	float invheight;
	
	int rowbytes;
	PPSize size;
	bool nofree;
	bool compressed;
	
	GLuint texture_name;

	int loadTexture();
	int bindTexture();
	int unbindTexture();

//	static int ReloadAllTexture();

//	static int ReleaseAllTexture();
//	static int UnbindAllTexture();
//	static int UnbindAllTextureForDebug();
//
//	static void idle();
//	static int BindTexture(int textureid);
//	static int UnbindTexture(int textureid);
//	static int CountTexture();
//	static int DeleteTexture(int textureid);
//	static void DeleteTextureForDevelop(int textureid);
//	static int LoadTexture(const char* filename,PPGameTextureOption option);
//	static int LoadTexture(int index);
//	static int FreeTexture(int index);
//	static GLuint GetTextureName(int index);
//	static const char* GetName(int index);
//	static PPSize GetTextureSize(int index);
//	static PPSize GetImageSize(int index);
//	static PPSize PatternSize(int textureid,int group);
//	static bool checkBind(int texture);
//	static int SetTexture(PPGameTexture* _texture);
//	static int SetTextureList(PPGameTextureInfo* texList);
//	static bool SetTexture(int index,unsigned char* pixel,unsigned long width,unsigned long height,unsigned long bytesPerRow,PPGameTextureOption option);
//	static unsigned char* loadPNG(unsigned char* pixel,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow);
//	static void ResetTextureState();
//	static void DisableTexture();
//	static void EnableTexture(int texNo=0);
//	static int GetTextureCount();
//	static PPGameTexture* GetTexture(int index);
//	static void SetDefaultTexture(int texture);
//	static int DefaultTexture();

//	static int defaultTexture;
//	static PPGameTexture* texture[PPGAME_MAX_TEXTURE];
	
//	static void dump();
private:
	bool loaded;
};

class PPFontManager;

class PPGameTextureManager {
public:
//	static PPGameTextureManager* select(PPGameTextureManager* manager);

	PPGameTextureManager();
	virtual ~PPGameTextureManager();

	PPGameTexture* texture[PPGAME_MAX_TEXTURE];
//	bool boot;
	PPFontManager* fontManager;

	int texId;
	int defaultTexture;

	bool setTexture(int index,unsigned char* pixel,unsigned long width,unsigned long height,unsigned long bytesPerRow,PPGameTextureOption option);
	int freeTexture(int index);
	int unbindAllTexture();
	int unbindAllTextureForDebug();
	int reloadAllTexture();
	int bindTexture(int i);
	int unbindTexture(int i);
	int countTexture();
	int releaseAllTexture();
	int deleteTexture(int textureid);
	void deleteTextureForDevelop(int textureid);
	int loadTexture(const char* filename,PPGameTextureOption option);
	int loadTexture(int i);
	void idle();
	GLuint getTextureName(int index);
	const char* getName(int index);
	PPSize getTextureSize(int index);
	PPSize getImageSize(int index);
	PPSize patternSize(int textureid,int group);
	bool checkBind(int index);
	int setTexture(PPGameTexture* _texture);
	int setTextureWithNameCheck(PPGameTexture* _texture);
	int setTextureList(PPGameTextureInfo* texList);
	int getTextureCount();
	void resetTextureState();
	void disableTexture();
	void enableTexture(int texNo=0);
	PPGameTexture* getTexture(int index);
	static unsigned char* loadPNG(unsigned char* pixel,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow);
	void dump();

	void pushTextureInfo(lua_State* L,int tex);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
