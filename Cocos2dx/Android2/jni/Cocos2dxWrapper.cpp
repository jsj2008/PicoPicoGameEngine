#include "Cocos2dxWrapper.h"

#include "CCImage.h"
#include "CCUserDefault.h"
//#include "CCApplication.h"
#include "CCFileUtils.h"
#include "png.h"
#include "SystemInfoJni.h"

#if 0
#include <android/log.h>
#define  LOG_TAG    "Cocos2dxWrapper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...) 
#endif

static char path[1024]={0};
static char path2[1024]={0};
static char path3[1024]={0};

static unsigned char* convert(unsigned char* image,png_uint_32 width,png_uint_32 height)
{
	if (image) {
		unsigned char* pixel = (unsigned char*)malloc(height*width*4);
		if (pixel) {
			int i,j;
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					unsigned char r,g,b,a;
					r = image[j*4+0+i*width*4];
					g = image[j*4+1+i*width*4];
					b = image[j*4+2+i*width*4];
					a = image[j*4+3+i*width*4];
					if (a == 0) {
						pixel[j*4+0+i*width*4] = 0;
						pixel[j*4+1+i*width*4] = 0;
						pixel[j*4+2+i*width*4] = 0;
						pixel[j*4+3+i*width*4] = 0;
					} else {
						pixel[j*4+0+i*width*4] = r;
						pixel[j*4+1+i*width*4] = g;
						pixel[j*4+2+i*width*4] = b;
						pixel[j*4+3+i*width*4] = a;
					}
				}
			}
			free(image);
			image = pixel;
		}
	}
	return image;
}

const char* ccPPGameResourcePath(const char* name)
{
//LOGD("PPGameResourcePath %s(1)",name);
	const char* r = cocos2d::CCFileUtils::fullPathFromRelativePath(name);
//LOGD("PPGameResourcePath %s(2)",name);
	return r;
}

unsigned char* ccPPGame_GetData(const char* key,int* dataSize)
{
	std::string path = cocos2d::CCFileUtils::getWriteablePath();
	sprintf(path2,"PPG_%s.param",key);
	std::string name = path2;
	path = path+name;
	FILE* fp = fopen(path.c_str(),"rb");
	if (fp) {
		fseek(fp,0,SEEK_END);
		int size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		unsigned char* str = (unsigned char*)malloc(size);
		fread(str,size,1,fp);
		fclose(fp);
		*dataSize = size;
		return str;
	}
	return NULL;
}

void ccPPGame_SetData(const char* key,unsigned char* data,int dataSize)
{
	std::string path = cocos2d::CCFileUtils::getWriteablePath();
	sprintf(path2,"PPG_%s.param",key);
	std::string name = path2;
	FILE* fp = fopen(path.c_str(),"wb");
	if (fp) {
		fwrite(data,dataSize,1,fp);
		fclose(fp);
	}
}

const char* ccPPGameDocumentPath(const char* dbfile)
{
	std::string path = cocos2d::CCFileUtils::getWriteablePath();
	std::string name = dbfile;
//	return path.c_str();
	path = path+name;
	strcpy(path3,path.c_str());
	return path3;
}

unsigned char* ccPPGame_LoadPNG(const char* fileName,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
LOGD("PPGame_LoadPNG %s",fileName);
	cocos2d::CCImage* img = new cocos2d::CCImage();
	if (img==NULL) {
LOGD("PPGame_LoadPNG error1");
		return NULL;
	}
	img->initWithImageFile(fileName);
	unsigned char *pix = img->getData();
	if (pix==NULL) {
LOGD("PPGame_LoadPNG error2");
		return NULL;
	}
	unsigned long size = img->getWidth()*img->getHeight()*4;
	unsigned char * pixel = (unsigned char*)malloc(size);
	if (pixel==NULL) {
LOGD("PPGame_LoadPNG error3");
		return NULL;
	}
	if (img->hasAlpha()) {
		memcpy(pixel,pix,size);
	} else {
		png_uint_32 width=img->getWidth();
		png_uint_32 height=img->getHeight();
		int i,j;
		int cm=3;
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				unsigned char r,g,b,a;
				r = pix[j*cm+0+i*width*cm];
				g = pix[j*cm+1+i*width*cm];
				b = pix[j*cm+2+i*width*cm];
				a = 255;
				if (a == 0) {
					pixel[j*4+0+i*width*4] = 0;
					pixel[j*4+1+i*width*4] = 0;
					pixel[j*4+2+i*width*4] = 0;
					pixel[j*4+3+i*width*4] = 0;
				} else {
					pixel[j*4+0+i*width*4] = r;
					pixel[j*4+1+i*width*4] = g;
					pixel[j*4+2+i*width*4] = b;
					pixel[j*4+3+i*width*4] = a;
				}
			}
		}
	}
	*imageWidth = img->getWidth();
	*imageHeight = img->getHeight();
	*bytesPerRow = img->getWidth()*4;
	int width = img->getWidth();
	int height = img->getHeight();
	delete img;
