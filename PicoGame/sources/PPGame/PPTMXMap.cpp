/*-----------------------------------------------------------------------------------------------
	名前	PPTMXMap.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPTMXMap.h"
#include "PPGameUtil.h"
#include <zlib.h>
#include <string>
#include <string.h>
#include "PPGameSprite.h"
#include "PPGameMap.h"
#include "PPFontTable.h"
#include "QBGame.h"
#include <sstream>

#ifdef _WIN32
#define __USE_PPSAX__
#endif

#ifdef __USE_PPSAX__
#include "PPXMLParser.h"
typedef char PPXmlChar;
#else
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
typedef xmlChar PPXmlChar;
#endif

#ifdef __COCOS2DX__
#include "Cocos2dxWrapper.h"
#endif

static void PPTMXMap_startElement(void *ctx, const PPXmlChar *name, const PPXmlChar **atts);
static void PPTMXMap_endElement(void *ctx, const PPXmlChar *name);
static void PPTMXMap_textHandler(void *ctx, const PPXmlChar *name, int len);

PPTMXTileSet::PPTMXTileSet()
{
}

PPTMXTileSet::~PPTMXTileSet()
{
}

//static int tmxlayercount = 0;

PPTMXLayer::PPTMXLayer() : chip(NULL)
{
	pos = PPPointZero;
	visible = true;
//printf("PPTMXLayer new %d\n",tmxlayercount);
//tmxlayercount++;
}

PPTMXLayer::~PPTMXLayer()
{
	if (chip) free(chip);
//tmxlayercount--;
//printf("PPTMXLayer delete %d\n",tmxlayercount);
}

void PPTMXLayer::preCalc(PPObject* obj,int x,int y,int index,PPPoint* outpos)
{
	PPSize s = obj->tileSize();
	PPGamePoly poly = obj->poly;
	PPPoint os = obj->poly.origin*obj->poly.scale;
	PPPoint op = obj->autoLayout(obj->pos)+os;
	PPPoint ts = origin*obj->poly.scale-origin;
	op.x = floorf(op.x);
	op.y = floorf(op.y);

	PPPoint p = op-obj->poly.origin+ts;
	PPPoint sp = PPPoint(x*s.width,y*s.height);
	poly.origin = obj->poly.origin+p+origin;
	poly.sprite(p.x+sp.x,p.y+sp.y,getGID(x,y)-1,0);
	poly._texture = obj->poly._texture;
	poly.scale = obj->poly.scale;

	{
		PPPoint p[4];
		PPRect r(0,0,s.width,s.height);
		obj->world()->projector->CalcPolyPoint(&poly,&r,&PPPointZero,p);
		obj->world()->projector->PreCalc(&p[index],outpos);
	}
}

void PPTMXLayer::draw(PPObject* obj,PPRect drawArea)
{
	if (obj->poly._texture >= 0) {
		PPSize s = obj->tileSize();
		PPGamePoly poly = obj->poly;
		PPPoint os = obj->poly.origin*obj->poly.scale;
		PPPoint op = obj->autoLayout(obj->pos)+os;
		PPPoint ts = origin*obj->poly.scale-origin;
		op.x = floorf(op.x);
		op.y = floorf(op.y);

#if 1
		int grid=8;
		if (mapSize.width > 0 && mapSize.height > 0) {
			for (int my=drawArea.y;my<drawArea.height;my+=grid) {
				for (int mx=drawArea.x;mx<drawArea.width;mx+=grid) {

					PPPoint pos[4];
					preCalc(obj,mx       ,my       ,0,&pos[0]);
					preCalc(obj,mx+grid-1,my       ,1,&pos[1]);
					preCalc(obj,mx+grid-1,my+grid-1,2,&pos[2]);
					preCalc(obj,mx       ,my+grid-1,3,&pos[3]);
					
					bool draw=true;
					
					{
						int nx=0;
						int ny=0;
						for (int i=0;i<4;i++) {
							if (pos[i].x < -1) nx --;
							if (pos[i].y < -1) ny --;
							if (pos[i].x >  1) nx ++;
							if (pos[i].y >  1) ny ++;
						}
						if (nx <= -4 || nx >= 4 || ny <= -4 || ny >= 4) {
							draw=false;
						}
					}
							
					if (draw) {
						for (int y=my;y<my+grid&&y<drawArea.height;y++) {
							for (int x=mx;x<mx+grid&&x<drawArea.width;x++) {
								
								if (getGID(x,y) != 0) {
									PPPoint p = op-obj->poly.origin+ts;
									PPPoint sp = PPPoint(x*s.width,y*s.height);
									PPTMXChip c = getAttribute(x,y);
									poly.origin = obj->poly.origin+p+origin;
									poly.sprite(p.x+sp.x,p.y+sp.y,getGID(x,y)-1,0);
									poly._texture = obj->poly._texture;
									poly.scale = obj->poly.scale;
									poly.flags = c.flag;
									poly.color.r = obj->poly.color.r*c.color.r/255;
									poly.color.g = obj->poly.color.g*c.color.g/255;
									poly.color.b = obj->poly.color.b*c.color.b/255;
									poly.color.a = obj->poly.color.a*c.color.a/255;
									obj->world()->projector->DrawPoly(&poly);
								}
								
							}
						}
					}
				}
			}
		}
#else
//		PPPoint pos[4];
//		preCalc(obj,drawArea.x,drawArea.y,0,&pos[0]);
//		preCalc(obj,drawArea.width-1,drawArea.y,1,&pos[1]);
//		preCalc(obj,drawArea.width-1,drawArea.height-1,2,&pos[2]);
//		preCalc(obj,drawArea.x,drawArea.height-1,3,&pos[3]);
		
		if (mapSize.width > 0 && mapSize.height > 0) {
			for (int y=drawArea.y;y<drawArea.height;y++) {
				for (int x=drawArea.x;x<drawArea.width;x++) {
					if (getGID(x,y) != 0) {
						PPPoint p = op-obj->poly.origin+ts;
						PPPoint sp = PPPoint(x*s.width,y*s.height);
						PPTMXChip c = getAttribute(x,y);
						poly.origin = obj->poly.origin+p+origin;
						poly.sprite(p.x+sp.x,p.y+sp.y,getGID(x,y)-1,0);
						poly._texture = obj->poly._texture;
						poly.scale = obj->poly.scale;
						poly.flags = c.flag;
						poly.color.r = obj->poly.color.r*c.color.r/255;
						poly.color.g = obj->poly.color.g*c.color.g/255;
						poly.color.b = obj->poly.color.b*c.color.b/255;
						poly.color.a = obj->poly.color.a*c.color.a/255;
						obj->world()->projector->DrawPoly(&poly);
					}
				}
			}
		}
#endif
	}
}

PPTMXProperty::PPTMXProperty()
{
}

PPTMXProperty::~PPTMXProperty()
{
}

PPTMXObject::PPTMXObject()
{
}

PPTMXObject::~PPTMXObject()
{
	if (!property.empty()) {
		std::vector<PPTMXProperty*>::iterator iter;
		for (iter = property.begin(); iter != property.end(); ++iter) if (*iter) delete (*iter);
		property.clear();
	}
}

PPTMXObjectGroup::PPTMXObjectGroup()
{
}

PPTMXObjectGroup::~PPTMXObjectGroup()
{
	if (!object.empty()) {
		std::vector<PPTMXObject*>::iterator iter;
		for (iter = object.begin(); iter != object.end(); ++iter) if (*iter) delete (*iter);
		object.clear();
	}
}

#pragma mark -

#define BUFFER_INC_FACTOR (2)

static int inflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint)
{
	/* ret value */
	int err = Z_OK;

	int bufferSize = outLenghtHint;
#if 1
	*out = (unsigned char*)malloc(bufferSize);
#else
	*out = new unsigned char[bufferSize];
