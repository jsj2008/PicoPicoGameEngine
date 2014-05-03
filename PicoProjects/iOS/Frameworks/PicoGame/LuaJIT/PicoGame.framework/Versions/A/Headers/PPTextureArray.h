/*-----------------------------------------------------------------------------------------------
	名前	PPTextureArray.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPTextureArray_h
#define picopico_PPTextureArray_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameDef.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPTextureArray {
public:
	PPTextureArray() : tex(NULL) {
		ptr = 0;
		count = 10;
		tex = (PPGameTextureInfo*)calloc(count,sizeof(PPGameTextureInfo));
		for (int i=0;i<count;i++) {
			tex[i].name = NULL;
		}
	}
	virtual ~PPTextureArray() {
		for (int i=0;i<count;i++) {
			if (tex[i].name != NULL) free(tex[i].name);
		}
		free(tex);
	}
	
	int add(const char* name,PPGameImage* layout,bool purge) {
		return addTexture(name,layout,purge);
	}
	int add(const char* name,int chipsize=32) {
		return addTexture(name,NULL,chipsize);
	}
	int add() {
		return addTexture("",NULL,32);
	}
	
	void linear(int index,bool flag) {
		tex[index].option.linear = flag;
	}
	
	int index(const char* name) {
		for (int i=0;i<count;i++) {
			if (tex[i].name != NULL) {
				if (strcmp(tex[i].name,name) == 0) return i;
			}
		}
		return 0;
	}
	
	PPGameTextureInfo* info() {
		return &tex[0];
	}

private:
	PPGameTextureInfo* tex;
	int count;
	int ptr;
	int addTexture(const char* name,PPGameImage* layout,int chipsize) {
		int s=-1;
		for (int i=ptr;i<count-2;i++) {
			if (tex[i].name == NULL) {
				s = i;
				break;
			}
		}
		if (s < 0) {
			tex = (PPGameTextureInfo*)realloc(tex,(count+10)*sizeof(PPGameTextureInfo));
			for (int i=count;i<count+10;i++) {
				tex[i].name = NULL;
			}
			s = count-2;
			count += 10;
		}
		tex[s].name = (char*)malloc(strlen(name)+1);
		strcpy(tex[s].name,name);
		tex[s].group = layout;
		tex[s].chipsize = chipsize;
		tex[s].option.linear = true;
		return s;
	}
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
