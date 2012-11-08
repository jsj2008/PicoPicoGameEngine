/*-----------------------------------------------------------------------------------------------
	名前	QBNodeSystemUtil.h
	説明		        
	作成	2007.01.02 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

#ifndef __QBNODESYSTEMUTIL_H__
#define __QBNODESYSTEMUTIL_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeSystemCore.h"

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

BSMem* BSMemCopy(QBNodePool* pool,void* ptr,unsigned long length,char* comment);

QBNode* QBNodeWithKeyPath(QBNode* node,const char* keyPath);
QBNode* QBNodeCreateWithKeyPath(QBNode* node,const char* keyPath);

QBNode* QBNodeAppendChildWithNumberKey(QBNode* node);
QBNode* QBNodeCreateChildWithNumberKey(QBNode* node);
QBNode* QBNodeRemoveLastChild(QBNode* node);

QBNode* QBNodeFirstChild(QBNode* node);
QBNode* QBNodeLastChild(QBNode* node);

BSMem* BSMallocULongArray(QBNodePool* pool,unsigned long count,char* comment);
BSMem* BSMallocSLongArray(QBNodePool* pool,unsigned long count,char* comment);
BSMem* BSMallocDoubleArray(QBNodePool* pool,unsigned long count,char* comment);
BSMem* BSMallocFloatArray(QBNodePool* pool,unsigned long count,char* comment);

BSMem* BSCallocULongArray(QBNodePool* pool,unsigned long count,char* comment);
BSMem* BSCallocSLongArray(QBNodePool* pool,unsigned long count,char* comment);
BSMem* BSCallocDoubleArray(QBNodePool* pool,unsigned long count,char* comment);
BSMem* BSCallocFloatArray(QBNodePool* pool,unsigned long count,char* comment);

BSMem* BSStr(QBNodePool* pool,char* string);
BSMem* BSStrAppend(BSMem* str,char* string);

#ifdef __cplusplus
};
#endif

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
