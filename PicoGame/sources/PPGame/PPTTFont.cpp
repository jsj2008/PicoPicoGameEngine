/*-----------------------------------------------------------------------------------------------
	名前	PPTTFont.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPTTFont.h"
#include "PPTTFont-Def.h"

static unsigned getCharBytesUTF8( const char* string )
{
    const char ch = string[0];

    // マルチバイト文字かどうか
    if( ch & 0x80 ){

        // 先頭文字かどうか
        if( ch & 0x40 ){
            if( !(ch & 0x20 ) ){
                return 2;
            }
            else if( !(ch & 0x10) ){
                return 3;
            }
            else {
                return 4;
            }
        } else {
            // 不正な文字です
            return 0;
        }
    }
    else {
        return 1;
    }
}

static unsigned int ConvertCharUTF8toUTF32( const char* string, int* bytes )
{   
    if( !string || *string == '\0' ){
        if( bytes ){
            *bytes = 0;
        }
        return 0;
    }
    
    const char* p = string;
    unsigned b  = getCharBytesUTF8( string );
    unsigned ch = 0;

    
    switch( b ){
    case 1:
        ch = *p;
        break;
    case 2:
        ch |= ( p[0] & 0x1F ) << 6;
        ch |= ( p[1] & 0x3F );
        break;
    case 3:
        ch |= ( p[0] & 0x0F ) << 12;
        ch |= ( p[1] & 0x3F ) << 6;
        ch |= ( p[2] & 0x3F );
        break;
    case 4:
        ch |= ( p[0] & 0x07 ) << 18;
        ch |= ( p[1] & 0x3F ) << 12;
        ch |= ( p[2] & 0x3F ) << 6;
        ch |= ( p[3] & 0x3F );
        break;
    default:
        ch = 0;
        b  = 0;
    }

    if( bytes ){
        *bytes = b;
    }

    return ch;
}

int PPTTFontImage::gwidth(PPTTFont* font) {
	return width/font->tileWidth();
}
int PPTTFontImage::gheight(PPTTFont* font) {
	return height/font->tileHeight();
}

PPTTFont::PPTTFont(PPWorld* world,const char* name,int size,int basewidth,int baseheight,int tilenum) : ftfont(NULL),base(NULL),tile(NULL),fontTile(NULL)
{
//#ifdef __COCOS2DX__
//	unsigned long datasize;
//	unsigned char* data = ccGetFileData(name,"r",&datasize);
//	std::string p = ccGetWriteablePath();
//	if (strcasecmp(name,"System") == 0) {
//    p = p+name;
//  } else {
//    p = p+name;
//  }
//	{
//		FILE* fp = fopen(p.c_str(),"w");
//		if (fp) {
//			fwrite(data, datasize, 1, fp);
//			fclose(fp);
//		}
//	}
//	free(data);
//#else
//	std::string p;
//  p = name;
//	if (strcasecmp(name,"System") == 0) {
//		p = name;
//	} else {
//		p = name; //PPGameDataSubPath(name);
//	}
//#endif
	const char* path = name;
	_updated = false;
	base = new PPTTFontImage(world,basewidth,baseheight);
	maxTile = tilenum;
	fontTile = new PPTTFontTile*[maxTile];
	for (int i=0;i<maxTile;i++) {
		fontTile[i] = NULL;
	}
	newFontCount = 0;
	tileCount = 0;
	texture = -1;
	ftfont = calloc(1,sizeof(FTFONT));
	FTFONT* f=(FTFONT*)ftfont;
	if (load(path,size)) {
		if (f->face) FT_Done_Face(f->face);
		if (f->library) FT_Done_FreeType(f->library);
		if (base) delete base;
		f->face = NULL;
		f->library = NULL;
		base = NULL;
		f->type = 0;
	}
}

PPTTFont::~PPTTFont()
{
	FTFONT* f=(FTFONT*)ftfont;
	if (f) {
		if (f->face) FT_Done_Face(f->face);
		if (f->library) FT_Done_FreeType(f->library);
		free(f);
	}
//	if (pixel) delete pixel;
	if (base) delete base;
	if (fontTile) {
		for (int i=0;i<maxTile;i++) {
			if (fontTile[i]) delete fontTile[i];
		}
		delete fontTile;
	}
	if (tile) {
		for (int t=0;t<tileCount;t++) {
			if (tile[t]) delete tile[t];
			tile[t] = NULL;
		}
		delete tile;
	}
}

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
    if (loadSystemFont(name,size)) {
      f->type = 2;
    }
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
    return imageSystemFont(string);
	}
}

void PPTTFont::cache(const char* str)
{
	PPTTFontTile* img;
	img = image(str);
	if (img) img->noRelease = true;
}

void PPTTFont::cacheAlphabetAndNumeric()
{
	PPTTFontTile* img;
	char a[2]={0};
	for (int i='a';i<='z';i++) {
		a[0] = i;
		img = image(a);
		if (img) img->noRelease = true;
	}
	for (int i='0';i<='9';i++) {
		a[0] = i;
		img = image(a);
		if (img) img->noRelease = true;
	}
	for (int i='A';i<='Z';i++) {
		a[0] = i;
		img = image(a);
		if (img) img->noRelease = true;
	}
	newFontCount = 0;
}

int PPTTFont::drawbitmap(PPTTFontTile* tile,signed int width,signed int height,unsigned char* buffer,signed int x,signed int y)
{
	signed int  i, j, p, q;
	signed int  x_max = x + width;
	signed int  y_max = y + height;

	int h = 0;
	for (j=y,q=0;j < y_max;j++,q++) {
		for (i=x,p=0;i < x_max;i++,p++) {
			tile->setPixel(i,j,tile->getPixel(i,j) | buffer[q*width+p]);
			if (h < j) h = j;
		}
	}

	return h;
}

PPTTFontImage* PPTTFont::getFreeTile()
{
	while (1) {
		bool notile = true;
		for (int i=1;i<base->gheight(this)*base->gwidth(this);i++) {
			if (tile[i]->retainCount == 0) return tile[i];
		}
		for (int i=maxTile-1;i>=0;i--) {
			if (fontTile[i]) {
				if (!fontTile[i]->noRelease) {
//printf("%d,count %d\n",i,fontTile[i]->retainCount);
					if (fontTile[i]->retainCount == 0) {
						fontTile[i]->tileRelease();
						delete fontTile[i];
						fontTile[i] = NULL;
						notile = false;
						break;
					}
				}
			}
		}
		if (notile) break;
	}
	return NULL;
}

PPTTFontTile* PPTTFont::length(const char* string)
{
	return image(string);
}

void PPTTFont::textureUpdate()
{
	if (isUpdated()) {
		base->world()->projector->textureManager->setTexture(texture,base->pixel,base->width,base->height,base->bytePerRow,option);
		updated();
	}
	//world()->projector->textureManager->setTileSize(texture,tileWidth(),tileHeight());
}

void PPTTFont::idle()
{
	FTFONT* f=(FTFONT*)ftfont;
	if (f->type == 0) return;
	if (!base->world()->projector->textureManager->checkBind(texture)) {
		base->world()->projector->textureManager->setTexture(texture,base->pixel,base->width,base->height,base->bytePerRow,option);
    base->world()->projector->textureManager->bindTexture(texture);
	}
	//よく使うものを上に
	for (int j=0;j<newFontCount;j++) {
		for (int i=0;i<maxTile-1;i++) {
			bool replace = false;
			if (fontTile[i] == NULL && fontTile[i+1] != NULL) {
				replace = true;
			} else
			if (fontTile[i] != NULL && fontTile[i+1] != NULL) {
				if (fontTile[i]->retainCount < fontTile[i+1]->retainCount) {
					replace = true;
				}
			}
			if (replace) {
				PPTTFontTile* t = fontTile[i];
				fontTile[i] = fontTile[i+1];
				fontTile[i+1] = t;
			}
		}
	}
	newFontCount = 0;
	//retainCountを１減らす
	for (int i=0;i<maxTile;i++) {
		if (fontTile[i] != NULL) {
			if (!fontTile[i]->noRelease) {
				if (fontTile[i]->retainCount > 0) {
					fontTile[i]->retainCount --;
					//retainCountが0の物は再利用候補
				}
			}
		}
	}
}

void PPTTFont::showTileTable()
{
	for (int i=0;i<maxTile;i++) {
		if (fontTile[i] != NULL) {
			printf("tile %d retain %d %s advanceX %d"
				,i,fontTile[i]->retainCount,fontTile[i]->str->c_str(),fontTile[i]->advanceX);
			if (fontTile[i]->noRelease) {
				printf(" no release\n");
			} else {
				printf("\n");
			}
		} else {
			printf("tile %d NULL\n",i);
		}
	}
	for (int y=0;y<base->gheight(this);y++) {
		for (int x=0;x<base->gwidth(this);x++) {
			int t = x+y*base->gwidth(this);
			printf("%d,",tile[t]->retainCount);
		}
		printf("\n");
	}
	
	printf("\n");
}

int PPTTFont::advanceX(int t)
{
	return fontTile[t]->advanceX;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
