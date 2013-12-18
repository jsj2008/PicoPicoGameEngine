/*-----------------------------------------------------------------------------------------------
	名前	PPGameStickClass.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPGameStickClass_h__
#define __PPGameStickClass_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPJoyReaderElement
{
public:
	int exist;
#ifdef __MACOSX_APP__
	IOHIDElementRef elem;
#endif
	int value;

	PPJoyReaderElement();
};

class PPJoyReaderAxis : public PPJoyReaderElement
{
public:
	int min,max;
	//int scaledMin,scaledMax;
	int calibCenter,calibMin,calibMax;

	PPJoyReaderAxis();
	void init(int min,int max,int scaledMin,int scaledMax);
	double GetCalibratedValue(void) const;

	void CaptureCenter(void);
	void BeginCaptureMinMax(void);
	void CaptureMinMax(void);
	void CenterFromMinMax(void);
};

class PPJoyReaderButton : public PPJoyReaderElement
{
public:
	PPJoyReaderButton();
};

class PPJoyReaderHatSwitch : public PPJoyReaderElement
{
public:
	PPJoyReaderHatSwitch();
	int valueNeutral;
	int value0Deg;
	int value90Deg;
	int value180Deg;
	int value270Deg;
	int GetDiscreteValue(void) const;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
