/*-----------------------------------------------------------------------------------------------
	名前	PPGameGeometry.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <math.h>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPPoint;
class PPSize;
class PPColor;

class PPFColor {
public:
	PPFColor() {
		r = 1.0;
		g = 1.0;
		b = 1.0;
		a = 1.0;
	}
	PPFColor(float _r,float _g,float _b,float _a) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	float r,g,b,a;
	
	PPColor byteColor();
	
	static PPFColor white() {
		return PPFColor(1.0,1.0,1.0,1.0);
	}
	
	static PPFColor black() {
		return PPFColor(0,0,0,1.0);
	}
	
	static PPFColor yellow() {
		return PPFColor(1.0,1.0,0,1.0);
	}
	
	static PPFColor red() {
		return PPFColor(1.0,0,0,1.0);
	}
	
	static PPFColor green() {
		return PPFColor(0,1.0,0,1.0);
	}
	
	static PPFColor blue() {
		return PPFColor(0,0,1.0,1.0);
	}
	
	PPFColor operator * (float value) {
		return PPFColor(r*value,g*value,b*value,a*value);
	}

	PPFColor operator + (PPFColor c) {
		return PPFColor(r+c.r,g+c.g,b+c.b,a+c.a);
	}

	void operator += (PPFColor c) {
		r = r+c.r;
		g = g+c.g;
		b = b+c.b;
		a = a+c.a;
	}
	
	bool operator != (PPFColor& o) {
		if (r == o.r && g == o.g && b == o.b && a == o.a) return false;
		return true;
	};

	bool operator == (PPFColor& o) {
		if (r == o.r && g == o.g && b == o.b && a == o.a) return true;
		return false;
	};
	
	void limit() {
		if (r <   0) r =   0;
		if (r > 1.0) r = 1.0;
		if (g <   0) g =   0;
		if (g > 1.0) g = 1.0;
		if (b <   0) b =   0;
		if (b > 1.0) b = 1.0;
		if (a <   0) a =   0;
		if (a > 1.0) a = 1.0;
	}
};

class PPColor {
public:
	PPColor() {
		r = 0xff;
		g = 0xff;
		b = 0xff;
		a = 0xff;
	}
	PPColor(unsigned char _r,unsigned char _g,unsigned char _b,unsigned char _a) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	PPColor(PPFColor c) {
		r = c.r*255;
		g = c.g*255;
		b = c.b*255;
		a = c.a*255;
	}

	unsigned char r,g,b,a;
	
	static PPColor white() {
		return PPColor(255,255,255,255);
	}
	
	static PPColor black() {
		return PPColor(0,0,0,255);
	}
	
	static PPColor yellow() {
		return PPColor(255,255,0,255);
	}
	
	static PPColor red() {
		return PPColor(255,0,0,255);
	}
	
	static PPColor green() {
		return PPColor(0,255,0,255);
	}
	
	static PPColor blue() {
		return PPColor(0,0,255,255);
	}
	
	PPColor operator * (float value) {
		return PPColor(r*value,g*value,b*value,a*value);
	}

	PPColor operator + (PPColor c) {
		return PPColor(r+c.r,g+c.g,b+c.b,a+c.a);
	}

	void operator += (PPColor c) {
		r = r+c.r;
		g = g+c.g;
		b = b+c.b;
		a = a+c.a;
	}
	
	bool operator != (PPColor& o) {
		if (r == o.r && g == o.g && b == o.b && a == o.a) return false;
		return true;
	};

	bool operator == (PPColor& o) {
		if (r == o.r && g == o.g && b == o.b && a == o.a) return true;
		return false;
	};
};

class PPRect {
public:
	PPRect() {
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}
	
	PPRect(PPPoint pos,PPSize size);

	PPRect(float _x,float _y,float _w,float _h) {
		x = _x;
		y = _y;
		width = _w;
		height = _h;
	}

	PPRect operator * (int a) {
		PPRect n(x*a,y*a,width*a,height*a);
		return n;
	};

	PPRect operator * (float a) {
		PPRect n(x*a,y*a,width*a,height*a);
		return n;
	};

	void operator *= (int a) {
		PPRect n(x*a,y*a,width*a,height*a);
		*this = n;
	};

	void operator *= (float a) {
		PPRect n(x*a,y*a,width*a,height*a);
		*this = n;
	}

	PPRect operator / (int a) {
		PPRect n(x/a,y/a,width/a,height/a);
		return n;
	};

	PPRect operator / (float a) {
		PPRect n(x/a,y/a,width/a,height/a);
		return n;
	};

	void operator /= (int a) {
		PPRect n(x/a,y/a,width/a,height/a);
		*this = n;
	};

	void operator /= (float a) {
		PPRect n(x/a,y/a,width/a,height/a);
		*this = n;
	}

	void operator += (PPPoint a);
	PPRect operator + (PPPoint a);
	void operator -= (PPPoint a);
	PPRect operator - (PPPoint a);
	void operator = (PPPoint a);
	void operator = (PPSize a);

	float x,y,width,height;
	
	bool hitCheck(PPRect rect) {
		float dw = (x+width)-rect.x;
		float dh = (y+height)-rect.y;
		if (dw >= 0 && dw < rect.width+width && dh >= 0 && dh < rect.height+height) return true;
		return false;
	}
  
  PPPoint min();
  PPPoint max();
	
	PPSize size();
	PPPoint pos();
	void setSize(PPSize size);
	void setPos(PPPoint pos);

	PPRect move(PPPoint pos);	
	PPRect moveTo(PPPoint pos);
};

extern PPRect PPRectZero;
extern PPPoint PPPointZero;
extern PPSize PPSizeZero;

class PPVertex {
public:
	PPVertex() {
		x = 0;
		y = 0;
		z = 0;
	}
	
	float x,y,z;
};

class PPPoint {
public:
	PPPoint() {
		x = 0;
		y = 0;
	}

	PPPoint(float _x,float _y) {
		x = _x;
		y = _y;
	}

	float x;
	float y;
	
	PPPoint operator + (PPPoint& o) {
		PPPoint n(x+o.x,y+o.y);
		return n;
	};

	PPPoint operator - (PPPoint& o) {
		PPPoint n(x-o.x,y-o.y);
		return n;
	};
	
	void operator -= (PPPoint& o) {
		PPPoint n(x-o.x,y-o.y);
		x = n.x;
		y = n.y;
	};
	
	void operator += (PPPoint& o) {
		PPPoint n(x+o.x,y+o.y);
		x = n.x;
		y = n.y;
	};
	
	PPPoint operator * (float a) {
		PPPoint n(x*a,y*a);
		return n;
	};

	PPPoint operator * (int a) {
		PPPoint n(x*a,y*a);
		return n;
	};

	PPPoint operator * (PPSize a);
	PPPoint operator * (PPPoint a);
	PPPoint operator / (PPPoint a);

	void operator *= (float a) {
		PPPoint n(x*a,y*a);
		x = n.x;
		y = n.y;
	}

	void operator *= (int a) {
		PPPoint n(x*a,y*a);
		x = n.x;
		y = n.y;
	};

	PPPoint operator / (float a) {
		PPPoint n(x/a,y/a);
		return n;
	};

	PPPoint operator / (int a) {
		PPPoint n(x/a,y/a);
		return n;
	};

	void operator /= (float a) {
		PPPoint n(x/a,y/a);
		x = n.x;
		y = n.y;
	}

	void operator /= (int a) {
		PPPoint n(x/a,y/a);
		x = n.x;
		y = n.y;
	};

	bool operator == (PPPoint& o) {
		if (x == o.x && y == o.y) return true;
		return false;
	}
	
	bool operator != (PPPoint& o) {
		if (x != o.x || y != o.y) return true;
		return false;
	}
	
//	bool InRect(float _x,float _y,float _w,float _h) {
//		if (_x <= x && x < _x+_w
//		 && _y <= y && y < _y+_h) {
//			return true;
//		}
//		return false;
//	};

	bool inRect(float _x,float _y,float _w,float _h) {
		if (_x <= x && x < _x+_w
		 && _y <= y && y < _y+_h) {
			return true;
		}
		return false;
//		return InRect(_x,_y,_w,_h);
	}

	bool hitCheck(PPRect r) {
		if (r.x <= x && x < r.x+r.width
		 && r.y <= y && y < r.y+r.height) {
			return true;
		}
		return false;
	};

	PPPoint normal() {
		float len = length();
		PPPoint p = *this;
		return p/len;
	};
	
	float length(PPPoint pos=PPPointZero) {
		return sqrt((pos.x-x)*(pos.x-x)+(pos.y-y)*(pos.y-y));
	}
};

PPRect PPRectInset(PPRect rect,float x,float y);

class PPSize {
public:
	PPSize() {
		width = 0;
		height = 0;
	}
	PPSize(float w,float h) {
		width = w;
		height = h;
	}

	PPSize operator * (int a) {
		PPSize n(width*a,height*a);
		return n;
	};

	PPSize operator * (float a) {
		PPSize n(width*a,height*a);
		return n;
	};

	PPSize operator * (PPSize a) {
		PPSize n(width*a.width,height*a.height);
		return n;
	};

	PPSize operator * (PPPoint a) {
		PPSize n(width*a.x,height*a.y);
		return n;
	};

	void operator *= (int a) {
		PPSize n(width*a,height*a);
		*this = n;
	};

	void operator *= (float a) {
		PPSize n(width*a,height*a);
		*this = n;
	};

	PPSize operator / (int a) {
		PPSize n(width/a,height/a);
		return n;
	};

	PPSize operator / (float a) {
		PPSize n(width/a,height/a);
		return n;
	};

	PPSize operator / (PPSize a) {
		PPSize n(width/a.width,height/a.height);
		return n;
	};

	PPSize operator / (PPPoint a) {
		PPSize n(width/a.x,height/a.y);
		return n;
	};

	void operator /= (int a) {
		PPSize n(width/a,height/a);
		*this = n;
	};

	void operator /= (float a) {
		PPSize n(width/a,height/a);
		*this = n;
	};

	bool operator == (PPSize& o) {
		if (width == o.width && height == o.height) return true;
		return false;
	}
	
	bool operator != (PPSize& o) {
		if (width != o.width || height != o.height) return true;
		return false;
	}

	float width;
	float height;
};

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
