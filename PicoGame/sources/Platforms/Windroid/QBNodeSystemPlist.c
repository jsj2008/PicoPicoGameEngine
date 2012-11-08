/*-----------------------------------------------------------------------------------------------
	ファイル名 QBNodeSystemPlist.c
-----------------------------------------------------------------------------------------------*/
/**
	@file   QBNodeSystemPlist.c
	@brief  MacOSX の plist XML を QBNode に変換する。
	@author H.Yamaguchi
*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "QBNodeSystemPlist.h"
#include "QBNodeBase64.h"

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

#define __PLISTPARSER_SPECIAL__
#define ADD_BUFFER_SIZE 10
#define MAL_BUFFER_SIZE 100000

#define T_ARRAY "array"
#define T_DICT "dict"
#define T_KEY "key"
#define T_STRING "string"
#define T_DATA "data"
#define T_DATE "date"
#define T_TRUE "true"
#define T_FALSE "false"
#define T_REAL "real"
#define T_INTEGER "integer"

enum {
	TYPE_ARRAY,
	TYPE_DICT,
	TYPE_KEY,
	TYPE_STRING,
	TYPE_DATA,
	TYPE_DATE,
	TYPE_TRUE,
	TYPE_FALSE,
	TYPE_REAL,
	TYPE_INTEGER,
	TYPE_VALUE_STRING,
	TYPE_OTHER
};

enum {
	STR_FLOAT,
	STR_INTEGER,
	STR_STRING
};

//#include "QBNodeSystemPlistDef.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

typedef struct _PLIST_NODE {
	int n;
	int step;
	QBNodePool* pool;
	char* plistString;
	int err;
} PLIST_NODE;

typedef struct _PLIST_OUT_BUFFER {
	char*		buffer;
	int			size;
	int			cur;
	QBNodePool*		pool;
} PLIST_OUT_BUFFER;

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

static void doParseFirst(PLIST_NODE* p,QBNode* node);
static void doParseArray(PLIST_NODE* p,QBNode* node);
static void doParseDict(PLIST_NODE* p,QBNode* node);

static int makePlistStr(PLIST_OUT_BUFFER* str,int level,QBNode* node,int type);

/*-----------------------------------------------------------------------------------------------
	局所変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部に公開しない関数
-----------------------------------------------------------------------------------------------*/

static int checkStringType(char* str)
{
	int i=0;
	int type = STR_INTEGER;
	if (str[i] == '-' || str[i] == '+') {
		i++;
	}
	for (;str[i]!=0;i++) {
		if (str[i] >= '0' && str[i] <= '9') {
		} else
		if (str[i] == '.') {
			if (type == STR_FLOAT) return STR_STRING;
			type = STR_FLOAT;
		} else {
			return STR_STRING;
		}
	}
	return type;
}

static int parseString(PLIST_NODE* p,char** word,int* type,int* close)
{
	QBNodePool* pool = p->pool;
	char* str = p->plistString;
	int i;
	int p2=0;
	
	*close = 0;
	*word = NULL;
	
	if (p->err != 0) return -1;
	if (str[p->n] == 0) return -1;
	
	for (i=p->n;str[i]!=0;i++) {
		switch (p->step) {
		case 0:
			if (str[i] == '<') {
				p->step = 1;
				p2=i;
				
				{
					int i;
					int n=0;
					char *ptr;
					ptr = (char*)BSCalloc(pool,p2-p->n+1,"parseString:1");
					if (ptr == NULL) {
						return -1;
					}
					for (i=p->n;i<p2;i++) {
						ptr[n] = str[i];
						n++;
					}
					*word = ptr;
					*type = TYPE_VALUE_STRING;
				}
				
				p->n=i+1;
				
				return p->n;
			}
			break;
		case 1:
			if (str[i] == '>') {
				p->step = 0;
				p2=i;
				
				if (str[p->n] == '/') {
					p->n++;
					*close = 1;
				}
				
				if (str[p2-1] == '/') {
					*close = 1;
				}

				if (strncmp(&str[p->n],T_ARRAY,strlen(T_ARRAY)) == 0) {
					*type = TYPE_ARRAY;
				} else
				if (strncmp(&str[p->n],T_DICT,strlen(T_DICT)) == 0) {
					*type = TYPE_DICT;
				} else
				if (strncmp(&str[p->n],T_KEY,strlen(T_KEY)) == 0) {
					*type = TYPE_KEY;
				} else
				if (strncmp(&str[p->n],T_STRING,strlen(T_STRING)) == 0) {
					*type = TYPE_STRING;
				} else
				if (strncmp(&str[p->n],T_DATA,strlen(T_DATA)) == 0) {
					*type = TYPE_DATA;
				} else
				if (strncmp(&str[p->n],T_DATE,strlen(T_DATE)) == 0) {
					*type = TYPE_DATE;
				} else
				if (strncmp(&str[p->n],T_TRUE,strlen(T_TRUE)) == 0) {
					*type = TYPE_TRUE;
				} else
				if (strncmp(&str[p->n],T_FALSE,strlen(T_FALSE)) == 0) {
					*type = TYPE_FALSE;
				} else
				if (strncmp(&str[p->n],T_REAL,strlen(T_REAL)) == 0) {
					*type = TYPE_REAL;
				} else
				if (strncmp(&str[p->n],T_INTEGER,strlen(T_INTEGER)) == 0) {
					*type = TYPE_INTEGER;
				} else {
					*type = TYPE_OTHER;
				}
				
				p->n=i+1;

				return p->n;
			}
			break;
		}
		
	}
	return -1;
}

