//
//  main.m
//  PicoGame
//
//  Created by Hiromitsu Yamaguchi on 12/07/19.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
	{
		id pool = [NSAutoreleasePool new];
		NSString *logPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Logs/PicoGame.log"];
		freopen([logPath fileSystemRepresentation], "a", stdout);
		[pool release];
	}
	return NSApplicationMain(argc, (const char **)argv);
}
