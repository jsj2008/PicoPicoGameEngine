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
#include "CCImage.h"
#include "CCUserDefault.h"
#include "CCApplication.h"
#include "CCFileUtils.h"
#include "png.h"
#include "PPGameDef.h"

//#include <android/log.h>
//#define  LOG_TAG    "main"
//#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define PNG_HEADER_SIZE 8

static int keySide = 0;
static char path[1024]={0};
static char path2[1024]={0};
static char path3[1024]={0};

const char* PPGameResourcePath(const char* name)
{
	return cocos2d::CCFileUtils::fullPathFromRelativePath(name);
}

//int PPGame_SetDefault(const char* name)
//{
//	if (strcmp(name,PPGAME_KEY_SIDE_RIGHT) == 0) {
//		keySide = 1;
//	} else
//	if (strcmp(name,PPGAME_KEY_SIDE_LEFT) == 0) {
//		keySide = 0;
//	}
//	return keySide;
//}

//int PPGame_GetDefault(const char* name)
//{
//	if (strcmp(name,PPGAME_KEY_SIDE_RIGHT) == 0) {
//		if (keySide == 1) return 1;
//	} else
//	if (strcmp(name,PPGAME_KEY_SIDE_LEFT) == 0) {
//		if (keySide == 0) return 1;
//	}
//	return 0;
//}

unsigned char* PPGame_GetData(const char* key,int* dataSize)
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

void PPGame_SetData(const char* key,unsigned char* data,int dataSize)
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

const char* PPGameDocumentPath(const char* dbfile)
{
	std::string path = cocos2d::CCFileUtils::getWriteablePath();
	std::string name = dbfile;
//	return path.c_str();
	path = path+name;
	strcpy(path3,path.c_str());
	return path3;
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

unsigned char* PPGame_LoadPNG(const char* fileName,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
	cocos2d::CCImage* img = new cocos2d::CCImage();
	img->initWithImageFile(fileName);
	unsigned char *pix = img->getData();
	unsigned long size = img->getWidth()*img->getHeight()*4;
	unsigned char * pixel = (unsigned char*)malloc(size);
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
	return convert(pixel,width,height);
}

unsigned char* PPGame_DecodePNG(unsigned char* bytes,unsigned long size,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
	if (bytes == NULL || size == 0) return NULL;
LOGD("PPGame_DecodePNG");
	my_png_buffer png_buff;
	png_buff.data_offset = 0;
	png_buff.data_len = size;
	png_buff.data = bytes;
	int is_png = png_check_sig((png_bytep)png_buff.data, 8);
	if (!is_png) {
		fprintf(stderr, "is not PNG!\n");
LOGD("is not PNG!");
		free(png_buff.data);
		return NULL;
	}
	png_structp PNG_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!PNG_reader) {
		fprintf(stderr, "can't create read_struct\n");
LOGD("can't create read_struct");
		free(png_buff.data);
		return NULL;
	}
	png_infop PNG_info = png_create_info_struct(PNG_reader);
	if (!PNG_info) {
		fprintf(stderr, "can't create info_struct\n");
LOGD("can't create info_struct");
		png_destroy_read_struct (&PNG_reader, NULL, NULL);
		free(png_buff.data);
		return NULL;
	}

    png_infop PNG_end_info = png_create_info_struct(PNG_reader);
    if (PNG_end_info == NULL)
    {
        fprintf(stderr, "Can't get end info for PNG data\n");
LOGD("Can't get end info for PNG data");
        png_destroy_read_struct(&PNG_reader, &PNG_info, NULL);
		free(png_buff.data);
		return NULL;
    }
    
    if (setjmp(png_jmpbuf(PNG_reader)))
    {
        fprintf(stderr, "Can't decode PNG data\n");
LOGD("Can't decode PNG data");
        png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
		free(png_buff.data);
		return NULL;
    }

	png_data_read(PNG_reader,&png_buff);
	png_read_info(PNG_reader,PNG_info);
	
    png_uint_32 width, height;
    width = png_get_image_width(PNG_reader, PNG_info);
    height = png_get_image_height(PNG_reader, PNG_info);

//printf("width %ld\n",width);
//printf("height %ld\n",height);
    
    png_uint_32 bit_depth, color_type;
    bit_depth = png_get_bit_depth(PNG_reader, PNG_info);
    color_type = png_get_color_type(PNG_reader, PNG_info);

//printf("bit_depth %ld\n",bit_depth);
//printf("rowbytes %ld\n",png_get_rowbytes(PNG_reader, PNG_info));

    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
	    png_set_palette_to_rgb(PNG_reader);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
    png_set_gray_to_rgb(PNG_reader);
    }
    if (png_get_valid(PNG_reader, PNG_info, PNG_INFO_tRNS))
    {
    png_set_tRNS_to_alpha(PNG_reader);
    }
    else
    {
    png_set_filler(PNG_reader, 0xff, PNG_FILLER_AFTER);
    }
    if (bit_depth == 16)
    {
        png_set_strip_16(PNG_reader);
    }
    
    png_read_update_info(PNG_reader, PNG_info);
    
    png_byte* PNG_image_buffer = (png_byte*)malloc(4 * width * height);
    png_byte** PNG_rows = (png_byte**)malloc(height * sizeof(png_byte*));
    
    unsigned int row;
    for (row = 0; row < height; ++row)
    {
        //PNG_rows[height - 1 - row] = PNG_image_buffer + (row * 4 * width);
        PNG_rows[row] = PNG_image_buffer + (row * 4 * width);
    }
    
    png_read_image(PNG_reader, PNG_rows);
    
    free(PNG_rows);
    
    png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);

	if (png_buff.data) free(png_buff.data);

    *imageWidth = width;
    *imageHeight = height;
	*bytesPerRow = width*4;