static void doParseFirst(PLIST_NODE* p,QBNode* root)
{
	int type;
	int close=0;
	char* word = NULL;
	while (parseString(p,&word,&type,&close) >= 0) {
		switch (type) {
		case TYPE_ARRAY:
			if (!close) doParseArray(p,root);
			break;
		case TYPE_DICT:
			if (!close) doParseDict(p,root);
			break;
		default:
			break;
		}
		QBNodePoolRecycle(p->pool);
	}
}

static void doParseArray(PLIST_NODE* p,QBNode* root)
{
	int type;
	int close=0;
	char* word = NULL;
#ifdef __PLISTPARSER_SPECIAL__
	double* doubleArray = NULL;
	//signed long* integerArray = NULL;
	unsigned long arraySize = 0;
	unsigned long arrayCount = 0;
	int doubleFlag = 0;
#endif

	int array = 0;
	while (parseString(p,&word,&type,&close) >= 0) {
		char str[64];
		sprintf(str,".%d",array);
		switch (type) {
		case TYPE_ARRAY:
			if (close) {
#ifdef __PLISTPARSER_SPECIAL__
#if 1
				if (doubleArray) {
					if (doubleFlag) {
						double* ptr = (double*)BSCallocDoubleArray(p->pool,arrayCount,"");
						int i;
						for (i=0;i<arrayCount;i++) {
							ptr[i] = doubleArray[i];
						}
						QBNodeSetDoubleArray(root,ptr);
					} else {
						signed long* ptr = (signed long*)BSCallocSLongArray(p->pool,arrayCount,"");
						int i;
						for (i=0;i<arrayCount;i++) {
							ptr[i] = (signed long)doubleArray[i];
						}
						QBNodeSetSLongArray(root,ptr);
					}
					BSFree(doubleArray);
				}
#else
				if (integerArray) {
					{
						signed long* ptr = (signed long*)BSCallocSLongArray(p->pool,arrayCount,"");
						int i;
						for (i=0;i<arrayCount;i++) {
							ptr[i] = integerArray[i];
						}
						QBNodeSetSLongArray(root,(BSMem*)ptr);
					}
					BSFree(integerArray);
				}
				if (doubleArray) {
					{
						double* ptr = (double*)BSCallocDoubleArray(p->pool,arrayCount,"");
						int i;
						for (i=0;i<arrayCount;i++) {
							ptr[i] = doubleArray[i];
						}
						QBNodeSetDoubleArray(root,(BSMem*)ptr);
					}
					BSFree(doubleArray);
				}
#endif
#endif
				return;
			} else {
				QBNode* node = QBNodeCreateWithKeyPath(root,str);
				doParseArray(p,node);
				array ++;
			}
			break;
		case TYPE_DICT:
			if (close) {
				p->err = -1;
				return;
			} else {
				QBNode* node = QBNodeCreateWithKeyPath(root,str);
				doParseDict(p,node);
				array ++;
			}
			break;
		case TYPE_KEY:
			p->err = -1;
			return;
		case TYPE_REAL:
			doubleFlag = 1;
		case TYPE_STRING:
		case TYPE_DATA:
		case TYPE_DATE:
		case TYPE_TRUE:
		case TYPE_FALSE:
		case TYPE_INTEGER:
			{
#ifndef __PLISTPARSER_SPECIAL__
				QBNode* node = QBNodeCreateWithKeyPath(root,str);
#endif
				if (parseString(p,&word,&type,&close) < 0) return;
				if (type != TYPE_VALUE_STRING) {
					p->err = -1;
					return;
				}
#ifdef __PLISTPARSER_SPECIAL__
				if (doubleArray == NULL) {
					doubleArray = BSCallocDoubleArray(p->pool,ADD_BUFFER_SIZE,"");
					BSRetain(doubleArray);
					arraySize = ADD_BUFFER_SIZE;
				} else
				if (arrayCount >= arraySize) {
					double* newArray = BSCallocDoubleArray(p->pool,arraySize+ADD_BUFFER_SIZE,"");
					int i;
					BSRetain(newArray);
					for (i=0;i<arrayCount;i++) {
						newArray[i] = doubleArray[i];
					}
					BSFree(doubleArray);
					doubleArray = newArray;
					arraySize += ADD_BUFFER_SIZE;
				}
				doubleArray[arrayCount] = atof(word);
				arrayCount ++;
#else
				QBNodeSetString(node,word);
#endif
				if (parseString(p,&word,&type,&close) < 0) return;
			}
			array ++;
			break;
#if 0
		case TYPE_STRING:
		case TYPE_DATA:
		case TYPE_DATE:
		case TYPE_TRUE:
		case TYPE_FALSE:
		case TYPE_INTEGER:
			{

#ifndef __PLISTPARSER_SPECIAL__
				QBNode* node = QBNodeCreateWithKeyPath(root,str);
#endif
				if (parseString(p,&word,&type,&close) < 0) return;
				if (type != TYPE_VALUE_STRING) {
					p->err = -1;
					return;
				}
#ifdef __PLISTPARSER_SPECIAL__
				if (integerArray == NULL) {
					integerArray = BSCallocSLongArray(p->pool,ADD_BUFFER_SIZE,"");
					BSRetain(integerArray);
					arraySize = ADD_BUFFER_SIZE;
				} else
				if (arrayCount >= arraySize) {
					signed long* newArray = BSCallocSLongArray(p->pool,arraySize+ADD_BUFFER_SIZE,"");
					int i;
					BSRetain(newArray);
					for (i=0;i<arrayCount;i++) {
						newArray[i] = integerArray[i];
					}
					BSFree(integerArray);
					integerArray = newArray;
					arraySize += ADD_BUFFER_SIZE;
				}
				integerArray[arrayCount] = atoi(word);
				arrayCount ++;
#else
				QBNodeSetString(node,word);
#endif
				if (parseString(p,&word,&type,&close) < 0) return;
			}
			array ++;
			break;
#endif
		case TYPE_VALUE_STRING:
			break;
		default:
			p->err = -1;
			break;
		}

		QBNodePoolRecycle(p->pool);
	}
	p->err = -1;
}

