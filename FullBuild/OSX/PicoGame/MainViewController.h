//
//  MainViewController.h
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/17.
//
//

#import <Foundation/Foundation.h>
#import <PicoGame/PPGameController.h>

@interface MainViewController : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet PPGameController* game;
@property (assign) IBOutlet NSWindow* window;

@end
