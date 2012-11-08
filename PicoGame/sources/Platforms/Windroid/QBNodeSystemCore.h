/*-----------------------------------------------------------------------------------------------
	名前	QBNodeSystemCore.h
	説明		        
	作成	2006.12.31 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

#ifndef __QBNODESYSTEMCORE_H__
#define __QBNODESYSTEMCORE_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

enum {
	BSERR_OK = 0,			//成功
	
	BSERR_PARAM = -1,		//パラメータエラー
	BSERR_MEM = -2,			//メモリエラー
	BSERR_NO_CHILD = -3,	//子ノードではないエラー
	BSERR_INVAILD = -4
};

enum {
	QBNODE_TYPE_NONE,

	_QBNODE_TYPE_VALUE,

	QBNODE_TYPE_ULONG = _QBNODE_TYPE_VALUE,
	QBNODE_TYPE_SLONG,
	QBNODE_TYPE_FLOAT,
	QBNODE_TYPE_DOUBLE,
	QBNODE_TYPE_CONST_STRING,
	
	_QBNODE_TYPE_BSMEM,
	
	QBNODE_TYPE_BYTE_ARRAY = _QBNODE_TYPE_BSMEM,
	QBNODE_TYPE_ULONG_ARRAY,
	QBNODE_TYPE_SLONG_ARRAY,
	QBNODE_TYPE_FLOAT_ARRAY,
	QBNODE_TYPE_DOUBLE_ARRAY,

	QBNODE_TYPE_STRING,
	
	_QBNODE_TYPE_END
};

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

typedef int BSErr;
typedef void BSMem;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*BSMemDeallocator) (BSMem* ptr);

#ifdef __cplusplus
};
#endif

typedef struct _QBNodePool {
	struct _BSMemHeader* first;
	struct _BSMemHeader* last;
} QBNodePool;

typedef struct _BSMemHeader {
	struct _QBNodePool* pool;
	struct _BSMemHeader* next;
	struct _BSMemHeader* prev;
	int retain;
	unsigned long size;
	BSMemDeallocator deallocator;
} BSMemHeader;

typedef struct _QBNodeData {
	int type;
	union {
		signed long s;
		unsigned long u;
		double d;
		float f;
		BSMem* ptr;
	} d;
} QBNodeData;

typedef struct _BSAttr {
	char* name;
	char* value;
} BSAttr;

typedef struct _QBNode {
	union {
		char key[12];
		struct {
			char pad;
			char* key;
		} b;
	} key;
	struct _QBNode** childArray;
	struct _BSAttr* attributeArray;
	struct _QBNodeData value;
} QBNode;

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

QBNodePool* QBNodePoolAlloc();
void QBNodePoolFree(QBNodePool* pool);

void QBNodePoolRecycle(QBNodePool* pool);
void QBNodePoolDealloc(QBNodePool* pool);
void QBNodePoolDescription(QBNodePool* pool);

BSMem* BSCalloc(QBNodePool* pool,unsigned long size,const char* comment);
BSMem* BSMalloc(QBNodePool* pool,unsigned long size,const char* comment);

#define BSSize(x) bsSize((BSMem*)(x))
unsigned long bsSize(BSMem* ptr);

#define BSComment(x) bsComment((BSMem*)(x))
char* bsComment(BSMem* ptr);

#define BSFree(x) bsFree((BSMem*)(x))
void bsFree(BSMem* ptr);

#define BSSetDeallocator(x,y) bsSetDeallocator((BSMem*)(x),y)
BSMem* bsSetDeallocator(BSMem* ptr,BSMemDeallocator deallocator);

#define BSRetainCount(x) bsRetainCount((BSMem*)(x))
int bsRetainCount(BSMem* ptr);

#define BSRetain(x) bsRetain((BSMem*)(x))
BSMem* bsRetain(BSMem* ptr);

#define BSRelease(x) bsRelease((BSMem*)(x))
BSMem* bsRelease(BSMem* ptr);

#define QBNodePoolPtr(x) bsPoolPtr((BSMem*)(x))
QBNodePool* bsPoolPtr(BSMem* ptr);

QBNode* QBNodeAlloc(QBNodePool* pool,const char* key);

BSErr QBNodeSetAttribute(QBNode* node,char* name,char* value);
char* QBNodeAttribute(QBNode* node,char* name);

BSErr QBNodeAppend(QBNode* node1,QBNode* node2);
BSErr QBNodeRemove(QBNode* node1,QBNode* node2);

unsigned long QBNodeChildNum(QBNode* node);
QBNode* QBNodeChild(QBNode* node,unsigned long index);

char* QBNodeKey(QBNode* node);
BSErr QBNodeSetKey(QBNode* node,const char* key);

#ifdef __cplusplus
};
#endif

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
