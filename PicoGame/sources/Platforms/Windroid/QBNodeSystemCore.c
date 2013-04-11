/*-----------------------------------------------------------------------------------------------
	名前	QBNodeSystemCore.c
	説明		        
	作成	2006.12.31 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeSystemCore.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

#define QBNODE_FIRST_ARRAY_NUM 1
#define QBNODE_ADD_ARRAY_NUM 2

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

static void* bsLowCalloc(unsigned long size);
static void* bsLowMalloc(unsigned long size);
static void bsLowFree(void* ptr);
static BSMem* bscalloc(QBNodePool* pool,unsigned long size,const char* comment,int c);
static void bsAddPool(QBNodePool* pool,BSMemHeader* header);
static void bsRemovePool(QBNodePool* pool,BSMemHeader* header);

static void bsNodeDealloc(BSMem* node);

static char* bsStrCopy(QBNodePool* pool,char* str,BSErr* err);

/*-----------------------------------------------------------------------------------------------
	局所変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部に公開するプログラム
-----------------------------------------------------------------------------------------------*/

QBNodePool* QBNodePoolAlloc()
{
	QBNodePool* pool = bsLowCalloc(sizeof(QBNodePool));
	//pool->first = NULL;
	//pool->last  = NULL;
	return pool;
}

void QBNodePoolRecycle(QBNodePool* pool)
{
	int flag = 1;
	while (flag) {
		BSMemHeader* h = pool->first;
		flag = 0;
		while (h) {
			BSMemHeader* n = h->next;
			if (h->retain == 0) {
				BSFree((BSMem*)&h[1]);
				flag = 1;
			}
			h = n;
		}
	}
}

void QBNodePoolDealloc(QBNodePool* pool)
{
	BSMemHeader* h = pool->first;
	while (h) {
		BSMemHeader* n = h->next;
		BSFree((BSMem*)&h[1]);
		h = n;
	}
}

void QBNodePoolFree(QBNodePool* pool)
{
	QBNodePoolDealloc(pool);
	pool->first = NULL;
	pool->last = NULL;
	bsLowFree(pool);
}

void QBNodePoolDescription(QBNodePool* pool)
{
	BSMemHeader* h = pool->first;
	int n=1;
//	printf("index comment size retain\n");
	while (h) {
		printf("%d %s %ld %d\n",n,BSComment(&h[1]),BSSize(&h[1]),BSRetainCount(&h[1]));
		h = h->next;
		n ++;
	}
}

#pragma mark -

BSMem* BSCalloc(QBNodePool* pool,unsigned long size,const char* comment)
{
	return bscalloc(pool,size,comment,1);
}

BSMem* BSMalloc(QBNodePool* pool,unsigned long size,const char* comment)
{
	return bscalloc(pool,size,comment,0);
}

unsigned long bsSize(BSMem* ptr)
{
	if (ptr == NULL) return 0;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		return h->size;
	}
}

char* bsComment(BSMem* ptr)
{
	if (ptr == NULL) return "";
	{
		unsigned long size = BSSize(ptr);
		unsigned char* b = (unsigned char*)ptr;
		return (char*)(b+size);
	}
	return "";
}

void bsFree(BSMem* ptr)
{
	if (ptr == NULL) return;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		if (h->deallocator) {
			h->deallocator(ptr);
		}
		bsRemovePool(h->pool,h);
		bsLowFree(h);
	}
}

BSMem* bsSetDeallocator(BSMem* ptr,BSMemDeallocator deallocator)
{
	if (ptr == NULL) return ptr;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		h->deallocator = deallocator;
	}
	return ptr;
}

int bsRetainCount(BSMem* ptr)
{
	if (ptr == NULL) return 0;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		return h->retain;
	}
	return 0;
}

BSMem* bsRetain(BSMem* ptr)
{
	if (ptr == NULL) return ptr;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		h->retain ++;
	}
	return ptr;
}

BSMem* bsRelease(BSMem* ptr)
{
	if (ptr == NULL) return ptr;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		if (h->retain > 0) h->retain --;
	}
	return ptr;
}

