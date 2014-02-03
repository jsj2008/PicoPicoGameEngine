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
//#include "PPGameBGM.h"
#include "PPGameSound.h"
#include "QBGame.h"
#include "PPGamePreference.h"
#import <GameController/GCController.h>
#import <AudioToolbox/AudioToolbox.h>

#if __LP64__ || TARGET_OS_IPHONE
#define __USE_GAME_CONTROLLER__ 1
#else
#define __USE_GAME_CONTROLLER__ 0
#endif

using namespace std;

const char* PPGameResourcePath(const char* name)
{
	if (name[0] == 0) {
		return [[[NSBundle mainBundle] resourcePath] fileSystemRepresentation];
	}

	NSString* n = [NSString stringWithUTF8String:name];

	const char* f = [[[NSBundle mainBundle] pathForResource:[n stringByDeletingPathExtension] ofType:[n pathExtension]] fileSystemRepresentation];
	if (f == NULL) return "";
	return f;
}

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

static PPGamePreference* customPreference=nil;
static NSString* customPreferencePath=nil;

const char* PPGetCustomPlistPath(void)
{
  if (customPreferencePath) {
    return [customPreferencePath fileSystemRepresentation];
  }
  return NULL;
}

void PPSetCustomPlistPath(const char* path)
{
	[customPreference release];
	customPreference = nil;
  [customPreferencePath release];
  customPreferencePath = nil;
	if (path) {
    customPreferencePath = [[NSString stringWithUTF8String:path] retain];
		customPreference = [[PPGamePreference alloc] initWithPath:customPreferencePath];
	}
}

static id PPPrefreceObject()
{
	if (customPreference) return customPreference;
	return [NSUserDefaults standardUserDefaults];
}

int PPGetInteger(const char* key,int defaultValue)
{
	id v = [PPPrefreceObject() objectForKey:[NSString stringWithUTF8String:key]];
	if (v != nil) {
		return [v intValue];
	}
	return defaultValue;
}

void PPSetInteger(const char* key,int value,bool sync)
{
	[PPPrefreceObject() setObject:[NSNumber numberWithInt:value] forKey:[NSString stringWithUTF8String:key]];
	if (sync) [PPPrefreceObject() synchronize];
}

float PPGetNumber(const char* key,float defaultValue)
{
	id v = [PPPrefreceObject() objectForKey:[NSString stringWithUTF8String:key]];
	if (v != nil) {
		return [v floatValue];
	}
	return defaultValue;
}

void PPSetNumber(const char* key,float value,bool sync)
{
	[PPPrefreceObject() setObject:[NSNumber numberWithFloat:value] forKey:[NSString stringWithUTF8String:key]];
	if (sync) [PPPrefreceObject() synchronize];
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

void PPSetString(const char* key,const std::string & value,bool sync)
{
	[PPPrefreceObject() setObject:[NSString stringWithUTF8String:value.c_str()] forKey:[NSString stringWithUTF8String:key]];
	if (sync) [PPPrefreceObject() synchronize];
}

void PPSync()
{
	[PPPrefreceObject() synchronize];
}

unsigned char* PPGame_GetData(const char* key,int* dataSize)
{
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
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSArray* paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory,NSUserDomainMask,YES);
  NSString* basePath = nil;
  if ([paths count] > 0) {
    basePath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"Application Support"];
  } else {
    basePath = NSTemporaryDirectory();
  }
  basePath = [basePath stringByAppendingPathComponent:[[[[NSBundle mainBundle] bundlePath] lastPathComponent] stringByDeletingPathExtension]];
  [fileManager createDirectoryAtPath:basePath withIntermediateDirectories:YES attributes:nil error:nil];
	return [[basePath stringByAppendingPathComponent:[NSString stringWithUTF8String:dbfile]] fileSystemRepresentation];
#endif
}

void PPGame_SetData(const char* key,unsigned char* data,int dataSize)
{
	[PPPrefreceObject() setObject:[NSData dataWithBytes:data length:dataSize] forKey:[NSString stringWithUTF8String:key]];
	[PPPrefreceObject() synchronize];
}

