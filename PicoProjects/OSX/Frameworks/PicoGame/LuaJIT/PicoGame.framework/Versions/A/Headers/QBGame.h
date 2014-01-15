/*-----------------------------------------------------------------------------------------------
	名前	QBGame.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __QBGAME_H__
#define __QBGAME_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGamePoly.h"
#include "PPGameDef.h"
#include "PPGameUtil.h"
#include "PPGameSprite.h"
#include "PPWorld.h"
#include "PPTextureArray.h"
#include <sys/time.h>

class PPFont;
class PPNormalFont;
class PPMiniFont;

enum {
	QBGAME_CONTROLLER_NORMAL,
	QBGAME_CONTROLLER_HIDE,
};

enum {
	QBGAME_CONTROLLER_TYPE_VERTICAL_CENTER = 0,
	QBGAME_CONTROLLER_TYPE_VERTICAL,
	QBGAME_CONTROLLER_TYPE_HORIZONTAL,

	QBGAME_CONTROLLER_TYPE_V_LEFT,
	QBGAME_CONTROLLER_TYPE_V_RIGHT,
	QBGAME_CONTROLLER_TYPE_H_LEFT,
	QBGAME_CONTROLLER_TYPE_H_RIGHT,
};

enum {
	QBGAME_CONTROLLER_ENABLE_ARROW = 0x01,
	QBGAME_CONTROLLER_ENABLE_A = 0x02,
	QBGAME_CONTROLLER_ENABLE_B = 0x04,
};

#define QBGAME_MAX_MENU 10
#define QBGAME_MAX_TOUCH 5
#define QBGAME_MAX_COLORTABLE 256

#define SET_ANIMATION(obj,method,animation) obj->display(this,(PP_DRAW_SELECTOR)(&method),animation)

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/


class PPFPS {
public:
	PPFPS() {
		m_fDeltaTime=0;
		m_uFrames=0;
		m_fAccumDt=0;
		m_fFrameRate=0;
	}
	
	struct timeval m_pLastUpdate;
	float m_fDeltaTime;
	unsigned int m_uFrames;
	float m_fAccumDt;
	float m_fFrameRate;

	void idle() {
		struct timeval now;
		gettimeofday((struct timeval *)&now,0);
		m_fDeltaTime = (now.tv_sec - m_pLastUpdate.tv_sec) + (now.tv_usec - m_pLastUpdate.tv_usec) / 1000000.0f;
		if (m_fDeltaTime < 0) m_fDeltaTime = 0;
		m_uFrames++;
		m_fAccumDt += m_fDeltaTime;
		if (m_fAccumDt > 0.1)
		{
			m_fFrameRate = m_uFrames / m_fAccumDt;
			m_uFrames = 0;
			m_fAccumDt = 0;
		}
		m_pLastUpdate = now;
	}
		
	float frameRate() {
		return m_fFrameRate;
	}
};

class QBGame : public PPWorld {
public:
	QBGame();
	virtual ~QBGame();
	
	virtual PPGameTextureInfo* TextureInfo();		//使用するテクスチャの情報 オーバーライドして使う//

	PPTextureArray* texture();

	virtual int Idle();								//ゲームのアイドル オーバーライドして使う//
	
	virtual void DrawGL() {}						//3D描画用

	//ゲーム固有のバイナリデータのSave/Load//
	virtual unsigned char* GetData(const char* key,int* dataSize);
	virtual void SetData(const char* key,unsigned char* data,int dataSize);

	//キー入力関連//
	virtual bool KeyWait(unsigned long mask,bool releaseHit=false);//ボタン入力待ち//
	virtual void RepeatKey(int firstDelay=20,int secondDelay=15);
	virtual bool Key(unsigned long mask);			//ボタン入力判定//
	virtual unsigned long GetKey();					//ボタン入力//
	virtual unsigned long GetPreKey();			//ボタン入力//
	virtual void SetKey(unsigned long key);
	
	//文字描画関連//
	virtual void Clear();							//画面クリア//
	virtual void Color(int color);					//文字の色を指定//
	virtual void Locate(float x,float y);				//文字の位置をキャラクタ単位で指定//
	virtual void Offset(float x,float y);				//文字の位置をドット単位で指定//
	virtual void Print(const char* format,...);		//文字描画//

	//文字の描画 オーバーライドして使う//
	virtual void PutFont(float x,float y,int pat,unsigned char r=0xff,unsigned char g=0xff,unsigned char b=0xff,unsigned char a=0xff);

	//色の取得/設定//
	unsigned char* GetRGBColor(int color,unsigned char* r,unsigned char* g,unsigned char* b,unsigned char* a);
	int SetRGBColor(int color,unsigned char r,unsigned char g,unsigned char b,unsigned char a);
	
	//パターンの描画//
//	virtual void Flip(unsigned short type=0);		//PPFLIP_H PPFLIP_V PPFLIP_R90 PPFLIP_R180 PPFLIP_R270
	virtual void Center(float center_x=0,float center_y=0);
	virtual void Scale(PPPoint scale,float center_x,float center_y);		//拡大縮小
	virtual void Scale(PPPoint scale);									//拡大縮小
	virtual void Rotate(float rotate,float center_x,float center_y);	//回転
	virtual void Rotate(float rotate);									//回転
	virtual void Put(float x,float y,int pat,int group,int texture,unsigned char r=0xff,unsigned char g=0xff,unsigned char b=0xff,unsigned char a=0xff);
	virtual PPPoint GetScale() {return scale_value;}

	virtual void Print2(const char* format,...);
//	virtual void FontPut2(int x,int y,int pat);

	PPSize PatternSize(int textureid,int group);

	//PPWorld
	virtual PPPoint locate(PPPoint pos=PPPointZero) {
		Offset(pos.x,pos.y);
		return PPPoint(locatex,locatey);
	};
	virtual int print(const char* format,...) {
		va_list args;
		va_start(args,format);
		__print(format,args);
 		va_end(args);
		return locatex;
 	}
	virtual void rawPrint(const char* string);
	virtual void print(const char* format,va_list args) {
		__print(format,args);
 	}
	virtual void color(PPColor col) {
		SetRGBColor(colorIndex,col.r,col.g,col.b,col.a);
	}
	virtual PPColor color() {
		PPColor col;
		GetRGBColor(colorIndex,&col.r,&col.g,&col.b,&col.a);
		return col;
	}
	virtual PPPoint scale(PPPoint scale=PPPointZero) {
		if (scale.x == 0 && scale.y == 0) return GetScale();
		Scale(scale);
		return scale;
	}
	virtual void rotate(float rotate=0,float cx=0,float cy=0) {
		Rotate(rotate,cx,cy);
	}
	virtual void origin(PPPoint origin=PPPointZero);
	virtual void flip(int type) {
		//Flip(type);
		poly.flags = type;
	}
	virtual void blend(PPGameBlend blendType) {
		poly.blend = blendType;
	}
	virtual void fill(PPRect r) {
		Fill(r.x,r.y,r.width,r.height,colorIndex);
	}
	virtual void line(PPPoint p1,PPPoint p2) {
		Line(p1.x,p1.y,p2.x,p2.y,colorIndex);
	}
	virtual void box(PPRect r) {
		Box(r.x,r.y,r.width,r.height,colorIndex);
	}
	virtual unsigned char* getData(const char* key,int* dataSize) {
		return GetData(key,dataSize);
	}
	virtual void setData(const char* key,unsigned char* data,int dataSize) {
		SetData(key,data,dataSize);
	}
	virtual int touchCount() {return __touchCount;}				//タッチ位置の取得//
	virtual PPPoint touchPosition(int i) {						//タッチ位置(座標)//
		return PPPoint(TouchXPosition(i),TouchYPosition(i));
	}
	virtual void enable(unsigned long flags) {
		__anyFlags |= flags;
	}
	virtual void disable(unsigned long flags) {
		__anyFlags &= (~flags);
	}
	virtual PPSize winSize() {
		return PPSize(screenWidth,screenHeight);
	}
	virtual bool play(const char* mml,int ch=0) {
		Play(mml,ch);
		return false;
	}
	virtual void put(PPPoint p,int gid,int group,int texture) {
		Put(p.x,p.y,gid,group,texture);
	}
	//virtual int printmini(PPPoint pos,const char* format,...);
	//virtual void viewPort(PPRect rect);
	virtual PPRect viewPort() {
		return projector->st.viewPort;
//		return PPGameSprite::instance()->st.viewPort;
	}
	virtual void font(const char* name=NULL);
	virtual int rawLength(const char* str);
	virtual int length(const char* format,...) {
		int len;
		va_list args;
		va_start(args,format);
		len = __length(format,args);
 		va_end(args);
		return len;
	}
	virtual int length(const char* format,va_list args) {
		return __length(format,args);
 	}
	//virtual void setTexture(int fontID,unsigned char* pixel,int width,int height,int bytesPerRow);
	//virtual void setTileSize(int texture,PPSize size);
	//virtual PPSize getTileSize(int texture);

#ifndef NO_TTFONT
	virtual int putFont(PPFont* font,float x,float y,const char* str);
	virtual int calcLength(PPFont* font,const char* str);
#endif
	
	//図形の描画//
//	virtual void Alpha(int alpha);
	virtual void Line(float x1,float y1,float x2,float y2,int color);
	virtual void Fill(float x,float y,float width,float height,int color);
	virtual void Box(float x,float y,float width,float height,int color);

	//ローカライズ//
//	virtual int Locale();							//日本語(QBGAME_LOCALE_JAPANESE)、それ以外(QBGAME_LOCALE_OTHER)//
	
	//画面のタッチ判定//
	virtual int TouchCount() {return __touchCount;}				//タッチ位置の取得//
	virtual int TouchXPosition(int i) {return touchX[i];}		//タッチ位置(X座標)//
	virtual int TouchYPosition(int i) {return touchY[i];}		//タッチ位置(Y座標)//

	//効果音関連//
#ifdef ANDROID_NDK
	void Play(int mml,int ch=0);					//サウンドのMML再生//
#else
	void Play(const char* mml,int ch=0);			//サウンドのMML再生//
#endif
	void Stop(int ch=0);							//サウンドの停止//
	bool IsPlaying(int ch=0);						//サウンド再生中か？//

	virtual void* LoadData(const char* name,unsigned long* outsize);
	virtual int SaveData(const char* name,void* data,unsigned long dataSize);

	//描画系パラメータ//
	float locatex,locatey;							//文字描画位置//
	PPPoint printlocate;							//文字描画位置//
	int colorIndex;									//描画カラー (16色)//
	//int polyCount;									//描画ポリゴン格納数//
	PPGamePoly poly;								//描画ポリゴン格納バッファ//

	int button_offset_x[3];
	int button_offset_y[3];
	
	float accelerometerValue[3];
	float gyroValue[3];
	float magnetometerValue[3];
	
	PPPoint mouseLocation;

	virtual unsigned long getKey() {
		return GetKey();
	}
	
	virtual void reloadData() {
	}

	int gameIdle(unsigned long key);
	void setTouchCount(int count) {__touchCount = count;}
	void setTouchPosition(int x,int y) {touchX[__touchCount]=x;touchY[__touchCount]=y;__touchCount++;}
//	PPGameText* text;
//	bool exitGame;
#ifdef _ANDROID
	bool openManual;
	bool showAd;
	bool showMarket;
#endif
	int drawAdStep;
	virtual void resetDrawAd() {drawAdStep=0;}
	virtual void DrawAd();
	
	bool keyConfig;
	bool resetKeyConfig;
	
//	PPGameSprite* gdc;

	int frameCount;
	bool hdVersion;
	bool stdVersion;
	int fontWidth;
	
	bool later3GS;
	
	int screenWidth;
	int screenHeight;

	void __print(const char* format,va_list args);
//	void __print2(const char* format,va_list args);
	int __length(const char* format,va_list args);
	
	static unsigned long blinkCounter;

//	PPMiniFont* __miniFont;
	PPNormalFont* __normalFont;
	PPNormalFont* __halfFont;
	PPNormalFont* __miniFont;
//	PPNormalFont* __normalFont2;

	virtual void openGameLibrary(PPLuaScript* script,const char* name);
	virtual void openTextureLibrary(PPLuaScript* script,const char* name);
	virtual void openAudioEngineBGM(PPLuaScript* script,const char* name);
	virtual void openAudioEngineEffect(PPLuaScript* script,const char* name);
	virtual void openAudioEngineMML(PPLuaScript* script,const char* name);
	virtual void openAudioEngineFlMML(PPLuaScript* script,const char* name);
	virtual void openFontLibrary(PPLuaScript* script,const char* name);
	virtual void openKeyLibrary(PPLuaScript* script,const char* name);
	virtual void openTouchLibrary(PPLuaScript* script,const char* name);
	virtual void openViewLibrary(PPLuaScript* script,const char* name);
	virtual void openAudioEngineSEMML(PPLuaScript* script,const char* name);
  virtual void openGameController(PPLuaScript* script,const char* name);
  virtual void openTextToSpeech(PPLuaScript* script,const char* name);

	virtual void drawPattern(PPPoint pos,unsigned short gid,void* userdata);
	virtual void disableIO();

//	unsigned short splite_flags;
	float rotate_value;
//	PPPoint rotate_center;
//	float rotate_center_x_value;
//	float rotate_center_y_value;
//	PPGameBlend blend_value;
	
//	std::string* __nextStepFunc;

	PPFont* curFont;
	bool lineWrap;
  bool pauseButton;
	
	int curTexture;

	PPFPS fps;
	
protected:

	int touchX[QBGAME_MAX_TOUCH];
	int touchY[QBGAME_MAX_TOUCH];
	int __touchCount;

	int chooseBGM;									//BGMの選択//
	int chooseBGM_X;
	int chooseBGM_Y;
	int chooseBGM_W;
	int chooseBGM_H;
	int playBGM;									//BGMの再生(>=0)/ストップ(-1)//
	bool playBGMOneTime;							//BGMのループ指定(true:1 false:ループ)//
	
	unsigned long key;
	unsigned long prekey;
	unsigned long keyRepeatTimer;
	unsigned char colortable[QBGAME_MAX_COLORTABLE][4];
	
//	int __alpha;
	
//	PPGameMapData* map[3];							//マップデータ//

	PPTextureArray* __tarray;
	
	unsigned long __anyFlags;

	void putNormalFont(PPPoint pos,unsigned short gid);
	void putHalfFont(PPPoint pos,unsigned short gid);
	void putMiniFont(PPPoint pos,unsigned short gid);
	
//	PPRect _portRect;
	
	int fontTexture;
	
	bool fontLoaded;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
