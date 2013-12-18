#include "PPTTFont.h"
#include "PPTTFont-Def.h"
#import <Cocoa/Cocoa.h>

int PPTTFont::loadSystemFont(const char* name,int size)
{
  NSFont *font = [NSFont boldSystemFontOfSize:size];
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
  NSFont *font = [NSFont boldSystemFontOfSize:_gridX];
  NSString* nsstring = [NSString stringWithUTF8String:string];
  NSSize textSize = [nsstring sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:font, NSFontAttributeName,nil]];

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
  
  NSBitmapImageRep* image = nil;
  
  image = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil
                      pixelsWide:width
                      pixelsHigh:height
                      bitsPerSample:8
                      samplesPerPixel:4
                      hasAlpha:YES
                      isPlanar:NO
                      colorSpaceName:NSDeviceRGBColorSpace
                      bitmapFormat:NSAlphaFirstBitmapFormat
                      bytesPerRow:width*4
                      bitsPerPixel:0];

  [NSGraphicsContext saveGraphicsState];
  NSGraphicsContext* context = [NSGraphicsContext graphicsContextWithBitmapImageRep:image];
  [NSGraphicsContext setCurrentContext:context];
  
    [nsstring drawAtPoint:NSMakePoint(0,0) withAttributes:[NSDictionary dictionaryWithObjectsAndKeys:font, NSFontAttributeName,[NSColor whiteColor],NSForegroundColorAttributeName,nil]];
   
  [NSGraphicsContext restoreGraphicsState];

  size_t bytesPerRow = width*4;
  size_t bytePerPixel = 4;
  UInt8* buffer = [image bitmapData];

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

  [image release];
  
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