QBNodePool* bsPoolPtr(BSMem* ptr)
{
	if (ptr == NULL) return NULL;
	{
		BSMemHeader* h = (BSMemHeader*)ptr;
		h = &h[-1];
		return h->pool;
	}
	return NULL;
}

#pragma mark -

QBNode* QBNodeAlloc(QBNodePool* pool,const char* key)
{
	QBNode* node = (QBNode*)BSCalloc(pool,sizeof(QBNode),key);
	if (node) {
		BSErr err = QBNodeSetKey(node,key);
		if (err != BSERR_OK) {
			BSFree(node);
			node = NULL;
		} else {
			BSSetDeallocator(node,bsNodeDealloc);
		}
	}
	return node;
}

#pragma mark -

BSErr QBNodeSetAttribute(QBNode* node,char* name,char* value)
{
	BSErr err = BSERR_OK;

	if (name == NULL) return BSERR_PARAM;
	
	if (node->attributeArray == NULL) {
		node->attributeArray = (BSAttr*)BSCalloc(QBNodePoolPtr(node),QBNODE_FIRST_ARRAY_NUM*sizeof(BSAttr),"QBNodeSetAttribute");
	}
	
	if (node->attributeArray == NULL) return BSERR_MEM;
	
	{
		int size = BSSize(node->attributeArray)/sizeof(BSAttr);
		//同名の属性を探す//
		{
			int i;
			for (i=0;i<size;i++) {
				if (node->attributeArray[i].name != NULL) {
					if (strcmp(node->attributeArray[i].name,name) == 0) {
						//入れ替え//
						BSRelease(node->attributeArray[i].value);
						if (value == NULL) {
							BSRelease(node->attributeArray[i].name);
							node->attributeArray[i].name = NULL;
							node->attributeArray[i].value = NULL;
						} else {
							node->attributeArray[i].value = bsStrCopy(QBNodePoolPtr(node),value,&err);
						}
						QBNodePoolRecycle(QBNodePoolPtr(node));
						return err;
					}
				}
			}
		}
		//空きバッファを探す//
		{
			int i;
			for (i=0;i<size;i++) {
				if (node->attributeArray[i].name == NULL) {
					node->attributeArray[i].name = bsStrCopy(QBNodePoolPtr(node),name,&err);
					if (err != BSERR_OK) return err;
					node->attributeArray[i].value = bsStrCopy(QBNodePoolPtr(node),value,&err);
					return err;
				}
			}
		}
		{
			BSAttr* attributeArray = (BSAttr*)BSCalloc(QBNodePoolPtr(node),BSSize(node->attributeArray)+QBNODE_FIRST_ARRAY_NUM*sizeof(BSAttr),"QBNodeSetAttribute");
			if (attributeArray == NULL) return BSERR_MEM;
			{
				int i;
				for (i=0;i<size;i++) {
					attributeArray[i] = node->attributeArray[i];
				}
				attributeArray[i].name = bsStrCopy(QBNodePoolPtr(node),name,&err);
				if (err != BSERR_OK) return err;
				attributeArray[i].value = bsStrCopy(QBNodePoolPtr(node),value,&err);
				if (err != BSERR_OK) return err;
				BSRelease(node->attributeArray);
				node->attributeArray = attributeArray;
			}
		}
	}
	
	return BSERR_OK;
}

char* QBNodeAttribute(QBNode* node,char* name)
{
	char* r = NULL;
	if (node) {
		int size = BSSize(node->attributeArray)/sizeof(BSAttr);
		int i;
		for (i=0;i<size;i++) {
			if (node->attributeArray[i].name != NULL) {
				if (strcmp(node->attributeArray[i].name,name) == 0) {
					//入れ替え//
					BSRelease(node->attributeArray[i].value);
					QBNodePoolRecycle(QBNodePoolPtr(node));
					r = node->attributeArray[i].value;
					break;
				}
			}
		}
	}
	return r;
}

#pragma mark -

