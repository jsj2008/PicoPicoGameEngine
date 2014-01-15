/*-----------------------------------------------------------------------------------------------
	名前	PPGameUtilLinux.cpp
	説明		        
	作成	2013.11.03 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameUtil.h"
#include <stdlib.h>
#include <stdio.h>
#include "png.h"
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

//static char fpath[1024];

static const char* WindowsProgramName()
{
	return "PicoPicoGames";
}

extern "C" {
void getApplicationPath(char* buffer,int length);
};

void getApplicationPath(char* buffer,int length)
{
}

static int QBSetDefault(const char* key,char* value)
{
	return -1;
}

static int QBGetDefault(const char* key,char** value,int* length,const char* def)
{
	return -1;
}

static std::string __watchDataPath;
static std::string __tempPath;

const char* PPGameResourcePath(const char* name)
{
	__tempPath = __watchDataPath;
	__tempPath += name;
	return __tempPath.c_str();
}

const char* PPGameDocumentPath(const char* dbfile)
{
	__tempPath = __watchDataPath;
	__tempPath += dbfile;
	return __tempPath.c_str();
}

int PPGetInteger(const char* key,int defaultValue)
{
	char def[1024];
	sprintf(def,"%d",defaultValue);
	char* svalue;
	int size;
	QBGetDefault(key,&svalue,&size,def);
	int value = defaultValue;
	if (svalue) {
		value = atoi(svalue);
		free(svalue);
	}
	return value;
}

void PPSetInteger(const char* key,int value,bool sync)
{
	char svalue[1024];
	sprintf(svalue,"%d",value);
	QBSetDefault(key,svalue);
}

float PPGetNumber(const char* key,float defaultValue)
{
	char def[1024];
	sprintf(def,"%f",defaultValue);
	char* svalue;
	int size;
	QBGetDefault(key,&svalue,&size,def);
	float value = defaultValue;
	if (svalue) {
		value = atof(svalue);
		free(svalue);
	}
	return value;
}

void PPSetNumber(const char* key,float value,bool sync)
{
	char svalue[1024];
	sprintf(svalue,"%f",value);
	QBSetDefault(key,svalue);
}

std::string PPGetString(const char* key,const std::string & defaultValue)
{
	char* svalue;
	int size;
	QBGetDefault(key,&svalue,&size,defaultValue.c_str());
	std::string str = defaultValue;
	if (svalue) {
		str = svalue;
		free(svalue);
	}
	return str;
}

void PPSetString(const char* key,const std::string & value,bool sync)
{
	QBSetDefault(key,(char*)value.c_str());
}

void PPSync()
{
}

unsigned char* PPGame_GetData(const char* key,int* dataSize)
{
	const char* path = PPGameResourcePath(key);
	FILE* fp = fopen(path,"rb");
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
	const char* path = PPGameResourcePath(key);
	FILE* fp = fopen(path,"wb");
	if (fp) {
		fwrite(data,dataSize,1,fp);
		fclose(fp);
	}
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

	FILE *PNG_file = fopen(fileName, "rb");

//    fprintf(stderr, "open PNG file %s\n", fileName);

    if (PNG_file == NULL)
    {
        fprintf(stderr, "Can't open PNG file %s\n", fileName);
		return NULL;
    }
    
    unsigned char PNG_header[PNG_HEADER_SIZE];
    
    fread(PNG_header, 1, PNG_HEADER_SIZE, PNG_file);
    if (png_sig_cmp(PNG_header, 0, PNG_HEADER_SIZE) != 0)
    {
        fprintf(stderr, "%s is not a PNG file\n", fileName);
        fclose(PNG_file);
		return NULL;
    }
/*	
	{
		for (int i=0;i<8;i++) {
			printf("%02X,",PNG_header[i]);
		}
		printf("\n");
	}
*/
    png_structp PNG_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (PNG_reader == NULL)
    {
        fprintf(stderr, "Can't start reading PNG file %s\n", fileName);
        fclose(PNG_file);
		return NULL;
    }

    png_infop PNG_info = png_create_info_struct(PNG_reader);
    if (PNG_info == NULL)
    {
        fprintf(stderr, "Can't get info for PNG file %s\n", fileName);
        png_destroy_read_struct(&PNG_reader, NULL, NULL);
        fclose(PNG_file);
		return NULL;
    }

    png_infop PNG_end_info = png_create_info_struct(PNG_reader);
    if (PNG_end_info == NULL)
    {
        fprintf(stderr, "Can't get end info for PNG file %s\n", fileName);
        png_destroy_read_struct(&PNG_reader, &PNG_info, NULL);
        fclose(PNG_file);
		return NULL;
    }
    
    if (setjmp(png_jmpbuf(PNG_reader)))
    {
        fprintf(stderr, "Can't load PNG file %s\n", fileName);
        png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
        fclose(PNG_file);
		return NULL;
    }
    
    png_init_io(PNG_reader, PNG_file);
    png_set_sig_bytes(PNG_reader, PNG_HEADER_SIZE);
    
    png_read_info(PNG_reader, PNG_info);

	if (PNG_reader == NULL) {
        fclose(PNG_file);
		return NULL;
    }
    
    png_uint_32 width, height;
    width = png_get_image_width(PNG_reader, PNG_info);
    height = png_get_image_height(PNG_reader, PNG_info);