#endif

	z_stream d_stream; /* decompression stream */	
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = in;
	d_stream.avail_in = inLength;
	d_stream.next_out = *out;
	d_stream.avail_out = bufferSize;

	/* window size to hold 256k */
	if( (err = inflateInit2(&d_stream, 15 + 32)) != Z_OK )
		return err;

	for (;;) 
	{
		err = inflate(&d_stream, Z_NO_FLUSH);

		if (err == Z_STREAM_END)
		{
			break;
		}

		switch (err) 
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != Z_STREAM_END) {
#if 1
//			free(*out);
			*out = (unsigned char*)realloc(*out,bufferSize * BUFFER_INC_FACTOR);
#else			
			delete [] *out;
			*out = new unsigned char[bufferSize * BUFFER_INC_FACTOR];
#endif
			/* not enough memory, ouch */
			if (! *out ) 
			{
//				CCLOG("cocos2d: ZipUtils: realloc failed");
				inflateEnd(&d_stream);
				return Z_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = bufferSize;
			bufferSize *= BUFFER_INC_FACTOR;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = inflateEnd(&d_stream);
	return err;
}

static unsigned char* base64decode(const char* string,unsigned int* length)
{
	long i;
	unsigned char* buffer = NULL;
	int outsize = 0;
	long size = 0;
	if (string == NULL) return NULL;
//	if (length == 0) length = strlen(string);
	while (1) {
	
		/* バッファサイズの計算 */
		for (i=0;i<*length;i++) {
			if ((string[i] >= 'A' && string[i] <= 'Z')
			 || (string[i] >= 'a' && string[i] <= 'z')
			 || (string[i] >= '0' && string[i] <= '9')
			 || (string[i] == '+' || string[i] == '/')) {
				size ++;
			} else
			if (string[i] == '=' || string[i] == 0) {
				break;
			}
		}
		
		if (buffer==NULL) {
			long tsize = (size / 4)*3;
			switch (size % 4) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				tsize += 1;
				break;
			case 3:
				tsize += 2;
				break;
			}
			if (tsize>0) {
				buffer = (unsigned char*)malloc(tsize+1);
			}
			if (buffer == NULL) break;
		}
		
		/* エンコード */
		{
			long j;
			int s=0;
			for (i=0,j=0;i<*length;i++) {
				int c = -1;
				char a = string[i];
			
				if (a >= 'A' && a <= 'Z') {
					c = 0+a-'A';
				} else
				if (a >= 'a' && a <= 'z') {
					c = 26+a-'a';
				} else
				if (a >= '0' && a <= '9') {
					c = 52+a-'0';
				} else
				if (a == '+') {
					c = 62;
				} else
				if (a == '/') {
					c = 63;
				} else
				if (a == '=' || a == 0) {
					break;
				}
				
				if (c >= 0) {
					switch (s%4) {
					case 0:
						buffer[j] = c << 2;
						break;
					case 1:
						buffer[j] |= c >> 4;
						j ++;
						buffer[j] = c << 4;
						break;
					case 2:
						buffer[j] |= c >> 2;
						j ++;
						buffer[j] = c << 6;
						break;
					case 3:
						buffer[j] |= c;
						j ++;
						break;
					}
					outsize = (int)j;
					s ++;
				}
			}
		}
		
		break;
	}
	*length = outsize;
	return buffer;
}

PPTMXMap::PPTMXMap(PPWorld* world) : PPObject(world),curLayer(-1),editPos(PPPointZero)
{
	PPTMXLayer* _layer = new PPTMXLayer();
	_layer->name = "layer";
	_layer->mapSize.width = 1;
	_layer->mapSize.height = 1;
	_layer->chip = (PPTMXChip*)calloc(1,_layer->mapSize.width*_layer->mapSize.height*sizeof(PPTMXChip));
	layer.push_back(_layer);
	curLayer = (int)layer.size()-1;
#ifdef _OBJMEM_DEBUG_
//printf("PPTMXMap new\n");
  objname="PPTMXMap";
  printf("alloc %s\n",objname);
  fflush(stdout);
#endif
}

PPTMXMap::~PPTMXMap()
{
	reset();
//printf("PPTMXMap delete %f,%f\n",mapSize.width,mapSize.height);
}

void PPTMXMap::reset()
{
//	PPObject::init();

	if (!tileset.empty()) {
		std::vector<PPTMXTileSet*>::iterator iter;
		for (iter = tileset.begin(); iter != tileset.end(); ++iter) if (*iter) delete (*iter);
		tileset.clear();
	}
	if (!layer.empty()) {
		std::vector<PPTMXLayer*>::iterator iter;
		for (iter = layer.begin(); iter != layer.end(); ++iter) if (*iter) delete (*iter);
		layer.clear();
	}
	if (!objectgroup.empty()) {
		std::vector<PPTMXObjectGroup*>::iterator iter;
		for (iter = objectgroup.begin(); iter != objectgroup.end(); ++iter) if (*iter) delete (*iter);
		objectgroup.clear();
	}
	
	curLayer = -1;
	editPos = PPPointZero;
	editAttr.reset();
}

//void PPTMXMap::start(const char* name)
//{
//	load(name);
//	PPObject::start();
//	NEXT(PPTMXMap::stepIdle);
//}
	
void PPTMXMap::stepIdle()
{
}

bool PPTMXMap::load(const char* path)
{
	reset();

	for (int i=0;path[i];i++) {
		if (path[i] == '.') {
			if (strcmp(&path[i+1],"plist") == 0) {
				PPGameMap map(path);
				
				if (map.loadError) {
					PPReadErrorSet(path);
					return false;
				}

				for (int j=0;j<3;j++) {
					PPTMXLayer* _layer = new PPTMXLayer();
					if (map.map[j].map) {
						{
							std::stringstream ss;
							ss << "layer" << (j+1);
							_layer->name = ss.str();
						}
						_layer->mapSize.width = map.map[j].width;
						_layer->mapSize.height = map.map[j].height;
						_layer->chip = (PPTMXChip*)calloc(1,_layer->mapSize.width*_layer->mapSize.height*sizeof(PPTMXChip));
	//					_layer->att = (unsigned char*)calloc(1,_layer->width*_layer->height);
						
						if (_layer->chip) {
							for (int y=0;y<map.map[j].height;y++) {
								for (int x=0;x<map.map[j].width;x++) {
									_layer->chip[x+y*map.map[j].width].reset();
									_layer->chip[x+y*map.map[j].width].tile = map.map[j].map[x+y*map.map[j].width];
									if (_layer->chip[x+y*map.map[j].width].tile > 0) _layer->chip[x+y*map.map[j].width].tile ++;
								}
							}
						}
					}
					
					layer.push_back(_layer);
				}

//				mapSize.width = map.map[0].width;
//				mapSize.height = map.map[0].height;

				{
					PPTMXObjectGroup* group = new PPTMXObjectGroup();
					group->name = "Objects";
					group->width = map.map[0].width;
					group->height = map.map[0].height;
					objectgroup.push_back(group);
					
					for (int j=0;j<map.map[0].eventNum;j++) {
						PPGameMapEvent* e = &map.map[0].event[j];
						PPTMXObject* object = new PPTMXObject();
						object->name = e->name;
						object->type = "";
						object->x = e->x*32+e->dx;
						object->y = e->y*32+e->dy;
						object->width = e->w*32;
						object->height = e->h*32;
						objectgroup[objectgroup.size()-1]->object.push_back(object);
					}
				}

				drawArea = PPRect(0,0,map.map[0].width,map.map[0].height);
				return true;
			}
		}
	}
	
	char* str = NULL;
	unsigned long size;
	_curobject = NULL;

#ifdef __COCOS2DX__
	str = (char*)ccGetFileData(PPGameResourcePath(path),"r",&size);
	if (str == NULL) {
		str = (char*)ccGetFileData(path,"r",&size);
	}
#else
	FILE* fp = fopen(PPGameDataPath(path),"r");
	if (fp == NULL) {
		fp = fopen(PPGameResourcePath(path),"r");
	}
	if (fp) {
		fseek(fp,0,SEEK_END);
		size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		str = (char*)malloc(size+1);
		fread(str,size,1,fp);
		str[size] = 0;
		fclose(fp);
	}
#endif

	if (str==NULL) {
		PPReadErrorSet(path);
		return false;
	}

#ifdef __USE_PPSAX__
	PPSAXHandler saxHandler;
	memset( &saxHandler, 0, sizeof(PPSAXHandler) );
	saxHandler.startElement = &PPTMXMap_startElement;
	saxHandler.endElement = &PPTMXMap_endElement;
	saxHandler.characters = &PPTMXMap_textHandler;
	saxHandler.userdata = this;
	saxHandler.xmltext = str;
	PPSAXParser(&saxHandler);
#else
	xmlSAXHandler saxHandler;
	memset( &saxHandler, 0, sizeof(saxHandler) );
	saxHandler.initialized = XML_SAX2_MAGIC;
	saxHandler.startElement = &PPTMXMap_startElement;
	saxHandler.endElement = &PPTMXMap_endElement;
	saxHandler.characters = &PPTMXMap_textHandler;
	int result = (int)xmlSAXUserParseMemory( &saxHandler, this, str, (int)size );
	if ( result != 0 )
	{
		return false;
	}
	xmlCleanupGlobals();
//	xmlCleanupParser();
//	xmlMemoryDump();
#endif

#ifdef __COCOS2DX__
	delete str;
#else
	free(str);
#endif		

	if (!layer.empty()) {
		curLayer = 0;
	}
//	drawArea = PPRect(0,0,mapSize.width,mapSize.height);
	return false;
}