BSErr QBNodeAppend(QBNode* node1,QBNode* node2)
{
	BSErr err = BSERR_MEM;
	int f = 2;
	if (node1 == NULL || node2 == NULL) return BSERR_PARAM;
	{
		QBNodePool* pool = QBNodePoolPtr(node1);
		
		if (node1->childArray == NULL) {
			node1->childArray = (QBNode**)BSCalloc(pool,QBNODE_FIRST_ARRAY_NUM*sizeof(QBNode*),"QBNodeAppend");
			f = 1;
		}

		// すでに登録済みか？//
		if (node1->childArray != NULL && f == 2) {
			int i;
			for (i=0;i<BSSize(node1->childArray)/sizeof(QBNode*);i++) {
				if (node1->childArray[i] == node2) {
					return BSERR_OK;
				}
			}
		}

		while (f) {
		
			// 登録先を探し登録//
			if (node1->childArray != NULL) {
				int i;
				for (i=0;i<BSSize(node1->childArray)/sizeof(QBNode*);i++) {
					if (node1->childArray[i] == NULL) {
						node1->childArray[i] = (QBNode*)BSRetain(node2);
						err = BSERR_OK;
						f = 1;
						break;
					}
				}
			}
			if (f == 1) break;
			
			// 登録失敗！ バッファを増やす//
			{
				unsigned long size = BSSize(node1->childArray)/sizeof(QBNode*);
				QBNode** newArray = (QBNode**)BSMalloc(pool,(size+QBNODE_ADD_ARRAY_NUM)*sizeof(QBNode*),"QBNodeAppendExtend");
				if (newArray) {
					int i;
					for (i=0;i<BSSize(node1->childArray)/sizeof(QBNode*);i++) {
						newArray[i] = node1->childArray[i];
					}
					newArray[i++] = (QBNode*)BSRetain(node2);
					for (;i<BSSize(newArray)/sizeof(QBNode*);i++) {
						newArray[i] = NULL;
					}
					BSFree(node1->childArray);
					node1->childArray = newArray;
					err = BSERR_OK;
					break;
				} else {
					break;
				}
			}

			f --;
		}
	}
	return err;
}

BSErr QBNodeRemove(QBNode* node1,QBNode* node2)
{
	BSErr err = BSERR_NO_CHILD;
	if (node1 == NULL || node2 == NULL) return BSERR_PARAM;
	{
		int i;
		for (i=0;i<BSSize(node1->childArray)/sizeof(QBNode*);i++) {
			if (node1->childArray[i] == node2) {
				node1->childArray[i] = NULL;
				
				{
					int j;
					for (j=i;j<BSSize(node1->childArray)/sizeof(QBNode*)-1;j++) {
						node1->childArray[j] = node1->childArray[j+1];
					}
				}
				
				BSRelease(node2);
				err = BSERR_OK;
				break;
			}
		}
	}
	return err;
}

unsigned long QBNodeChildNum(QBNode* node)
{
	unsigned long num = 0;
	if (node != NULL) {
		int i;
		for (i=0;i<BSSize(node->childArray)/sizeof(QBNode*);i++) {
			if (node->childArray[i]) {
				num ++;
			}
		}
	}
	return num;
}

QBNode* QBNodeChild(QBNode* node,unsigned long index)
{
	if (node == NULL) return NULL;
	{
		int i;
		int idx = 0;
		for (i=0;i<BSSize(node->childArray)/sizeof(QBNode*);i++) {
			if (node->childArray[i]) {
				if (index == idx) {
					return node->childArray[i];
				}
				idx ++;
			}
		}
	}
	return NULL;
}

#pragma mark -

char* QBNodeKey(QBNode* node)
{
	if (node != NULL) {
		if (node->key.b.pad == 0) {
			if (node->key.b.key == NULL) return "";
			return node->key.b.key;
		} else {
			return node->key.key;
		}
	}
	return "";
}

BSErr QBNodeSetKey(QBNode* node,const char* key)
{
	if (node != NULL && key != NULL) {
		if (node->key.b.pad == 0) {
			if (node->key.b.key) {
				BSFree(node->key.b.key);
				node->key.b.key = NULL;
			}
		}
		{
			int len = 0;
			for (len=0;key[len]!=0&&key[len]!='.';len++) ;
			if (len+1 <= 12) {
				{
					int i;
					for (i=0;i<len;i++) {
						node->key.key[i] = key[i];
					}
					node->key.key[i] = 0;
				}
			} else {
				node->key.b.pad = 0;
				node->key.b.key = (char*)BSMalloc(QBNodePoolPtr(node),len+1,key);
				if (node->key.b.key) {
					int i;
					for (i=0;i<len;i++) {
						node->key.b.key[i] = key[i];
					}
					node->key.b.key[i] = 0;
				} else {
					return BSERR_MEM;
				}
			}
		}
		return BSERR_OK;
	}
	return BSERR_PARAM;
}