static void doParseDict(PLIST_NODE* p,QBNode* root)
{
	int type;
	int close=0;
	char* word = NULL;

	while (parseString(p,&word,&type,&close) >= 0) {
		switch (type) {
		case TYPE_DICT:
			if (close) {
				return;
			}
			break;
		case TYPE_KEY:
			{
				QBNode * node = NULL;

				//キーの取り出し//
				{
					if (parseString(p,&word,&type,&close) < 0) return;
					if (type != TYPE_VALUE_STRING) return;
					
					//ノードの作成//
					{
						BSMem* str = BSStr(p->pool,".");
						str = BSStrAppend(str,word);
						if (str) {
							node = QBNodeCreateWithKeyPath(root,(char*)str);
						}
						QBNodePoolRecycle(p->pool);
					}

					if (parseString(p,&word,&type,&close) < 0) return;
					if (type != TYPE_KEY) return;
					if (close == 0) return;
				}

				//オブジェクトの取り出し//
				{
					if (parseString(p,&word,&type,&close) < 0) return;
					if (type != TYPE_VALUE_STRING) {
						p->err = -1;
						return;
					}
					if (parseString(p,&word,&type,&close) < 0) return;
					switch (type) {
					case TYPE_ARRAY:
						if (!close) doParseArray(p,node);
						break;
					case TYPE_DICT:
						if (!close) doParseDict(p,node);
						break;
					case TYPE_KEY:
						p->err = -1;
						return;
					case TYPE_DATA:
						{
							if (parseString(p,&word,&type,&close) < 0) return;
							if (type != TYPE_VALUE_STRING) {
								p->err = -1;
								return;
							}
							// word(base64 string) -> bytes
							{
								unsigned char* bytes = BSRelease(base64decode(QBNodePoolPtr(node),word,0));
								QBNodeSetBytes(node,bytes);
							}
							if (parseString(p,&word,&type,&close) < 0) return;
						}
						break;
					case TYPE_STRING:
						{
							if (parseString(p,&word,&type,&close) < 0) return;
							if (type != TYPE_VALUE_STRING) {
								p->err = -1;
								return;
							}
							switch (checkStringType(word)) {
							case STR_FLOAT:
								QBNodeSetDouble(node,atof(word));
								break;
							case STR_INTEGER:
								QBNodeSetSLong(node,atoi(word));
								break;
							default:
								QBNodeSetString(node,word);
								break;
							}
							if (parseString(p,&word,&type,&close) < 0) return;
						}
						break;
					case TYPE_DATE:
						{
							if (parseString(p,&word,&type,&close) < 0) return;
							if (type != TYPE_VALUE_STRING) {
								p->err = -1;
								return;
							}
							QBNodeSetString(node,word);
							if (parseString(p,&word,&type,&close) < 0) return;
						}
						break;
					case TYPE_TRUE:
						if (close) {
							QBNodeSetULong(node,1);
						} else {
							p->err = -1;
							return;
						}
						break;
					case TYPE_FALSE:
						if (close) {
							QBNodeSetULong(node,0);
						} else {
							p->err = -1;
							return;
						}
						break;
					case TYPE_REAL:
						{
							if (parseString(p,&word,&type,&close) < 0) return;
							if (type != TYPE_VALUE_STRING) {
								p->err = -1;
								return;
							}
							QBNodeSetDouble(node,atof(word));
							if (parseString(p,&word,&type,&close) < 0) return;
						}
						break;
					case TYPE_INTEGER:
						{
							if (parseString(p,&word,&type,&close) < 0) return;
							if (type != TYPE_VALUE_STRING) {
								p->err = -1;
								return;
							}
							QBNodeSetSLong(node,atoi(word));
							if (parseString(p,&word,&type,&close) < 0) return;
						}
						break;
					default:
						p->err = -1;
						return;
					}
				}
			}
			break;
		case TYPE_VALUE_STRING:
			break;
		default:
			p->err = -1;
			return;
		}

		QBNodePoolRecycle(p->pool);
	}
	p->err = -1;
}

