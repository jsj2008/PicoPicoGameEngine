/*-----------------------------------------------------------------------------------------------
	名前	QBNodeSystemValue.c
	説明		        
	作成	2007.01.02 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeSystemValue.h"
#include "QBNodeSystemUtil.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

#define BS_LOG_SPACE "  "

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

static void bsNodeDescription(QBNode* node,int level);

/*-----------------------------------------------------------------------------------------------
	局所変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部に公開するプログラム
-----------------------------------------------------------------------------------------------*/

int QBNodeValueType(QBNode* node)
{
	if (node == NULL) return QBNODE_TYPE_NONE;
	return node->value.type;
}

#pragma mark -

BSErr QBNodeSetStringNoCopy(QBNode* node,char* string)
{
	if (string == NULL) return BSERR_PARAM;
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_CONST_STRING;
	node->value.d.ptr = string;
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetString(QBNode* node,char* string)
{
	if (string == NULL) return BSERR_PARAM;
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_STRING;
	node->value.d.ptr = BSMemCopy(QBNodePoolPtr(node),string,strlen(string)+1,string);
	QBNodePoolRecycle(QBNodePoolPtr(node));
	if (node->value.d.ptr == NULL) return BSERR_MEM;
	return BSERR_OK;
}

BSErr QBNodeSetBytes(QBNode* node,BSMem* bytes)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_BYTE_ARRAY;
	node->value.d.ptr = BSRetain(bytes);
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetULong(QBNode* node,unsigned long value)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_ULONG;
	node->value.d.u = value;
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetSLong(QBNode* node,signed long value)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_SLONG;
	node->value.d.s = value;
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetFloat(QBNode* node,float value)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_FLOAT;
	node->value.d.f = value;
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetDouble(QBNode* node,double value)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_DOUBLE;
	node->value.d.d = value;
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetULongArray(QBNode* node,BSMem* array)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_ULONG_ARRAY;
	node->value.d.ptr = BSRetain(array);
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetSLongArray(QBNode* node,BSMem* array)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_SLONG_ARRAY;
	node->value.d.ptr = BSRetain(array);
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetFloatArray(QBNode* node,BSMem* array)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_FLOAT_ARRAY;
	node->value.d.ptr = BSRetain(array);
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

BSErr QBNodeSetDoubleArray(QBNode* node,BSMem* array)
{
	if (node->value.d.ptr && node->value.type >= _QBNODE_TYPE_BSMEM) BSRelease(node->value.d.ptr);
	node->value.type = QBNODE_TYPE_DOUBLE_ARRAY;
	node->value.d.ptr = BSRetain(array);
	QBNodePoolRecycle(QBNodePoolPtr(node));
	return BSERR_OK;
}

#pragma mark -

char* QBNodeString(QBNode* node)
{
	if (node == NULL) return NULL;
	if (node->value.type != QBNODE_TYPE_CONST_STRING 
	 && node->value.type != QBNODE_TYPE_STRING ) return NULL;
	return node->value.d.ptr;
}

BSMem* QBNodeBytes(QBNode* node)
{
	if (node == NULL) return NULL;
	if (node->value.type != QBNODE_TYPE_BYTE_ARRAY) return NULL;
	return node->value.d.ptr;
}

unsigned long QBNodeULong(QBNode* node)
{
	if (node == NULL) return 0;
	return node->value.d.u;
}

signed long QBNodeSLong(QBNode* node)
{
	if (node == NULL) return 0;
	return node->value.d.s;
}

float QBNodeFloat(QBNode* node)
{
	if (node == NULL) return 0;
	return node->value.d.f;
}

double QBNodeDouble(QBNode* node)
{
	if (node == NULL) return 0;
	return node->value.d.d;
}

unsigned long* QBNodeULongArray(QBNode* node)
{
	if (node == NULL) return NULL;
	if (node->value.type != QBNODE_TYPE_ULONG_ARRAY) return NULL;
	return (unsigned long*)node->value.d.ptr;
}

signed long* QBNodeSLongArray(QBNode* node)
{
	if (node == NULL) return NULL;
	if (node->value.type != QBNODE_TYPE_SLONG_ARRAY) return NULL;
	return (signed long*)node->value.d.ptr;
}

float* QBNodeFloatArray(QBNode* node)
{
	if (node == NULL) return NULL;
	if (node->value.type != QBNODE_TYPE_FLOAT_ARRAY) return NULL;
	return (float*)node->value.d.ptr;
}

double* QBNodeDoubleArray(QBNode* node)
{
	if (node == NULL) return NULL;
	if (node->value.type != QBNODE_TYPE_DOUBLE_ARRAY) return NULL;
	return (double*)node->value.d.ptr;
}

#pragma mark -

BSErr QBNodeSetStringNoCopyWithKeyPath(QBNode* node,char* string,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && string != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetStringNoCopy(n,string);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetStringWithKeyPath(QBNode* node,char* string,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && string != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetString(n,string);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetBytesWithKeyPath(QBNode* node,BSMem* bytes,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && bytes != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetBytes(n,bytes);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetULongWithKeyPath(QBNode* node,unsigned long value,const char* keyPath)
{
	if (node != NULL && keyPath != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetULong(n,value);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetSLongWithKeyPath(QBNode* node,signed long value,const char* keyPath)
{
	if (node != NULL && keyPath != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetSLong(n,value);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetFloatWithKeyPath(QBNode* node,float value,const char* keyPath)
{
	if (node != NULL && keyPath != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetFloat(n,value);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetDoubleWithKeyPath(QBNode* node,double value,const char* keyPath)
{
	if (node != NULL && keyPath != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetDouble(n,value);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetULongArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && array != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetULongArray(n,array);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetSLongArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && array != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetSLongArray(n,array);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetFloatArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && array != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetFloatArray(n,array);
		return err;
	}
	return BSERR_PARAM;
}

BSErr QBNodeSetDoubleArrayWithKeyPath(QBNode* node,BSMem* array,const char* keyPath)
{
	if (node != NULL && keyPath != NULL && array != NULL) {
		BSErr err = BSERR_MEM;
		QBNode* n = QBNodeCreateWithKeyPath(node,keyPath);
		if (n) return QBNodeSetDoubleArray(n,array);
		return err;
	}
	return BSERR_PARAM;
}

#pragma mark -

char* QBNodeStringWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeString(QBNodeWithKeyPath(node,keyPath));
}

BSMem* QBNodeBytesWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeBytes(QBNodeWithKeyPath(node,keyPath));
}

unsigned long QBNodeULongWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeULong(QBNodeWithKeyPath(node,keyPath));
}

signed long QBNodeSLongWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeSLong(QBNodeWithKeyPath(node,keyPath));
}

float QBNodeFloatWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeFloat(QBNodeWithKeyPath(node,keyPath));
}

double QBNodeDoubleWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeDouble(QBNodeWithKeyPath(node,keyPath));
}

unsigned long* QBNodeULongArrayWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeULongArray(QBNodeWithKeyPath(node,keyPath));
}

signed long* QBNodeSLongArrayWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeSLongArray(QBNodeWithKeyPath(node,keyPath));
}

float* QBNodeFloatArrayWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeFloatArray(QBNodeWithKeyPath(node,keyPath));
}

double* QBNodeDoubleArrayWithKeyPath(QBNode* node,const char* keyPath)
{
	return QBNodeDoubleArray(QBNodeWithKeyPath(node,keyPath));
}

#pragma mark -

unsigned long QBNodeArrayCount(QBNode* node)
{
	unsigned long count = 0;
	switch (node->value.type) {
	case QBNODE_TYPE_BYTE_ARRAY:
		count = BSSize(node->value.d.ptr)/sizeof(unsigned char);
		break;
	case QBNODE_TYPE_ULONG_ARRAY:
		count = BSSize(node->value.d.ptr)/sizeof(unsigned long);
		break;
	case QBNODE_TYPE_SLONG_ARRAY:
		count = BSSize(node->value.d.ptr)/sizeof(signed long);
		break;
	case QBNODE_TYPE_FLOAT_ARRAY:
		count = BSSize(node->value.d.ptr)/sizeof(float);
		break;
	case QBNODE_TYPE_DOUBLE_ARRAY:
		count = BSSize(node->value.d.ptr)/sizeof(double);
		break;
	}
	return count;
}

#pragma mark -

void QBNodeDescription(QBNode* node)
{
	bsNodeDescription(node,0);
}

/*-----------------------------------------------------------------------------------------------
	外部に公開しないプログラム
-----------------------------------------------------------------------------------------------*/

static void bsNodeValueDescription(QBNode* node,int level)
{
	switch (node->value.type) {
	case QBNODE_TYPE_ULONG:
		printf("%lu",node->value.d.u);
		break;
	case QBNODE_TYPE_SLONG:
		printf("%ld",node->value.d.s);
		break;
	case QBNODE_TYPE_DOUBLE:
		printf("%lf",node->value.d.d);
		break;
	case QBNODE_TYPE_FLOAT:
		printf("%f",node->value.d.f);
		break;
	case QBNODE_TYPE_ULONG_ARRAY:
		if (node->value.d.ptr) {
			int i,j;
			unsigned long* ptr = QBNodeULongArray(node);
			if (ptr) {
				printf("\n");
				for (i=0;i<QBNodeArrayCount(node);i++) {
					if (i>0) printf("\n");
					for (j=0;j<level+1;j++) printf(BS_LOG_SPACE);
					printf(BS_LOG_SPACE);
					printf("%lu",ptr[i]);
				}
			}
		}
		break;
	case QBNODE_TYPE_SLONG_ARRAY:
		if (node->value.d.ptr) {
			int i,j;
			signed long* ptr = QBNodeSLongArray(node);
			if (ptr) {
				printf("\n");
				for (i=0;i<QBNodeArrayCount(node);i++) {
					if (i>0) printf("\n");
					for (j=0;j<level+1;j++) printf(BS_LOG_SPACE);
					printf(BS_LOG_SPACE);
					printf("%ld",ptr[i]);
				}
			}
		}
		break;
	case QBNODE_TYPE_FLOAT_ARRAY:
		if (node->value.d.ptr) {
			int i,j;
			float* ptr = QBNodeFloatArray(node);
			if (ptr) {
				printf("\n");
				for (i=0;i<QBNodeArrayCount(node);i++) {
					if (i>0) printf("\n");
					for (j=0;j<level+1;j++) printf(BS_LOG_SPACE);
					printf(BS_LOG_SPACE);
					printf("%f",ptr[i]);
				}
			}
		}
		break;
	case QBNODE_TYPE_DOUBLE_ARRAY:
		if (node->value.d.ptr) {
			int i,j;
			double* ptr = QBNodeDoubleArray(node);
			if (ptr) {
				printf("\n");
				for (i=0;i<QBNodeArrayCount(node);i++) {
					if (i>0) printf("\n");
					for (j=0;j<level+1;j++) printf(BS_LOG_SPACE);
					printf(BS_LOG_SPACE);
					printf("%lf",ptr[i]);
				}
			}
		}
		break;
	case QBNODE_TYPE_BYTE_ARRAY:
		if (node->value.d.ptr) {
			int i,j;
			unsigned char* ptr = (unsigned char*)node->value.d.ptr;
			printf("\n");
			for (i=0;i<BSSize(node->value.d.ptr);i++) {
				if ((i % 16) == 0) {
					if (i > 0) printf("\n");
					for (j=0;j<level+1;j++) printf(BS_LOG_SPACE);
				} else {
					printf(" ");
				}
				printf("%02X",ptr[i]);
			}
		}
		break;
	case QBNODE_TYPE_STRING:
	case QBNODE_TYPE_CONST_STRING:
		printf("\"%s\"",(char*)node->value.d.ptr);
		break;
	default:
		printf("NONE");
		break;
	}
}

static void bsNodeDescription(QBNode* node,int level)
{
	if (node != NULL) {
		int i;
		for (i=0;i<level;i++) printf(BS_LOG_SPACE);
		printf("%s",QBNodeKey(node));
		{
			int j;
			for (j=0;j<BSSize(node->attributeArray)/sizeof(BSAttr);j++) {
				if (node->attributeArray[j].name) {
					char* value = "";
					if (node->attributeArray[j].value) value = node->attributeArray[j].value;
					printf(" %s=\"%s\"",node->attributeArray[j].name,value);
				}
			}
		}
		//if (node->childArray) printf(" %d",BSSize(node->childArray)); else printf(" 0");
		printf(" ");
		bsNodeValueDescription(node,level);
		printf("\n");
		{
			int i;
			for (i=0;i<BSSize(node->childArray)/sizeof(QBNode*);i++) {
				if (node->childArray[i]) {
					bsNodeDescription(node->childArray[i],level+1);
				}
			}
		}
	} else {
		printf("node is NULL\n");
	}
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