void PPTMXMap::drawSelf(PPPoint _pos)
{
	if (!layer.empty()) {
//		PPPoint p = autoLayout(pos);
    poly.origin = PPPointZero;
		for (int i=0;i<layer.size();i++) {
			if (layer[i]) {
				if (layer[i]->visible) {
					//layer[i]->pos = p+_pos;
					layer[i]->origin = origin;
					layer[i]->draw(this,drawArea);
				}
			}
		}
	}
}


PPSize PPTMXMap::size()
{
#if 1
	PPSize mapSize=drawArea.size();
#else
	PPSize mapSize=PPSizeZero;
	if (curLayer > 0) {
		mapSize = layer[curLayer]->mapSize;
	}
#endif
	PPSize rs = mapSize*tileSize()*poly.scale;
	return rs;
}

void PPTMXMap::print(const char* _str)
{
	const signed char* str = (const signed char*)_str;
	for (int i=0;str[i]!=0;i++) {
		if (str[i] == ' ') {
			tset(str[i]+1,editPos);
			moveCursor(1,0);
		} else
		if (str[i] >= 0 && str[i] <= 0x20) {
			if (str[i] == '\n') {
				editPos.y += 1;
				editPos.x = 0;
				continue;
			} else
			if (str[i] == '\r') {
				editPos.x = 0;
				continue;
			}
		} else
		if (0x20 < str[i] && str[i] != '%') {
			tset(str[i]+1,editPos);
			moveCursor(1,0);
		} else {
			const signed char* dst = &str[i];
			char ccode = -1;
			int ccodelen = 0;

			//濁点チェック
#if 1
			{
				int j=PPFontTable::findDakuten((const char*)dst);
				if (j>=0) {
					dst = (const signed char*)PPFontTable::dakutend[j];
					ccode = PPFontTable::controlcode[j];
					ccodelen = (int)strlen(PPFontTable::dakutens[j]);
				}
			}
#else
			for (int j=0;PPFontTable::dakutens[j]!=NULL;j++) {
				int l = strlen(PPNormalFont::dakutens[j]);
				if (l > 0) {
					if (strncmp((const char*)dst,PPNormalFont::dakutens[j],l) == 0) {
						dst = (const signed char*)PPNormalFont::dakutend[j];
						ccode = PPNormalFont::controlcode[j];
						ccodelen = l;
						break;
					}
				}
			}
#endif
			
			//コントロールコードの処理
			switch (ccode) {
			case 0:
				i += ccodelen-1;
				break;
			case 1:
				tset(19*16+14+1,PPPoint(editPos.x,editPos.y-1));
				break;
			case 2:
				tset(19*16+15+1,PPPoint(editPos.x,editPos.y-1));
				break;
			case 0x11:		//右
				moveCursor(1,0);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x12:		//左
				moveCursor(-1,0);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x13:		//上
				moveCursor(0,-1);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x14:		//下
				moveCursor(0,1);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x15:		//上下反転
				if (editAttr.flag & PPFLIP_H) {
					editAttr.flag &= (~PPFLIP_H);
				} else {
					editAttr.flag |= PPFLIP_H;
				}
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x16:		//左右反転
				if (editAttr.flag & PPFLIP_V) {
					editAttr.flag &= (~PPFLIP_V);
				} else {
					editAttr.flag |= PPFLIP_V;
				}
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x17:		//左回転
				{
					signed short f = (editAttr.flag & PPFLIP_RMASK)>>2;
					f ++;
					if (f < 0) f = 3;
					if (f > 3) f = 0;
					editAttr.flag &= ~PPFLIP_RMASK;
					editAttr.flag |= (f<<2);
				}
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x18:		//右回転
				{
					signed short f = (editAttr.flag & PPFLIP_RMASK)>>2;
					f --;
					if (f < 0) f = 3;
					if (f > 3) f = 0;
					editAttr.flag &= ~PPFLIP_RMASK;
					editAttr.flag |= (f<<2);
				}
				i += ccodelen-1;
				dst = NULL;
			case 0x19:		//リセット
				editAttr.flag = 0;
				i += ccodelen-1;
				dst = NULL;
				break;
			default:
				break;
			}

			//文字描画
			if (dst) {
#if 1
				int j=PPFontTable::findLetter((const char*)dst);
				if (j>=0) {
					tset(j+1,editPos);
					if (ccode != 0) i += strlen(PPFontTable::strtable[j])-1;
					moveCursor(1,0);
				}
#else
				for (int j=0;PPNormalFont::strtable[j]!=NULL;j++) {
					int l = strlen(PPNormalFont::strtable[j]);
					if (l > 0) {
						if (strncmp((const char*)dst,PPNormalFont::strtable[j],l) == 0) {
							tset(j+1,editPos);
							if (ccode != 0) i += l-1;
							moveCursor(1,0);
							break;
						}
					}
				}
#endif
			}

		}
	}
}

void PPTMXMap::moveCursor(int dx,int dy)
{
	if (curLayer>=0) {
		PPSize mapSize = layer[curLayer]->mapSize;

		editPos.x += dx;
		if (editPos.x >= mapSize.width) {
			editPos.x = 0;
			editPos.y += 1;
		}
		if (editPos.x < 0) {
			editPos.x = mapSize.width-1;
			editPos.y -= 1;
		}
		editPos.y += dy;
	}
}

void PPTMXMap::tset(unsigned int gid,PPPoint pos)
{
	if (curLayer>=0) {
		layer[curLayer]->setGID(pos.x,pos.y,gid);
		layer[curLayer]->setAttribute(pos.x,pos.y,editAttr);
	}
}

unsigned int PPTMXMap::tget(PPPoint pos)
{
	if (curLayer >= 0) {
		return layer[curLayer]->getGID(pos.x,pos.y);
	}
	return 0;
}

void PPTMXMap::aset(PPTMXChip gid,PPPoint pos)
{
	if (curLayer>=0) layer[curLayer]->setAttribute(pos.x,pos.y,gid);
}

PPTMXChip PPTMXMap::aget(PPPoint pos)
{
	if (curLayer >= 0) {
		return layer[curLayer]->getAttribute(pos.x,pos.y);
	}
	return PPTMXChip();
}

