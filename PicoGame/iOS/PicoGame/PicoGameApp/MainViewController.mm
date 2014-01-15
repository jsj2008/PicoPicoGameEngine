//
//  MainViewController.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/10/17.
//
//

#import "MainViewController.h"
#import <PicoGame/PPGame.h>
#import <PicoGame/PPGameView.h>
#import <PicoGame/PPGameUtil.h>
#
@interface MainViewController ()

@property (strong, nonatomic) IBOutlet PPGame *game1;
@property (strong, nonatomic) IBOutlet PPGame *game2;
@property (strong, nonatomic) IBOutlet PPGameView *gameView1;
@property (strong, nonatomic) IBOutlet PPGameView *gameView2;

@end

@implementation MainViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
	
	if ([[UIScreen mainScreen] scale] != 1.0) {
		if ([self.gameView1 respondsToSelector:@selector(setContentScaleFactor:)]) {
			[self.gameView1 setContentScaleFactor:[[UIScreen mainScreen] scale]];
		}
	}

	PPGameSetDataPath([[[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"GameData/main.lua"] stringByDeletingLastPathComponent] fileSystemRepresentation]);
	
	[self.game1 start];
	[self.game2 start];
	
	[self.gameView1 startAnimation];
	[self.gameView2 startAnimation];

	[[NSNotificationCenter defaultCenter] addObserver:self
	selector:@selector(applicationDidEnterBackground:)
	name:UIApplicationDidEnterBackgroundNotification
	object:[UIApplication sharedApplication]];

	[[NSNotificationCenter defaultCenter] addObserver:self
	selector:@selector(applicationWillEnterForeground:)
	name:UIApplicationWillEnterForegroundNotification
	object:[UIApplication sharedApplication]];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	[self.gameView1 stopAnimation];
	[self.gameView2 stopAnimation];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	[self.gameView1 startAnimation];
	[self.gameView2 startAnimation];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[self.gameView1 stopAnimation];
	[self.gameView2 stopAnimation];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
  return YES;
}

- (void)viewDidUnload {
	[self setGame1:nil];
	[self setGame2:nil];
	[self setGameView1:nil];
	[self setGameView2:nil];
	[super viewDidUnload];
}

@end
