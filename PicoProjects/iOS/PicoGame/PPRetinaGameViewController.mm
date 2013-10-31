//
//  PPRetinaGameViewController.m
//  PicoGame
//
//  Created by 山口 博光 on 2012/11/21.
//
//

#import "PPRetinaGameViewController.h"

@interface PPRetinaGameViewController ()
{
	int old_stdout;
  BOOL timerStop;
  NSInteger logSize;
}

@property (strong, nonatomic) IBOutlet UIButton *reloadButton;

@property (retain, nonatomic) IBOutlet UIButton *logButton;
@property (retain, nonatomic) IBOutlet UITextView *logView;
@property (retain, nonatomic) IBOutlet UIButton *clearButton;

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

	[self makeButtonColored:self.reloadButton];
	[self.reloadButton setTitle: NSLocalizedString(@"Reload",@"") forState:UIControlStateNormal];
	[self.reloadButton setTitleColor:[UIColor colorWithWhite:0.502 alpha:1.000] forState:UIControlStateHighlighted];

	[self makeButtonColored:self.logButton];
	[self.logButton setTitle: NSLocalizedString(@"Close",@"") forState:UIControlStateNormal];
	[self.logButton setTitleColor:[UIColor colorWithWhite:0.502 alpha:1.000] forState:UIControlStateHighlighted];

	[self makeButtonColored:self.clearButton];
	[self.clearButton setTitle: NSLocalizedString(@"Clear",@"") forState:UIControlStateNormal];
	[self.clearButton setTitleColor:[UIColor colorWithWhite:0.502 alpha:1.000] forState:UIControlStateHighlighted];
  
  self.logButton.hidden = YES;
  self.logView.hidden = YES;
  self.clearButton.hidden = YES;

  NSFileManager* fm = [NSFileManager defaultManager];
  NSString* mainPath = [self docPath:@"main.lua"];
  if ([fm fileExistsAtPath:mainPath]) {
    self.reloadButton.hidden = NO;
  } else {
    self.reloadButton.hidden = YES;
  }
}

-(void)makeButtonColored:(UIButton*)button
{
	CGRect rect = CGRectMake(0, 0, 1, 1);
  UIGraphicsBeginImageContext(rect.size);
  CGContextRef context = UIGraphicsGetCurrentContext();
  CGContextSetFillColorWithColor(context,[UIColor colorWithWhite:0.000 alpha:0.600].CGColor);
  CGContextFillRect(context, rect);
  UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
  UIGraphicsEndImageContext();
 
	[button setBackgroundImage:img	forState:UIControlStateNormal];
	button.layer.cornerRadius = 8.0;
	button.layer.masksToBounds = YES;
	button.layer.borderColor = [UIColor lightGrayColor].CGColor;
	button.layer.borderWidth = 1;
}

- (BOOL)prefersStatusBarHidden
{
  return YES;
}

- (void)viewWillAppear:(BOOL)animated
{
  NSDictionary *fileDictionary = [[NSFileManager defaultManager] attributesOfItemAtPath:[self docPath:@"console.log"] error:nil];
  logSize = [fileDictionary fileSize];

  timerStop=NO;
  [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(timerFire:) userInfo:nil repeats:YES];

	old_stdout = dup(fileno(stdout));
	freopen([[self docPath:@"console.log"] fileSystemRepresentation], "a", stdout);

  [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
  [super viewDidAppear:animated];
}

- (IBAction)reload:(id)sender {
	[((PPGameView*)self.view) stopAnimation];
	[self.game reloadData];
	[((PPGameView*)self.view) startAnimation];
}

- (NSString*)docPath:(NSString*)dbfile
{
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
  return [documentsDirectory stringByAppendingPathComponent:dbfile];
}

- (void)timerFire:(NSTimer*)theTimer
{
  if (timerStop) {
    [theTimer invalidate];
    return;
  }
  
  NSDictionary *fileDictionary = [[NSFileManager defaultManager] attributesOfItemAtPath:[self docPath:@"console.log"] error:nil];
  if ([fileDictionary fileSize] != logSize) {
    logSize = [fileDictionary fileSize];
    self.logButton.hidden = NO;
    self.logView.hidden = NO;
    self.logView.text = [NSString stringWithContentsOfFile:[self docPath:@"console.log"] encoding:NSUTF8StringEncoding error:nil];
    self.clearButton.hidden = NO;
    self.logButton.hidden = NO;
    self.reloadButton.hidden = NO;
    [self.logButton setTitle: NSLocalizedString(@"Close",@"") forState:UIControlStateNormal];
  }
}

- (IBAction)openLog:(id)sender
{
  if (self.logView.hidden) {
    self.logView.hidden = NO;
    self.clearButton.hidden = NO;
    self.logButton.hidden = NO;
    [self.logButton setTitle: NSLocalizedString(@"Close",@"") forState:UIControlStateNormal];
  } else {
    self.logView.hidden = YES;
    self.clearButton.hidden = YES;
    [self.logButton setTitle: NSLocalizedString(@"Open",@"") forState:UIControlStateNormal];
  }
}

- (IBAction)clearLog:(id)sender
{
  FILE* fp=fopen([[self docPath:@"console.log"] fileSystemRepresentation],"w");
  if (fp) {
    ftruncate(fileno(fp),0);
    fclose(fp);
    logSize=0;
  }
  self.logView.text = @"";
}

@end
