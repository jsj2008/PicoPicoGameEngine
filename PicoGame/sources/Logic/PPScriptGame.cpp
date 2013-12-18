/*-----------------------------------------------------------------------------------------------
 名前	PPScriptGame.c
 説明
 作成	2012.07.22 by H.Yamaguchi
 更新
 -----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 インクルードファイル
 -----------------------------------------------------------------------------------------------*/

#include "PPScriptGame.h"
#include "PPTMXMap.h"
#include "QBSound.h"
#include "PPBox2D.h"
#include "PPSensor.h"
#include "PPScriptGameEmbbed.h"
#include "PPFont.h"

PPScriptGame::PPScriptGame() : script(NULL)
{
	vKey=NULL;
	textObject=NULL;
	spriteObject=NULL;
	tmxObject=NULL;
	particleObject=NULL;
	offscreenObject=NULL;
	stick=NULL;
	scrollViewObject=NULL;
	NEXT(PPScriptGame::stepInit);
}

PPScriptGame::~PPScriptGame()
{
	if (script) delete script;
	if (vKey) delete vKey;
	if (stick) delete stick;
  
  if (spriteObject) delete spriteObject;
  if (tmxObject) delete tmxObject;
  if (particleObject) delete particleObject;
  if (offscreenObject) delete offscreenObject;
  if (scrollViewObject) delete scrollViewObject;
  if (textObject) delete textObject;
}

void PPScriptGame::stepInit()
{
	if (script) {
		NEXT(PPScriptGame::stepError);
		return;
	}
	systemTextureID = projector->textureManager->countTexture();
	
	reloadData();
}

void PPScriptGame::initGraph()
{
	projector->textureManager->deleteTextureForDevelop(systemTextureID);
	projector->textureManager->fontManager->deleteFont(3);
	projector->st.init();
	poly.reset();
	Scale(PPPoint(1,1),0,0);
	Rotate(0);
	color(PPColor::white());
	lineWrap = false;
	rotate_center=PPPoint(0,0);
	curFont = NULL;
	projector->textureManager->reloadAllTexture();
	font("default");
}

static int funcPPRect(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  
	PPRect r;
	if (s->argCount > 0 && s->isTable(L,-1)) {
		r = s->getRect(L,-1);
	} else
    if (s->argCount > 1) {
      r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
    }
  
	return s->returnRect(L,r);
}

static int funcPPPoint(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->initarg(L);
  
	PPPoint p;
	if (s->argCount > 0 && s->isTable(L,-1)) {
		p = s->getPoint(L,-1);
	} else
    if (s->argCount > 1) {
      p = PPPoint(s->number(0),s->number(1));
    }
  
	return s->returnPoint(L,p);
}

typedef struct _hitdata {
	int index;
	int mask;
	unsigned long type;
	PPPoint pos;
	PPPoint center;
	PPRect rect;
	lua_Number length;
} hitdata;

static lua_Number getNumber(lua_State* L,int stack,int index,const char* field)
{
	lua_getfield(L,stack,field);
	if (lua_isnil(L,-1)) {
		lua_rawgeti(L,stack,index);
		if (lua_isnil(L,-1)) {
			return 0;
		}
	}
	return lua_tonumber(L,-1);
}

static hitdata* hit2[2]={0};
static int hitsize[2]={0};

