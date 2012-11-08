/*-----------------------------------------------------------------------------------------------
	名前	QBNodeSystemValue.h
	説明		        
	作成	2007.01.02 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

#ifndef __QBNODESYSTEMVALUE_H__
#define __QBNODESYSTEMVALUE_H__

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

int QBNodeValueType(QBNode* node);

BSErr QBNodeSetStringNoCopy(QBNode* node,char* string);
BSErr QBNodeSetString(QBNode* node,char* string);
BSErr QBNodeSetBytes(QBNode* node,BSMem* bytes);
BSErr QBNodeSetULong(QBNode* node,unsigned long value);
BSErr QBNodeSetSLong(QBNode* node,signed long value);
BSErr QBNodeSetFloat(QBNode* node,float value);
BSErr QBNodeSetDouble(QBNode* node,double value);

BSErr QBNodeSetULongArray(QBNode* node,BSMem* array);
BSErr QBNodeSetSLongArray(QBNode* node,BSMem* array);
BSErr QBNodeSetFloatArray(QBNode* node,BSMem* array);
BSErr QBNodeSetDoubleArray(QBNode* node,BSMem* array);

BSErr QBNodeSetStringNoCopyWithKeyPath(QBNode* node,char* string,const char* keyPath);
BSErr QBNodeSetStringWithKeyPath(QBNode* node,char* string,const char* keyPath);
BSErr QBNodeSetBytesWithKeyPath(QBNode* node,BSMem* bytes,const char* keyPath);
BSErr QBNodeSetULongWithKeyPath(QBNode* node,unsigned long value,const char* keyPath);
BSErr QBNodeSetSLongWithKeyPath(QBNode* node,signed long value,const char* keyPath);
BSErr QBNodeSetFloatWithKeyPath(QBNode* node,float value,const char* keyPath);
BSErr QBNodeSetDoubleWithKeyPath(QBNode* node,double value,const char* keyPath);

BSErr QBNodeSetULongArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath);
BSErr QBNodeSetSLongArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath);
BSErr QBNodeSetFloatArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath);
BSErr QBNodeSetDoubleArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath);

char* QBNodeString(QBNode* node);
BSMem* QBNodeBytes(QBNode* node);
unsigned long QBNodeULong(QBNode* node);
signed long QBNodeSLong(QBNode* node);
float QBNodeFloat(QBNode* node);
double QBNodeDouble(QBNode* node);

unsigned long* QBNodeULongArray(QBNode* node);
signed long* QBNodeSLongArray(QBNode* node);
float* QBNodeFloatArray(QBNode* node);
double* QBNodeDoubleArray(QBNode* node);

char* QBNodeStringWithKeyPath(QBNode* node,const char* keyPath);
BSMem* QBNodeBytesWithKeyPath(QBNode* node,const char* keyPath);
unsigned long QBNodeULongWithKeyPath(QBNode* node,const char* keyPath);
signed long QBNodeSLongWithKeyPath(QBNode* node,const char* keyPath);
float QBNodeFloatWithKeyPath(QBNode* node,const char* keyPath);
double QBNodeDoubleWithKeyPath(QBNode* node,const char* keyPath);

unsigned long* QBNodeULongArrayWithKeyPath(QBNode* node,const char* keyPath);
signed long* QBNodeSLongArrayWithKeyPath(QBNode* node,const char* keyPath);
float* QBNodeFloatArrayWithKeyPath(QBNode* node,const char* keyPath);
double* QBNodeDoubleArrayWithKeyPath(QBNode* node,const char* keyPath);

unsigned long QBNodeArrayCount(QBNode* node);

void QBNodeDescription(QBNode* node);

#ifdef __cplusplus
};
#endif

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
