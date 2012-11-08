/*-----------------------------------------------------------------------------------------------
	名前	PPGameMapCocos2dx.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameMap.h"
#include "QBNodeSystemPlist.h"
#include "PPGameUtil.h"
#ifdef __COCOS2DX__
#include "CCFileUtils.h"
#endif

PPGameMap::PPGameMap(const char* path) : texture(NULL)
{
	QBNodePool* pool = QBNodePoolAlloc();
	if (pool) {
		
#ifdef __COCOS2DX__
		unsigned long size;
		char* str = (char*)cocos2d::CCFileUtils::getFileData(PPGameResourcePath(path),"r",&size);
		if (str == NULL) {
			str = (char*)cocos2d::CCFileUtils::getFileData(path,"r",&size);
		}
#else
		FILE* fp = fopen(PPGameDataPath(path),"r");
		if (fp == NULL) {
			fp = fopen(PPGameResourcePath(path),"r");
		}
		fseek(fp,0,SEEK_END);
		int size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		char* str = (char*)malloc(size);
		fread(str,size,1,fp);
		fclose(fp);
#endif

		QBNode* root = PlistToQBNode(pool,str);
		//QBNodeDescription(root);
#ifdef __COCOS2DX__
		delete str;
#else
		free(str);
#endif		
		map[0].init();
		map[1].init();
		map[2].init();
		map[0].decompress(QBNodeBytesWithKeyPath(root,".bg0"));
		map[1].decompress(QBNodeBytesWithKeyPath(root,".bg1"));
		map[2].decompress(QBNodeBytesWithKeyPath(root,".bg2"));
		
		map[0].eventNum = 0;
		{
			QBNode* event = QBNodeWithKeyPath(root,".event");
			if (event) {
				int num = QBNodeChildNum(event);
				map[0].eventNum = num;
				map[0].event = new PPGameMapEvent[map[0].eventNum];
				for (int i=0;i<num;i++) {
					QBNode* area = QBNodeChild(event,i);
					if (area) {
						//QBNodeDescription(area);
						
						float dx = QBNodeDoubleWithKeyPath(area,".dx");
						float dy = QBNodeDoubleWithKeyPath(area,".dy");
						float spx = QBNodeDoubleWithKeyPath(area,".spx");
						float spy = QBNodeDoubleWithKeyPath(area,".spy");
						float epx = QBNodeDoubleWithKeyPath(area,".epx");
						float epy = QBNodeDoubleWithKeyPath(area,".epy");
						const char* name = QBNodeStringWithKeyPath(area,".name");
						
						map[0].event[i].dx = dx;
						map[0].event[i].dy = dy;
						map[0].event[i].x = spx;
						map[0].event[i].y = spy;
						map[0].event[i].w = epx-spx+1;
						map[0].event[i].h = epy-spy+1;
						strcpy(map[0].event[i].name,name);
						
					}
				}
			}
		}
		map[1].eventNum = 0;
		map[2].eventNum = 0;
		QBNodePoolFree(pool);
	}
}

PPGameMap::~PPGameMap()
{
	map[0].freedata();
	map[1].freedata();
	map[2].freedata();
	if (texture) free(texture);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
