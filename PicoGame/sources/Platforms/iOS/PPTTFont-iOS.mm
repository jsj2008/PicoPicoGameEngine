#include "PPTTFont.h"
#include "PPTTFont-Def.h"
#import <Foundation/Foundation.h>

int PPTTFont::loadSystemFont(const char* name,int size)
{
  UIFont *font = [UIFont boldSystemFontOfSize:size];
  baseline = (int)(-font.descender+(font.capHeight-(font.ascender-font.descender)));
  _gridX = size;
  _gridY = size;
  if (baseline < 0) {
    _gridY -= baseline;
  }
  return 1;
}

PPTTFontTile* PPTTFont::imageSystemFont(const char* string)
{
  UIFont *font = [UIFont boldSystemFontOfSize:_gridX];
  NSString* nsstring = [NSString stringWithUTF8String:string];
  CGSize textSize = [nsstring sizeWithFont:font];

  int width  = (int)textSize.width;//-min.x;
  int height = (int)textSize.height;//-min.y;

  PPTTFontTile* ntile = new PPTTFontTile(this,string,width,height);
  for (int y=0;y<ntile->gheight();y++) {
    for (int x=0;x<ntile->gwidth();x++) {
      PPTTFontImage* img = getFreeTile();
      if (img) {
        img->retainCount = PPTTFontImage::RETAIN_COUNT;
        ntile->setTile(x,y,img->index);
        img->clear();
      } else {
        ntile->setTile(x,y,0);
      }
    }
  }
  ntile->retain();

  while (1) {
    bool t = false;
    for (int i=0;i<maxTile;i++) {
      if (fontTile[i] == NULL) {
        fontTile[i] = ntile;
        t = true;
        break;
      }
    }
    if (!t) {
      int addTile = 1;
      PPTTFontTile** t = new PPTTFontTile*[maxTile+addTile];
      for (int i=0;i<maxTile+addTile;i++) {
        if (i<maxTile) {
          t[i] = fontTile[i];
        } else {
          t[i] = NULL;
        }
      }
      delete fontTile;
      fontTile = t;
      maxTile = maxTile+addTile;
    } else {
      break;
    }
  }

  UIGraphicsBeginImageContextWithOptions(textSize,NO,1);
  CGContextRef context = UIGraphicsGetCurrentContext();
  CGContextSetFillColorWithColor(context,[UIColor whiteColor].CGColor);
  [nsstring drawAtPoint:CGPointZero withFont:font];
  UIImage* image = UIGraphicsGetImageFromCurrentImageContext();
  UIGraphicsEndImageContext();
  
  size_t bytesPerRow = CGImageGetBytesPerRow(image.CGImage);
  size_t bytePerPixel = CGImageGetBitsPerPixel(image.CGImage)/8;
  CGDataProviderRef provider = CGImageGetDataProvider(image.CGImage);
  CFDataRef data = CGDataProviderCopyData(provider);
  UInt8* buffer = (UInt8*)CFDataGetBytePtr(data);
  
  {
    int x=0;
    int y=0;
    signed int  i, j, p, q;
    signed int  x_max = x + width;
    signed int  y_max = y + height;

    int h = 0;
    for (j=y,q=0;j < y_max;j++,q++) {
      for (i=x,p=0;i < x_max;i++,p++) {
        ntile->setPixel(i,j,ntile->getPixel(i,j) | buffer[q*bytesPerRow+p*bytePerPixel+1]);
        if (h < j) h = j;
      }
    }
  }

  CFRelease(data);
  
  ntile->bearingX = 0;
  ntile->advanceX = width;//+min.x;
  
  ntile->checkEmptyBlock();

  newFontCount++;
  _updated = true;

  return ntile;
}

#include "PPFont.h"

const char* PPFont::systemFontFilePath()
{
  return "System";
}
