/*-----------------------------------------------------------------------------------------------
	名前	PPGameUtil.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameUtil.h"
#include "PPGameBGM.h"
#include "PPGameSound.h"
#include "QBGame.h"
#if TARGET_OS_IPHONE
#include "PPGamePreference.h"
#endif

using namespace std;

#define PPGAME_MAX_BGM 10

//static int keySide = 0;
static PPGameBGM* bgm[PPGAME_MAX_BGM] = {0};
static BOOL selectingBGM=NO;
static int later3GS = 1;

const char* PPGameResourcePath(const char* name)
{
	if (name[0] == 0) {
		return [[[NSBundle mainBundle] resourcePath] fileSystemRepresentation];
	}

	NSString* n = [NSString stringWithUTF8String:name];

//NSLog([n stringByDeletingPathExtension]);
//NSLog([n pathExtension]);

	const char* f = [[[NSBundle mainBundle] pathForResource:[n stringByDeletingPathExtension] ofType:[n pathExtension]] fileSystemRepresentation];
	if (f == NULL) return "";
	return f;
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

const char* PPGameLoadShader(const char* name)
{
	NSString* n = [NSString stringWithUTF8String:name];
	NSString* path = [[NSBundle mainBundle] pathForResource:[n stringByDeletingPathExtension] ofType:[n pathExtension]];
#if TARGET_OS_IPHONE
	NSString* src = [NSString stringWithFormat:@"#define IPHONE_GL 1\n%@",[NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil]];
#else
	NSString* src = [NSString stringWithFormat:@"#define IPHONE_GL 0\n%@",[NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil]];
#endif
	return [src UTF8String];
}

unsigned char* PPGame_LoadPNG(const char* path,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow)
{
	unsigned char* pixel = NULL;
#if TARGET_OS_IPHONE
	UIImage* image = [UIImage imageWithContentsOfFile:[[NSFileManager defaultManager] stringWithFileSystemRepresentation:path length:strlen(path)]];
	CGImageRef cgImage = image.CGImage;
#else
	NSImage* image = [[[NSImage alloc] initWithContentsOfFile:[[NSFileManager defaultManager] stringWithFileSystemRepresentation:path length:strlen(path)]] autorelease];
	NSBitmapImageRep* bitmapImage=[NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
	CGImageRef cgImage = [bitmapImage CGImage];
#endif
	if (cgImage) {
		CGContextRef spriteContext;
		int w = (int)CGImageGetWidth(cgImage);
		int h = (int)CGImageGetHeight(cgImage);
		pixel = (unsigned char*)calloc(w*h*4,1);
		spriteContext = CGBitmapContextCreate(pixel,w,h,8,w*4,CGColorSpaceCreateDeviceRGB(),kCGImageAlphaPremultipliedLast);
		CGContextDrawImage(spriteContext,CGRectMake(0.0,0.0,(CGFloat)w,(CGFloat)h),cgImage);
		CGContextRelease(spriteContext);
		*width = w;
		*height = h;
		*bytesPerRow = w*4;
	}
	return pixel;
}

#if 0
#include "png.h"
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
printf("png_buff->data_offset %ld\n",png_buff->data_offset);
printf("size %ld\n",size);
printf("png_buff->data_len %ld\n",png_buff->data_len);
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

unsigned char* PPGame_DecodePNG(unsigned char* bytes,unsigned long size,unsigned long* imageWidth,unsigned long* imageHeight,unsigned long* bytesPerRow)
{
	if (bytes == NULL || size == 0) return NULL;
	my_png_buffer png_buff;
	png_buff.data_offset = 0;
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
#else
unsigned char* PPGame_DecodePNG(unsigned char* bytes,unsigned long size,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow)
{
	unsigned char* pixel = NULL;
#if TARGET_OS_IPHONE
	UIImage* image = [UIImage imageWithData:[NSData dataWithBytesNoCopy:bytes length:size]];
	CGImageRef cgImage = image.CGImage;
#else
	NSImage* image = [[[NSImage alloc] initWithData:[NSData dataWithBytesNoCopy:bytes length:size]] autorelease];
	NSBitmapImageRep* bitmapImage=[NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
	CGImageRef cgImage = [bitmapImage CGImage];
#endif
	if (cgImage) {
		CGContextRef spriteContext;
		int w = (int)CGImageGetWidth(cgImage);
		int h = (int)CGImageGetHeight(cgImage);
		pixel = (unsigned char*)calloc(w*h*4,1);
		spriteContext = CGBitmapContextCreate(pixel,w,h,8,w*4,CGColorSpaceCreateDeviceRGB(),kCGImageAlphaPremultipliedLast);
		CGContextDrawImage(spriteContext,CGRectMake(0.0,0.0,(CGFloat)w,(CGFloat)h),cgImage);
		CGContextRelease(spriteContext);
		*width = w;
		*height = h;
		*bytesPerRow = w*4;
	}
	return pixel;
}
#endif

#if TARGET_OS_IPHONE
static PPGamePreference* customPrefernce=nil;

void PPSetCustomPlistPath(NSString* path);

void PPSetCustomPlistPath(NSString* path)
{
	[customPrefernce release];
	customPrefernce = nil;
	if (path) {
		customPrefernce = [[PPGamePreference alloc] initWithPath:path];
	}
}

static id PPPrefreceObject()
{
	if (customPrefernce) return customPrefernce;
	return [NSUserDefaults standardUserDefaults];
}
#else
static id PPPrefreceObject()
{
	return [NSUserDefaults standardUserDefaults];
}
#endif

int PPGetInteger(const char* key,int defaultValue)
{
	id v = [PPPrefreceObject() objectForKey:[NSString stringWithUTF8String:key]];
	if (v != nil) {
		return [v intValue];
	}
	return defaultValue;
}

void PPSetInteger(const char* key,int value)
{
	[PPPrefreceObject() setObject:[NSNumber numberWithInt:value] forKey:[NSString stringWithUTF8String:key]];
	[PPPrefreceObject() synchronize];
}

float PPGetNumber(const char* key,float defaultValue)
{
	id v = [PPPrefreceObject() objectForKey:[NSString stringWithUTF8String:key]];
	if (v != nil) {
		return [v floatValue];
	}
	return defaultValue;
}

void PPSetNumber(const char* key,float value)
{
	[PPPrefreceObject() setObject:[NSNumber numberWithFloat:value] forKey:[NSString stringWithUTF8String:key]];
	[PPPrefreceObject() synchronize];
}

std::string PPGetString(const char* key,const std::string & defaultValue)
{
	string ret = defaultValue;
	NSString* v = [PPPrefreceObject() objectForKey:[NSString stringWithUTF8String:key]];
	if (v != nil) {
		ret = string([v UTF8String]);
	}
	return ret;
}

void PPSetString(const char* key,const std::string & value)
{
	[PPPrefreceObject() setObject:[NSString stringWithUTF8String:value.c_str()] forKey:[NSString stringWithUTF8String:key]];
	[PPPrefreceObject() synchronize];
}

unsigned char* PPGame_GetData(const char* key,int* dataSize)
{
//	NSData* data = [[NSDictionary dictionaryWithContentsOfFile:[NSString stringWithUTF8String:PPGameResourcePath("com.yamagame.PicoPicoQuest.plist")]] objectForKey:[NSString stringWithUTF8String:key]];
	NSData* data = [PPPrefreceObject() objectForKey:[NSString stringWithUTF8String:key]];
	*dataSize = (int)[data length];
	return (unsigned char*)[data bytes];
}

const char* PPGameDocumentPath(const char* dbfile)
{
#if TARGET_OS_IPHONE
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    return [[documentsDirectory stringByAppendingPathComponent:[NSString stringWithUTF8String:dbfile]] fileSystemRepresentation];
#else
	NSString* p = [[[NSBundle mainBundle] bundlePath] stringByDeletingLastPathComponent];
	return [[p stringByAppendingPathComponent:[NSString stringWithUTF8String:dbfile]] fileSystemRepresentation];
#endif
}

void PPGame_SetData(const char* key,unsigned char* data,int dataSize)
{
	[PPPrefreceObject() setObject:[NSData dataWithBytes:data length:dataSize] forKey:[NSString stringWithUTF8String:key]];
	[PPPrefreceObject() synchronize];
}

int PPGame_InitBGM(int no,const char* key)
{
	if (bgm[no-1] == nil) {
		bgm[no-1] = [[PPGameBGM alloc] init];
	}
	if (bgm[no-1] != nil) {
		[bgm[no-1] reset];
		bgm[no-1].key = [NSString stringWithUTF8String:key];
		[bgm[no-1] loadForKey:[NSString stringWithUTF8String:key]];
		bgm[no-1].selectedPlay = PPGame_GetSoundEnable();
	}
	return 0;
}

int PPGame_InitBGMiPad(int no,const char* key)
{
	if (bgm[no-1] == nil) {
		bgm[no-1] = [[PPGameBGM alloc] init];
#if TARGET_OS_IPHONE
		bgm[no-1].popover = TRUE;
#endif
	}
	if (bgm[no-1] != nil) {
		[bgm[no-1] reset];
		bgm[no-1].key = [NSString stringWithUTF8String:key];
		[bgm[no-1] loadForKey:[NSString stringWithUTF8String:key]];
		bgm[no-1].selectedPlay = PPGame_GetSoundEnable();
	}
	return 0;
}

void PPGame_ReleaseBGM()
{
	for (int i=0;i<PPGAME_MAX_BGM;i++) {
		[bgm[i] release];
		bgm[i] = nil;
	}
}

int PPGame_GetSelectingBGM()
{
	return selectingBGM;
}

void PPGame_SetSelectingBGM(int flag)
{
	selectingBGM = flag;
}

void PPGame_IdleBGM(void* controller,int playBGM,bool playBGMOneTime,int chooseBGM,int x,int y,int w,int h)
{
	for (int no=1;no<=PPGAME_MAX_BGM;no++) {
		if (bgm[no-1]) {
			if (chooseBGM != 0) {
				if (chooseBGM == no) {
#if TARGET_OS_IPHONE
#ifndef __NO_POPOVER__
					[bgm[no-1] popoverPosition:CGRectMake(x, y, w, h)];
					[bgm[no-1] selectBGM:(UIViewController*)controller];
#endif
#endif
				} else
				if (chooseBGM < 0) {
					[bgm[no-1] reset];
					[bgm[no-1] saveForKey:bgm[no-1].key];
				}
			}
			if (playBGM != 0) {
				if (playBGM == no) {
					if (playBGMOneTime) {
						if (PPGame_GetSoundEnable()) [bgm[no-1] playOneTime];
						playBGMOneTime = false;
					} else {
						if (PPGame_GetSoundEnable()) [bgm[no-1] play];
					}
				} else
				if (playBGM < 0) {
					if (PPGame_GetSoundEnable()) [bgm[no-1] stop];
				}
			}
		}
	}
}

int PPGame_GetLocale()
{
	//return QBGAME_LOCALE_OTHER;
	//NSLog([[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode]);
	NSString* code = NSLocalizedString(@"language",@"");
//NSLog(code);
	if ([code isEqualToString:@"jp"]) {
		return QBGAME_LOCALE_JAPANESE;
	}
	return QBGAME_LOCALE_OTHER;
}

const char* PPGame_LocaleString(const char* jp,const char* en)
{
	if (PPGame_GetLocale() == QBGAME_LOCALE_JAPANESE) {
		return jp;
	}
	return en;
}

int PPGame_LoadAIFF(const char* name,const char* type)
{
#ifndef __MACOSX_APP__
	//create soundid
	SystemSoundID soundId;
	NSURL *countdownFileURL = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:name] ofType:[NSString stringWithUTF8String:type]] isDirectory:NO];
 
	OSStatus error = AudioServicesCreateSystemSoundID((CFURLRef)countdownFileURL, &soundId);
 
	if (error != kAudioServicesNoError) {
		NSLog(@"Error %ld loading sound at path: %@", error, [NSString stringWithUTF8String:name]);
	}
	
	return soundId;
#else
	return 0;
#endif
}

int PPGame_PlayAIFF(int soundId)
{
#ifndef __MACOSX_APP__
	AudioServicesPlaySystemSound(soundId);
#endif
	return 0;
}

void PPGame_Vibrate()
{
#if TARGET_OS_IPHONE
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif
}

int PPGame_3GSLater()
{
	return later3GS;
}

void __PPGame_Set3GSLater(int later);

void __PPGame_Set3GSLater(int later)
{
	later3GS = later;
}

#if !TARGET_OS_IPHONE
@implementation NSDictionary (TextureMarge)

- (CGRect)rect
{
#if TARGET_OS_IPHONE
	CGRect rect;
	rect.size =  CGSizeFromString([self objectForKey:@"size"]);
	rect.origin = CGPointFromString([self objectForKey:@"point"]);
#else
	NSRect rect;
	rect.size =  NSSizeFromString([self objectForKey:@"size"]);
	rect.origin = NSPointFromString([self objectForKey:@"point"]);
#endif
	return *(CGRect*)&rect;
}

@end
#endif

static NSString* __watchDataPath = nil;

void PPGameSetDataPath(const char* datapath)
{
	[__watchDataPath release];
	__watchDataPath = nil;
	if (datapath) {
		__watchDataPath = [[NSString stringWithUTF8String:datapath] retain];
	}
}

static NSString* __luaFilePath = nil;

const char* PPGameMainLua()
{
	if (__luaFilePath == nil) return "main.lua";
	return [__luaFilePath fileSystemRepresentation];
}

void PPGameSetMainLua(const char* luaname)
{
	[__luaFilePath release];
	__luaFilePath = nil;
	if (luaname) {
		__luaFilePath = [[NSString stringWithUTF8String:luaname] retain];
//NSLog(@"PPGameSetMainLua %@",__luaFilePath);
	}
}

const char* PPGameDataPath(const char* name)
{
//#if !TARGET_OS_IPHONE
	if (__watchDataPath) {
		NSFileManager* f = [NSFileManager defaultManager];
		NSString* n = [__watchDataPath stringByAppendingPathComponent:[NSString stringWithUTF8String:name]];
		if ([f fileExistsAtPath:n]) {
			return [n fileSystemRepresentation];
		}
	}
//#endif
	return PPGameResourcePath(name);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
