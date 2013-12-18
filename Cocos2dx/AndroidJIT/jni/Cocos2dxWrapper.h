#ifndef __COCOS2DX_WRAPPER__
#define __COCOS2DX_WRAPPER__

#include <string>

const char* ccPPGameResourcePath(const char* name);
unsigned char* ccPPGame_GetData(const char* key,int* dataSize);
void ccPPGame_SetData(const char* key,unsigned char* data,int dataSize);
const char* ccPPGameDocumentPath(const char* dbfile);
unsigned char* ccPPGame_LoadPNG(const char* fileName,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow);
unsigned char* ccPPGame_DecodePNG(unsigned char* bytes,unsigned long datalen,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow);
int ccPPGetInteger(const char* key,int defaultValue);
void ccPPSetInteger(const char* key,int value,bool sync);
float ccPPGetNumber(const char* key,float defaultValue);
void ccPPSetNumber(const char* key,float value,bool sync);
std::string ccPPGetString(const char* key,const std::string & defaultValue);
void ccPPSetString(const char* key,const std::string & value,bool sync);
const char* ccPPGame_LocaleString(const char* jp,const char* en);
unsigned char* ccGetFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
std::string ccGetWriteablePath();

#endif
