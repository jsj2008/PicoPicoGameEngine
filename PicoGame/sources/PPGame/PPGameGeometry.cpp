/*-----------------------------------------------------------------------------------------------
	名前	PPGameGeometry.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include "PPGamePoly.h"

PPRect PPRectZero = PPRect(0,0,0,0);
PPPoint PPPointZero = PPPoint(0,0);
PPSize PPSizeZero = PPSize(0,0);

static bool hitLine(PPPoint* l1,PPPoint* l2,PPPoint* nml1,PPPoint* cross);

PPPoint PPPoint::operator * (PPSize a)
{
	PPPoint n(x*a.width,y*a.height);
	return n;
}

PPPoint PPPoint::operator * (PPPoint a)
{
	PPPoint n(x*a.x,y*a.y);
	return n;
}

PPPoint PPPoint::operator / (PPPoint a)
{
	PPPoint n(x/a.x,y/a.y);
	return n;
}

PPRect::PPRect(PPPoint pos,PPSize size) {
	x = pos.x;
	y = pos.y;
	width = size.width;
	height = size.height;
}

PPRect PPRect::move(PPPoint pos) {
	PPRect rect = *this;
	rect.x += pos.x;
	rect.y += pos.y;
	return rect;
}

PPRect PPRect::moveTo(PPPoint pos) {
	PPRect rect = *this;
	rect.x = x;
	rect.y = y;
	return rect;
}

PPSize PPRect::size()
{
	return PPSize(width,height);
}

PPPoint PPRect::pos()
{
	return PPPoint(x,y);
}

void PPRect::setSize(PPSize size)
{
	width = size.width;
	height = size.height;
}

void PPRect::setPos(PPPoint pos)
{
	x = pos.x;
	y = pos.y;
}

void PPRect::operator += (PPPoint a)
{
	x += a.x;
	y += a.y;
}

PPRect PPRect::operator + (PPPoint a)
{
	PPRect n(x+a.x,y+a.y,width,height);
	return n;
}

void PPRect::operator = (PPPoint a)
{
	x = a.x;
	y = a.y;
}

void PPRect::operator = (PPSize a)
{
	width = a.width;
	height = a.height;
}

#pragma mark -

int PPRegion::init(PPPoint* point,int num,float x,float y)
{
	int p=0;
	pointNum = num;
	o.x = x;
	o.y = y;
	n.x = x;
	n.y = y;
	points = &point[p];
	p += pointNum+1;
	normals = &point[p];
	p += pointNum;
	return p;
}

void PPRegion::update(PPPoint* org,int num)
{
	if (num > 0) {
		this->pointNum = num;
	}
	
	PPPoint* pos = this->points;
	PPPoint* nml = this->normals;
	{
		int i;
		for (i=0;i<this->pointNum;i++) {
			pos[i] = org[i]+o;
		}
	}
	for (int i=0;i<this->pointNum-1;i++) {
		nml[i].x = -(pos[i+1].y-pos[i].y);
		nml[i].y =   pos[i+1].x-pos[i].x;
		nml[i] = nml[i].normal();
	}
	calcBoundingBox();
}

//法線とバウンディングBOXを計算
void PPRegion::update()
{
	PPPoint* pos = this->points;
	PPPoint* nml = this->normals;
	if (orgpoints) {
		int i;
		for (i=0;i<this->pointNum;i++) {
			pos[i] = orgpoints[i]+o;
		}
	}
	for (int i=0;i<this->pointNum-1;i++) {
		nml[i].x = -(pos[i+1].y-pos[i].y);
		nml[i].y =   pos[i+1].x-pos[i].x;
		nml[i] = nml[i].normal();
	}
	calcBoundingBox();
}

void PPRegion::calcBoundingBox()
{
	PPPoint* pos = this->points;
	min.x = 10000;
	min.y = 10000;
	max.x = -10000;
	max.y = -10000;
	for (int i=0;i<this->pointNum;i++) {
		if (pos[i].x < min.x) min.x = pos[i].x;
		if (pos[i].y < min.y) min.y = pos[i].y;
		if (pos[i].x > max.x) max.x = pos[i].x;
		if (pos[i].y > max.y) max.y = pos[i].y;
	}
}

float PPRegion::calcLength(PPPoint* point)
{
	float len;
	PPPoint* p1 = this->points;
	PPPoint* n1 = this->normals;
	for (int i=0;i<this->pointNum-1;i++) {
		float l = (point->x-p1[i].x)*n1[i].x+(point->y-p1[i].y)*n1[i].y;
		if (i == 0) {
			len = l;
		} else {
			if (len > l) len = l;
		}
	}
	return len;
}

int PPRegion::hitLine(PPRegion* region)
{
	//hitCheckTarget = false;
	
	if (region->min.x > max.x) return -1;
	if (region->min.y > max.y) return -1;
	if (region->max.x < min.x) return -1;
	if (region->max.y < min.y) return -1;

	PPPoint* p1 = this->points;
	PPPoint* p2 = region->points;
	PPPoint* n1 = this->normals;

	for (int i=0;i<this->pointNum-1;i++) {
		for (int j=0;j<region->pointNum-1;j++) {
			if (::hitLine(&p1[i],&p2[j],&n1[i],&cross)) {
				region->hitCheckTarget = true;
				return j;
			}
		}
	}
	return -1;
}

int PPRegion::hitPoints(PPRegion* region,PPPoint* outPos,int maxPos)
{
	return 0;
}

bool PPRegion::hitCheck(PPRegion* region)
{
	if (region->min.x > max.x) return false;
	if (region->min.y > max.y) return false;
	if (region->max.x < min.x) return false;
	if (region->max.y < min.y) return false;
	PPPoint* p1 = this->points;
	PPPoint* p2 = region->points;
	PPPoint* n1 = this->normals;
	for (int j=0;j<region->pointNum-1;j++) {
		for (int i=0;i<this->pointNum-1;i++) {
			PPPoint d;
			d.y = n1[i].y;//p1[i+1].x-p1[i].x;
			d.x = n1[i].x;//-(p1[i+1].y-p1[i].y);
			int n = d.x*(p2[j].x-p1[i].x)+d.y*(p2[j].y-p1[i].y);
			if (n < 0) {
				goto Next;
			}
		}
		goto Exit;
Next:;
	}
	return false;
Exit:
//	if (hit1) *hit1 = -1;
//	if (hit2) *hit2 = -1;
	return true;
}

bool PPRegion::hitCheck(PPPoint* point)
{
	if (min.x > point->x) return false;
	if (min.y > point->y) return false;
	if (max.x < point->x) return false;
	if (max.y < point->y) return false;
	PPPoint* p1 = this->points;
	PPPoint* n1 = this->normals;
	PPPoint* p2 = point;
	int j=0;
	for (int i=0;i<this->pointNum-1;i++) {
		PPPoint d;
		d.y = n1[i].y;//p1[i+1].x-p1[i].x;
		d.x = n1[i].x;//-(p1[i+1].y-p1[i].y);
		int n = d.x*(p2[j].x-p1[i].x)+d.y*(p2[j].y-p1[i].y);
		if (n < 0) {
			return false;
		}
	}
	return true;
}

#define SH 8

static bool hitLine(PPPoint* l1,PPPoint* l2,PPPoint* nml,PPPoint* cross)
{
	//float版
	bool irf = false;
	float itf = 0;
	while (1) {
		
		float d0,d1,d2,t;
		d0 = l1[1].x*nml->x+l1[1].y*nml->y;
		d1 = l2[0].x*nml->x+l2[0].y*nml->y;
		d2 = l2[1].x*nml->x+l2[1].y*nml->y;

		if (d1 == d2) {
			break;
		}
		if ((d0 > d1 && d0 > d2) || (d0 < d1 && d0 < d2)) {
			break;
		}
		itf = t = (d0 - d2) / (d1 - d2);

		if (t > 0 && t <= 1.0) {
#if 1
			float x = l2[1].x+t*(l2[0].x-l2[1].x);
			float y = l2[1].y+t*(l2[0].y-l2[1].y);

			if (cross) {
				cross->x = x;
				cross->y = y;
			}

			float dx = 0;
			if (l1[1].x-l1[0].x != 0) {
				dx = (x-l1[0].x)/(l1[1].x-l1[0].x);
			} else {
				//dx = t;
			}
			float dy = 0;
			if (l1[1].y-l1[0].y != 0) {
				dy = (y-l1[0].y)/(l1[1].y-l1[0].y);
			} else {
				//dy = t;
			}

			if (dx >= 0 && dx < 1.0 && dy >= 0 && dy < 1.0) {
				irf = true;
				break;
			}
#endif
		}
		break;
	}

	return irf;
}

int PPRegion::poly(PPGamePoly* poly,PPPoint* offset,unsigned char r,unsigned char g,unsigned char b)
{
	int n=0;
#if 0
	int i;
	PPPoint off = PPZeroPoint;
	if (offset) off = *offset;
	PPPoint* pos = points;
	for (i=0;i<(pointNum-3);i++) {
		poly[n].r = r;
		poly[n].g = g;
		poly[n].b = b;
		poly[n].alpha = 255;
		poly[n++].poly(pos[0].x-off.x,pos[0].y-off.y,pos[i+1].x-off.x,pos[i+1].y-off.y,pos[i+2].x-off.x,pos[i+2].y-off.y);
		
#if 0
		int xp[4],yp[4];
		
		xp[0] = pos[0].x;
		yp[0] = pos[0].y;

		xp[1] = pos[i+1].x;
		yp[1] = pos[i+1].y;

		xp[2] = pos[i+2].x;
		yp[2] = pos[i+2].y;

		xp[3] = pos[0].x;
		yp[3] = pos[0].y;

		for (int j=0;j<3;j++) {
			float snx =  (yp[j+1]-yp[j]);
			float sny = -(xp[j+1]-xp[j]);
			
			poly[n].r = 0;
			poly[n].g = 0;
			poly[n].b = 255;
			poly[n].alpha = 255;
			poly[n++].line(xp[j],yp[j],xp[j]+snx,yp[j]+sny);
		}
#endif
	}
#endif
	return n;
}

int PPRegion::line(PPGamePoly* poly,PPPoint* offset,unsigned char r,unsigned char g,unsigned char b)
{
	int i;
	int n=0;
	PPPoint off = PPPointZero;
	if (offset) off = *offset;
	PPPoint* pos = points;
	for (i=0;i<pointNum-1;i++) {
		poly[n].color.r = r;
		poly[n].color.g = g;
		poly[n].color.b = b;
		poly[n].color.a = 255;
		poly[n++].line(pos[i].x-off.x,pos[i].y-off.y,pos[i+1].x-off.x,pos[i+1].y-off.y);
	}
	return n;
}

PPRect PPRectInset(PPRect rect,float x,float y)
{
	rect.x += x;
	rect.y += y;
	rect.width -= x*2;
	rect.height -= y*2;
	return rect;
}

PPColor PPFColor::byteColor() {
	return PPColor(r*255,g*255,b*255,a*255);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
