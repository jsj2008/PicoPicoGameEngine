/*-----------------------------------------------------------------------------------------------
	名前	main.c
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <windows.h>
#include <WinNls.h> 
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <GL/glew.h>
#include <Shlwapi.h>
#include <mmsystem.h>
#include "QBGame.h"
#include "QBSoundWin.h"
#include "PPGameWinScene.h"
#include "PPGameUtil.h"
#include "JpString.h"
#include <CocosDenshion/SimpleAudioEngine.h>

#define VERSION_STRING "PicoPicoGameEngine Simulator Ver. 1.1\n"

static int screenWidth = 640;
static int screenHeight = 480;
static int gFullscreenFlag = 0;

static PPGameWinScene* scene=NULL;
static HGLRC hRC=NULL;
static bool reload = false;
static bool resizing = false;
static bool resetCommand = false;
static char* dropFileName=NULL;
// static int timerStep=0;
// static DWORD timeCount=0;

#define APP_NAME "PicoPicoGames"
#define APP_TITLE "Window Test Application"
#define TIMER_ID   1
#define TIMER_RATE 1

int WINAPI gWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

BOOL bSetupPixelFormat(HDC hdc)
{ 
	PIXELFORMATDESCRIPTOR pfd, *ppfd; 
	int pixelformat; 
	
	ppfd = &pfd; 
	
	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	ppfd->nVersion = 1; 
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  PFD_DOUBLEBUFFER; 
	ppfd->dwLayerMask = PFD_MAIN_PLANE; 
	ppfd->iPixelType = PFD_TYPE_RGBA;	//PFD_TYPE_COLORINDEX;
	ppfd->cColorBits = 8; 
	ppfd->cDepthBits = 16; 
	ppfd->cAccumBits = 0; 
	ppfd->cStencilBits = 0; 
	
	pixelformat = ChoosePixelFormat(hdc, ppfd); 
	
	if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
	{ 
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
		return FALSE; 
	} 
	
	if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
	{ 
		MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
		return FALSE; 
	} 
	
	return TRUE; 
}

void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	BOOL reloadData=false;
	HDC hDC;
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hRC);

//	GLuint viewFramebuffer;
//	glGenFramebuffersEXT(1, &viewFramebuffer);
//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, viewFramebuffer);

	// if (timerStep==0) {
	// 	timeCount = dwTime;
	// 	timerStep++;
	// } else if (timerStep==1) {
	// 	if (dwTime-timeCount!=0) {
	// 		timerStep++;
	// 	}
	// } else {

		if (scene) {

			if (reload || resizing) {
				RECT r;
				GetClientRect(hWnd,&r);
				screenWidth = r.right;
				screenHeight = r.bottom;
				scene->windowSize.width = screenWidth;
				scene->windowSize.height = screenHeight;
				resizing = false;
				if (reload) {
	//				PPGameTexture::ReloadAllTexture();
					reload=false;
				}
			}

			glViewport(0,0,screenWidth,screenHeight);
		
			unsigned long key = 0;

			if (GetForegroundWindow() == hWnd) {
				if ((GetKeyState(VK_LEFT) & 0x8000)) {
					key |= PAD_LEFT;
				}
				if ((GetKeyState(VK_RIGHT) & 0x8000)) {
					key |= PAD_RIGHT;
				}
				if ((GetKeyState(VK_UP) & 0x8000)) {
					key |= PAD_UP;
				}
				if ((GetKeyState(VK_DOWN) & 0x8000)) {
					key |= PAD_DOWN;
				}
				if ((GetKeyState(VK_ESCAPE) & 0x8000)) {
					//ExitGame();
					PostQuitMessage(0);
				}
				if ((GetKeyState(VK_SPACE) & 0x8000)) {
					key |= PAD_A;
				}
				if ((GetKeyState(VK_RETURN) & 0x8000)) {
	//				key |= JOY_PAD_RETURN;
					key |= PAD_B;
				}
				if ((GetKeyState(VK_SHIFT) & 0x8000)) {
	//				key |= JOY_PAD_SHIFT;
					key |= PAD_B;
				}
				if ((GetKeyState(VK_CONTROL) & 0x8000)) {
	//				key |= JOY_PAD_CONTROL;
					key |= PAD_B;
				}
				if ((GetKeyState('X') & 0x8000)) {
					key |= PAD_B;
				}
				if ((GetKeyState('Z') & 0x8000)) {
					key |= PAD_A;
				}
				if ((GetKeyState('A') & 0x8000)) {
					key |= PAD_A;
				}
				if ((GetKeyState('B') & 0x8000)) {
					key |= PAD_B;
				}
				if ((GetKeyState('C') & 0x8000)) {
					key |= PAD_C;
				}
				if ((GetKeyState('D') & 0x8000)) {
					key |= PAD_D;
				}
				if ((GetKeyState('E') & 0x8000)) {
					key |= PAD_E;
				}
				if ((GetKeyState('F') & 0x8000)) {
					key |= PAD_F;
				}
				if ((GetKeyState('G') & 0x8000)) {
					key |= PAD_G;
				}
				if ((GetKeyState('H') & 0x8000)) {
					key |= PAD_H;
				}
				if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState('R') & 0x8000)) {
					if (!resetCommand) {
						if (scene->game) {
							//scene->game->reloadData();
							reloadData=true;
						}
					}
					resetCommand = true;
				} else {
					resetCommand = false;
				}
			}
			
      if (dropFileName) {
        if (dropFileName[0] != 0) {
          if (strncasecmp(PathFindExtension(dropFileName),".lua",4) == 0) {
printf("%s\n",dropFileName);
            PPGameSetMainLua(PathFindFileName(dropFileName));
            PathRemoveFileSpec(dropFileName);
            PathAddBackslash(dropFileName);
            PPGameSetDataPath(dropFileName);
            //scene->game->reloadData();
            reloadData=true;
          }
          free(dropFileName);
          dropFileName=NULL;
        }
      }

			POINT cpos;
			GetCursorPos(&cpos);
			ScreenToClient(hWnd,&cpos);
			if (scene->game) {
				scene->game->mouseLocation.x = cpos.x;
				scene->game->mouseLocation.y = cpos.y;
			}
			scene->hardkey = key;
			scene->draw();
		}

	// }

	SwapBuffers(hDC);
	ReleaseDC(hWnd,hDC);

	if (reloadData) {
		if (scene) {
      CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
      QBSoundWin* snd = (QBSoundWin*)QBSound::sharedSound();
      if (snd) {
        snd->stopAll();
      }
			if (scene->game) {
				scene->game->reloadData();
			}
		}
	}
}

void glSetInterval(int isinterval)
{
	//拡張関数を受け取る関数ポインタ
	BOOL (WINAPI *wglSwapIntervalEXT)(int) = NULL;
	
	//拡張関数のWGL_EXT_swap_controlをサポートしているか調べる
	if( strstr( (char*)glGetString( GL_EXTENSIONS ), "WGL_EXT_swap_control") == 0)
	{
		//WGL_EXT_swap_controlをサポートしていない
		return;
	}
	else
	{
		//APIを取得して実行
		wglSwapIntervalEXT = (BOOL (WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT");
		if( wglSwapIntervalEXT )
		{
			wglSwapIntervalEXT(isinterval);
		}
	}
}

void OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
#pragma unused (lpcs)

	{
		HDC hDC;    /* device context handles */
		
		/* Get the handle of the windows device context. */
		hDC = GetDC(hWnd);
		
		bSetupPixelFormat(hDC);
		
		/* Create a rendering context and make it the current context */
		hRC = wglCreateContext(hDC);
		if (!hRC)
		{
			MessageBox(NULL, "Cannot create context.", "Error", MB_OK);
			return;
		}
		wglMakeCurrent(hDC, hRC);
	}
	
	glewInit();

	glSetInterval(1);
	
	// タイマーの作成
	SetTimer(hWnd, TIMER_ID, 1000/70, TimerProc);
}