static int initAppendStr(PLIST_OUT_BUFFER* outbuff,QBNodePool* pool)
{
	memset(outbuff,0,sizeof(PLIST_OUT_BUFFER));
	outbuff->pool = pool;
	return 0;
}

static int doAppend(PLIST_OUT_BUFFER* outbuff,char* format,...)
{
	int done = 0;
	
	if (format == NULL) {
		return doAppend(outbuff,"NULL");
	}
	
	if (format[0] == 0) return 0;
	
	if (outbuff->buffer == NULL) {
		outbuff->buffer = (char*)BSCalloc(outbuff->pool,MAL_BUFFER_SIZE,"appendStr");
		if (outbuff->buffer == NULL) return -1;
		outbuff->size = MAL_BUFFER_SIZE;
	}
	
	while (!done) {
		int rsize;
		va_list args;
		va_start(args,format);
		rsize = vsnprintf(&outbuff->buffer[outbuff->cur],outbuff->size-outbuff->cur,format,args);
		if (rsize <= 0 || rsize >= outbuff->size-outbuff->cur) {
			unsigned long size = outbuff->cur+MAL_BUFFER_SIZE+1;
			char* ptr = (char*)BSCalloc(outbuff->pool,size,"appendStr");
			if (ptr == NULL) {
				va_end(args);
				return -1;
			}
			memcpy(ptr,outbuff->buffer,outbuff->size);
			BSFree(outbuff->buffer);
			outbuff->buffer = ptr;
			outbuff->size = outbuff->cur+MAL_BUFFER_SIZE+1;
		} else {
			outbuff->cur += rsize;
			done = 1;
		}
		va_end(args);
	}
	
	return 0;
}

static char* appendCStr(PLIST_OUT_BUFFER* outbuff)
{
	if (outbuff->buffer == NULL) return "";
	return outbuff->buffer;
}

static int desposeAppendStr(PLIST_OUT_BUFFER* outbuff)
{
	if (outbuff->buffer) {
		BSFree(outbuff->buffer);
		outbuff->buffer = NULL;
		outbuff->cur = 0;
		outbuff->size = 0;
	}
	return 0;
}

