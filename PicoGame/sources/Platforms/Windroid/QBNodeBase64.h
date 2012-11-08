/*-----------------------------------------------------------------------------------------------
	名前	QBNodeBase64.h
	説明		        
	作成	2006.12.29 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

#ifndef __QBNODEBASE64_H__
#define __QBNODEBASE64_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeSystem.h"

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

long base64encodeBufferSize(long length);
char* base64encode(QBNodePool* pool,unsigned char* inBuffer,long inLength);
char* base64encodeWithSpace(QBNodePool* pool,unsigned char* inBuffer,long inLength,int space);
unsigned char* base64decode(QBNodePool* pool,char* string,long length);

#ifdef __cplusplus
};
#endif

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/