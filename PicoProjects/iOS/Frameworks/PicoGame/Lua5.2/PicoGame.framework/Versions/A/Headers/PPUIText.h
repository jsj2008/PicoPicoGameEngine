/*-----------------------------------------------------------------------------------------------
	名前	PPUIText.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef PicoPicoGames_PPUIText_h
#define PicoPicoGames_PPUIText_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPUIScrollView.h"
#include <string>
#include <vector>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPUIText : public PPUIScrollView
{
public:
	enum {
		MAX_COLUMN = 256
	};

	PPUIText(PPWorld* world);
	virtual ~PPUIText();

	virtual void init();
	virtual void start();
	virtual void drawSelf(PPPoint pos);

	virtual void setFrameSize(PPSize r);

	virtual void stepIdle();
	
	virtual void calcContentsRect(bool updateLineStr=true);
	virtual PPRect getLineRect(int line,PPRect* outRect=NULL,int maxout=0);
	virtual PPRect getCurRect(int line);

	virtual void touchDown(PPPoint pos);
	virtual void touchUp(PPPoint pos);
	virtual void touchDrag(PPPoint pos);

	float lineHeight;
	float lineWidth;
	float lineSpan;
	int maxLine;
	int column;
	bool autoScroll;
	bool runScroll;
	bool autoCenter;
	bool hideCell;
	bool updateString;
	bool dragged;
	bool selectable;
	bool multiselectable;
	bool emptyselection;
	
	float marginLeft;
	float marginRight;
	float marginTop;
	float marginBottom;
	
	PPColor selectedColor;

	std::string str;
	std::vector<std::vector<std::string*>*> lineStr;
	std::vector<int> selectedLines;
	
	bool isEmpty() {
		for (int i=0;i<selectedLines.size();i++) {
			if (selectedLines[i]>=0 && selectedLines[i]<maxLine) return false;
		}
		return true;
	}
	
	void clearLineStr() {
		std::vector<std::vector<std::string*>*>::iterator iter;
		for (iter = lineStr.begin(); iter != lineStr.end(); ++iter) {
			if (*iter) {
				std::vector<std::string*>::iterator iter2;
				for (iter2 = (*iter)->begin(); iter2 != (*iter)->end(); ++iter2) {
					if (*iter2) {
						delete (*iter2);
					}
				}
				delete (*iter);
			}
		}
		lineStr.clear();
	}
	void appendLineStr(std::vector<std::string*>* str) {
		lineStr.push_back(str);
	}
	
	PPPoint nextLine(int line,PPPoint pos);
	void resetColPos() {
		for (int i=0;i<column;i++) {
			colPos[i] = PPPoint((lineWidth/column)*i+marginLeft,marginTop);
		}
	}
	PPPoint colPos[MAX_COLUMN];

	std::string message;
	int messagePtr;
	int messageTimer;
	int messageSpeed;
	
	int selectedCell;

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass=NULL);

	static PPObject* registClass(PPLuaScript* script,const char* name=NULL,const char* superclass=NULL);
	static PPObject* registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass=NULL);
  
  static std::string classname;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