/*-----------------------------------------------------------------------------------------------
	外部に公開しないプログラム
-----------------------------------------------------------------------------------------------*/

static void* bsLowCalloc(unsigned long size)
{
	return calloc(1,size);
}

static void* bsLowMalloc(unsigned long size)
{
	return malloc(size);
}

static void bsLowFree(void* ptr)
{
	free(ptr);
}

#pragma mark -

static void bsAddPool(QBNodePool* pool,BSMemHeader* header)
{
	if (pool->first == NULL) pool->first = header;
	header->next = NULL;
	header->prev = pool->last;
	if (pool->last  != NULL) pool->last->next = header;
	pool->last = header;
}

static void bsRemovePool(QBNodePool* pool,BSMemHeader* header)
{
	if (header->prev != NULL) header->prev->next = header->next;
	if (header->next != NULL) header->next->prev = header->prev;
	if (pool->first == header) pool->first = header->next;
	if (pool->last  == header) pool->last  = header->prev;
	header->next = NULL;
	header->prev = NULL;
}

#pragma mark -

static char* bsStrCopy(QBNodePool* pool,char* str,BSErr* err)
{
	if (str == NULL) return NULL;
	{
		char* mem = BSMalloc(pool,strlen(str)+1,"bsStrCopy");
		*err = BSERR_MEM;
		if (mem) {
			memcpy(mem,str,strlen(str)+1);
			*err = BSERR_OK;
		}
		return mem;
	}
}

static void bsNodeDealloc(BSMem* ptr)
{
	QBNode* node = (QBNode*)ptr;
	if (node != NULL) {
		int i;
		for (i=0;i<BSSize(node->childArray)/sizeof(QBNode*);i++) {
			if (node->childArray[i]) {
				BSRelease(node->childArray[i]);
			}
		}
		if (node->attributeArray) {
			int i;
			int size = BSSize(node->attributeArray)/sizeof(BSAttr);
			for (i=0;i<size;i++) {
				if (node->attributeArray[i].name) {
					BSRelease(node->attributeArray[i].name);
				}
				if (node->attributeArray[i].value) {
					BSRelease(node->attributeArray[i].value);
				}
			}
			BSRelease(node->attributeArray);
			node->attributeArray = NULL;
		}
		if (node->childArray) {
			BSRelease(node->childArray);
			node->childArray = NULL;
		}
		if (node->key.b.pad == 0) {
			if (node->key.b.key) BSRelease(node->key.b.key);
			node->key.b.key = NULL;
		}
		if (node->value.type >= _QBNODE_TYPE_BSMEM) {
			BSRelease(node->value.d.ptr);
			node->value.d.ptr = NULL;
		}
	}
}

#pragma mark -

static BSMem* bscalloc(QBNodePool* pool,unsigned long size,const char* comment,int c)
{
	if (pool == NULL) return NULL;
	{
		BSMemHeader* r = NULL;
		unsigned long commentLen = 4;
		unsigned char* p = NULL;
		unsigned long bsize = size+sizeof(BSMemHeader);
#if 1
		comment = NULL;
#endif
		if (comment) {
			commentLen = strlen(comment)+1;
			commentLen = (commentLen+3)/4*4;
		}
		bsize += commentLen;
		if (c) {
			p = bsLowCalloc(bsize);
		} else {
			p = bsLowMalloc(bsize);
		}
		if (p) {
			r = (BSMemHeader*)p;
			r->pool = pool;
			//r->next = NULL;
			//r->prev = NULL;
			r->size = size;
			r->retain = 1;
			r->deallocator = NULL;
			if (comment) {
				strcpy((char*)(p+sizeof(BSMemHeader)+size),comment);
			} else {
				strcpy((char*)(p+sizeof(BSMemHeader)+size),"");
			}
			bsAddPool(pool,r);
			return (BSMem*)&r[1];
		}
	}
	return NULL;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
