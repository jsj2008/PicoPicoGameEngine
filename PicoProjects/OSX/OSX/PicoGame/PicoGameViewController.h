//
//  PicoGameViewController.h
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/15.
//
//

#import <Foundation/Foundation.h>
#import <PicoGame/PPGameController.h>

@interface PicoGameViewController : NSResponder
{
	IBOutlet NSWindow* window;
	
	IBOutlet PPGameController* game1;
	IBOutlet PPGameController* game2;
}

@end