void PPTMXMap::line(PPPoint pos1,PPPoint pos2,int tid)
{
	if (curLayer>=0) {
		PPSize mapSize = layer[curLayer]->mapSize;

		int x1,y1,x2,y2;
		int _width = mapSize.width;
		int _height = mapSize.height;
		x1 = pos1.x;
		y1 = pos1.y;
		x2 = pos2.x;
		y2 = pos2.y;

		int dx = 1;
		int dy = 1;
		
		if (x1 >= 0 && x1 < _width && y1 >= 0 && y1 < _height) {
		} else
		if (x2 >= 0 && x2 < _width && y2 >= 0 && y2 < _height) {
			int t;
			t = x2;
			x2 = x1;
			x1 = t;
			t = y2;
			y2 = y1;
			y1 = t;
		}
		
		int width = x2-x1;
		if (width < 0) {
			width = -width;
			dx = -1;
		}
		int height = y2-y1;
		if (height < 0) {
			height = -height;
			dy = -1;
		}
		
		if (width > height) {
			int px,py;
			int sum=0;

			if (x2 < x1) {
				int t;
				t = x2;
				x2 = x1;
				x1 = t;
				t = y2;
				y1 = t;
				dy = -dy;
			}
		
			py = y1;
			for (px=x1;px<=x2;px++) {
				sum += height;
				if (sum > width) {
					sum -= width;
					py += dy;
				}
				if (px >= 0 && px < _width && py >= 0 && py < _height) {
					tset(tid,PPPoint(px,py));
				}
			}
		} else {
			int px,py;
			int sum=0;

			if (y2 < y1) {
				int t;
				t = x2;
				x1 = t;
				t = y2;
				y2 = y1;
				y1 = t;
				dx = -dx;
			}

			px = x1;
			for (py=y1;py<=y2;py++) {
				sum += width;
				if (sum > height) {
					sum -= height;
					px += dx;
				}
				if (px >= 0 && px < _width && py >= 0 && py < _height) {
					tset(tid,PPPoint(px,py));
				}
			}
		}
	}
}

void PPTMXMap::box(PPRect rect,int tid)
{
	for (int x=rect.x;x<rect.x+rect.width;x++) {
		tset(tid,PPPoint(x,rect.y));
		tset(tid,PPPoint(x,rect.y+rect.height-1));
	}
	for (int y=rect.y+1;y<rect.y+rect.height-1;y++) {
		tset(tid,PPPoint(rect.x,y));
		tset(tid,PPPoint(rect.x+rect.width-1,y));
	}
}

void PPTMXMap::fill(PPRect rect,int tid)
{
	for (int x=rect.x;x<rect.x+rect.width;x++) {
		for (int y=rect.y;y<rect.y+rect.height;y++) {
			tset(tid,PPPoint(x,y));
		}
	}
}

void PPTMXMap::paint(PPPoint pos,int tid)
{
	if (curLayer>=0) {
		PPSize mapSize = layer[curLayer]->mapSize;
		if (pos.x >= 0 && pos.x < mapSize.width && pos.y >= 0 && pos.y < mapSize.height) {
			int spat = tget(pos);
			if (spat == tid) return;
			{
				int px;
				int sx;
				int ex;

				for (sx=pos.x;;--sx) {
					if (!(sx >= 0 && sx < mapSize.width)) break;
					if (tget(PPPoint(sx,pos.y)) != spat) {
						sx ++;
						break;
					}
				}
				if (sx<0) sx=0;
				if (sx>=mapSize.width) sx=mapSize.width-1;

				for (ex=pos.x;;++ex) {
					if (!(ex >= 0 && ex < mapSize.width)) break;
					if (tget(PPPoint(ex,pos.y)) != spat) {
						ex --;
						break;
					}
				}
				if (ex<0) ex=0;
				if (ex>=mapSize.width) ex=mapSize.width-1;

				for (px=sx;px<=ex;px++) {
					tset(tid,PPPoint(px,pos.y));
				}
				
				for (px=sx;px<=ex;px++) {
					if (pos.y-1>=0) {
						if (tget(PPPoint(px,pos.y-1)) == spat) {
							paint(PPPoint(px,pos.y-1),tid);
						}
					}
					if (pos.y+1<mapSize.height) {
						if (tget(PPPoint(px,pos.y+1)) == spat) {
							paint(PPPoint(px,pos.y+1),tid);
						}
					}
				}
			}
		}
	}
}

void PPTMXMap::paint(PPPoint pos,int tid,int borderid)
{
	if (curLayer>=0) {
		PPSize mapSize = layer[curLayer]->mapSize;
		if (pos.x >= 0 && pos.x < mapSize.width &&  pos.y >= 0 && pos.y < mapSize.height) {
			int spat = tget(pos);
			if (tid == spat) return;
			{
				int px;
				int sx;
				int ex;

				for (sx=pos.x;;--sx) {
					if (!(sx >= 0 && sx < mapSize.width)) break;
					if (tget(PPPoint(sx,pos.y)) == borderid) {
						sx ++;
						break;
					}
				}
				if (sx<0) sx=0;
				if (sx>=mapSize.width) sx=mapSize.width-1;

				for (ex=pos.x;;++ex) {
					if (!(ex >= 0 && ex < mapSize.width)) break;
					if (tget(PPPoint(ex,pos.y)) == borderid) {
						ex --;
						break;
					}
				}
				if (ex<0) ex=0;
				if (ex>=mapSize.width) ex=mapSize.width-1;

				for (px=sx;px<=ex;px++) {
					tset(tid,PPPoint(px,pos.y));
				}
				
				for (px=sx;px<=ex;px++) {
					if (pos.y-1>=0) {
						if (tget(PPPoint(px,pos.y-1)) != borderid) {
							paint(PPPoint(px,pos.y-1),tid,borderid);
						}
					}
					if (pos.y+1<mapSize.height) {
						if (tget(PPPoint(px,pos.y+1)) != borderid) {
							paint(PPPoint(px,pos.y+1),tid,borderid);
						}
					}
				}
			}
		}
	}
}

void PPTMXMap::circle(PPPoint pos,float r,int tid,float start,float end)
{
	while (start < 0) {
		start += 360;
		end += 360;
	}
	while (start >= 360) {
		start -= 360;
		end -= 360;
	}
	int count = 0;
	{
		int x = r;
		int y = 0;
		int F = -2 * r + 3;
		while (x >= y) {
			if ( F >= 0 ) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
			count ++;
		}
	}
	int sq = start*count*8/360;
	int eq = end  *count*8/360;
	int c[16];
	int d[8] = { 1,-1,-1, 1,-1, 1, 1,-1};
	int t[8] = { 0, 4, 8, 4, 2, 2, 6, 6};
	for (int i=0;i<16;i++) {
		c[i] = t[i%8]*count+8*count*(i/8);
	}
	{
		int x = r;
		int y = 0;
		int F = -2 * r + 3;
		while (x >= y) {
			if ((sq <= c[0] && c[0] <= eq) || (sq <= c[ 8] && c[ 8] <= eq)) tset(tid,PPPoint(pos.x+x,pos.y+y));
			if ((sq <= c[1] && c[1] <= eq) || (sq <= c[ 9] && c[ 9] <= eq)) tset(tid,PPPoint(pos.x-x,pos.y+y));
			if ((sq <= c[2] && c[2] <= eq) || (sq <= c[10] && c[10] <= eq)) tset(tid,PPPoint(pos.x+x,pos.y-y));
			if ((sq <= c[3] && c[3] <= eq) || (sq <= c[11] && c[11] <= eq)) tset(tid,PPPoint(pos.x-x,pos.y-y));
			if ((sq <= c[4] && c[4] <= eq) || (sq <= c[12] && c[12] <= eq)) tset(tid,PPPoint(pos.x+y,pos.y+x));
			if ((sq <= c[5] && c[5] <= eq) || (sq <= c[13] && c[13] <= eq)) tset(tid,PPPoint(pos.x-y,pos.y+x));
			if ((sq <= c[6] && c[6] <= eq) || (sq <= c[14] && c[14] <= eq)) tset(tid,PPPoint(pos.x+y,pos.y-x));
			if ((sq <= c[7] && c[7] <= eq) || (sq <= c[15] && c[15] <= eq)) tset(tid,PPPoint(pos.x-y,pos.y-x));
			if ( F >= 0 ) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
			for (int i=0;i<16;i++) {
				c[i] += d[i%8];
			}
		}
	}
}