//static int funcPPHitCheck2(lua_State *L)
//{
//	int top=lua_gettop(L);
//	if (lua_isfunction(L,3)) {
//		if (top >= 2) {
//			int n[2];
//#ifdef __LUAJIT__
//			n[0] = (int)lua_objlen(L,1);
//#else
//			lua_len(L,1);
//			n[0] = (int)lua_tointeger(L,-1);
//#endif
//#ifdef __LUAJIT__
//			n[1] = (int)lua_objlen(L,2);
//#else
//			lua_len(L,2);
//			n[1] = (int)lua_tointeger(L,-1);
//#endif
//			lua_settop(L,top);
//			
//			if (n[0] > 0 && n[1] > 0) {
//        for (int i=0;i<2;i++) {
//          if (hitsize[i]<n[i]) {
//            if (hit2[i]) {
//              free(hit2[i]);
//            }
//            hitsize[i]=n[i]+1024;
//            hit2[i] = (hitdata*)malloc(hitsize[i]*sizeof(hitdata));
//          }
//        }
//        
//				for (int i=0;i<2;i++) {
//					for (int j=0;j<n[i];j++) {
//						int top=lua_gettop(L);
//						hit2[i][j].index = j+1;
//						hit2[i][j].mask = 0;
//						hit2[i][j].length = 0;
//						hit2[i][j].type = 0;
//						hit2[i][j].center = PPPoint(0,0);
//						lua_rawgeti(L,1+i,j+1);
//						int table=lua_gettop(L);
//						{
//							lua_getfield(L,table,"hitmask");
//							if (!lua_isnil(L,-1)) {
//								hit2[i][j].mask = (int)lua_tointeger(L,-1);
//							}
//							lua_pop(L,1);
//						}
//						if (hit2[i][j].mask != 0) {
//							lua_getfield(L,table,"hitlength");
//							if (!lua_isnil(L,-1)) {
//								hit2[i][j].length = lua_tonumber(L,-1);
//							}
//							lua_pop(L,1);
//						}
//						if (hit2[i][j].mask != 0) {
//							lua_getfield(L,table,"hitcenter");
//							if (lua_istable(L,-1)) {
//								int center=lua_gettop(L);
//								hit2[i][j].center.x = getNumber(L,center,1,"x");
//								hit2[i][j].center.y = getNumber(L,center,2,"y");
//							}
//							{
//								lua_getfield(L,table,"x");
//								hit2[i][j].pos.x = lua_tonumber(L,-1);
//								lua_getfield(L,table,"y");
//								hit2[i][j].pos.y = lua_tonumber(L,-1);
//							}
//							lua_settop(L,table);
//						}
//						if (hit2[i][j].mask != 0) {
//							lua_getfield(L,table,"hitpos");
//							if (lua_istable(L,-1)) {
//								int center=lua_gettop(L);
//								hit2[i][j].pos.x = getNumber(L,center,1,"x");
//								hit2[i][j].pos.y = getNumber(L,center,2,"y");
//								lua_settop(L,table);
//							}
//						}
//						if (hit2[i][j].mask != 0) {
//							lua_getfield(L,table,"hitrect");
//							if (lua_istable(L,-1)) {
//								int s=lua_gettop(L);
//								PPRect r;
//								r.x = getNumber(L,s,1,"x");
//								r.y = getNumber(L,s,2,"y");
//								r.width = getNumber(L,s,3,"width");
//								r.height = getNumber(L,s,4,"height");
//								hit2[i][j].rect = PPRect(hit2[i][j].pos.x+r.x,hit2[i][j].pos.y+r.y,r.width,r.height);
//								hit2[i][j].pos.x += r.x+r.width/2;
//								hit2[i][j].pos.y += r.y+r.height/2;
//								hit2[i][j].length=sqrt(r.width*r.width/4+r.height*r.height/4);
//								hit2[i][j].type = 1;
//							}
//						}
//						lua_settop(L,top);
//					}
//				}
//				
//				for (int i=0;i<n[0];i++) {
//					hitdata* a = &hit2[0][i];
//					if (a->mask != 0 && a->length > 0) {
//						for (int j=0;j<n[1];j++) {
//							hitdata* b = &hit2[1][j];
//							if (b->mask != 0 && b->length > 0) {
//								if (a->mask & b->mask) {
//									bool hitcheck = false;
//									if (b->type && a->type) {
//										if (b->rect.hitCheck(a->rect)) {
//											hitcheck = true;
//										}
//									} else
//                    if ((b->pos+b->center).length(a->pos+a->center) < a->length+b->length) {
//                      hitcheck = true;
//                    }
//									if (hitcheck) {
//										lua_pushvalue(L,3);
//										lua_rawgeti(L,1,a->index);
//										lua_rawgeti(L,2,b->index);
//										lua_call(L,2,0);
//										lua_settop(L,top);
//									}
//								}
//							}
//						}
//					}
//				}
//				
//			}
//		}
//	}
//	return 0;
//}