void OnDestroy(HWND hWnd)
{
	// タイマーの削除
	KillTimer(hWnd, TIMER_ID);
	
	{
		HGLRC hRC;       /* rendering context handle */
		HDC   hDC;       /* device context handle */
		
		/* Release and free the device context and rendering context. */
		hDC = wglGetCurrentDC();
		hRC = wglGetCurrentContext();
		
		wglMakeCurrent(NULL, NULL);
		
		if (hRC)
			wglDeleteContext(hRC);
		
		if (hDC)
			ReleaseDC(hWnd, hDC);
		
	}
	
	//	if (_gameSystem) {
	//		_gameSystem->ExitGame();
	//	}
	
	/*	
	 stopWave = true;
	 resetWave();
	 for (int i=0;i<NumWaveBuff;++i) {
	 unprepareWave(i);
	 }
	 closeWave();
	 */
	
	PostQuitMessage(0);
}

LRESULT CALLBACK WndMainProc(HWND hWnd, UINT Msg,
							 WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_SYSCOMMAND:
			switch (wParam & 0xfff0) {
				case SC_MONITORPOWER:
				case SC_SCREENSAVE :
					return 0;		//スクリーンセーバー対策//
			}
			return (DefWindowProc(hWnd, Msg, wParam, lParam));
		case WM_CREATE:
			OnCreate(hWnd, (LPCREATESTRUCT)lParam);
			break;
		case WM_SIZE:
			if (wParam < 3) 
			{
				screenWidth  = lParam & 0xFFFF;        // xサイズ
				screenHeight = (lParam >> 16) & 0xFFFF;
				if (scene) {
					scene->windowSize.width = screenWidth;
					scene->windowSize.height = screenHeight;
				}
			}
			break;
		case WM_EXITSIZEMOVE:
			reload = true;
			break;

		case WM_SIZING:
			resizing = true;
			break;
			
		case WM_DROPFILES:
			{
				HDROP hDrop = (HDROP)wParam;
				UINT uFileNo = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
				for(int i=0;i<(int)uFileNo;i++) {
					UINT namelen = DragQueryFile(hDrop,i,NULL,0);
          if (dropFileName) free(dropFileName);
          dropFileName = (char*)malloc(namelen+1);
          if (dropFileName) {
            DragQueryFile(hDrop,i,dropFileName,namelen+1);

            HANDLE hFind;
            WIN32_FIND_DATA FindFileData;

            hFind = ::FindFirstFile(dropFileName,&FindFileData);
            if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
              const char* n="\\main.lua";
              char* s=(char*)malloc(namelen+1+strlen(n)+1);
              strcpy(s,dropFileName);
              strcat(s,n);
              free(dropFileName);
              dropFileName = s;
            }

            std::string fn = dropFileName;
            std::string ext = fn.substr(fn.find_last_of(".")+1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if(ext == "lua") {
              break;
            }
          }
				}
				DragFinish(hDrop);
			}
			break;
			
		case WM_DESTROY:
			OnDestroy(hWnd);
			break;
			
		case WM_WINDOWPOSCHANGED:
			break;
			
		case WM_ACTIVATE:
			break;
			
		case WM_CHAR:
			break;
			
		case MM_WOM_OPEN:
			break;
		case MM_WOM_DONE:
			break;
		case MM_WOM_CLOSE:
			break;
		case MM_MOM_DONE:
			//		H_MovieIdleWin(wParam,lParam);
			return 0;
		case WM_LBUTTONDOWN:
			if (scene) {
				scene->touchLocation_x = LOWORD(lParam);
				scene->touchLocation_y = HIWORD(lParam);
				scene->touchScreen = true;
			}
			break;
		case WM_MOUSEMOVE:
			if(wParam & MK_LBUTTON) {
				if (scene) {
					scene->touchLocation_x = LOWORD(lParam);
					scene->touchLocation_y = HIWORD(lParam);
				}
			}
			break;
		case WM_LBUTTONUP:
			if (scene) {
				scene->touchScreen = false;
			}
			break;
			
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	
	return 0L;
}