void PPTMXMap::scroll(PPPoint move)
{
	if (curLayer>=0) {
		PPSize mapSize = layer[curLayer]->mapSize;
		unsigned int size = mapSize.width*mapSize.height*sizeof(PPTMXChip);
		PPTMXChip* p = (PPTMXChip*)malloc(size);
		if (p) {
			if (layer[curLayer]->chip) {
				int mx,my;
				mx = move.x;
				my = move.y;
				while (mx < 0) mx += mapSize.width;
				while (my < 0) my += mapSize.height;
				while (mx >= mapSize.width) mx -= mapSize.width;
				while (my >= mapSize.height) my -= mapSize.height;
				memcpy(p,layer[curLayer]->chip,size);
				for (int y=0;y<mapSize.height;y++) {
					for (int x=0;x<mapSize.width;x++) {
						int tx,ty;
						tx = x+mx;
						ty = y+my;
						if (tx >= mapSize.width) tx -= mapSize.width;
						if (ty >= mapSize.height) ty -= mapSize.height;
						layer[curLayer]->chip[(int)(tx+ty*mapSize.width)] = p[(int)(x+y*mapSize.width)];
					}
				}
			}
			free(p);
		}
	}
}

void PPTMXMap::copy(PPRect rect,PPPoint pos)
{
	if (curLayer>=0) {
		PPSize mapSize = layer[curLayer]->mapSize;
		unsigned int size = mapSize.width*mapSize.height*sizeof(PPTMXChip);
		PPTMXChip* p = (PPTMXChip*)malloc(size);
		if (p) {
			if (layer[curLayer]->chip) {
				while (rect.x < 0) rect.x+=mapSize.width;
				while (rect.y < 0) rect.y+=mapSize.height;
				while (rect.x >= mapSize.width) rect.x-=mapSize.width;
				while (rect.y >= mapSize.height) rect.y-=mapSize.height;
				memcpy(p,layer[curLayer]->chip,size);
				for (int y=0;y<rect.height;y++) {
					for (int x=0;x<rect.width;x++) {
						int tx,ty;
						tx = pos.x+x;
						ty = pos.y+y;
						if (tx >= mapSize.width) tx -= mapSize.width;
						if (ty >= mapSize.height) ty -= mapSize.height;
						int sx,sy;
						sx = rect.x+x;
						sy = rect.y+y;
						if (sx >= mapSize.width) sx -= mapSize.width;
						if (sy >= mapSize.height) sy -= mapSize.height;
						layer[curLayer]->chip[(int)(tx+ty*mapSize.width)] = p[(int)(sx+sy*mapSize.width)];
					}
				}
			}
			free(p);
		}
	}
}

static const char* valueForKey(const char* key,const PPXmlChar **atts)
{
	if (atts) {
		if (atts[0]) {
			for(int i=0;atts[i];i+=2) {
				if (strcmp((char*)atts[i],(char*)key) == 0) {
//printf("%s=%s\n",atts[i],atts[i+1]);
					return (char*)atts[i+1];
				}
			}
		}
	}
	return "";
}

void PPTMXMap_startElement(void *ctx, const PPXmlChar *name, const PPXmlChar **atts)
{
	PPTMXMap* map = (PPTMXMap*)ctx;
	std::string elementName = (char*)name;

	if(elementName == "map")
	{
		map->version = valueForKey("version",atts);
		map->drawArea.width = atoi(valueForKey("width",atts));
		map->drawArea.height = atoi(valueForKey("height",atts));
		map->tilewidth = (float)atof(valueForKey("tilewidth",atts));
		map->tileheight = (float)atof(valueForKey("tileheight",atts));
	} 
	else if(elementName == "tileset") 
	{
		PPTMXTileSet* tset = new PPTMXTileSet();
		tset->firstgid = atoi(valueForKey("firstgid",atts));
		tset->name = valueForKey("name",atts);
		tset->tilewidth = atoi(valueForKey("tilewidth",atts));
		tset->tileheight = atoi(valueForKey("tileheight",atts));
		map->tileset.push_back(tset);

	}
	else if(elementName == "tile")
	{
	}
	else if(elementName == "layer")
	{
		PPTMXLayer* layer = new PPTMXLayer();
		layer->name = valueForKey("name",atts);
		layer->mapSize.width = atoi(valueForKey("width",atts));
		layer->mapSize.height = atoi(valueForKey("height",atts));
		map->layer.push_back(layer);
	} 
	else if(elementName == "objectgroup")
	{
		PPTMXObjectGroup* group = new PPTMXObjectGroup();
		group->name = valueForKey("name",atts);
		group->width = atoi(valueForKey("width",atts));
		group->height = atoi(valueForKey("height",atts));
		map->objectgroup.push_back(group);
	}
	else if(elementName == "image")
	{
		if (!map->tileset.empty()) {

			map->tileset[map->tileset.size()-1]->source = valueForKey("source",atts);
			map->tileset[map->tileset.size()-1]->width = atoi(valueForKey("width",atts));
			map->tileset[map->tileset.size()-1]->height = atoi(valueForKey("height",atts));
		}
	}
	else if(elementName == "data")
	{
		if (!map->layer.empty()) {
			PPTMXLayer* layer = map->layer[map->layer.size()-1];
			layer->encoding = valueForKey("encoding",atts);
			layer->compression = valueForKey("compression",atts);
		}
	}
	else if(elementName == "object")
	{
		if (!map->objectgroup.empty()) {
			PPTMXObject* object = new PPTMXObject();
			object->name = valueForKey("name",atts);
			object->type = valueForKey("type",atts);
			object->x = atoi(valueForKey("x",atts));
			object->y = atoi(valueForKey("y",atts));
			object->width = atoi(valueForKey("width",atts));
			object->height = atoi(valueForKey("height",atts));
			object->gid = atoi(valueForKey("gid",atts));
			map->objectgroup[map->objectgroup.size()-1]->object.push_back(object);
			map->_curobject = object;
		}
	}
	else if(elementName == "property")
	{
		if (map->_curobject) {
			PPTMXProperty* property = new PPTMXProperty();
			property->name = valueForKey("name",atts);
			property->value = valueForKey("value",atts);
			map->_curobject->property.push_back(property);
		}
	}
}

void PPTMXMap_endElement(void *ctx, const PPXmlChar *name)
{
	PPTMXMap* map = (PPTMXMap*)ctx;
	std::string elementName = (char*)name;
	if(elementName == "data") {
		if (!map->layer.empty()) {
			PPTMXLayer* layer = map->layer[map->layer.size()-1];
			if (layer->encoding == "base64" && (layer->compression == "gzip" || layer->compression == "zlib")) {
				unsigned char* mapdata;
				unsigned int maplength;
				unsigned int length = (unsigned int)map->currentStr.length();
				unsigned char* b = base64decode(map->currentStr.c_str(),&length);
				if (b) {
					if (layer->chip) free(layer->chip);
					layer->chip = NULL;
					if (inflateMemoryWithHint(b,length,&mapdata,&maplength,32) == 0) {
						layer->chip = (PPTMXChip*)malloc(layer->mapSize.width*layer->mapSize.height*sizeof(PPTMXChip));
						for (int i=0;i<maplength/sizeof(unsigned int);i++) {
							layer->chip[i].reset();
							layer->chip[i].tile = ((unsigned int*)mapdata)[i];
							layer->chip[i].flag = 0;
						}
						free(mapdata);
					}
					free(b);
				}
			} else
			if (layer->encoding == "csv") {
				
			}
		}
		map->currentStr = "";
	} 
	else if (elementName == "map") {
	}	
	else if (elementName == "layer") {
	}
	else if (elementName == "objectgroup") {
	} 
	else if (elementName == "object") {
		map->_curobject = NULL;
	}
}