int PPGame_GetLocale()
{
	NSString* code = NSLocalizedString(@"language",@"");
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

void PPGame_Vibrate()
{
#if TARGET_OS_IPHONE
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif
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

static NSString* __watchDataPath2 = nil;

void PPGameSetDataSubPath(const char* datapath)
{
	[__watchDataPath2 release];
	__watchDataPath2 = nil;
	if (datapath) {
		__watchDataPath2 = [[NSString stringWithUTF8String:datapath] retain];
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
	}
}

const char* PPGameDataPath(const char* name)
{
	if (__watchDataPath) {
		NSFileManager* f = [NSFileManager defaultManager];
		NSString* n = [__watchDataPath stringByAppendingPathComponent:[NSString stringWithUTF8String:name]];
		if ([f fileExistsAtPath:n]) {
			return [n fileSystemRepresentation];
		}
	}
	return PPGameResourcePath(name);
}

const char* PPGameDataSubPath(const char* name)
{
	if (__watchDataPath2) {
		NSFileManager* f = [NSFileManager defaultManager];
		NSString* n = [__watchDataPath2 stringByAppendingPathComponent:[NSString stringWithUTF8String:name]];
		if ([f fileExistsAtPath:n]) {
			return [n fileSystemRepresentation];
		}
	}
  return PPGameDataPath(name);
}

bool PPGameControllerAvailable()
{
#if __USE_GAME_CONTROLLER__
  return true;
#else
  return false;
#endif
}

void PPGameControllerStartDiscoverty()
{
#if __USE_GAME_CONTROLLER__
  if ([GCController class]!=nil) {
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:nil];
  }
#endif
}

void PPGameControllerStopDiscoverty()
{
#if __USE_GAME_CONTROLLER__
  if ([GCController class]!=nil) {
    [GCController stopWirelessControllerDiscovery];
  }
#endif
}

int PPGameControllerCount()
{
#if __USE_GAME_CONTROLLER__
  if ([GCController class]!=nil) {
    return (int)[GCController controllers].count;
  }
#endif
  return 0;
}

static void pushValue(lua_State* L,int table,const char* value,const char* key)
{
  lua_pushstring(L,value);
  lua_setfield(L,table,key);
}

static void pushValue(lua_State* L,int table,int value,const char* key)
{
  lua_pushinteger(L,value);
  lua_setfield(L,table,key);
}

static void pushValue(lua_State* L,int table,float value,const char* key)
{
  lua_pushnumber(L,value);
  lua_setfield(L,table,key);
}

static void pushValue(lua_State* L,int table,BOOL value,const char* key)
{
  lua_pushboolean(L,value);
  lua_setfield(L,table,key);
}

static void pushValue(lua_State* L,int table,GCControllerAxisInput* axis,const char* key)
{
  if (axis==nil) return;
  lua_createtable(L,0,0);
  int info=lua_gettop(L);
  pushValue(L,info,axis.value,"value");
  lua_setfield(L,table,key);
}

static void pushValue(lua_State* L,int table,GCControllerButtonInput* button,const char* key)
{
  if (button==nil) return;
  lua_createtable(L,0,0);
  int info=lua_gettop(L);
  pushValue(L,info,button.value,"value");
  pushValue(L,info,button.pressed,"pressed");
  lua_setfield(L,table,key);
}

static void pushValue(lua_State* L,int table,GCControllerDirectionPad* dpad,const char* key)
{
  if (dpad==nil) return;
  lua_createtable(L,0,0);
  int info=lua_gettop(L);
  pushValue(L,info,dpad.up,"up");
  pushValue(L,info,dpad.down,"down");
  pushValue(L,info,dpad.left,"left");
  pushValue(L,info,dpad.right,"right");
  pushValue(L,info,dpad.yAxis,"yAxis");
  pushValue(L,info,dpad.xAxis,"xAxis");
  lua_setfield(L,table,key);
}

int PPGameControllerInfo(void* script,int index)
{
  lua_State* L = (lua_State*)script;
#if __USE_GAME_CONTROLLER__
  if ([GCController class]!=nil) {
    NSArray* controllers = [GCController controllers];
    if (controllers.count > index && index >= 0) {
      GCController* gc = [controllers objectAtIndex:index];

      lua_createtable(L,0,0);
      int info=lua_gettop(L);

      pushValue(L,info,[gc.vendorName UTF8String],"vendorName");
      pushValue(L,info,gc.attachedToDevice,"attachedToDevice");
      pushValue(L,info,(int)gc.playerIndex,"playerIndex");

      if (gc.extendedGamepad) {
        pushValue(L,info,gc.extendedGamepad.buttonA,"buttonA");
        pushValue(L,info,gc.extendedGamepad.buttonB,"buttonB");
        pushValue(L,info,gc.extendedGamepad.buttonX,"buttonX");
        pushValue(L,info,gc.extendedGamepad.buttonY,"buttonY");
        pushValue(L,info,gc.extendedGamepad.leftThumbstick,"leftThumbstick");
        pushValue(L,info,gc.extendedGamepad.rightThumbstick,"rightThumbstick");
        pushValue(L,info,gc.extendedGamepad.leftShoulder,"leftShoulder");
        pushValue(L,info,gc.extendedGamepad.rightShoulder,"rightShoulder");
        pushValue(L,info,gc.extendedGamepad.leftTrigger,"leftTrigger");
        pushValue(L,info,gc.extendedGamepad.rightTrigger,"rightTrigger");
        pushValue(L,info,gc.extendedGamepad.dpad,"dpad");
      } else
      if (gc.gamepad) {
        pushValue(L,info,gc.gamepad.buttonA,"buttonA");
        pushValue(L,info,gc.gamepad.buttonB,"buttonB");
        pushValue(L,info,gc.gamepad.buttonX,"buttonX");
        pushValue(L,info,gc.gamepad.buttonY,"buttonY");
        pushValue(L,info,gc.gamepad.leftShoulder,"leftShoulder");
        pushValue(L,info,gc.gamepad.rightShoulder,"rightShoulder");
        pushValue(L,info,gc.gamepad.dpad,"dpad");
      }

      return 1;
    }
  }
#endif
  lua_pushnil(L);
  return 1;
}

void PPGameControllerSetPlayerIndex(int index,int playerIndex)
{
#if __USE_GAME_CONTROLLER__
  if ([GCController class]!=nil) {
    NSArray* controllers = [GCController controllers];
    if (controllers.count > index && index >= 0) {
      GCController* gc = [controllers objectAtIndex:index];
      gc.playerIndex=playerIndex;
    }
  }
#endif
}

int PPGameControllerGetPlayerIndex(int index)
{
#if __USE_GAME_CONTROLLER__
  if ([GCController class]!=nil) {
    NSArray* controllers = [GCController controllers];
    if (controllers.count > index && index >= 0) {
      GCController* gc = [controllers objectAtIndex:index];
      return (int)gc.playerIndex;
    }
  }
#endif
  return 0;
}

void PPGameOpenWeb(const char* url,const char* title)
{
  if (url) {
    if (title==NULL) title="";
    NSString* _url = [[NSString alloc] initWithUTF8String:url];
    NSString* _title = [[NSString alloc] initWithUTF8String:title];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"PPOpenWeb" object:@{@"url":_url,@"title":_title}];
  }
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
