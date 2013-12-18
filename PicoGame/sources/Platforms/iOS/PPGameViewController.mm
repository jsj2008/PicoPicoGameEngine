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
#import "QBSoundMac.h"
#import "PPSensoriOS.h"
#import "iCadeReaderView.h"
#import "PPGameView.h"

#define kAccelerometerFrequency     10

@interface PPGameViewController ()  <iCadeEventDelegate,PPGameViewDelegate>
{
}

@end

@implementation PPGameViewController

- (void)viewDidLoad {
  [super viewDidLoad];

	if (self.game == nil) {
		PPGame* game = [[PPGame alloc] init];
		self.game = game;
		[game release];
	}
	
  iCadeReaderView *control = [[iCadeReaderView alloc] initWithFrame:CGRectZero];
  [self.view addSubview:control];
  control.active = YES;
  control.delegate = self;
  [control release];

	[[NSNotificationCenter defaultCenter] addObserver:self
	selector:@selector(applicationDidEnterBackground:)
	name:UIApplicationDidEnterBackgroundNotification
	object:[UIApplication sharedApplication]];

	[[NSNotificationCenter defaultCenter] addObserver:self
	selector:@selector(applicationWillEnterForeground:)
	name:UIApplicationWillEnterForegroundNotification
	object:[UIApplication sharedApplication]];

  if (QBSound::sharedSound()==NULL) {
    QBSoundMac* snd = new QBSoundMac(5);
    if (snd) {
      snd->Init();
      snd->Reset();
      snd->setMasterVolume(0.5);
    }
  }

	PPSensor::instance = new PPSensoriOS();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	[((PPGameView*)self.view) stopAnimation];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	[((PPGameView*)self.view) startAnimation];
}

- (void)viewWillAppear:(BOOL)animated {
  [super viewWillAppear:animated];
	[self.navigationController setNavigationBarHidden:YES animated:YES];
	[((PPGameView*)self.view) startAnimation];
	if (self.game.initFlag == NO) {
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