static int funcPPHitCheck(lua_State *L)
{
	int top=lua_gettop(L);
	if (lua_isfunction(L,3)) {
		if (top >= 2) {
			int n[2];
			hitdata* hit[2];
#ifdef __LUAJIT__
			n[0] = (int)lua_objlen(L,1);
#else
			lua_len(L,1);
			n[0] = (int)lua_tointeger(L,-1);
#endif
#ifdef __LUAJIT__
			n[1] = (int)lua_objlen(L,2);
#else
			lua_len(L,2);
			n[1] = (int)lua_tointeger(L,-1);
#endif
			lua_settop(L,top);
			
			if (n[0] > 0 && n[1] > 0) {
				hit[0] = (hitdata*)malloc(n[0]*sizeof(hitdata));
				hit[1] = (hitdata*)malloc(n[1]*sizeof(hitdata));
        
				for (int i=0;i<2;i++) {
					for (int j=0;j<n[i];j++) {
						int top=lua_gettop(L);
						hit[i][j].index = j+1;
						hit[i][j].mask = 0;
						hit[i][j].length = 0;
						hit[i][j].type = 0;
						hit[i][j].center = PPPoint(0,0);
						lua_rawgeti(L,1+i,j+1);
						int table=lua_gettop(L);
						{
							lua_getfield(L,table,"hitmask");
							if (!lua_isnil(L,-1)) {
								hit[i][j].mask = (int)lua_tointeger(L,-1);
							}
							lua_pop(L,1);
						}
						if (hit[i][j].mask != 0) {
							lua_getfield(L,table,"hitlength");
							if (!lua_isnil(L,-1)) {
								hit[i][j].length = lua_tonumber(L,-1);
							}
							lua_pop(L,1);
						}
						if (hit[i][j].mask != 0) {
							lua_getfield(L,table,"hitcenter");
							if (lua_istable(L,-1)) {
								int center=lua_gettop(L);
								hit[i][j].center.x = getNumber(L,center,1,"x");
								hit[i][j].center.y = getNumber(L,center,2,"y");
							}
							{
								lua_getfield(L,table,"x");
								hit[i][j].pos.x = lua_tonumber(L,-1);
								lua_getfield(L,table,"y");
								hit[i][j].pos.y = lua_tonumber(L,-1);
							}
							lua_settop(L,table);
						}
						if (hit[i][j].mask != 0) {
							lua_getfield(L,table,"hitpos");
							if (lua_istable(L,-1)) {
								int center=lua_gettop(L);
								hit[i][j].pos.x = getNumber(L,center,1,"x");
								hit[i][j].pos.y = getNumber(L,center,2,"y");
								lua_settop(L,table);
							}
						}
						if (hit[i][j].mask != 0) {
							lua_getfield(L,table,"hitrect");
							if (lua_istable(L,-1)) {
								int s=lua_gettop(L);
								PPRect r;
								r.x = getNumber(L,s,1,"x");
								r.y = getNumber(L,s,2,"y");
								r.width = getNumber(L,s,3,"width");
								r.height = getNumber(L,s,4,"height");
								hit[i][j].rect = PPRect(hit[i][j].pos.x+r.x,hit[i][j].pos.y+r.y,r.width,r.height);
								hit[i][j].pos.x += r.x+r.width/2;
								hit[i][j].pos.y += r.y+r.height/2;
								hit[i][j].length=sqrt(r.width*r.width/4+r.height*r.height/4);
								hit[i][j].type = 1;
							}
						}
						lua_settop(L,top);
					}
				}
				
				for (int i=0;i<n[0];i++) {
					hitdata* a = &hit[0][i];
					if (a->mask != 0 && a->length > 0) {
						for (int j=0;j<n[1];j++) {
							hitdata* b = &hit[1][j];
							if (b->mask != 0 && b->length > 0) {
								if (a->mask & b->mask) {
									bool hitcheck = false;
									if (b->type && a->type) {
										if (b->rect.hitCheck(a->rect)) {
											hitcheck = true;
										}
									} else
                    if ((b->pos+b->center).length(a->pos+a->center) < a->length+b->length) {
                      hitcheck = true;
                    }
									if (hitcheck) {
										lua_pushvalue(L,3);
										lua_rawgeti(L,1,a->index);
										lua_rawgeti(L,2,b->index);
										lua_call(L,2,0);
										lua_settop(L,top);
									}
								}
							}
						}
					}
				}
				
				free(hit[0]);
				free(hit[1]);
        
			}
		}
	}
	return 0;
}