void PPTMXMap_textHandler(void *ctx, const PPXmlChar *name, int len)
{
	PPTMXMap* map = (PPTMXMap*)ctx;
	std::string text((char*)name,0,len);
	if (strlen(text.c_str()) > 0) {
		for (int i=0;i<text.length();i++) {
			if (text.c_str()[i] < 0 || (text.c_str()[i] >= 0x20 && text.c_str()[i] != ' ')) {
				map->currentStr = map->currentStr + text;
				return;
			}
		}
	}
}


#pragma mark -

#include "PPLuaScript.h"

static int funcLoad(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (s->argCount > 0) {
		if (!m->load(s->args(0))) {
			if (PPReadError()) {
				PPReadErrorReset();
				return luaL_error(L,"file read error '%s'",s->args(0));
			}
		}
		PPGameTextureOption option;
		if (s->argCount > 1) {
			if (s->isTable(L,1)) {
				option = s->getTextureOption(L,1,option);
			}
		}
		if (!m->tileset.empty()) {
			const char* str = m->tileset[m->tileset.size()-1]->source.c_str();
			if (str) {
				m->poly.initTexture(m->world()->projector->textureManager->loadTexture(str,option));

				if (PPReadError()) {
					PPReadErrorReset();
					return luaL_error(L,"texture file read error '%s'",str);
				}
			} else {
				return luaL_error(L,"TMX file error");
			}
		}
	} else {
		return luaL_argerror(L,1,"no value");
	}
	return 0;
}

static int funcGID(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	PPPoint pos = m->editPos;
	int gid = 0;

	if (s->argCount > 0) {
		if (s->argCount == 1) {
			if (s->isTable(L,0)) {
				pos = PPPoint(s->tableNumber(L,0,1,"x"),s->tableNumber(L,0,2,"y"));
			} else {
				gid = (int)s->integer(0);
				m->tset(gid,pos);
				return 0;
			}
		} else
		if (s->argCount >= 2) {
			if (s->isTable(L,0)) {
				pos = PPPoint(s->tableNumber(L,0,1,"x"),s->tableNumber(L,0,2,"y"));
				gid = (int)s->integer(1);
				m->tset(gid,pos);
				return 0;
			} else {
				pos = PPPoint(s->number(0),s->number(1));
				if (s->argCount >= 3) {
					gid = (int)s->integer(2);
					m->tset(gid,pos);
					return 0;
				}
			}
		}
	}

	lua_pushinteger(L,m->tget(pos));
	return 1;
}

static int funcSelectLayer(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (s->argCount > 0) {
		m->curLayer = -1;
		if (!m->layer.empty()) {
			for (int i=0;i<m->layer.size();i++) {
				if (m->layer[i]->name == s->args(0)) {
					m->curLayer = i;
					break;
				}
			}
		}
//		return 0;
	}
	lua_pushinteger(L,m->curLayer+1);
	return 1;
}

static int funcLayerList(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (!m->layer.empty()) {
		lua_createtable(L, (int)m->layer.size(), 0);
		for (int i=0;i<m->layer.size();i++) {
			lua_pushstring(L,m->layer[i]->name.c_str());
			lua_rawseti(L, -2, i+1);
		}
	} else {
		lua_createtable(L, 0, 0);
	}
	return 1;
}

static int funcMapSize(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	PPSize mapSize = PPSizeZero;
	if (s->argCount > 0) {
		PPSize size = s->getSize(L,0,1,1);
		if (size.width  <= 0) size.width  = 1;
		if (size.height <= 0) size.height = 1;
		if (m->curLayer >= 0) {
			m->layer[m->curLayer]->resize(size.width,size.height);
		}
		return 0;
//		m->resize(size);
	}
	if (m->curLayer >= 0) {
		mapSize = m->layer[m->curLayer]->mapSize;
	}
	return s->returnSize(L,mapSize);
}

static int funcObjectGroupList(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (!m->objectgroup.empty()) {
		lua_createtable(L, 0, (int)m->layer.size());
		for (int i=0;i<m->objectgroup.size();i++) {
			lua_pushstring(L,m->objectgroup[i]->name.c_str());
			lua_rawseti(L, -2, i+1);
		}
	} else {
		lua_createtable(L, 0, 0);
	}
	return 1;
}

static bool pushTMXObject(lua_State* L,PPTMXObject* o)
{
	if (o==NULL) return false;
	lua_createtable(L, 0, 7);
	lua_pushnumber(L,o->x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L,o->y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L,o->width);
	lua_setfield(L, -2, "width");
	lua_pushnumber(L,o->height);
	lua_setfield(L, -2, "height");
	lua_pushstring(L,o->name.c_str());
	lua_setfield(L, -2, "name");
	lua_pushstring(L,o->type.c_str());
	lua_setfield(L, -2, "type");
	lua_pushnumber(L,o->gid);
	lua_setfield(L, -2, "gid");
	return true;
}

static int funcObjectList(lua_State* L)
{
	bool found=false;
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (s->argCount > 0) {
		if (!m->objectgroup.empty()) {
			for (int i=0;i<m->objectgroup.size();i++) {
				if (m->objectgroup[i]->name == s->args(0)) {
					lua_createtable(L, 0, (int)m->objectgroup[i]->object.size());
					for (int j=0;j<m->objectgroup[i]->object.size();j++) {
						PPTMXObject* o = m->objectgroup[i]->object[j];
						if (pushTMXObject(L,o)) {
							lua_rawseti(L, -2, j+1);
						}
					}
					found = true;
					break;
				}
			}
		}
	}
	if (!found) {
		lua_createtable(L, 0, 0);
	}
	return 1;
}

static int funcObjectInfo(lua_State* L)
{
	bool found=false;
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (s->argCount > 1) {
		if (!m->objectgroup.empty()) {
			for (int i=0;i<m->objectgroup.size();i++) {
				if (m->objectgroup[i]->name == s->args(0)) {
					int t = (int)m->objectgroup[i]->object.size();
					int j = atoi(s->args(1));
					if (j >= 1 && j <= t) {
						PPTMXObject* o = m->objectgroup[i]->object[j-1];
						if (pushTMXObject(L,o)) {
							found = true;
						}
					}
					break;
				}
			}
		}
	}
	if (!found) {
		lua_createtable(L, 0, 0);
	}
	return 1;
}

static int funcObjectIndex(lua_State* L)
{
	bool found=false;
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (s->argCount > 1) {
		if (!m->objectgroup.empty()) {
			for (int i=0;i<m->objectgroup.size();i++) {
				if (m->objectgroup[i]->name == s->args(0)) {
					int j;
					PPTMXObject* o = m->objectgroup[i]->findObject(s->args(1),&j);
					if (pushTMXObject(L,o)) {
						lua_pushinteger(L,j+1);
						found = true;
					}
					break;
				}
			}
		}
	}
	if (!found) {
		lua_createtable(L, 0, 0);
	}
	return 1;
}

static int funcShowAllLayer(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (!m->layer.empty()) {
		if (s->argCount > 0) {
			int l = (int)s->integer(0)-1;
			if (l >= 0 && l < m->layer.size()) {
				m->layer[l]->show();
			}
		} else {
			m->show();
		}
	}
	return 0;
}

static int funcHideAllLayer(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (!m->layer.empty()) {
		if (s->argCount > 0) {
			int l = (int)s->integer(0)-1;
			if (l >= 0 && l < m->layer.size()) {
				m->layer[l]->hide();
			}
		} else {
			m->hide();
		}
	}
	return 0;
}

static int funcMapArea(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
//	PPSize t = PPSizeZero;
	if (s->argCount > 0) {
		if (s->argCount > 0 && s->isTable(L,0)) {
			m->drawArea = s->getRect(L,0);
			return 0;
		} else
		if (s->argCount > 3) {
			m->drawArea = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
			return 0;
		}
		return luaL_argerror(L,2,"invalid value");
	}
	return s->returnRect(L,m->drawArea);
}

