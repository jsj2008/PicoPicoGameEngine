/*-----------------------------------------------------------------------------------------------
	名前	PPFontTable.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef PicoPicoGames_PPFontTable_h
#define PicoPicoGames_PPFontTable_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPFontTable {
public:
	static const char* strtable[];
	static const char* dakutens[];
	static const char* dakutend[];
	static const char controlcode[];
	
	static PPFontTable* table;
	static PPFontTable* table2;
	
	static int findLetter(const char* letter);
	static int findDakuten(const char* letter);
	
	int index;
	class PPFontTable* next;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