static int SetWinCenter(HWND hWnd)
{
	HWND hDeskWnd;
	RECT deskrc, rc;
	int x, y;
	
	hDeskWnd = GetDesktopWindow();
	GetWindowRect(hDeskWnd, (LPRECT)&deskrc);
	GetWindowRect(hWnd, (LPRECT)&rc);
	x = (deskrc.right - (rc.right-rc.left)) / 2;
	y = (deskrc.bottom - (rc.bottom-rc.top)) / 2;
	SetWindowPos(hWnd, HWND_TOP, x, y, (rc.right-rc.left), (rc.bottom-rc.top),SWP_SHOWWINDOW); 
	return 0;
}

extern "C" {
void getApplicationPath(char* buffer,int length);
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow)
{
	HWND        hWnd = NULL;
	WNDCLASSEX  wcex;
	MSG         msg;

	memset(&wcex,0,sizeof(wcex));

	{
		char buf[1024];
		getApplicationPath(buf,1024);
		std::string p(buf);
		p = p + "Resources\\";
		PPGameSetDataPath(p.c_str());
	}

	QBSoundWin* snd = new QBSoundWin(5);
	if (snd) {
		snd->Init();
		snd->Reset();
		snd->setMasterVolume(0.5);
	}
//	QBSound_Start(4);
//	QBSound_Reset();
	
	scene = new PPGameWinScene();
	scene->init();
	
	screenWidth = PPGetInteger("screenWidth",640);
	screenHeight = PPGetInteger("screenHeight",480);
	
	scene->windowSize.width = screenWidth;
	scene->windowSize.height = screenHeight;
	//GetWindowSize(&screenWidth,&screenHeight);
	//screenWidth += 8;
	
	wcex.cbSize         = sizeof(wcex);
	wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc    = WndMainProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, "icon");
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = APP_NAME;
	wcex.hIconSm        = NULL;
	
	//	QBSoundWin::BufferSize = GetParam("soundBuffer",BUFFSIZE);
	//	QBSoundWin::BufferSizeVista = GetParam("soundBufferVista",BUFFSIZEVISTA);
	//printf("soundBuffer %d\n",QBSoundWin::BufferSize);
	
	if(!RegisterClassEx(&wcex))
	{
		return 0;
	}
	
	//	_gameSystem = CreateGameSystem();
	//	if (_gameSystem) {
	//		_gameSystem->InitGame();
	//	}
	
	gFullscreenFlag = 1;
  if (PPGame_GetLocale() == QBGAME_LOCALE_JAPANESE) {
    if (MessageBox(NULL,JP_STRING_DO_YOU_PLAY_FULLSCREEN,APP_NAME,MB_ICONEXCLAMATION | MB_YESNO) == IDNO) {
      gFullscreenFlag = 0;
    }
  } else {
    if (MessageBox(NULL,"Do you play with fullscreen?",APP_NAME,MB_ICONEXCLAMATION | MB_YESNO) == IDNO) {
      gFullscreenFlag = 0;
    }
  }
	//	SetFullscreenFlag(gFullscreenFlag);
	
	if (gFullscreenFlag) {
	//フルスクリーンにする
		DEVMODE devMode;
		ZeroMemory(&devMode,sizeof(devMode));
		devMode.dmSize = sizeof(devMode);
		devMode.dmPelsWidth = 640;//screenWidth;
		devMode.dmPelsHeight = 480;//screenHeight;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		
		ChangeDisplaySettings(&devMode,CDS_FULLSCREEN);
	
		RECT windowRect;
		windowRect.left = 0;
		windowRect.right = 640;
		windowRect.top = 0;
		windowRect.bottom = 480;
		AdjustWindowRectEx(&windowRect,WS_POPUP,FALSE,WS_EX_APPWINDOW);
		screenWidth = windowRect.right;
		screenHeight = windowRect.bottom;
		
		hWnd = CreateWindowEx( WS_EX_APPWINDOW, 
							  APP_NAME, 
							  APP_NAME, 
							  WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  windowRect.right, 
							  windowRect.bottom, 
							  NULL, 
							  NULL, 
							  hInstance, 
							  NULL );
	} else {

		auto RECT rc = { 0, 0, screenWidth,screenHeight } ;
		AdjustWindowRect(
				&rc,                                      // クライアント矩形
				(WS_OVERLAPPEDWINDOW | WS_POPUP | WS_SIZEBOX) & ~(/*WS_THICKFRAME | */WS_MAXIMIZEBOX),  // ウィンドウスタイル
				FALSE                                     // メニューフラグ
		);

		hWnd = CreateWindowEx( WS_EX_APPWINDOW, 
							  APP_NAME, 
							  APP_NAME, 
							  (WS_OVERLAPPEDWINDOW | WS_POPUP | WS_SIZEBOX) & ~(/*WS_THICKFRAME | */WS_MAXIMIZEBOX),  
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  rc.right - rc.left, 
							  rc.bottom - rc.top, 
							  NULL, 
							  NULL, 
							  hInstance, 
							  NULL );
		SetWinCenter(hWnd);
		DragAcceptFiles(hWnd,TRUE);
	}
	
  printf(VERSION_STRING);
  fflush(stdout);
  
	//winGameWindow = hWnd;
	
	//DragAcceptFiles(hWnd,TRUE);
	
	//openWave(hWnd);
	//playSound();
	//playSound();
	
	//gJOY_CONFIGCHANGED_MSGSTRING = RegisterWindowMessage(JOY_CONFIGCHANGED_MSGSTRING);
	
	//printf("gJOY_CONFIGCHANGED_MSGSTRING %d\n",gJOY_CONFIGCHANGED_MSGSTRING);
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	//InitDInput(hInstance,hWnd);
	
	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	//EndDirectSound();
	
	return msg.wParam;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
