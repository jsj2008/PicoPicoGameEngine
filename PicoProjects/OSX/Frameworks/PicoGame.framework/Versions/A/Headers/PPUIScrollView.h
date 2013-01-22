/*-----------------------------------------------------------------------------------------------
	名前	PPUIScrollView.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef PicoPicoGames_PPUIScrollView_h
#define PicoPicoGames_PPUIScrollView_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
#include "PPGameSprite.h"
#include <math.h>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPUIScrollView : public PPObject {
public:
	enum {
		SPEED_QUE = 3
	};

	PPUIScrollView(PPWorld* world);
	virtual ~PPUIScrollView();

	virtual void init();
	virtual void start();
	virtual void stepIdle();

	virtual void setFrameSize(PPSize r);
	virtual PPSize frameSize();
	virtual void setContentsSize(PPSize s);
	virtual PPSize contentsSize();
	
	virtual PPSize size() {
		return _frameSize;
	}
	
	virtual bool tapCheck() {
		for (int i=0;i<world()->touchCount();i++) {
			PPRect b = bounds();
			PPPoint p = convertToScreen();
			PPPoint s = world()->projector->st.scale;
			b.x = p.x;
			b.y = p.y;
			PPPoint tp = world()->touchPosition(i);
			tp -= world()->projector->st.offset;
			tp.x /= s.x;
			tp.y /= s.y;
			if (tp.hitCheck(b)) {
				if (pretouchCount == 0) {
					touchPos = world()->touchPosition(i);
					return true;
				}
			}
		}
		return false;
	}

	virtual void touchDown(PPPoint pos) {
		tpos = pos;
		touchDownFlag = true;
		if (speed.length() < 0.1) {
			longtouch = 1;
//printf("touchDown 1\n");
		} else {
			longtouch = 0;
//printf("touchDown 0\n");
		}
	}

	virtual void touchDrag(PPPoint pos) {
		if (_contentsRect.width <= size().width
		 && _contentsRect.height <= size().height) {
			longtouch = 1;
		} else {
			longtouch = 0;
		}
	}

	virtual void touchUp(PPPoint pos) {
		touchDownFlag = false;
		if (longtouch  > 0) {
			longtouch = 5;
			__directTouchUp(pos);
		}
	}

	virtual void __directTouchUp(PPPoint pos) {
		PPObject* c = child();
		while (c) {
			c->__directTouchUp(pos);
			c = c->sibling();
		}
	}
	
	virtual void resetScroll() {
		touchStep=0;
		touchDownFlag=false;
		if (dpos.x > 0) dpos.x = 0;
		if (dpos.y > 0) dpos.y = 0;
		if (dpos.x+_contentsRect.width < size().width) {
			dpos.x = size().width-_contentsRect.width;
		}
		if (dpos.y+_contentsRect.height < size().height) {
			dpos.y = size().height-_contentsRect.height;
		}
		speed = PPPointZero;
	}

	virtual bool touchIdle(bool touch=true) {
		if (manualScroll) {
			if (touch) {
				setTouch(false);
				switch (touchStep) {
				case 0:
					touchStep1();
					break;
				case 1:
					touchStep2();
					break;
				}
			}
	//		PPObject::touchIdle(touch);//PPViewPort::touchIdle(touch);
	//		{
			for (int i=0;i<SPEED_QUE-1;i++) {
				speedque[i] = speedque[i+1];
			}
			if (touchDownFlag && touchCount() > 0) {
				speedque[SPEED_QUE-1] = touchPos-tpos;
				speed = PPPointZero;
				for (int i=0;i<SPEED_QUE;i++) {
					speed += speedque[i];
				}
				speed /= SPEED_QUE;
				tpos = touchPos;
			} else {
				speed *= 0.99f;
				speedque[SPEED_QUE-1] = speed;
				if (fabsf(speed.x) < 0.1) speed.x = 0;
				if (fabsf(speed.y) < 0.1) speed.y = 0;
			}
			dpos += speed;
			if (touchDownFlag) {
			} else {
				PPPoint gpos = dpos;
				bool flag = false;
				{
					if (dpos.y > 0) {
						speed.y *= 0.8f;
						gpos.y = 0;
						flag = true;
					}
					if (dpos.y+_contentsRect.height < size().height) {
						speed.y *= 0.8f;
						gpos.y = size().height-_contentsRect.height;
						flag = true;
					}
				}
				{
					if (dpos.x > 0) {
						speed.x *= 0.8f;
						gpos.x = 0;
						flag = true;
					}
					if (dpos.x+_contentsRect.width  < size().width ) {
						speed.x *= 0.8f;
						gpos.x = size().width -_contentsRect.width;
						flag = true;
					}
				}
				PPPoint d = (gpos-dpos)*0.2f;
				dpos += d;
			}
			if (_contentsRect.width <= size().width) {
				dpos.x = 0;
			}
			if (_contentsRect.height <= size().height) {
				dpos.y = 0;
			}
		}
		cpos = dpos;
		{
			if (cpos.x > 0) cpos.x /= 2;
			if (cpos.y > 0) cpos.y /= 2;
			float w = size().width -_contentsRect.width ;
			float h = size().height-_contentsRect.height;
			if (cpos.x < w) cpos.x = w-(w-cpos.x)/2;
			if (cpos.y < h) cpos.y = h-(h-cpos.y)/2;
		}
		cpos.x = floorf(cpos.x);
		cpos.y = floorf(cpos.y);
		_contentsRect = cpos+pos;
//		}
//		if (r) {
//			r = false;
//			if (longtouch > 0) {
//				longtouch++;
//				if (longtouch > 5) {
//					longtouch = 5;
//					r = true;
//				}
//			}
//		}
		pretouchCount = world()->touchCount();
		return false;
	}

	PPPoint cpos;
	PPPoint dpos;
	PPPoint tpos;
	PPPoint speedque[SPEED_QUE];
	PPPoint speed;
	
	unsigned long counter;
	unsigned long time;
	unsigned long longtouch;
	int pretouchCount;
	
	bool touchDownFlag;
	bool manualScroll;

	PPSize _frameSize;
	PPRect _contentsRect;

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

	static std::string className;
	static PPObject* registClass(PPLuaScript* script,const char* name=NULL,const char* superclass=NULL);
	static PPObject* registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass=NULL);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
