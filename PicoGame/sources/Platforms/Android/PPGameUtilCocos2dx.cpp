/*-----------------------------------------------------------------------------------------------
	名前	PPGameUtilCocos2dx.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameUtil.h"
#include "Cocos2dxWrapper.h"
#include "png.h"
#include "PPGameDef.h"
#ifdef __LUAJIT__
#include <lua.hpp>
#define LUA_OK 0
#else
extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lua/ldebug.h>
}
#endif

#define PNG_HEADER_SIZE 8

const char* PPGameResourcePath(const char* name)
{
  return ccPPGameResourcePath(name);
}

unsigned char* PPGame_GetData(const char* key,int* dataSize)
{
  return ccPPGame_GetData(key,dataSize);
}

void PPGame_SetData(const char* key,unsigned char* data,int dataSize)
{
  return ccPPGame_SetData(key,data,dataSize);
}

const char* PPGameDocumentPath(const char* dbfile)
{
  return ccPPGameDocumentPath(dbfile);
}

const char* PPGameLoadShader(const char* name)
{
	return "";
}

typedef struct my_png_buffer_ {
	unsigned char *data;
	unsigned long data_len;
	unsigned long data_offset;
} my_png_buffer;

static void png_memread_func(png_structp png_ptr, png_bytep buf, png_size_t size){
	my_png_buffer *png_buff = (my_png_buffer *)png_get_io_ptr(png_ptr);
	if (png_buff->data_offset + size <= png_buff->data_len) {
		memcpy(buf, png_buff->data + png_buff->data_offset, size);
		png_buff->data_offset += size;
	} else {
		png_error(png_ptr,"png_mem_read_func failed");
	}
}

static void png_data_read(png_structp png_ptr, my_png_buffer *png_buff) {
	png_set_read_fn(png_ptr, (png_voidp) png_buff, (png_rw_ptr)png_memread_func);
}

unsigned char* PPGame_LoadPNG(const char* fileName,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
  return ccPPGame_LoadPNG(fileName,imageWidth,imageHeight,bytesPerRow);
}

unsigned char* PPGame_DecodePNG(unsigned char* bytes,unsigned long datalen,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
  return ccPPGame_DecodePNG(bytes,datalen,imageWidth,imageHeight,bytesPerRow);
}

int PPGetInteger(const char* key,int defaultValue)
{
  return ccPPGetInteger(key,defaultValue);
}

void PPSetInteger(const char* key,int value,bool sync)
{
  ccPPSetInteger(key,value,sync);
}

float PPGetNumber(const char* key,float defaultValue)
{
  return ccPPGetNumber(key,defaultValue);
}

void PPSetNumber(const char* key,float value,bool sync)
{
  ccPPSetNumber(key,value,sync);
}

std::string PPGetString(const char* key,const std::string & defaultValue)
{
  return ccPPGetString(key,defaultValue);
}

void PPSetString(const char* key,const std::string & value,bool sync)
{
  return ccPPSetString(key,value,sync);
}

void PPSync()
{
}

int PPGame_GetLocale()
{
	return QBGAME_LOCALE_JAPANESE;
	return QBGAME_LOCALE_OTHER;
}

const char* PPGame_LocaleString(const char* jp,const char* en)
{
  return ccPPGame_LocaleString(jp,en);
}

void PPGame_Vibrate()
{
}

const char* PPGameMainLua()
{
	return "main.lua";
}

void PPGameSetMainLua(const char* luaname)
{
}

void PPGameSetDataPath(const char* datapath)
{
}

void PPGameSetDataSubPath(const char* datapath)
{
}

const char* PPGameDataPath(const char* name)
{
	return PPGameResourcePath(name);
}

const char* PPGameDataSubPath(const char* name)
{
  return PPGameDataPath(name);
}

/*
void PPAdjustUVMap(QB3DModel* model,const char* layout)
{
}
*/

bool PPGameControllerAvailable()
{
  return false;
}

void PPGameControllerStartDiscoverty()
{
}

void PPGameControllerStopDiscoverty()
{
}

int PPGameControllerCount()
{
  return 0;
}

int PPGameControllerInfo(void* script,int index)
{
  lua_State* L = (lua_State*)script;
  lua_createtable(L,0,0);
  return 1;
}

void PPGameControllerSetPlayerIndex(int index,int playerIndex)
{
}

int PPGameControllerGetPlayerIndex(int index)
{
  return 0;
}

void PPGameOpenWeb(const char* url,const char* title)
{
}

void PPSetCustomPlistPath(const char* path)
{
}

const char* PPGetCustomPlistPath(void)
{
  return NULL;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
