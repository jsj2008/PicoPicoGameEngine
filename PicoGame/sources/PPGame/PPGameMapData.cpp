/*-----------------------------------------------------------------------------------------------
	名前	PPGameMapData.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameMapData.h"
#include "PPGameRunlength.h"
#include "PPObject.h"
#include "PPGameSprite.h"

PPGameMapData::PPGameMapData()
{
	map = NULL;
	event = NULL;
	pos = PPPointZero;
}

PPGameMapData::~PPGameMapData()
{
	if (map) free(map);
	if (event) free(event);
}

void PPGameMapData::init()
{
	map = NULL;
	event = NULL;
}

void PPGameMapData::freedata()
{
	if (map) free(map);
	map = NULL;
	if (event) free(event);
	event = NULL;
}

unsigned short PPGameMapData::pat(int x,int y)
{
	if (map == NULL) return 0;
	while (x < 0) x += width;
	while (y < 0) y += height;
	while (x >= width) x -= width;
	while (y >= height) y -= height;
	if (x >= 0 && x < width) {
		if (y >= 0 && y < height) {
			return map[x+y*width];
		}
	}
	return 0;
}

void PPGameMapData::setPat(int x,int y,unsigned short pat)
{
	if (map == NULL) return;
	while (x < 0) x += width;
	while (y < 0) y += height;
	while (x >= width) x -= width;
	while (y >= height) y -= height;
	if (x >= 0 && x < width) {
		if (y >= 0 && y < height) {
			map[x+y*width] = pat;
		}
	}
}

static int ss = 0;

bool PPGameMapData::decompress(const void* bytes)
{
	if (bytes == NULL) return false;
	unsigned short* s = (unsigned short*)bytes;
	width = s[0];
	height = s[1];
	long size = *((long*)&s[2]);
	unsigned short* b = &s[4];
	map = (unsigned short*)PPGameRunlength_Decompress(b,&size);
	if (map) {
		if (ss == 1) {
			int n=0;
			for (int y=0;y<height;y++) {
				for (int x=0;x<width;x++) {
//					printf("%d,",map[n]);
					n++;
				}
//				printf("\n");
			}
//			printf("\n");
		}
		ss ++;
		return true;
	}
	return false;
}

void PPGameMapData::draw(PPObject* obj,PPRect drawArea)
{
	PPSize s = obj->tileSize();
	PPGamePoly poly = obj->poly;;
//	poly.rotate = obj->__rotate;
//	poly.color = obj->__color;
//	poly.flags = obj->__flip;
	for (int y=drawArea.y;y<drawArea.height;y++) {
		for (int x=drawArea.x;x<drawArea.width;x++) {
			PPPoint p = pos;
			p.x += x*s.width;
			p.y += y*s.height;
//			poly.origin = obj->__rotateCenter+p;
			poly.sprite(p.x,p.y,pat(x,y),0);
			poly._texture = obj->poly._texture;
			poly.scale = obj->poly.scale;
			obj->world()->projector->DrawPoly(&poly);
		}
	}
}

void PPGameMapData::draw(PPObject* obj,PPWorld* target,PP_DRAW_SELECTOR selector) {
	PPSize s = obj->tileSize();
	s = s*target->scale();
	int mapx = pos.x;
	int mapy = pos.y;
	int mmx = mapx/s.width;
	int mmy = mapy/s.height;
	for (int y=-1;y<=target->viewPort().height/s.width;y++) {
		for (int x=-1;x<=target->viewPort().width/s.height;x++) {
			int mx = x+mmx;
			int my = y+mmy;
			if (pat(mx,my) != 0) {
				float px = x*s.width-(mapx % (int)s.width);
				float py = y*s.height-(mapy % (int)s.height);
				(target->*selector)(PPPoint(px,py),pat(mx,my),obj);
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
