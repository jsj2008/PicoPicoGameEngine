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

int PPTTFontImage::gwidth(PPTTFont* font) {
	return width/font->tileWidth();
}
int PPTTFontImage::gheight(PPTTFont* font) {
	return height/font->tileHeight();
}

PPTTFont::PPTTFont(PPWorld* world,const char* name,int size,int basewidth,int baseheight,int tilenum) : ftfont(NULL),base(NULL),tile(NULL),fontTile(NULL)
{
#ifdef __COCOS2DX__
	unsigned long datasize;
	unsigned char* data = cocos2d::CCFileUtils::getFileData(name,"r",&datasize);
	std::string p = cocos2d::CCFileUtils::getWriteablePath();
	p = p+name;
	{
		FILE* fp = fopen(p.c_str(),"w");
		if (fp) {
			fwrite(data, datasize, 1, fp);
			fclose(fp);
		}
	}
	free(data);
#else
	std::string p;
	if (strcasecmp(name,"System") == 0) {
		p = name;
	} else {
		p = PPGameDataPath(name);
	}
#endif
	const char* path = p.c_str();
	_updated = false;
	base = new PPTTFontImage(world,basewidth,baseheight);
	maxTile = tilenum;
	fontTile = new PPTTFontTile*[maxTile];
	for (int i=0;i<maxTile;i++) {
		fontTile[i] = NULL;
	}
	newFontCount = 0;
	tileCount = 0;
	texture = 0;
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

#if !TARGET_OS_MAC
int PPTTFont::load(const char* name,int size)
{
//printf("load %s size %d\n",name,size);
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
	FT_Error error = 0;
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
			return error;
		}
		error = FT_Set_Pixel_Sizes(f->face, size, size);
//printf("face->bbox.yMax %d,h %d,height %d\n",(int)face->bbox.yMax,h,face->height);
//printf("face->size->height %d\n",(int)face->size->metrics.height);
		baseline = (int)(-f->face->size->metrics.descender+(f->face->size->metrics.height-(f->face->size->metrics.ascender-f->face->size->metrics.descender)));
//printf("baseline %d\n",baseline);
	}
	_gridX = size;
	_gridY = f->face->size->metrics.height>>6;
//printf("gridx %d,gridy %d\n",_gridX,_gridY);
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
	return error;
}
#endif

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

#if !TARGET_OS_MAC
PPTTFontTile* PPTTFont::image(const char* string)
{
	FTFONT* f=(FTFONT*)ftfont;
	if (f->face == NULL) return NULL;
//printf("draw %s\n",string);
	for (int i=0;i<maxTile;i++) {
		if (fontTile[i]) {
			if (strcmp(fontTile[i]->str->c_str(),string)==0) {
				fontTile[i]->retain();
				return fontTile[i];
			}
		}
	}

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

	int target_height = f->face->size->metrics.height>>6;

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

//printf("max.y %d,min.y %d\n",(int)max.y,(int)min.y);
	int width  = (int)max.x;//-min.x;
	int height = (int)max.y;//-min.y;

//	printf("-------- first\n");
//	tile[97]->show();

//printf("%s\n",string);
//printf("width %d,height %d\n",width,height);
	PPTTFontTile* ntile = new PPTTFontTile(this,string,width,height);
	for (int y=0;y<ntile->gheight();y++) {
		for (int x=0;x<ntile->gwidth();x++) {
			PPTTFontImage* img = getFreeTile();
			if (img) {
				img->retainCount = PPTTFontImage::RETAIN_COUNT;
				ntile->setTile(x,y,img->index);
//printf("%d,%d img->index %d,%08X\n",x,y,img->index,(unsigned int)img->pixel);
//				img->show();
				img->clear();
//printf("%d,%d img->index %d,%08X\n",x,y,img->index,(unsigned int)img->pixel);
//				img->show();
			} else {
				ntile->setTile(x,y,0);
//printf("img->index %d\n",0);
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

//	make(width,height);
//	clear();

	pen.x = -min.x*64;
	pen.y = baseline;

	int h = 0;
	for (int n=0;string[n]!=0;) {
		int len;
		unsigned int ch = ConvertCharUTF8toUTF32(&string[n],&len);
		FT_Set_Transform(f->face,&matrix,&pen);
		error = FT_Load_Char(f->face,ch,FT_LOAD_RENDER);
		int t = drawbitmap(ntile,slot->bitmap.width,slot->bitmap.rows,slot->bitmap.buffer,slot->bitmap_left,target_height-slot->bitmap_top);
		if (h < t) h = t;
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
		if (n == 0) {
			ntile->bearingX = f->face->glyph->metrics.horiBearingX>>6;
		}
		n += getCharBytesUTF8(&string[n]);
  	}
	
	ntile->advanceX = (pen.x>>6);//+min.x;
	
//	height = h+1;
	ntile->checkEmptyBlock();
//	ntile->show();
//	printf("-------- after\n");
//	tile[97]->show();
//	resize(width,height);

	newFontCount++;
	_updated = true;

	return ntile;
}
#endif

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
//printf("----- idle\n");
	FTFONT* f=(FTFONT*)ftfont;
	if (f->type == 0) return;
	if (!base->world()->projector->textureManager->checkBind(texture)) _updated = true;
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