static int appendTabs(PLIST_OUT_BUFFER* str,int level)
{
	int i;
	for (i=0;i<level;i++) {
		if (doAppend(str,"¥t") != 0) return -1;
	}
	return 0;
}

static int appendValue(PLIST_OUT_BUFFER* str,int level,QBNode* node)
{
	appendTabs(str,level);
	switch (QBNodeValueType(node)) {
	case QBNODE_TYPE_ULONG:
		doAppend(str,"<integer>");
		doAppend(str,"%ld",QBNodeULong(node));
		doAppend(str,"</integer>¥n");
		break;
	case QBNODE_TYPE_SLONG:
		doAppend(str,"<integer>");
		doAppend(str,"%ld",QBNodeSLong(node));
		doAppend(str,"</integer>¥n");
		break;
	case QBNODE_TYPE_DOUBLE:
		doAppend(str,"<string>");
		doAppend(str,"%lf",QBNodeDouble(node));
		doAppend(str,"</string>¥n");
		break;
	case QBNODE_TYPE_CONST_STRING:
	case QBNODE_TYPE_STRING:
		doAppend(str,"<string>");
		doAppend(str,QBNodeString(node));
		doAppend(str,"</string>¥n");
		break;
	case QBNODE_TYPE_BYTE_ARRAY:
		doAppend(str,"<data>");
		{
			unsigned char* bytes = QBNodeBytes(node);
			unsigned long size = BSSize(bytes);
			char* base64str = base64encode(QBNodePoolPtr(node),bytes,size);
			doAppend(str,base64str);
			BSFree(base64str);
		}
		doAppend(str,"</data>¥n");
		break;
	case QBNODE_TYPE_SLONG_ARRAY:
		doAppend(str,"<array>¥n");
		{
			int i;
			int n = QBNodeArrayCount(node);
			signed long* array = QBNodeSLongArray(node);
			if (array) {
				for (i=0;i<n;i++) {
					appendTabs(str,level+1);
					doAppend(str,"<integer>%ld</integer>¥n",array[i]);
				}
			}
		}
		appendTabs(str,level);
		doAppend(str,"</array>¥n");
		return 1;
	case QBNODE_TYPE_ULONG_ARRAY:
		doAppend(str,"<array>¥n");
		{
			int i;
			int n = QBNodeArrayCount(node);
			unsigned long* array = QBNodeULongArray(node);
			if (array) {
				for (i=0;i<n;i++) {
					appendTabs(str,level+1);
					doAppend(str,"<integer>%ld</integer>¥n",array[i]);
				}
			}
		}
		appendTabs(str,level);
		doAppend(str,"</array>¥n");
		return 1;
	case QBNODE_TYPE_DOUBLE_ARRAY:
		doAppend(str,"<array>¥n");
		{
			int i;
			int n = QBNodeArrayCount(node);
			double* array = QBNodeDoubleArray(node);
			if (array) {
				for (i=0;i<n;i++) {
					appendTabs(str,level+1);
					doAppend(str,"<real>%lf</real>¥n",array[i]);
				}
			}
		}
		appendTabs(str,level);
		doAppend(str,"</array>¥n");
		return 1;
	case QBNODE_TYPE_FLOAT_ARRAY:
		doAppend(str,"<array>¥n");
		{
			int i;
			int n = QBNodeArrayCount(node);
			float* array = QBNodeFloatArray(node);
			if (array) {
				for (i=0;i<n;i++) {
					appendTabs(str,level+1);
					doAppend(str,"<real>%f</real>¥n",array[i]);
				}
			}
		}
		appendTabs(str,level);
		doAppend(str,"</array>¥n");
		return 1;
	}
	return 0;
}

static int isChildArray(QBNode* node)
{
	int i;
	int n = QBNodeChildNum(node);
	int isChildArray = 0;

	isChildArray = 1;
	for (i=0;i<n;i++) {
		QBNode* child = QBNodeChild(node,i);
		if (checkStringType(QBNodeKey(child)) != STR_INTEGER) {
			isChildArray = 0;
			break;
		}
		if (atoi(QBNodeKey(child)) != i) {
			isChildArray = 0;
			break;
		}
	}
	
	return isChildArray;
}

