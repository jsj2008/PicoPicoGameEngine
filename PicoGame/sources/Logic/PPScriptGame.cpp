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

#include "PPVertualKey.h"
#include "PPTMXMap.h"
#include "PPParticle.h"
#include "PPGameStick.h"
#include "PPOffscreenTexture.h"
#include "PPUIScrollView.h"
#include "PPUIText.h"

PPScriptGame::PPScriptGame() : script(NULL),disableIOFlag(false)
{
	vKey=NULL;
	textObject=NULL;
	spriteObject=NULL;
	tmxObject=NULL;
	particleObject=NULL;
	offscreenObject=NULL;
	stick=NULL;
	scrollViewObject=NULL;
  box2d=NULL;
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
  projector->textureManager->defaultTexture=-1;
	font("default");
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

void PPScriptGame::disableIO()
{
  if (script) {
    lua_State* L = script->L;
    lua_pushnil(L);
    lua_setglobal(L,"io");
  }
  disableIOFlag = true;
}

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
  
  if (disableIOFlag) {
    disableIO();
  }

  std::string datpath = PPGameDataPath("");
  std::string respath = PPGameResourcePath("");
  
  if (datpath == respath) {
    script->addSearchPath(datpath.c_str());
  } else {
    script->addSearchPath(datpath.c_str());
    script->addSearchPath(respath.c_str());
  }
  
	openGameLibrary(script,"ppgame");
	openViewLibrary(script,"ppgraph");
	openTouchLibrary(script,"pptouch");
	openFontLibrary(script,"ppfont");
	openKeyLibrary(script,"ppkey");
	openTextureLibrary(script,"pptex");
	openAudioEngineBGM(script,"ppbgm");
	openAudioEngineEffect(script,"ppse");
  openGameController(script,"ppgamecontroller");
	openTextToSpeech(script,"ppspeech");
  
  if (box2d) delete box2d;
	box2d = PPBox2D::openLibrary(script,"ppb2");
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
  
  PPGameStickElement::openLibrary(script,"ppjoystickElement");
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
	textObject = (PPUIText*)PPUIText::registClass(script,"pptext","ppscroll");
  
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
  
	script->execString((const char*)EMBEDDED_LUA_GAME_CODE);
  script->setupGeometryCommand();

//	script->addCommand("pprect",funcPPRect);
//	script->addCommand("pppoint",funcPPPoint);
//	script->addCommand("pphitcheck",funcPPHitCheck);
//	script->addCommand("ppdraw",funcPPDraw);
//	script->addCommand("pplength",funcPPLength);
//	script->addCommand("ppforeach",funcPPIterator);
//  lua_sethook(script->L,funcPPHook,LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET,0);

//	script->addCommand("ppdebug",funcPPDebug);
	
#ifdef __MACOSX_APP__
  script->addCommand("OpenWeb",funcOpenWeb);
#endif
  
	initScript();
  
	std::string path = luaScriptPath;
	if (path == "") {
		path = PPGameDataPath(PPGameMainLua());
    if (path == "") {
      path = PPGameMainLua();
    }
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
			return;
		}
		script->startProcess();
	}
	script->idle();
	script->drawDisplayList("ppgraph");
	
//	if (PPReadError()) {
//		PPGameSprite* p =projector;
//		p->viewPort(PPRect(0,0,p->st.windowSize.width,p->st.windowSize.height));
//		p->unlockoffscreen();
//		script->errorMessage = PPReadError();
//		printf("%s\n",script->errorMessage.c_str());
//		NEXT(PPScriptGame::stepError);
//	}
  
	script->resetTimeout();
}

void PPScriptGame::stepError()
{
	projector->st.init();
	poly.reset();
	Scale(PPPoint(1,1),0,0);
	Rotate(0,0,0);
	color(PPColor::white());
	rotate_center=PPPoint(0,0);
	stick->read();
	font(PPFONT_HALF);
	color(PPColor::white());
	lineWrap = true;
  float y=32;
	locate(PPPoint(0,y));
	print("%s",script->errorMessage.c_str());
  
  y=printlocate.y;
  y+=32;

	color(PPColor::yellow());
	locate(PPPoint(0,y));
	print("FILE  : %s",script->errorName.c_str());
  y+=16;
	locate(PPPoint(0,y));
  if (script->errorLine >= 0) {
    print("LINE  : %d",script->errorLine);
  } else {
    print("LINE  :");
  }
  y+=16;
	locate(PPPoint(0,y));
	print("REASON: %s",script->errorReason.c_str());
  y+=16;
	script->resetTimeout();
}

/*-----------------------------------------------------------------------------------------------
 このファイルはここまで
 -----------------------------------------------------------------------------------------------*/