static int funcPPIterator(lua_State *L)
{
	int top=lua_gettop(L);
	if (top>=2) {
		if (lua_istable(L,1)) {
#ifdef __LUAJIT__
			int n=(int)lua_objlen(L,1);
#else
			lua_len(L,1);
			int n = (int)lua_tointeger(L,-1);
#endif
			lua_settop(L,top);
			if (lua_isfunction(L,2)) {
				for (int i=0;i<n;i++) {
					lua_pushvalue(L,2);
					lua_pushinteger(L,i+1);
					lua_rawgeti(L,1,i+1);
					lua_call(L,2,1);
					if (!lua_isnil(L,-1)) {
						return 1;
					}
					lua_settop(L,top);
				}
			}
		}
	}
	lua_pushnil(L);
	return 1;
}

static int funcPPLength(lua_State *L)
{
	int n=0;
	lua_Number len=0;
	lua_Number x1,y1;
	{
		if (lua_istable(L,n+1)) {
			lua_getfield(L,n+1,"x");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+1,1);
			}
			x1 = lua_tonumber(L,-1);
      
			lua_getfield(L,n+1,"y");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+1,2);
			}
			y1 = lua_tonumber(L,-1);
		} else {
			x1 = lua_tonumber(L,n+1);
			y1 = lua_tonumber(L,n+2);
			n=1;
		}
	}
  
	if (lua_isnil(L,n+2)) {
		len = sqrt(x1*x1+y1*y1);
	} else {
		lua_Number x2,y2;
		x2=y2=0;
		if (lua_istable(L,n+2)) {
			lua_getfield(L,n+2,"x");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+2,1);
			}
			x2 = lua_tonumber(L,-1);
      
			lua_getfield(L,n+2,"y");
			if (lua_isnil(L,-1)) {
				lua_rawgeti(L,n+2,2);
			}
			y2 = lua_tonumber(L,-1);
		} else {
			x2 = lua_tonumber(L,n+2);
			y2 = lua_tonumber(L,n+3);
		}
		len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
	}
  
	lua_pushnumber(L,len);
	return 1;
}

#ifdef __MACOSX_APP__
static int funcOpenWeb(lua_State *L)
{
  const char* url = NULL;
  const char* title = NULL;
	if (lua_type(L,1) == LUA_TSTRING) {
		url = lua_tostring(L,1);
	}
	if (lua_type(L,2) == LUA_TSTRING) {
		title = lua_tostring(L,2);
	}
  PPGameOpenWeb(url,title);
	return 0;
}
#endif

void PPScriptGame::initScript()
{
}

