/*-----------------------------------------------------------------------------------------------
	名前	QBNodeSystemUtil.c
	説明		        
	作成	2007.01.02 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeSystemUtil.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

static int cmpKey(char* key1,const char* key2,int* cnt);

/*-----------------------------------------------------------------------------------------------
	局所変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部に公開するプログラム
-----------------------------------------------------------------------------------------------*/

BSMem* BSMemCopy(QBNodePool* pool,void* ptr,unsigned long length,char* comment)
{
	BSMem* mem = BSMalloc(pool,length,comment);
	if (mem) {
		memcpy(mem,ptr,length);
	}
	return mem;
}

QBNode* QBNodeWithKeyPath(QBNode* node,const char* keyPath)
{
	int m;
	int n = 0;
	int cnt;
	if (keyPath[0] == '.') {
		n = 1;
		if (keyPath[n] == 0) return NULL;
	} else
	if ((m = cmpKey(QBNodeKey(node),&keyPath[0],&cnt))) {
		if (cnt==0) return node;
		n = m+1;
	} else {
		return NULL;
	}
	{
		int i=0;
		for (i=0;i<BSSize(node->childArray)/sizeof(QBNode*);i++) {
			if (node->childArray[i]) {
				if ((m = cmpKey(QBNodeKey(node->childArray[i]),&keyPath[n],&cnt))) {
					if (cnt == 0) return node->childArray[i];
					return QBNodeWithKeyPath(node->childArray[i],&keyPath[m+n]);
				}
			}
		}
	}
	return NULL;
}

QBNode* QBNodeCreateWithKeyPath(QBNode* node,const char* keyPath)
{
	int m;
	int n = 0;
	int cnt;
	if (keyPath[0] == '.') {
		n = 1;
		if (keyPath[n] == 0) return NULL;
	} else
	if ((m = cmpKey(QBNodeKey(node),&keyPath[0],&cnt))) {
		if (cnt==0) return node;
		n = m+1;
	} else {
		return NULL;
	}
	{
		int i=0;
		for (i=0;i<BSSize(node->childArray)/sizeof(QBNode*);i++) {
			if (node->childArray[i]) {
				if ((m = cmpKey(QBNodeKey(node->childArray[i]),&keyPath[n],&cnt))) {
					if (cnt == 0) return node->childArray[i];
					return QBNodeCreateWithKeyPath(node->childArray[i],&keyPath[m+n]);
				}
			}
		}
	}
	{
		QBNode* child = QBNodeAlloc(QBNodePoolPtr(node),&keyPath[n]);
		if (child) {
			BSRelease(child);
			QBNodeAppend(node,child);
			if ((m = cmpKey(QBNodeKey(child),&keyPath[n],&cnt))) {
				if (cnt==0) return child;
				return QBNodeCreateWithKeyPath(child,&keyPath[m+n]);
			}
		}
	}
	return NULL;
}

QBNode* QBNodeAppendChildWithNumberKey(QBNode* node)
{
	return QBNodeCreateChildWithNumberKey(node);
}

QBNode* QBNodeCreateChildWithNumberKey(QBNode* node)
{
	if (node != NULL) {
		QBNode* child;
		int childnum = QBNodeChildNum(node);
		char numberkey[64];
		sprintf(numberkey,".%d",childnum);
		child = QBNodeCreateWithKeyPath(node,numberkey);
		return child;
	}
	return NULL;
}

QBNode* QBNodeRemoveLastChild(QBNode* node)
{
	if (node != NULL) {
		QBNode* last = QBNodeLastChild(node);
		if (last) {
			QBNodeRemove(node,last);
			QBNodePoolRecycle(QBNodePoolPtr(node));
		}
	}
	return NULL;
}

QBNode* QBNodeFirstChild(QBNode* node)
{
	if (node != NULL) {
		int childnum = QBNodeChildNum(node);
		if (childnum > 0) {
			return QBNodeChild(node,0);
		}
	}
	return NULL;
}

QBNode* QBNodeLastChild(QBNode* node)
{
	if (node != NULL) {
		int childnum = QBNodeChildNum(node);
		if (childnum > 0) {
			return QBNodeChild(node,childnum-1);
		}
	}
	return NULL;
}

#pragma mark -

BSMem* BSMallocULongArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSMalloc(pool,count*sizeof(unsigned long),comment));
}

BSMem* BSMallocSLongArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSMalloc(pool,count*sizeof(signed long),comment));
}

BSMem* BSMallocDoubleArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSMalloc(pool,count*sizeof(double),comment));
}

BSMem* BSMallocFloatArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSMalloc(pool,count*sizeof(float),comment));
}

BSMem* BSCallocULongArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSCalloc(pool,count*sizeof(unsigned long),comment));
}

BSMem* BSCallocSLongArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSCalloc(pool,count*sizeof(signed long),comment));
}

BSMem* BSCallocDoubleArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSCalloc(pool,count*sizeof(double),comment));
}

BSMem* BSCallocFloatArray(QBNodePool* pool,unsigned long count,char* comment)
{
	return BSRelease(BSCalloc(pool,count*sizeof(float),comment));
}

#pragma mark -

BSMem* BSStr(QBNodePool* pool,char* string)
{
	BSMem* mem = BSMalloc(pool,strlen(string)+1,string);
	if (mem) {
		strcpy((char*)mem,string);
	}
	return mem;
}

BSMem* BSStrAppend(BSMem* str,char* string)
{
	BSMem* mem = BSMalloc(QBNodePoolPtr(str),strlen(string)+strlen((char*)str)+1,string);
	if (mem) {
		strcpy((char*)mem,(char*)str);
		strcat((char*)mem,string);
	}
	//BSRelease(str);
	return mem;
}

/*-----------------------------------------------------------------------------------------------
	外部に公開しないプログラム
-----------------------------------------------------------------------------------------------*/

//キーの比較//
static int cmpKey(char* key1,const char* key2,int* cnt)
{
	int r = 0;
	int n = 0;
	int f1,f2;
	*cnt = 1;
	while (1) {
		f1 = (key1[n] == 0 || key1[n] == '.');
		f2 = (key2[n] == 0 || key2[n] == '.');
		if (f1 != 0 || f2 != 0) break;		//どちらかが終了の場合は終わり//
		if (key1[n] != key2[n]) break;		//キーが違う場合は終わり//
		n ++;
	}
	if (f1 != 0 && f2 != 0) r = n;			//両方終了の場合は同じ//
EXIT_FUNCTION:
	if (key2[n] == 0) *cnt = 0;				//文字列終わりの場合は終了//
	return r;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
