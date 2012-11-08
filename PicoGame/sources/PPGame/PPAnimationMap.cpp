/*-----------------------------------------------------------------------------------------------
	名前	PPAnimationMap.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPAnimationMap.h"

static PPAnimationMap** mapTable=NULL;
static int tableCount = 0;

static PPAnimationMap** table()
{
	if (mapTable == NULL) {
		tableCount = 10;
		mapTable = new PPAnimationMap*[tableCount];
		for (int i=0;i<tableCount;i++) mapTable[i] = NULL;
	}
	return mapTable;
}

static int getFreeTable()
{
	for (int j=0;j<2;j++) {
		for (int i=0;i<tableCount;i++) {
			if (mapTable[i] == NULL) return i;
		}
		{
			PPAnimationMap** table = new PPAnimationMap*[tableCount+10];
			for (int i=0;i<tableCount+10;i++) table[i] = NULL;
			for (int i=0;i<tableCount;i++) table[i] = mapTable[i];
			delete mapTable;
			mapTable = table;
			tableCount += 10;
		}
	}
	return -1;
}

PPAnimationMap* PPAnimationMap::getAnimationMap(const char* name)
{
	table();
	for (int i=0;i<tableCount;i++) {
		if (mapTable[i]) {
			if (strcmp(mapTable[i]->name(),name) == 0) return mapTable[i];
		}
	}
	return NULL;
}

void PPAnimationMap::deleteAnimationMap(const char* name)
{
	table();
	for (int i=0;i<tableCount;i++) {
		if (mapTable[i]) {
			if (strcmp(mapTable[i]->name(),name) == 0) {
				delete mapTable[i];
				mapTable[i] = NULL;
			}
		}
	}
}

static void addAnimationMap(PPAnimationMap* map)
{
	table();
	int index = getFreeTable();
	mapTable[index] = map;
	map->index = index;
}

static void removeAnimationMap(PPAnimationMap* map)
{
	table();
	if (map->index>=0) mapTable[map->index] = NULL;
}

PPAnimationMap::PPAnimationMap(const char* filename,const char* name) : /*array(NULL),*/count(0),_name(NULL) {
//	array = loadAnimationFrames(filename);
	addAnimationMap(this);
	if (name == NULL) {
		setName(filename);
	} else {
		setName(name);
	}
}

PPAnimationMap::~PPAnimationMap() {
	removeAnimationMap(this);
//	if (array) {
//		for (int i=0;i<count;i++) delete array[i];
//		delete array;
//	}
	if (event) {
		for (int i=0;i<count;i++) delete event[i];
		delete event;
	}
	if (_name) delete _name;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
