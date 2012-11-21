/*-----------------------------------------------------------------------------------------------
	名前	PPGameViewController.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "PPGameViewController.h"

#define kAccelerometerFrequency     10

@implementation PPGameViewController

- (void)viewDidLoad {
    [super viewDidLoad];

	if (self.game == nil) {
		PPGame* game = [[PPGame alloc] init];
		self.game = game;
		[game release];
	}
	
//	if (![[UIScreen mainScreen] scale] == 1.0) {
//		if ([self.view respondsToSelector:@selector(setContentScaleFactor:)]) {
//			[self.view setContentScaleFactor:2];
//		}
//	}

    iCadeReaderView *control = [[iCadeReaderView alloc] initWithFrame:CGRectZero];
    [self.view addSubview:control];
    control.active = YES;
    control.delegate = self;
    [control release];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
	[self.navigationController setNavigationBarHidden:YES animated:YES];
	[((PPGameView*)self.view) startAnimation];
//	if (self.game) {
//		((PPGameView*)self.view).game = self.game;
//	}
	if (self.game.initFlag == NO) {
//		self.game.view = ((PPGameView*)self.view);
		[self.game start];
		self.game.initFlag = YES;
	}
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
	[((PPGameView*)self.view) stopAnimation];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
//	((PPGameView*)self.view).game = nil;
	self.view = nil;
}

- (void)dealloc
{
	[_game release];
    [super dealloc];
}

- (void)exitGame
{
	[self.game exit];
	self.game = nil;
//	((PPGameView*)self.view).game = nil;
}

- (void)stateChanged:(iCadeState)state
{
	((PPGameView*)self.view).staticKey = state;
}

- (PPGame*)gameController
{
	return self.game;
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
