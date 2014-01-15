/*-----------------------------------------------------------------------------------------------
	名前	PPVertualKey.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPVertualKey.h"
#include "PPGameSprite.h"

std::string PPVertualKey::className;

//PPVertualKey* PPVertualKey::instance = NULL;

PPVertualKey::PPVertualKey(PPWorld* world) : PPObject(world)
{
//	instance = this;
	fixed = false;
#ifdef _OBJMEM_DEBUG_
  objname="PPVertualKey";
  printf("alloc %s\n",objname);
  fflush(stdout);
#endif
}

PPVertualKey::~PPVertualKey()
{
}

void PPVertualKey::start()
{
	vKeyTouch = false;
	vKeyTouch2 = false;
	vKeyTouch3 = false;
	fixed = false;
	doIdle = true;
//	touchState = 0;
	vKeyArea = PPRectZero;
	PPObject::start();
	NEXT(PPVertualKey::stepIdle);
}
	
void PPVertualKey::stepIdle()
{
//	if (touchCount() > 0) {
//		vKeyDelta = PPPointZero;
//		if (!fixed) {
//			startTouch = touchPosition(0);
//		}
//		NEXT(PPVertualKey::stepTouch);
//	}
	doIdle=true;
//	stepTouch();
}

int PPVertualKey::nearTouch(PPPoint pos,PPPoint* outPos)
{
	int r=-1;
	if (touchCount() > 0) {
		PPGameState st=world()->projector->st;
		float len=0;
		for (int i=0;i<touchCount();i++) {
			PPPoint p = touchPosition(i);
			p=p-st.offset;
			p=p/st.scale;
			if (i==0) {
				len = p.length(pos);
				*outPos=p;
				r=i;
			} else {
				float l=p.length(pos);
				if (l<len) {
					len=l;
					*outPos=p;
					r=i;
				}
			}
		}
	}
	return r;
}

void PPVertualKey::stepTouch()
{
	if (!doIdle) return;
	doIdle=false;
	PPPoint outPos;
//	PPGameState st=world()->projector->st;
//	if (touchCount() > 0 && !vKeyTouch2) {
//		if (!fixed) {
//			for (int i=0;i<touchCount();i++) {
//				PPPoint p = touchPosition(i);
//				p=p-st.offset;
//				p=p/st.scale;
//				if (vKeyArea.width > 0 && vKeyArea.height > 0) {
//					if (p.hitCheck(vKeyArea)) {
//						startTouch = p;
//					}
//				} else {
//					startTouch = p;
//				}
//			}
//		}
//	}
	int touchIndex=nearTouch(vKeyDelta+startTouch,&outPos);
	if (touchIndex >= 0) {
#if 1
		while (true) {
			//if (fixed)
			{
				if (vKeyTouch2) {
					if (preTouch.length(outPos)>96) {
//						vKeyTouch2=false;
						vKeyTouch = false;
						vKeyTouch2 = false;
						vKeyTouch3 = false;
						break;
					}
				}
			}

			bool enable = false;
			if (vKeyArea.width > 0 && vKeyArea.height > 0) {
				if (outPos.hitCheck(vKeyArea)) {
					enable = true;
				}
			} else {
				enable = true;
			}

			if (enable) {
				if (!vKeyTouch2) {
					if (enable) {
						vKeyTouch3=true;
					}
				}
			}

			vKeyTouch=false;
			if (vKeyTouch3) {
				if (outPos.length(startTouch)>8) {
					if (!fixed) {
						if (!vKeyTouch2) {
							if (enable) {
								startTouch = outPos;
							}
						}
					}
					vKeyTouch=true;
				}
			}
			
			vKeyTouch2 = true;
			
			if (vKeyTouch3) {
				vKeyDelta = outPos-startTouch;
			}
			break;
		}
		preTouch=outPos;
#else
//		int t=0;
//		float len=-1;
//		for (int i=0;i<touchCount();i++) {
//			PPPoint p = touchPosition(i);
//			bool enable = false;
//			if (vKeyArea.width > 0 && vKeyArea.height > 0) {
//				if (p.hitCheck(vKeyArea)) {
//					enable = true;
//				}
//			} else {
//				enable = true;
//			}
//			if (enable) {
//				if (len < 0) {
//					len = p.length(startTouch);
//				} else {
//					if (p.length(startTouch) < len) {
//						len = p.length(startTouch);
//						t = i;
//					}
//				}
//			}
//		}
//		if (len > 16) {
//			vKeyTouch = true;
//		} else {
//			vKeyTouch = false;
//		}
//		vKeyTouch2 = true;
////		vKeyTouch = true;
//		vKeyDelta = touchPosition(t)-startTouch;
#endif
	} else {
		vKeyTouch = false;
		vKeyTouch2 = false;
		vKeyTouch3 = false;
//		NEXT(PPVertualKey::stepIdle);
	}
}

int PPVertualKey::calcDir(int div)
{
	if (div==0) div=1;
	stepTouch();
	if (!vKeyTouch) return -1;
	float length = vKeyDelta.length();
	PPPoint d = vKeyDelta;
	if (length==0) return -1;
	float q = atan2(-d.y/length,d.x/length)*360/(2*M_PI);
	q += (360/div)/2;
	while (q < 0) q += 360;
	while (q >= 360) q -= 360;
	return ((int)q)/(360/div);
}

int PPVertualKey::calcDir(int div,PPRect area)
{
	PPRect a=vKeyArea;
	vKeyArea=area;
	int r=calcDir(div);
	vKeyArea=a;
	return r;
}

#pragma mark -

#include "PPLuaScript.h"

static int funcTouch(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaArg::UserData(L,PPVertualKey::className);
  PPUserDataAssert(m!=NULL);
	m->stepTouch();
	lua_pushboolean(L,m->vKeyTouch);
	return 1;
}

static int funcDelta(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaArg::UserData(L,PPVertualKey::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	m->stepTouch();
	return s->returnPoint(L,m->vKeyDelta);
}

static int funcDir(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaArg::UserData(L,PPVertualKey::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
#if 1
		if (s->argCount > 1) {
			PPRect area = s->getRect(L,1);
			lua_pushinteger(L,m->calcDir((int)s->integer(0),area));
		} else {
			lua_pushinteger(L,m->calcDir((int)s->integer(0)));
		}
#else
		if (s->argCount > 1) {
			PPRect area = s->getRect(L,1);
			int d = m->calcDir((int)s->integer(0));
			PPPoint t = m->vKeyDelta+m->startTouch;
			bool b = m->vKeyTouch2;
			if (!b) {
				if (m->touchCount() > 0) {
					if (m->vKeyDelta.length() > 0 && m->vKeyDelta.length() <= 16) {
						b = true;
					}
				}
			}
			if (m->touchState == 0 && b) {
				m->touchState = -1;
				if (t.hitCheck(area)) m->touchState = 1;
			}
			
			if (!b) {
				m->touchState = 0;
			}
			
			if (m->touchState == -1) d = -1;
			lua_pushinteger(L,d);
		} else {
			lua_pushinteger(L,m->calcDir((int)s->integer(0)));
		}
#endif
	} else {
		lua_pushinteger(L,m->calcDir(4));
	}
	return 1;
}

static int funcCenter(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaArg::UserData(L,PPVertualKey::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	m->stepTouch();
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->startTouch.x = s->tableNumber(L,0,1,"x");
			m->startTouch.y = s->tableNumber(L,0,2,"y");
		} else {
			if (s->argCount > 1) {
				m->startTouch.x = s->number(0);
				m->startTouch.y = s->number(1);
			} else {
				m->startTouch.x = s->number(0);
				m->startTouch.y = s->number(0);
			}
		}
	}
	return s->returnPoint(L,m->startTouch);
}

static int funcArea(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaArg::UserData(L,PPVertualKey::className);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	m->stepTouch();
	if (s->argCount > 0) {
		m->vKeyArea = s->getRect(L,0,m->vKeyArea);
	}
	return s->returnRect(L,m->vKeyArea);
}

static int funcFixed(lua_State* L)
{
	PPVertualKey* m = (PPVertualKey*)PPLuaArg::UserData(L,PPVertualKey::className);
  PPUserDataAssert(m!=NULL);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	m->stepTouch();
	if (s->argCount > 0) {
		m->fixed = s->boolean(0,m->fixed);
	}
	lua_pushboolean(L,m->fixed);
	return 1;
}

void PPVertualKey::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
  className = name;
	s->openModule(name,this,0,superclass);
		s->addCommand("center",funcCenter);
		s->addCommand("fixed",funcFixed);
		s->addCommand("touch",funcTouch);
		s->addCommand("delta",funcDelta);
		s->addCommand("area",funcArea);
		s->addCommand("dir",funcDir);
	s->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
