//
//  PicoGameAppDelegate.h
//  PicoTest
//
//  Created by 山口 博光 on 12/06/27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <PicoGame/PPGameViewController.h>

@interface PicoGameAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    PPGameViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet PPGameViewController *viewController;

@end
