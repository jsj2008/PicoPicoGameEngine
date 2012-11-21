//
//  PPRetinaGameViewController.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/11/21.
//
//

#import "PPRetinaGameViewController.h"

@interface PPRetinaGameViewController ()

@end

@implementation PPRetinaGameViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

	if ([[UIScreen mainScreen] scale] != 1.0) {
		if ([self.view respondsToSelector:@selector(setContentScaleFactor:)]) {
			[self.view setContentScaleFactor:[[UIScreen mainScreen] scale]];
		}
	}
}

@end