//printf("width %d\n",width);
//printf("height %d\n",height);
    
    png_uint_32 bit_depth, color_type;
    bit_depth = png_get_bit_depth(PNG_reader, PNG_info);
    color_type = png_get_color_type(PNG_reader, PNG_info);

//printf("bit_depth %d\n",bit_depth);
//printf("rowbytes %d\n",png_get_rowbytes(PNG_reader, PNG_info));
    
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
    //png_set_palette_to_rgb(PNG_reader);
    png_set_palette_to_rgb(PNG_reader);
    }
/*
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) 
    {
    png_set_gray_1_2_4_to_8(PNG_reader);
    }
*/
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
    fclose(PNG_file);

    *imageWidth = width;
    *imageHeight = height;
	*bytesPerRow = width*4;

	return convert(PNG_image_buffer,width,height);
}

unsigned char* PPGame_DecodePNG(unsigned char* bytes,unsigned long size,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
	if (bytes == NULL || size == 0) return NULL;
	my_png_buffer png_buff;
	memset(&png_buff,0,sizeof(png_buff));
	png_buff.data_len = size;
	png_buff.data = bytes;
	int is_png = png_check_sig((png_bytep)png_buff.data, 8);
	if (!is_png) {
		fprintf(stderr, "is not PNG!\n");
		free(png_buff.data);
		return NULL;
	}
	png_structp PNG_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
	if (!PNG_reader) {
		fprintf(stderr, "can't create read_struct\n");
		free(png_buff.data);
		return NULL;
	}
	png_infop PNG_info = png_create_info_struct(PNG_reader);
	if (!PNG_info) {
		fprintf(stderr, "can't create info_struct\n");
		png_destroy_read_struct (&PNG_reader, NULL, NULL);
		free(png_buff.data);
		return NULL;
	}

    png_infop PNG_end_info = png_create_info_struct(PNG_reader);
    if (PNG_end_info == NULL)
    {
        fprintf(stderr, "Can't get end info for PNG data\n");
        png_destroy_read_struct(&PNG_reader, &PNG_info, NULL);
		free(png_buff.data);
		return NULL;
    }
    
    if (setjmp(png_jmpbuf(PNG_reader)))
    {
        fprintf(stderr, "Can't decode PNG data\n");
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

	return convert(PNG_image_buffer,width,height);
}

const char* PPGameLoadShader(const char* name)
{
	return NULL;
}

int PPGame_GetLocale()
{
	return QBGAME_LOCALE_JAPANESE;
}

const char* PPGame_LocaleString(const char* jp,const char* en)
{
	if (PPGame_GetLocale() == QBGAME_LOCALE_JAPANESE) {
		return jp;
	}
	return en;
}

void PPGame_Vibrate()
{
}

int PPGame_3GSLater()
{
	return 0;
}

static std::string __luaFilePath="main.lua";

const char* PPGameMainLua()
{
	return __luaFilePath.c_str();
}

void PPGameSetMainLua(const char* luaname)
{
printf("PPGameSetMainLua %s\n",luaname);
	__luaFilePath = luaname;
}

void PPGameSetDataPath(const char* datapath)
{
	__watchDataPath = datapath;
}

const char* PPGameDataPath(const char* name)
{
	return PPGameResourcePath(name);
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
