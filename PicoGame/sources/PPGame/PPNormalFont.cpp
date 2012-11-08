/*-----------------------------------------------------------------------------------------------
	名前	PPNormalFont.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPNormalFont.h"
#include "PPGameSprite.h"
#include "PPFontTable.h"

int PPNormalFont::__print(PPWorld* _target,int x,int y,float _scale,const char* _str)
{
//printf("%s\n",_str);
	PPPoint s = world()->scale()*localScale;
	cur = PPPoint(x,y);
	const signed char* str = (signed char*)_str;
	for (int i=0;str[i]!=0;i++) {
		if (str[i] == ' ') {
			moveCursor(1,0);
		} else
		if (str[i] >= 0 && str[i] <= 0x20) {
			if (str[i] == '\n') {
				cur.x = 0;
				cur.y += fontHeight*localScale.y;
				continue;
			} else
			if (str[i] == '\r') {
				cur.x = 0;
				continue;
			}
		} else
		if (0x20 < str[i] && str[i] != '%') {
			if (_target) (_target->*selector)(cur,str[i],this);
			moveCursor(1,0);
		} else {
			const signed char* dst = &str[i];
			char ccode = -1;
			int ccodelen = 0;

			//濁点チェック
			{
				int j=PPFontTable::findDakuten((const char*)dst);
				if (j>=0) {
					dst = (const signed char*)PPFontTable::dakutend[j];
					ccode = PPFontTable::controlcode[j];
					ccodelen = (int)strlen(PPFontTable::dakutens[j]);
				}
			}

			//コントロールコードの処理
			switch (ccode) {
			case 0:
				i += ccodelen-1;
				break;
			case 1:
				if (_target) (_target->*selector)(PPPoint(cur.x,cur.y-fontHeight*s.y),19*16+14,this);
				break;
			case 2:
				if (_target) (_target->*selector)(PPPoint(cur.x,cur.y-fontHeight*s.y),19*16+15,this);
				break;
			case 0x11:		//右
				moveCursor(1,0);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x12:		//左
				moveCursor(-1,0);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x13:		//上
				moveCursor(0,-1);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x14:		//下
				moveCursor(0,1);
				i += ccodelen-1;
				dst = NULL;
				break;
			case 0x15:		//上下反転
			case 0x16:		//左右反転
			case 0x17:		//左回転
			case 0x18:		//右回転
			case 0x19:		//リセット
				dst = NULL;
				break;
			default:
				break;
			}

			//文字描画
			if (dst) {
				int j=PPFontTable::findLetter((const char*)dst);
				if (j>=0) {
					if (_target) (_target->*selector)(cur,j,this);
					if (ccode != 0) i += strlen(PPFontTable::strtable[j])-1;
					moveCursor(1,0);
				}
			}
			
		}
	}
	return cur.x;
}

void PPNormalFont::moveCursor(int dx,int dy)
{
	PPPoint s = PPPoint(1,1)*localScale;
	PPPoint ws=world()->scale();
	PPPoint o = PPPointZero;
	cur.x += dx*fontWidth*s.x;
	if (lineWrap) {
		PPRect r = world()->viewPort();
		if (r.width < (cur.x+fontWidth*s.x)*ws.x-o.x-r.x) {
			cur.x = (o.x+r.x)/ws.x;
			cur.y += fontHeight*s.y;
		}
	}
	cur.y += dy*fontHeight*localScale.y;
}

float PPNormalFont::height()
{
	return fontHeight*localScale.y;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