static int funcAddLayer(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (s->argCount > 0) {
		if (!m->layer.empty()) {
			for (int i=0;i<m->layer.size();i++) {
				if (m->layer[i]->name == s->args(0)) {
					PPSize size = s->getSize(L,1,1,1);
					if (size.width  <= 0) size.width  = 1;
					if (size.height <= 0) size.height = 1;
					m->layer[i]->resize(size.width,size.height);
					return 0;
				}
			}
		}
		PPTMXLayer* layer = new PPTMXLayer();
		layer->name = s->args(0);
		layer->mapSize.width = 1;
		layer->mapSize.height = 1;
		if (s->argCount > 1) {
			layer->mapSize = s->getSize(L,1,1,1);
		}
		layer->chip = (PPTMXChip*)calloc(1,layer->mapSize.width*layer->mapSize.height*sizeof(PPTMXChip));
		m->layer.push_back(layer);
		m->curLayer = (int)m->layer.size()-1;
	}
	return 0;
}

static int funcReset(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	m->reset();
	return 0;
}

void PPTMXMap::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
	registClass(s,name,this,superclass);
}

static int funcDelete(lua_State *L)
{
	delete (PPTMXMap*)(PPLuaScript::DeleteObject(L));
	return 0;
}

static int funcNew(lua_State *L)
{
	PPWorld* world = PPLuaScript::World(L);
//	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	if (s->argCount > 0) {
		lua_createtable(L,(int)s->integer(0),0);
		int table = lua_gettop(L);
		for (int i=0;i<s->integer(0);i++) {
			PPTMXMap* obj = new PPTMXMap(world);
			obj->start();
			obj->poly._texture = world->projector->textureManager->defaultTexture;
			PPLuaScript::newObject(L,PPTMXMap::className.c_str(),obj,funcDelete);
			lua_rawseti(L,table,i+1);
		}
	} else {
		PPTMXMap* obj = new PPTMXMap(world);
		obj->start();
		obj->poly._texture = world->projector->textureManager->defaultTexture;
		PPLuaScript::newObject(L,PPTMXMap::className.c_str(),obj,funcDelete);
	}
	return 1;
}

std::string PPTMXMap::className = "PPTMXMap";

PPObject* PPTMXMap::registClass(PPLuaScript* script,const char* name,const char* superclass)
{
	if (name) PPTMXMap::className = name;
	return PPTMXMap::registClass(script,PPTMXMap::className.c_str(),new PPTMXMap(script->world()),superclass);
}

static int funcInitAttribute(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	m->editAttr.reset();
	return 0;
}

static int funcAttribute(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		int t=-1;
		if (s->argCount > 0) {
			if (s->isTable(L,0)) {
				t = 0;
			} else
			if (s->argCount > 2) {
				if (s->isTable(L,2)) {
					t = 2;
				}
			}
			if (s->argCount > 1 && t != 0) {
				m->editPos.x = s->number(0);
				m->editPos.y = s->number(1);
			}
		}
		if (t>=0) {
			m->editPos.x = s->tableNumber(L,t,"x",m->editPos.x);
			m->editPos.y = s->tableNumber(L,t,"y",m->editPos.y);
			m->editAttr.tile = s->tableNumber(L,t,"tile",m->editAttr.tile);
			bool hflip = s->tableBoolean(L,t,"hflip",(m->editAttr.flag & PPFLIP_H)!=0);
			bool vflip = s->tableBoolean(L,t,"vflip",(m->editAttr.flag & PPFLIP_V)!=0);
			int rflip = s->tableNumber(L,t,"rotate90",(m->editAttr.flag & PPFLIP_RMASK)>>2);
			m->editAttr.flag = 0;
			if (hflip) m->editAttr.flag |= PPFLIP_H;
			if (vflip) m->editAttr.flag |= PPFLIP_V;
			m->editAttr.flag |= (rflip<<2);
			m->editAttr.color = s->tableColor(L,t,"color",m->editAttr.color);
			return 0;
		}
	}
	lua_createtable(L, 0, 2);
	lua_pushnumber(L,m->tget(m->editPos));
	lua_setfield(L, -2, "tile");
	PPTMXChip a = m->aget(m->editPos);
	lua_pushboolean(L,(a.flag & PPFLIP_H)!=0);
	lua_setfield(L, -2, "hflip");
	lua_pushboolean(L,(a.flag & PPFLIP_V)!=0);
	lua_setfield(L, -2, "vflip");
	lua_pushnumber(L,(a.flag & PPFLIP_RMASK)>>2);
	lua_setfield(L, -2, "rotate90");
	s->pushColor(L,a.color);
	lua_setfield(L, -2, "color");
	return 1;
}

static int funcLocate(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->editPos = s->getPoint(L,0,m->editPos.x,m->editPos.y);
	}
	return s->returnPoint(L,m->editPos);
}

static int funcPrint(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->print(s->args(0));
	}
	return 0;
}

static int funcLine(lua_State *L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		PPPoint pos1 = PPPointZero;
		PPPoint pos2 = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos1 = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos1 = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		if (s->argCount > n && s->isTable(L,n)) {
			pos2 = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos2 = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		int col = m->editAttr.tile;
		if (n < s->argCount) {
			col = (int)s->integer(n,m->editAttr.tile);
		}
		m->line(pos1,pos2,col);
	}
	return 0;
}

static int funcBox(lua_State *L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		PPRect rect = PPRect(0,0,0,0);
		if (n < s->argCount && s->isTable(L,n)) {
			rect = s->getRect(L,n);
		} else
		if (n+3 < s->argCount) {
			rect = s->getRect(L,n);
			n+=3;
		}
		n ++;
		int col = m->editAttr.tile;
		if (n < s->argCount) {
			col = (int)s->integer(n,m->editAttr.tile);
		}
		m->box(rect,col);
	}
	return 0;
}

static int funcFill(lua_State *L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		PPRect rect = PPRect(0,0,0,0);
		if (n < s->argCount && s->isTable(L,n)) {
			rect = s->getRect(L,n);
		} else
		if (n+3 < s->argCount) {
			rect = s->getRect(L,n);
			n+=3;
		}
		n ++;
		int col = m->editAttr.tile;
		if (n < s->argCount) {
			col = (int)s->integer(n,m->editAttr.tile);
		}
		m->fill(rect,col);
	}
	return 0;
}

static int funcClear(lua_State *L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		PPSize mapSize = m->layer[m->curLayer]->mapSize;
		int n=0;
		int col = 0;//m->editAttr.tile;
		if (n < s->argCount) {
			col = (int)s->integer(n,m->editAttr.tile);
		}
		m->editAttr.reset();
		m->fill(PPRect(0,0,mapSize.width,mapSize.height),col);
	}
	return 0;
}

static int funcPaint(lua_State *L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		PPPoint pos = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		int col = m->editAttr.tile;//PPColor::white();
		if (n < s->argCount) {
			col = (int)s->integer(n,m->editAttr.tile);
			n ++;
			if (n < s->argCount) {
				m->paint(pos,col,(int)s->integer(n,(int)m->editAttr.tile));
				return 0;
			}
		}
		m->paint(pos,col);
	}
	return 0;
}

static int funcCircle(lua_State *L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		float r,start=0,end=360;
		PPPoint pos = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		int col = m->editAttr.tile;//PPColor::white();
		if (n < s->argCount) {
			r = s->number(n);
			n++;
			if (n < s->argCount) {
				col = (int)s->integer(n);//s->getColor(L,n);
				n++;
				if (n < s->argCount) {
					start = s->number(n);
					n++;
					if (n < s->argCount) {
						end = s->number(n);
						n++;
					}
				}
			}
			m->circle(pos,r,col,start,end);
		}
	}
	return 0;
}

