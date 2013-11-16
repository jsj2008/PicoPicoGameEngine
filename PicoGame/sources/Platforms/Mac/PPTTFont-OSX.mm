//
//  PPTTFont-OSX.m
//  PicoGame
//
//  Created by H.Yamaguchi on 2012/11/09.
//
//

#include "PPTTFont.h"
#include "PPTTFont-Def.h"
#import <Cocoa/Cocoa.h>

int PPTTFont::load(const char* name,int size)
{
	FTFONT* f=(FTFONT*)ftfont;
	if (tile) {
		for (int t=0;t<tileCount;t++) {
			if (tile[t]) delete tile[t];
			tile[t] = NULL;
		}
		delete tile;
	}
	for (int i=0;i<maxTile;i++) {
		if (fontTile[i]) delete fontTile[i];
		fontTile[i] = NULL;
	}
	f->type = 0;
	FT_Error error = 0;
	if (strcasecmp(name,"System") == 0) {
		NSFont *font = [NSFont boldSystemFontOfSize:size];
		baseline = (int)(-font.descender+(font.capHeight-(font.ascender-font.descender)));
		_gridX = size;
		_gridY = size;
		if (baseline < 0) {
			_gridY -= baseline;
		}
		f->type = 2;
	} else {
		while (true) {
			if (f->library == NULL) {
				error = FT_Init_FreeType(&f->library);
			}
			if (f->face != NULL) {
				FT_Done_Face(f->face);
				f->face = NULL;
			}
			if (f->face == NULL) {
				error = FT_New_Face(f->library,name,0,&f->face);
				if (error != 0) {
					std::string s = name;
					s += " (loadTTF)";
					PPReadErrorSet(s.c_str());
					break;
				}
				error = FT_Set_Pixel_Sizes(f->face, size, size);
				baseline = (int)(-f->face->size->metrics.descender+(f->face->size->metrics.height-(f->face->size->metrics.ascender-f->face->size->metrics.descender)));
			}
			_gridX = size;
			_gridY = (int)(f->face->size->metrics.height>>6);
			if (baseline < 0) {
				_gridY -= baseline;
			}
			f->type = 1;
			break;
		}
	}
	if (error==0) {
		tileCount = base->gwidth(this)*base->gheight(this);
		tile = new PPTTFontImage*[tileCount];
		for (int y=0;y<base->gheight(this);y++) {
			for (int x=0;x<base->gwidth(this);x++) {
				int t = x+y*base->gwidth(this);
				tile[t] = new PPTTFontImage(base,x,y,tileWidth(),tileHeight());
				tile[t]->retainCount = 0;
			}
		}
		newFontCount = 0;
		_updated = true;
	}
	return error;
}

PPTTFontTile* PPTTFont::image(const char* string)
{
	FTFONT* f=(FTFONT*)ftfont;
	for (int i=0;i<maxTile;i++) {
		if (fontTile[i]) {
			if (strcmp(fontTile[i]->str->c_str(),string)==0) {
				fontTile[i]->retain();
				return fontTile[i];
			}
		}
	}

	if (f->type==1) {
		if (f->face == NULL) return NULL;

		FT_Error error = 0;
		FT_GlyphSlot slot = f->face->glyph;
		FT_Matrix matrix;
		FT_Vector pen;
		float angle=0;

		matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
		matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
		matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
		matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

		FT_Vector min={0,0};
		FT_Vector max={0,0};

		int target_height = (int)(f->face->size->metrics.height>>6);

		pen.x = 0;
		pen.y = baseline;

		min.x = 0;
		max.x = 0;
		min.y = 0;//-(baseline+y);
		max.y = 0;//-(baseline+y);

		for (int n=0;string[n]!=0;) {
			int len;
			unsigned int ch = ConvertCharUTF8toUTF32(&string[n],&len);
			FT_Set_Transform(f->face,&matrix,&pen);
			error = FT_Load_Char(f->face,ch,FT_LOAD_RENDER);
			
			int x = slot->bitmap_left;
			int y = target_height-slot->bitmap_top;
			int w = slot->bitmap.width;
			int h = slot->bitmap.rows;

			if (n==0) {
				min.x = x;
				max.x = x+w;
				min.y = y;
				max.y = y+h;
			} else {
				if (min.x > x  ) min.x = x;
				if (max.x < x+w) max.x = x+w;
				if (min.y > y  ) min.y = y;
				if (max.y < y+h) max.y = y+h;
			}
			
			pen.x += slot->advance.x;
			pen.y += slot->advance.y;

			if (max.x < pen.x/64) max.x = pen.x/64;

			n += getCharBytesUTF8(&string[n]);
		}

		int width  = (int)max.x;//-min.x;
		int height = (int)max.y;//-min.y;

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
	//				tile->index = i;
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

		pen.x = -min.x*64;
		pen.y = baseline;

		for (int n=0;string[n]!=0;) {
			int len;
			unsigned int ch = ConvertCharUTF8toUTF32(&string[n],&len);
			FT_Set_Transform(f->face,&matrix,&pen);
			error = FT_Load_Char(f->face,ch,FT_LOAD_RENDER);

			drawbitmap(ntile,slot->bitmap.width,slot->bitmap.rows,slot->bitmap.buffer,slot->bitmap_left,target_height-slot->bitmap_top);

			pen.x += slot->advance.x;
			pen.y += slot->advance.y;
			if (n == 0) {
				ntile->bearingX = (int)(f->face->glyph->metrics.horiBearingX>>6);
			}
			n += getCharBytesUTF8(&string[n]);
		}
		
		ntile->advanceX = (int)(pen.x>>6);//+min.x;
		
		ntile->checkEmptyBlock();

		newFontCount++;
		_updated = true;

		return ntile;
	} else {
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
}