void PPScriptGame::reloadData()
{
	PPReadErrorReset();
	
	initGraph();
  
	if (script) {
		delete script;
	}
  
	script = new PPLuaScript(this);
  
	script->addSearchPath(PPGameDataPath(""));
	script->addSearchPath(PPGameResourcePath(""));
  
	openGameLibrary(script,"ppgame");
	openViewLibrary(script,"ppgraph");
	openTouchLibrary(script,"pptouch");
	openFontLibrary(script,"ppfont");
	openKeyLibrary(script,"ppkey");
	openTextureLibrary(script,"pptex");
	openAudioEngineBGM(script,"ppbgm");
	openAudioEngineEffect(script,"ppse");
  //	openGameController(script,"ppgamecontroller");
	openTextToSpeech(script,"ppspeech");
  
	PPBox2D::openLibrary(script,"ppb2");
	PPSensor::openAccelerometerLibrary(script,"ppaccelerometer");
  
	if (QBSound::sharedSound()) {
		QBSound::sharedSound()->openSEMMLLibrary(script,"ppsemml");
		QBSound::sharedSound()->openFlMMLLibrary(script,"ppflmml");
	}
  
	openAudioEngineSEMML(script,"ppsewave");
  
	if (vKey) delete vKey;
	vKey = new PPVertualKey(this);
	vKey->openLibrary(script,"ppvkey");
	vKey->start();
  //  vKey->objname = "vKey";
  
	if (stick) delete stick;
	stick = new PPGameStick(this);
	stick->start();
	stick->openLibrary(script,"ppjoystick");
  //  stick->objname = "stick";
  
  if (spriteObject) delete spriteObject;
	spriteObject = PPObject::registClass(script,"ppobject");
  
  if (tmxObject) delete tmxObject;
	tmxObject = (PPTMXMap*)PPTMXMap::registClass(script,"ppmap","ppobject");
  
  if (particleObject) delete particleObject;
	particleObject = (PPParticleEmitter*)PPParticleEmitter::registClass(script,"ppparticle","ppobject");
  
  if (offscreenObject) delete offscreenObject;
	offscreenObject = (PPOffscreenTexture*)PPOffscreenTexture::registClass(script,"ppoffscreen","ppobject");
  
  if (scrollViewObject) delete scrollViewObject;
	scrollViewObject = (PPUIScrollView*)PPUIScrollView::registClass(script,"ppscroll","ppobject");
  
  if (textObject) delete textObject;
	textObject = (PPUIText*)PPUIText::registClass(script,"pptext","ppobject");
  
#ifdef _OBJMEM_DEBUG_
  spriteObject->objname = "spriteObject";
  tmxObject->objname = "tmxObject";
  particleObject->objname = "particleObject";
  offscreenObject->objname = "offscreenObject";
  scrollViewObject->objname = "scrollViewObject";
  textObject->objname = "textObject";
#endif
  
	projector->openLibrary(script,"ppscreen");
	projector->animationFrameInterval=1;
  
	script->execString(EMBEDDED_LUA_GAME_CODE);
	script->addCommand("pprect",funcPPRect);
	script->addCommand("pppoint",funcPPPoint);
#ifdef __LUAJIT__
	script->addCommand("pphitcheck",funcPPHitCheck2);
#else
	script->addCommand("pphitcheck",funcPPHitCheck);
#endif
  //	script->addCommand("ppdraw",funcPPDraw);
	script->addCommand("pplength",funcPPLength);
	script->addCommand("ppforeach",funcPPIterator);
	
#ifdef __MACOSX_APP__
  script->addCommand("OpenWeb",funcOpenWeb);
#endif
  
	initScript();
  
	std::string path = luaScriptPath;
	if (path == "") {
		path = PPGameDataPath(PPGameMainLua());
	}
  
	if (!script->load(path.c_str())) {
		PPGameSprite* p =projector;
		p->viewPort(PPRect(0,0,p->st.windowSize.width,p->st.windowSize.height));
		p->unlockoffscreen();
		NEXT(PPScriptGame::stepError);
	} else {
		lua_State* L = script->L;
		int top = lua_gettop(L);
		lua_getglobal(L,"___scriptIdleFunc");
		if (lua_isnil(L,-1)) {
			lua_getglobal(L,"start");
			if (lua_isfunction(L,-1)) {
				lua_setglobal(L,"___scriptIdleFunc");
			}
		}
		lua_settop(L,top);
		NEXT(PPScriptGame::stepIdle);
	}
}

void PPScriptGame::stepIdle()
{
	if (stick) stick->read();
	
  if (script==NULL) return;
	locate(PPPointZero);
	vKey->idle();
	if (!script->alive) {
		if (script->errorMessage != "") {
			NEXT(PPScriptGame::stepError);
#ifdef _LINUX
  	  printf("%s\n",script->errorMessage.c_str());
#endif
			return;
		}
		script->startProcess();
	}
	script->idle();
	script->drawDisplayList("ppgraph");
	
	if (PPReadError()) {
		PPGameSprite* p =projector;
		p->viewPort(PPRect(0,0,p->st.windowSize.width,p->st.windowSize.height));
		p->unlockoffscreen();
		script->errorMessage = PPReadError();
		printf("%s\n",script->errorMessage.c_str());
		NEXT(PPScriptGame::stepError);
	}
  
	script->resetTimeout();
}

void PPScriptGame::stepError()
{
	projector->st.scale = PPPoint(1,1);
	projector->st.offset = PPPointZero;
	projector->st.offset2 = PPPointZero;
	poly.reset();
	Scale(PPPoint(1,1),0,0);
	Rotate(0);
	color(PPColor::white());
	rotate_center=PPPoint(0,0);
	stick->read();
	font(PPFONT_HALF);
	color(PPColor::white());
	lineWrap = true;
	locate(PPPoint(0,32));
	print("%s\n",script->errorMessage.c_str());
	script->resetTimeout();
}

/*-----------------------------------------------------------------------------------------------
 このファイルはここまで
 -----------------------------------------------------------------------------------------------*/
