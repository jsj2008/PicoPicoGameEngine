/*-----------------------------------------------------------------------------------------------
	名前	PPFontTable.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "PPFontTable.h"

PPFontTable* PPFontTable::table = NULL;
PPFontTable* PPFontTable::table2 = NULL;

const char* PPFontTable::strtable[] = {
	"","А","Б","В","Г","Д","Е","Ё","Ж","З","★","Й","К","Л","М","Н",
	"♪","О","н","П","Р","С","Т","У","Ф","Х","◎","蛇","→","←","↑","↓",
	"　","！","”","＃","＄","％","＆","’","（","）","＊","＋","，","－","．","／",
	"０","１","２","３","４","５","６","７","８","９","：","；","＜","＝","＞","？",
	"＠","Ａ","Ｂ","Ｃ","Ｄ","Ｅ","Ｆ","Ｇ","Ｈ","Ｉ","Ｊ","Ｋ","Ｌ","Ｍ","Ｎ","Ｏ",
	"Ｐ","Ｑ","Ｒ","Ｓ","Ｔ","Ｕ","Ｖ","Ｗ","Ｘ","Ｙ","Ｚ","［","￥","］","＾","＿",
	"｀","ａ","ｂ","ｃ","ｄ","ｅ","ｆ","ｇ","ｈ","ｉ","ｊ","ｋ","ｌ","ｍ","ｎ","ｏ",
	"ｐ","ｑ","ｒ","ｓ","ｔ","ｕ","ｖ","ｗ","ｘ","ｙ","ｚ","｛","￤","｝","~","＼",
	"◇","━","┃","┏","┓","┛","┗","┣","┳","┫","┻","╋","┠","┯","┨","┷",
	"┴","┬","├","┼","┤","─","│","┿","┌","┐","└","┘","┝","┰","┥","┸",
	"―","。","「","」","、","・","ヲ","ァ","ィ","ゥ","ェ","ォ","ャ","ュ","ョ","ッ",
	"ー","ア","イ","ウ","エ","オ","カ","キ","ク","ケ","コ","サ","シ","ス","セ","ソ",
	"タ","チ","ツ","テ","ト","ナ","ニ","ヌ","ネ","ノ","ハ","ヒ","フ","ヘ","ホ","マ",
	"ミ","ム","メ","モ","ヤ","ユ","ヨ","ラ","リ","ル","レ","ロ","ワ","ン","゛","゜",
	"■","●","▲","▼","□","○","△","▽","家","Ш","戸","鍵","車","Э","Ю","Ь",
	"♠","♥","♦","♣","人","е","ё","д","ж","з","и","й","к","л","м","╂",
	"©","剣","宝","瓶","板","壁","を","ぁ","ぃ","ぅ","ぇ","ぉ","ゃ","ゅ","ょ","っ",
	"…","あ","い","う","え","お","か","き","く","け","こ","さ","し","す","せ","そ",
	"た","ち","つ","て","と","な","に","ぬ","ね","の","は","ひ","ふ","へ","ほ","ま",
	"み","む","め","も","や","ゆ","よ","ら","り","る","れ","ろ","わ","ん","点","丸",
	NULL
};

const char* PPFontTable::dakutens[] = {
	"ガ","ギ","グ","ゲ","ゴ","ザ","ジ","ズ","ゼ","ゾ","ダ","ヂ","ヅ","デ","ド","バ","ビ","ブ","ベ","ボ",
	"が","ぎ","ぐ","げ","ご","ざ","じ","ず","ぜ","ぞ","だ","ぢ","づ","で","ど","ば","び","ぶ","べ","ぼ",
	"パ","ピ","プ","ペ","ポ",
	"ぱ","ぴ","ぷ","ぺ","ぽ",
	"→","←","↑","↓",
	"%→","%←","%↑","%↓","%↔","%↕","%⤹","%⤵","%0","%%","%",
	NULL
};

const char* PPFontTable::dakutend[] = {
	"カ","キ","ク","ケ","コ","サ","シ","ス","セ","ソ","タ","チ","ツ","テ","ト","ハ","ヒ","フ","ヘ","ホ",
	"か","き","く","け","こ","さ","し","す","せ","そ","た","ち","つ","て","と","は","ひ","ふ","へ","ほ",
	"ハ","ヒ","フ","ヘ","ホ",
	"は","ひ","ふ","へ","ほ",
	"→","←","↑","↓",
	"","","","","","","","","","％","％",
	NULL
};

const char PPFontTable::controlcode[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,
	2,2,2,2,2,
	0,0,0,0,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0,0,
	0,
};

static PPFontTable* getTable(PPFontTable** table,const char** strtable)
{
	if ((*table) == NULL) {
		(*table) = new PPFontTable[256]();
		for (int i=0;i<256;i++) {
			(*table)[i].index = -1;
			(*table)[i].next = NULL;
		}
		for (int j=0;strtable[j]!=NULL;j++) {
			unsigned char* p = (unsigned char*)strtable[j];
			unsigned char a,b,c,d;
			a=b=c=d=0;
			if (p[0] != 0) {
				a=p[0];
				if (p[1] != 0) {
					b=p[1];
					if (p[2] != 0) {
						c=p[2];
						if (p[3] != 0) {
							d=p[3];
//							if (p[4] != 0) {
//								e=p[4];
//							}
						}
					}
				}
			}

			if (b>0) {
				if ((*table)[a].next == NULL) {
					(*table)[a].next = new PPFontTable[256]();
					for (int i=0;i<256;i++) {
						(*table)[a].next[i].index = -1;
						(*table)[a].next[i].next = NULL;
					}
				}
			}
			if (c>0) {
				if ((*table)[a].next[b].next == NULL) {
					(*table)[a].next[b].next = new PPFontTable[256]();
					for (int i=0;i<256;i++) {
						(*table)[a].next[b].next[i].index = -1;
						(*table)[a].next[b].next[i].next = NULL;
					}
				}
			}
			if (d>0) {
				if ((*table)[a].next[b].next[c].next == NULL) {
					(*table)[a].next[b].next[c].next = new PPFontTable[256]();
					for (int i=0;i<256;i++) {
						(*table)[a].next[b].next[c].next[i].index = -1;
						(*table)[a].next[b].next[c].next[i].next = NULL;
					}
				}
			}
//			if (e>0) {
//				if ((*table)[a].next[b].next[c].next[d].next == NULL) {
//					(*table)[a].next[b].next[c].next[d].next = new PPFontTable[256]();
//					for (int i=0;i<256;i++) {
//						(*table)[a].next[b].next[c].next[d].next[i].index = -1;
//						(*table)[a].next[b].next[c].next[d].next[i].next = NULL;
//					}
//				}
//			}
			
			if ((*table)[a].next) {
				if ((*table)[a].next[b].next) {
					if ((*table)[a].next[b].next[c].next) {
						if ((*table)[a].next[b].next[c].next[d].index >= 0) {
							printf("error %d\n",j);
						}
						(*table)[a].next[b].next[c].next[d].index = j;
					} else {
						if ((*table)[a].next[b].next[c].index >= 0) {
							printf("error %d\n",j);
						}
						(*table)[a].next[b].next[c].index = j;
					}
				} else {
					if ((*table)[a].next[b].index >= 0) {
						printf("error %d\n",j);
					}
					(*table)[a].next[b].index = j;
				}
			} else {
				if ((*table)[a].index >= 0) {
					printf("error %d\n",j);
				}
				(*table)[a].index = j;
			}
			
//			printf("%d:",j);
//			for (int i=0;i<strlen(strtable[j]);i++) {
//				printf("%02x,",p[i]);
//			}
//			printf("\n");
		}
		
	}
	return (*table);
}

int PPFontTable::findLetter(const char* letter)
{
#if 1
	PPFontTable* t = getTable(&PPFontTable::table,PPFontTable::strtable);
	if (t) {
		unsigned char* l = (unsigned char*)letter;
		int index=-1;
		if (t[l[0]].index >= 0) index=t[l[0]].index;
		if (t[l[0]].next) {
			if (t[l[0]].next[l[1]].index >= 0) index=t[l[0]].next[l[1]].index;
			if (t[l[0]].next[l[1]].next) {
				if (t[l[0]].next[l[1]].next[l[2]].index >= 0) index=t[l[0]].next[l[1]].next[l[2]].index;
				if (t[l[0]].next[l[1]].next[l[2]].next) {
					if (t[l[0]].next[l[1]].next[l[2]].next[l[3]].index >= 0) index=t[l[0]].next[l[1]].next[l[2]].next[l[3]].index;
				}
			}
		}
		return index;
	}
#else
	for (int j=0;PPFontTable::strtable[j]!=NULL;j++) {
		int l = strlen(PPFontTable::strtable[j]);
		if (l > 0) {
			if (strncmp(letter,PPFontTable::strtable[j],l) == 0) {
				return j;
			}
		}
	}
#endif
	return -1;
}

int PPFontTable::findDakuten(const char* letter)
{
#if 1
	PPFontTable* t = getTable(&PPFontTable::table2,PPFontTable::dakutens);
	if (t) {
		unsigned char* l = (unsigned char*)letter;
		if (t[l[0]].index >= 0) return t[l[0]].index;
		if (t[l[0]].next) {
			if (t[l[0]].next[l[1]].index >= 0) return t[l[0]].next[l[1]].index;
			if (t[l[0]].next[l[1]].next) {
				if (t[l[0]].next[l[1]].next[l[2]].index >= 0) return t[l[0]].next[l[1]].next[l[2]].index;
				if (t[l[0]].next[l[1]].next[l[2]].next) {
					if (t[l[0]].next[l[1]].next[l[2]].next[l[3]].index >= 0) return t[l[0]].next[l[1]].next[l[2]].next[l[3]].index;
				}
			}
		}
	}
#else
	for (int j=0;PPFontTable::dakutens[j]!=NULL;j++) {
		int l = strlen(PPFontTable::dakutens[j]);
		if (l > 0) {
			if (strncmp(letter,PPFontTable::dakutens[j],l) == 0) {
				return j;
			}
		}
	}
#endif
	return -1;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
