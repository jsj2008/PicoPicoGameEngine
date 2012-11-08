/*-----------------------------------------------------------------------------------------------
	名前	PPGamePreference.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PPGamePreference.h"

@interface PPGamePreference ()

@property (nonatomic, readonly) NSMutableDictionary* info;

@end

@implementation PPGamePreference
@synthesize path=_path;
@synthesize info=_info;

- (id)init
{
    [self dealloc];
    @throw [NSException exceptionWithName:@"PPGamePreferenceBadInitCall"
                                   reason:@"Initialize Lowsuit with initWithPath"
                                 userInfo:nil];
    return nil;
}

- (id)initWithPath:(NSString*)path
{
	self = [super init];
	if (self != nil) {
		_path = [path retain];
	}
	return self;
}

- (void)dealloc
{
	[self synchronize];
	[_path release];
	[_info release];
    [super dealloc];
}

- (NSMutableDictionary*)info
{
	if (_info==nil) {
		NSFileManager* f=[NSFileManager defaultManager];
		if ([f fileExistsAtPath:self.path]) {
			_info = [[NSMutableDictionary alloc] initWithContentsOfFile:self.path];
		} else {
			_info = [[NSMutableDictionary alloc] init];
		}
	}
	return _info;
}

- (id)objectForKey:(NSString *)defaultName
{
	return [self.info objectForKey:defaultName];
}

- (void)setObject:(id)value forKey:(NSString *)defaultName
{
	[self.info setObject:value forKey:defaultName];
}

- (BOOL)synchronize
{
	if (self.path) {
		return [self.info writeToFile:self.path atomically:YES];
	}
	return NO;
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
