/*-----------------------------------------------------------------------------------------------
	名前	UIImage-FontTexture.mm
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#import "UIImage-FontTexture.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIStringDrawing.h>

static CGFloat fontSize = 15;
static UIColor* fontBGColor = nil;
static UIColor* fontColor = nil;

@implementation UIImage (FontTexture)

+ (void)setFontSize:(CGFloat)size
{
	fontSize = size;
}

+ (void)setFontImageBGColor:(UIColor*)col
{
	if (fontBGColor != col) {
		[fontBGColor release];
		fontBGColor = [col retain];
	}
}

+ (void)setFontColor:(UIColor*)col
{
	if (fontColor != col) {
		[fontColor release];
		fontColor = [col retain];
	}
}

+ (UIImage*)fontImage:(NSString*)string
{
	CGSize textSize;
	UIImage* image = self;
	UIFont *font = [UIFont boldSystemFontOfSize:fontSize];

	textSize = [string sizeWithFont:font minFontSize:fontSize actualFontSize:&fontSize forWidth:320 lineBreakMode:UILineBreakModeMiddleTruncation];
	textSize.width = fontSize+1;
	textSize.height = fontSize+1;

	UIGraphicsBeginImageContext(textSize);
	CGContextRef context = UIGraphicsGetCurrentContext();
	if (fontBGColor == nil) {
		CGContextSetFillColorWithColor(context,[UIColor blackColor].CGColor);
	} else {
		CGContextSetFillColorWithColor(context,fontBGColor.CGColor);
	}
	CGContextFillRect(context, CGRectMake(0,0,textSize.width,textSize.height));

	CGPoint point;
	if (fontColor == nil) {
		[[UIColor whiteColor] set];
	} else {
		[fontColor set];
	}
	point = CGPointMake(0,0);
	[string drawAtPoint:point forWidth:textSize.width withFont:font fontSize:fontSize lineBreakMode:UILineBreakModeMiddleTruncation baselineAdjustment:UIBaselineAdjustmentAlignBaselines];

	image = UIGraphicsGetImageFromCurrentImageContext();
	
	UIGraphicsEndImageContext();

	return image;
}

@end

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