static int makePlistStr(PLIST_OUT_BUFFER* str,int level,QBNode* node,int type)
{
	int i;
	int array = -1;
	int n=QBNodeChildNum(node);
	int _isChildArray = isChildArray(node);
	
/*
	switch (QBNode_DataType(node,NULL)) {
	case EC_NODE_TYPE_SLONG_ARRAY:
	case EC_NODE_TYPE_ULONG_ARRAY:
	case EC_NODE_TYPE_DOUBLE_ARRAY:
		return 0;
	}
	
	// 子供のキーが0からの連番になっているか調べる
	isChildArray = 1;
	for (i=0;i<n;i++) {
		QBNode* child = QBNode_Child(node,i);
		if (checkStringType(QBNode_Key(child)) != STR_INTEGER) {
			isChildArray = 0;
			break;
		}
		if (atoi(QBNode_Key(child)) != i) {
			isChildArray = 0;
			break;
		}
	}
*/
	
	for (i=0;i<n;i++) {
		QBNode* child = QBNodeChild(node,i);
		if (child) {
			int skip = 0;

			if (i==0) {
				switch (checkStringType(QBNodeKey(child))) {
				case STR_FLOAT:
					return -1;
				case STR_INTEGER:
					if (_isChildArray == 1) {
						array = 0;
						if (level > 0 && type == 1) {
							appendTabs(str,level);
							doAppend(str,"<key>");
							doAppend(str,QBNodeKey(node));
							doAppend(str,"</key>¥n");
						}
						appendTabs(str,level);
						doAppend(str,"<array>¥n");
						type = 0;
						break;
					}
				case STR_STRING:
					if (level > 0 && type == 1) {
						appendTabs(str,level);
						doAppend(str,"<key>");
						doAppend(str,QBNodeKey(node));
						doAppend(str,"</key>¥n");
					}
					appendTabs(str,level);
					doAppend(str,"<dict>¥n");
					type = 1;
					break;
				}
			}
			
			if (QBNodeValueType(child) != QBNODE_TYPE_NONE) {
				if (array >= 0) {
					/* NSArrayとして扱う */
					appendValue(str,level+1,child);
				} else {
					/* NSDictionaryとして扱う */
					appendTabs(str,level+1);
					doAppend(str,"<key>");
					doAppend(str,QBNodeKey(child));
					doAppend(str,"</key>¥n");
					if (appendValue(str,level+1,child) == 1) {
						skip = 1;
					}
				}
			}
			
			if (skip == 0) {
				if (makePlistStr(str,level+1,child,type) != 0) return -1;
			}
		}
	}
	
	if (n > 0) {
		appendTabs(str,level);
		if (array >= 0) {
			doAppend(str,"</array>¥n");
		} else {
			doAppend(str,"</dict>¥n");
		}
	}
	
	return 0;
}
	
/*-----------------------------------------------------------------------------------------------
	外部に公開する関数
-----------------------------------------------------------------------------------------------*/

#pragma mark -

/*!
   @brief
		plist XML を QBNode に変換する。
   @param
		pool メモリゾーン。
   @param
		plistString plist 形式の XML 文字列
   @return
		QBNodeのポインタ。エラーの場合はNULL。
   @note
		__PLISTPARSER_SPECIAL__を定義すると array は singed long 型の配列として扱う。¥n
*/
QBNode* PlistToQBNode(QBNodePool* pool,char* plistString)
{
	QBNode* root = QBNodeAlloc(pool,"Root");

	PLIST_NODE p;
	p.n = 0;
	p.step = 0;
	p.pool = pool;
	p.err = 0;
	p.plistString = plistString;

	doParseFirst(&p,root);

	if (p.err != 0) {
		BSRelease(root);
		root = NULL;
		QBNodePoolRecycle(pool);
	}
	
	return root;
}

/*!
   @brief
		QBNode を plist 形式の XML に変換する。
   @param
		node 変換するQBNode
   @return
		文字列へのポインタ。¥n
		エラーの場合はNULL。
   @note
*/
char* QBNodeToPlist(QBNode* node)
{
/*
	QBNodePool* pool = QBNodePoolPtr(node);
	PLIST_OUT_BUFFER str;
	initAppendStr(&str,pool);
	
	if (doAppend(&str,PLIST_HEADER) != 0) goto ERROR;
	if (makePlistStr(&str,0,node,0) != 0) goto ERROR;
	if (doAppend(&str,PLIST_FOOTER) != 0) goto ERROR;

	BSRelease(str.buffer);
	
	return str.buffer;

ERROR:
	desposeAppendStr(&str);
*/
	return NULL;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