LOGD("PPGame_DecodePNG2");
	return convert(PNG_image_buffer,width,height);
}

int PPGetInteger(const char* key,int defaultValue)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	return def->getIntegerForKey(key,defaultValue);
}

void PPSetInteger(const char* key,int value)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	def->setIntegerForKey(key,value);
}

float PPGetNumber(const char* key,float defaultValue)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	return def->getFloatForKey(key,defaultValue);
}

void PPSetNumber(const char* key,float value)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	def->setFloatForKey(key,value);
}

std::string PPGetString(const char* key,const std::string & defaultValue)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	return def->getStringForKey(key,defaultValue);
}

void PPSetString(const char* key,const std::string & value)
{
	cocos2d::CCUserDefault* def = cocos2d::CCUserDefault::sharedUserDefault();
	def->setStringForKey(key,value);
}

int PPGame_InitBGM(int no,const char* key)
{
	return 0;
}

int PPGame_InitBGMiPad(int no,const char* key)
{
	return 0;
}

void PPGame_ReleaseBGM()
{
}

//int PPGame_GetSelectingBGM()
//{
//	return 0;
//}

//void PPGame_SetSelectingBGM(int flag)
//{
//}

//void PPGame_IdleBGM(void* controller,int playBGM,bool playBGMOneTime,int chooseBGM,int x,int y,int w,int h)
//{
//}

int PPGame_GetLocale()
{
	return QBGAME_LOCALE_JAPANESE;
	return QBGAME_LOCALE_OTHER;
}

const char* PPGame_LocaleString(const char* jp,const char* en)
{
	if (cocos2d::CCApplication::getCurrentLanguage() == cocos2d::kLanguageEnglish) {
		return en;
	}
	return jp;
}

int PPGame_LoadAIFF(const char* name,const char* type)
{
	return 0;
}

int PPGame_PlayAIFF(int soundId)
{
	return 0;
}

void PPGame_Vibrate()
{
}

int PPGame_3GSLater()
{
	return 0;
}

void __PPGame_Set3GSLater(int later)
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

const char* PPGameDataPath(const char* name)
{
	return PPGameResourcePath(name);
}

/*
void PPAdjustUVMap(QB3DModel* model,const char* layout)
{
}
*/

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