LOGD("PPGame_LoadPNG out %s",fileName);
	return convert(pixel,width,height);
}

unsigned char* ccPPGame_DecodePNG(unsigned char* bytes,unsigned long datalen,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
	if (bytes == NULL || datalen == 0) return NULL;
LOGD("PPGame_DecodePNG");
	cocos2d::CCImage* img = new cocos2d::CCImage();
	if (img==NULL) {
LOGD("PPGame_DecodePNG error1");
		return NULL;
	}
	img->initWithImageData(bytes,datalen);
	unsigned char *pix = img->getData();
	if (pix==NULL) {
LOGD("PPGame_DecodePNG error2");
		return NULL;
	}
	unsigned long size = img->getWidth()*img->getHeight()*4;
	unsigned char * pixel = (unsigned char*)malloc(size);
	if (pixel==NULL) {
LOGD("PPGame_DecodePNG error3");
		return NULL;
	}
	if (img->hasAlpha()) {
		memcpy(pixel,pix,size);
	} else {
		png_uint_32 width=img->getWidth();
		png_uint_32 height=img->getHeight();
		int i,j;
		int cm=3;
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				unsigned char r,g,b,a;
				r = pix[j*cm+0+i*width*cm];
				g = pix[j*cm+1+i*width*cm];
				b = pix[j*cm+2+i*width*cm];
				a = 255;
				if (a == 0) {
					pixel[j*4+0+i*width*4] = 0;
					pixel[j*4+1+i*width*4] = 0;
					pixel[j*4+2+i*width*4] = 0;
					pixel[j*4+3+i*width*4] = 0;
				} else {
					pixel[j*4+0+i*width*4] = r;
					pixel[j*4+1+i*width*4] = g;
					pixel[j*4+2+i*width*4] = b;
					pixel[j*4+3+i*width*4] = a;
				}
			}
		}
	}
	*imageWidth = img->getWidth();
	*imageHeight = img->getHeight();
	*bytesPerRow = img->getWidth()*4;
	int width = img->getWidth();
	int height = img->getHeight();
	delete img;
LOGD("PPGame_DecodePNG out");
	return convert(pixel,width,height);
}

int ccPPGetInteger(const char* key,int defaultValue)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	return def->getIntegerForKey(key,defaultValue);
}

void ccPPSetInteger(const char* key,int value,bool sync)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	def->setIntegerForKey(key,value);
}

float ccPPGetNumber(const char* key,float defaultValue)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	return def->getFloatForKey(key,defaultValue);
}

void ccPPSetNumber(const char* key,float value,bool sync)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	def->setFloatForKey(key,value);
}

std::string ccPPGetString(const char* key,const std::string & defaultValue)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	return def->getStringForKey(key,defaultValue);
}

void ccPPSetString(const char* key,const std::string & value,bool sync)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	def->setStringForKey(key,value);
}

static cocos2d::ccLanguageType getCurrentLanguage()
{
    const char *pLanguageName = getCurrentLanguageJNI();
    cocos2d::ccLanguageType ret = cocos2d::kLanguageEnglish;

    if (0 == strcmp("zh", pLanguageName))
    {
        ret = cocos2d::kLanguageChinese;
    }
    else if (0 == strcmp("en", pLanguageName))
    {
        ret = cocos2d::kLanguageEnglish;
    }
    else if (0 == strcmp("fr", pLanguageName))
    {
        ret = cocos2d::kLanguageFrench;
    }
    else if (0 == strcmp("it", pLanguageName))
    {
        ret = cocos2d::kLanguageItalian;
    }
    else if (0 == strcmp("de", pLanguageName))
    {
        ret = cocos2d::kLanguageGerman;
    }
    else if (0 == strcmp("es", pLanguageName))
    {
        ret = cocos2d::kLanguageSpanish;
    }
	else if (0 == strcmp("ru", pLanguageName))
    {
        ret = cocos2d::kLanguageRussian;
    }
    
    return ret;
}

const char* ccPPGame_LocaleString(const char* jp,const char* en)
{
	if (getCurrentLanguage() == cocos2d::kLanguageEnglish) {
		return en;
	}
	return jp;
}

unsigned char* ccGetFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
	return cocos2d::CCFileUtils::getFileData(pszFileName,pszMode,pSize);
}

std::string ccGetWriteablePath()
{
	return cocos2d::CCFileUtils::getWriteablePath();
}
