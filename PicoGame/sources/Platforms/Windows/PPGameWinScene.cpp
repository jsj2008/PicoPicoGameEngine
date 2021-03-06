/*-----------------------------------------------------------------------------------------------
	名前	PPGameWinScene.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameWinScene.h"
#include "PPGameStart.h"

#define PPGAME_MAX_POLY (10000*10)

int PPGameWinScene::spriteLimit()
{
	return PPGAME_MAX_POLY;
}

QBGame* PPGameWinScene::newGame()
{
	return PPGameStart();
}

PPGameWinScene::PPGameWinScene()
{
	game = NULL;
	g = NULL;
}

PPGameWinScene::~PPGameWinScene()
{
	if (game) delete game;
	if (g) delete g;
	game = NULL;
	g = NULL;
}

bool PPGameWinScene::init()
{
	if (game == NULL) game = newGame();
	touchScreen = false;
  
  if (game) {
    game->disableIO();    //ファイルの読み書きを制限
  }
	
	return true;
}

PPGameTextureInfo* PPGameWinScene::textures()
{
	return game->TextureInfo();
}

void PPGameWinScene::textureIdle(PPGameSprite* g)
{
	if (textures()) {
		if (!g->initLoadTex) {
			if (g->textureManager->setTextureList(textures()) == 0) {
				g->initLoadTex = true;
			}
		}
	}
}


//void PPGameWinScene::closeGame()
//{
//}

void PPGameWinScene::draw()
{
	GLfloat pm[16];
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glGetFloatv(GL_PROJECTION_MATRIX,pm);
	glEnable(GL_TEXTURE_2D);
	if (g) {
		g->setWorld(game);
		g->ClearScreen2D(0.0f, 0.0f, 0.0f);
		game->DrawGL();
		textureIdle(g);
		g->SetViewPort();
		g->InitOT();
		gameIdle();
//		for (int i=0;i<polyCount;i++) {
//			g->DrawPoly(&poly[i]);
//		}
		g->DrawOT();
		g->ResetOT();
//		textureIdle(g);
		QBGame::blinkCounter++;
	} else {
		g = new PPGameSprite();
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(pm);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void PPGameWinScene::gameIdle()
{
	//game->Clear();
	if (g) {
		game->setTouchCount(0);
		if (touchScreen) {
			game->setTouchPosition(touchLocation_x,touchLocation_y);
//			touchMenu(&game->menu[game->curMenu]);
		}
		g->InitBuffer(spriteLimit());
		g->st.screenWidth = winSize().width;
		g->st.screenHeight = winSize().height;
		g->st.framebuffer = 0;
		g->st.renderbuffer = 0;
//		game->arrowKeyTouchNo = arrowKeyTouchNo;
		game->screenWidth = winSize().width;
		game->screenHeight = winSize().height;
//		unsigned long key=getKey();
//		key = hardkey;
		game->gameIdle(hardkey);
//		if (!game->HideButton()) {
//			if (game->KeyTextureID() >= 0) drawKey(game->KeyTextureID());
//		}
//		if (game->exitGame) {
//			game->exitGame = false;
//			closeGame();
//		}
	}
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