static int funcSwap(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		PPSize mapSize = m->layer[m->curLayer]->mapSize;
		int t1 = (int)s->integer(0);
		int t2 = (int)s->integer(1);
		for (int y=0;y<mapSize.height;y++) {
			for (int x=0;x<mapSize.width;x++) {
				if (t1 == m->tget(PPPoint(x,y))) {
					m->tset(t2,PPPoint(x,y));
				} else
				if (t2 == m->tget(PPPoint(x,y))) {
					m->tset(t1,PPPoint(x,y));
				}
			}
		}
	}
	return 0;
}

static int funcCopy(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		PPRect rect;
		if (s->argCount > n && s->isTable(L,n)) {
			rect = s->getRect(L,n);
			n+=1;
		} else
		if (s->argCount > n+3) {
			rect = s->getRect(L,n);
			n+=4;
		} else {
			return 0;
		}
		PPPoint pos = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos = s->getPoint(L,n);
			n+=1;
		} else
		if (s->argCount > n+1) {
			pos = s->getPoint(L,n);
			n+=1;
		} else {
			return 0;
		}
		m->copy(rect,pos);
	}
	return 0;
}

static int funcTileset(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	lua_createtable(L,(int)m->tileset.size(),0);
	int t = lua_gettop(L);
	if (!m->tileset.empty()) {
		for (int i=0;i<m->tileset.size();i++) {
			lua_createtable(L,0,3);
			lua_pushinteger(L,m->tileset[i]->firstgid);
			lua_setfield(L,-2,"firstgid");
			lua_pushstring(L,m->tileset[i]->name.c_str());
			lua_setfield(L,-2,"name");
			lua_pushinteger(L,m->tileset[i]->tilewidth);
			lua_setfield(L,-2,"tilewidth");
			lua_pushinteger(L,m->tileset[i]->tileheight);
			lua_setfield(L,-2,"tileheight");
			lua_rawseti(L,t,i+1);
		}
	}
	return 1;
}

static int funcScroll(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->curLayer >= 0) {
		int n=0;
		PPPoint pos = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos = s->getPoint(L,n);
			n += 1;
		}
		m->scroll(pos);
	}
	return 0;
}

static int funcAABB(lua_State* L)
{
	PPTMXMap* m = (PPTMXMap*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPTMXMap* m = (PPTMXMap*)s->userdata;
	QBGame* g = (QBGame*)m->world();//s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}

	lua_getfield(L, 1, "x");
	m->pos.x = lua_tonumber(L, -1);
	lua_getfield(L, 1, "y");
	m->pos.y = lua_tonumber(L, -1);

	PPPoint p = m->pos;
	
	lua_getfield(L,1,"autolayout");
	if (lua_toboolean(L,-1)) {
//		PPPoint op = m->pos;
		bool centerx;
		bool centery;
		PPRect layoutarea;
		lua_getfield(L,1,"centerx");
		centerx = lua_toboolean(L,-1);
		lua_getfield(L,1,"centery");
		centery = lua_toboolean(L,-1);
		lua_getfield(L,1,"layoutarea");
		layoutarea = s->getRect(L,lua_gettop(L)-2);
		if (layoutarea.width == 0 || layoutarea.height == 0) {
			layoutarea = g->viewPort();
		}
		int flag = 0;
		if (centerx) flag |= PP_CENTER_X;
		if (centery) flag |= PP_CENTER_Y;
		p = g->layout(m->size(),m->pos,flag,layoutarea);
	}	

	PPGamePoly poly = m->poly;
	PPRect size = m->drawArea;
	PPSize scale = m->tileSize()*m->poly.scale;
	size.x*=scale.width;
	size.y*=scale.height;
	size.width*=scale.width;
	size.height*=scale.height;

	poly.pos = PPPointZero;
	poly.scale.x = 1;
	poly.scale.y = 1;
	poly.origin = m->origin*m->poly.scale;//PPPoint(size.width/2,size.height/2);
	
	PPPoint delta=PPPointZero;
	PPPoint o[4];
	m->world()->projector->CalcPolyPoint(&poly,&size,&delta,o);

	float minx,miny,maxx,maxy;
	minx = o[0].x;
	maxx = o[0].x;
	miny = o[0].y;
	maxy = o[0].y;
	for (int i=1;i<4;i++) {
		if (minx > o[i].x) minx = o[i].x;
		if (miny > o[i].y) miny = o[i].y;
		if (maxx < o[i].x) maxx = o[i].x;
		if (maxy < o[i].y) maxy = o[i].y;
	}
	
	return s->returnRect(L,PPRect(minx+p.x,miny+p.y,maxx-minx,maxy-miny));
}

static int funcCheck(lua_State* L)
{
	int top=lua_gettop(L);
	bool r=false;
	if (top >= 3) {
		int tile = (int)lua_tointeger(L,2);
		if (tile > 0) {
			lua_getfield(L,1,"_ctileinfo");
			int ctile=lua_gettop(L);
			if (!lua_isnil(L,-1)) {
				
				if (lua_istable(L,3)) {
#ifdef __LUAJIT__
					int l= (int)lua_objlen(L,3);
#else
					lua_len(L,3);
					int l=(int)lua_tointeger(L,-1);
#endif
					for (int j=1;j<=l;j++) {
						lua_rawgeti(L,3,j);
						int i=(int)lua_tointeger(L,-1);
						lua_rawgeti(L,ctile,i);
						int n=(int)lua_tointeger(L,-1);
						lua_pop(L,2);
						if (n>0) {
							if (n==tile) {
								r=true;
								break;
							}
						}
					}
				} else {
					int s,e;
					s = (int)lua_tointeger(L,3);
					if (lua_isnil(L,4)) {
						e=s;
					} else {
						e=(int)lua_tointeger(L,4);
					}
					if (e>=s&&s>0&&e>0) {
						for (int i=s;i<=e;i++) {
							lua_rawgeti(L,ctile,i);
							int n=(int)lua_tointeger(L,-1);
							lua_pop(L,1);
							if (n>0) {
								if (n==tile) {
									r=true;
									break;
								}
							}
						}
					}
				}
				
			}
		}
	}
	lua_pushboolean(L,r);
	return 1;
}

PPObject* PPTMXMap::registClass(PPLuaScript* s,const char* name,PPObject* obj,const char* superclass)
{
//	PPObject::registClass(s,name,obj);
	s->openModule(name,obj,0,superclass);
		s->addCommand("new",funcNew);
//		s->addCommand("start",funcLoad);
		s->addCommand("load",funcLoad);
		s->addCommand("show",funcShowAllLayer);
		s->addCommand("hide",funcHideAllLayer);
		s->addCommand("aabb",funcAABB);

		s->addCommand("reset",funcReset);
		s->addCommand("mapSize",funcMapSize);
		s->addCommand("area",funcMapArea);
		s->addCommand("addLayer",funcAddLayer);

		s->addCommand("layerList",funcLayerList);
		s->addCommand("layer",funcSelectLayer);
//		s->addCommand("showLayer",funcShowLayer);
//		s->addCommand("hideLayer",funcHideLayer);

		s->addCommand("tile",funcGID);
		s->addCommand("locate",funcLocate);
		s->addCommand("print",funcPrint);
		s->addCommand("fill",funcFill);
		s->addCommand("box",funcBox);
		s->addCommand("circle",funcCircle);
		s->addCommand("line",funcLine);
		s->addCommand("clear",funcClear);
		s->addCommand("paint",funcPaint);
		s->addCommand("swap",funcSwap);
		s->addCommand("scroll",funcScroll);
		s->addCommand("copy",funcCopy);
		s->addCommand("tileset",funcTileset);
		s->addCommand("check",funcCheck);

		s->addCommand("objectGroupList",funcObjectGroupList);
		s->addCommand("objectList",funcObjectList);
		s->addCommand("objectInfo",funcObjectInfo);
		s->addCommand("objectIndex",funcObjectIndex);

		s->addCommand("attribute",funcAttribute);
//		s->addCommand("attr",funcAttribute);
		s->addCommand("resetAttribute",funcInitAttribute);
//		s->addCommand("initAttr",funcInitAttribute);
	s->closeModule();
	return obj;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
