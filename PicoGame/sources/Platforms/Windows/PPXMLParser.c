/*-----------------------------------------------------------------------------------------------
	名前	PPXMLParser.c
	説明		        
	作成	2007.01.06 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPXMLParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

typedef struct _xmlParserQBNodeMaker {
	QBNodePool* pool;
	char* buffer;
	long ptr;
	QBNode* node;
	QBNode* stack;
	int open;
} xmlParserQBNodeMaker;

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

static BSErr startTag(XMLParser* parser);
static BSErr endTag(XMLParser* parser);
static BSErr startDoc(XMLParser* parser);
static BSErr endDoc(XMLParser* parser);
static BSErr foundAtr(XMLParser* parser,unsigned char* string);
static BSErr foundTag(XMLParser* parser,unsigned char* string);
static BSErr foundDoc(XMLParser* parser,unsigned char* string);

static unsigned char* stringPtr(XMLParser* parser);
static unsigned char* copyString(XMLParser* parser,unsigned char* string);

static BSErr resetStr(XMLParser* parser);
static BSErr appendChar(XMLParser* parser,unsigned char a);
static BSErr findXMLTag(XMLParser* parser);
static BSErr readMore(XMLParser* parser);

static BSErr readQBNodeMakerFunc(XMLParser* parser,char* buffer,long* size);
static BSErr resultQBNodeMakerFunc(XMLParser* parser,QBNode* node,int openclose);

static BSErr readFunc(XMLParser* parser,char* buffer,long* size);
static BSErr resultFunc(XMLParser* parser,QBNode* node,int openclose);
static BSErr resultDumpFunc(XMLParser* parser,QBNode* node,int openclose);

/*-----------------------------------------------------------------------------------------------
	局所変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部に公開するプログラム
-----------------------------------------------------------------------------------------------*/

int PPSAXParser(PPSAXHandler* handler)
{
printf("PPSAXParser in\n");
	QBNodePool* pool = QBNodePoolAlloc();
	if (pool) {
		if (handler->xmltext) {
			XMLParser* parser = XMLParserInit(pool);
			if (parser) {
				XMLParserSetUserData(parser,handler);
				XMLParserSetReadCallback(parser,readFunc);
				XMLParserSetResultCallback(parser,resultFunc);
//				XMLParserSetResultCallback(parser,resultDumpFunc);
				XMLParserDo(parser);
				XMLParserDispose(parser);
			}
		}
		QBNodePoolDescription(pool);
		QBNodePoolDealloc(pool);
	}
printf("PPSAXParser out\n");
    return 0;
}

static BSErr readQBNodeMakerFunc(XMLParser* parser,char* buffer,long* size)
{
	xmlParserQBNodeMaker* seri = (xmlParserQBNodeMaker*)XMLParserUserData(parser);
	if (seri->buffer[seri->ptr] == 0) {
		*size = 0;
	} else {
		int i;
		for (i=0;(i<*size) && (seri->buffer[seri->ptr]!=0);i++,seri->ptr++) {
			buffer[i] = seri->buffer[seri->ptr];
		}
		*size = i;
	}
	return BSERR_OK;
}

static BSErr resultQBNodeMakerFunc(XMLParser* parser,QBNode* node,int openclose)
{
	xmlParserQBNodeMaker* seri = (xmlParserQBNodeMaker*)XMLParserUserData(parser);
	switch (openclose) {
	case XMLPARSER_HEAD:
		break;
	case XMLPARSER_DOC:
		if (seri->open == 1) {
			QBNode* last = QBNodeFirstChild(QBNodeLastChild(seri->stack));
			if (last) {
#if 0
				int i;
				for (i=0;i<QBNodeChildNum(node);i++) {
					QBNodeSetString(QBNodeCreateChildWithNumberKey(last),QBNodeString(QBNodeChild(node,i)));
				}
#else
				{
					int noText = 0;
					{
						if (QBNodeChildNum(node) == 1) {
							char* str = QBNodeString(QBNodeChild(node,0));
							if (str) {
								if (str[0] == '\n') {
									noText = 1;
								}
							}
						}
					}
					if (noText==0) {
						char* str = BSCalloc(seri->pool,1,"resultQBNodeMakerFunc");
						if (str) {
							int i;
							for (i=0;i<QBNodeChildNum(node);i++) {
								//if (i > 0) str = BSStrAppend(str,BSRelease(BSStr(seri->pool,",")));
								if ((str = BSStrAppend(BSRelease(str),QBNodeString(QBNodeChild(node,i)))) == NULL) {
									QBNodePoolRecycle(seri->pool);
									return BSERR_MEM;
								}
							}
							QBNodeSetString(last,BSRelease(str));
							QBNodePoolRecycle(seri->pool);
						} else {
							return BSERR_MEM;
						}
					}
				}
#endif
			} else {
				return BSERR_INVAILD;
			}
		}
		break;
	case XMLPARSER_OPEN:
		{
			QBNode* newNode = QBNodeAlloc(seri->pool,QBNodeString(QBNodeFirstChild(node)));
			if (newNode == NULL) return BSERR_MEM;

			//スタックに積む
			{
				QBNode* last = QBNodeFirstChild(QBNodeLastChild(seri->stack));
				if (QBNodeAppend(QBNodeAppendChildWithNumberKey(seri->stack),newNode) != BSERR_OK) {
					BSRelease(newNode);
					return BSERR_MEM;
				}
				BSRelease(newNode);
				if (seri->node == NULL) {
					seri->node = BSRetain(newNode);
				} else
				if (last) {
					QBNodeAppend(last,newNode);
				} else {
					return BSERR_INVAILD;
				}
			}

			//属性の解析
			{
				int i;
				for (i=1;i<QBNodeChildNum(node);i++) {
					QBNode* child = QBNodeChild(node,i);
					if (strcmp(QBNodeString(child),"/") != 0) {
						QBNodeSetAttribute(newNode,QBNodeString(child),QBNodeStringWithKeyPath(child,".Attr"));
					}
				}
			}
			
			seri->open = 1;
		}
		break;
	case XMLPARSER_OPEN_CLOSE:
		{
			QBNode* newNode = QBNodeAlloc(seri->pool,QBNodeString(QBNodeFirstChild(node)));
			if (newNode == NULL) return BSERR_MEM;

			{
				QBNode* last = QBNodeFirstChild(QBNodeLastChild(seri->stack));
				if (last) {
					QBNodeAppend(last,newNode);
				}
			}

			//属性の解析
			{
				int i;
				for (i=1;i<QBNodeChildNum(node);i++) {
					QBNode* child = QBNodeChild(node,i);
					if (strcmp(QBNodeString(child),"/") != 0) {
						QBNodeSetAttribute(newNode,QBNodeString(child),QBNodeStringWithKeyPath(child,".Attr"));
					}
				}
			}

			seri->open = 0;
		}
		break;
	case XMLPARSER_CLOSE:
		QBNodeRemoveLastChild(seri->stack);
		break;
	}
	return BSERR_OK;
}

QBNode* XMLParserMakeQBNode(QBNodePool* pool,char* xmlstring)
{
	xmlParserQBNodeMaker maker;
	memset(&maker,0,sizeof(maker));
	maker.pool = pool;
	maker.buffer = xmlstring;
	maker.ptr = 0;
	maker.node = NULL;
	maker.open = 0;
	maker.stack = QBNodeAlloc(pool,"stack");
	if (maker.stack == NULL) {
		if (maker.stack) BSRelease(maker.stack);
		return NULL;
	}
	XMLParser* parser = XMLParserInit(pool);
	if (parser) {
		XMLParserSetUserData(parser,&maker);
		XMLParserSetReadCallback(parser,readQBNodeMakerFunc);
		XMLParserSetResultCallback(parser,resultQBNodeMakerFunc);
		XMLParserDo(parser);
		XMLParserDispose(parser);
	}
	if (maker.stack) BSRelease(maker.stack);
	QBNodePoolRecycle(pool);
	return maker.node;
}

XMLParser* XMLParserInit(QBNodePool* pool)
{
	XMLParser* parser = (XMLParser*)BSCalloc(pool,sizeof(XMLParser),"XMLParserInit");
	if (parser) {
		parser->node = QBNodeAlloc(pool,"");
		if (parser->node == NULL) {
			XMLParserDispose(parser);
			return NULL;
		}
		parser->stack = QBNodeAlloc(pool,"stack");
		if (parser->stack == NULL) {
			XMLParserDispose(parser);
			return NULL;
		}
	}
	return parser;
}

void* XMLParserUserData(XMLParser* parser)
{
	if (parser == NULL) return NULL;
	return parser->userData;
}

BSErr XMLParserSetUserData(XMLParser* parser,void* userData)
{
	if (parser == NULL) return BSERR_PARAM;
	parser->userData = userData;
	return BSERR_OK;
}

BSErr XMLParserSetReadCallback(XMLParser* parser,XMLParserReadFunc func)
{
	if (parser == NULL) return BSERR_PARAM;
	parser->readCallback = (void*)func;
	return BSERR_OK;
}

BSErr XMLParserSetResultCallback(XMLParser* parser,XMLParserResultFunc func)
{
	if (parser == NULL) return BSERR_PARAM;
	parser->resultCallback = (void*)func;
	return BSERR_OK;
}

BSErr XMLParserDo(XMLParser* parser)
{
	if (parser == NULL) return BSERR_PARAM;
	
	parser->ptr = 0;
	parser->size = 0;
	
	return findXMLTag(parser);
}

void XMLParserDispose(XMLParser* parser)
{
	if (parser == NULL) return;
	{
		QBNodePool* pool = QBNodePoolPtr(parser);
		if (parser->node) {
			BSRelease(parser->node);
			parser->node = NULL;
		}
		if (parser->stack) {
			BSRelease(parser->stack);
			parser->stack = NULL;
		}
		if (parser->string) {
			BSFree(parser->string);
			parser->string = NULL;
		}
		BSFree(parser);
		QBNodePoolRecycle(pool);
	}
}

/*-----------------------------------------------------------------------------------------------
	外部に公開しないプログラム
-----------------------------------------------------------------------------------------------*/

static BSErr startTag(XMLParser* parser)
{
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("start tag\n");
		return BSERR_OK;
	}
	BSRelease(parser->node);
	QBNodePoolRecycle(QBNodePoolPtr(parser));
	parser->node = QBNodeAlloc(QBNodePoolPtr(parser),"Tag");
	if (parser->node == NULL) return BSERR_MEM;
	return BSERR_OK;
}

static BSErr endTag(XMLParser* parser)
{
	//int openclose = 0;
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("end tag\n");
		return BSERR_OK;
	}
//printf("(");
//QBNodeDescription(parser->node);
//printf(")\n");
	{
		char* tag = QBNodeString(QBNodeFirstChild(parser->node));
		if (tag) {
			if (strcmp(tag,"?") == 0) {
				parser->openclose = XMLPARSER_HEAD;
			} else
			if (strcmp(tag,"/") == 0) {
				tag = QBNodeString(QBNodeLastChild(parser->node));
				if (tag) {
					QBNode* last = QBNodeLastChild(parser->stack);
					char* s = QBNodeString(last);
					if (s) {
						if (strcmp(s,tag) == 0) {
							QBNodeRemoveLastChild(parser->stack);
							parser->openclose = XMLPARSER_CLOSE;
						} else {
							return BSERR_INVAILD;
						}
					} else {
						return BSERR_MEM;
					}
				} else {
					return BSERR_MEM;
				}
//printf("{");
//QBNodeDescription(parser->stack);
//printf("}\n");
			} else {
				char* lasttag = QBNodeString(QBNodeLastChild(parser->node));
//printf("[");
//printf(ltag);
//printf("]\n");
				if (lasttag) {
					if (strcmp(lasttag,"/") == 0) {
						QBNodeRemoveLastChild(parser->node);
						parser->openclose = XMLPARSER_OPEN_CLOSE;
					} else {
						QBNode* child;
						if ((child = QBNodeAppendChildWithNumberKey(parser->stack)) != NULL) {
							QBNodeSetString(child,tag);
							parser->openclose = XMLPARSER_OPEN;
						} else {
							return BSERR_MEM;
						}
					}
				}
//printf("{");
//QBNodeDescription(parser->stack);
//printf("}\n");
			}
		} else {
			return BSERR_MEM;
		}
	}
	return resultCallback(parser,parser->node,parser->openclose);
}

static BSErr startDoc(XMLParser* parser)
{
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("start doc\n");
		return BSERR_OK;
	}
	BSRelease(parser->node);
	QBNodePoolRecycle(QBNodePoolPtr(parser));
	parser->node = QBNodeAlloc(QBNodePoolPtr(parser),"Doc");
	if (parser->node == NULL) return BSERR_MEM;
	return BSERR_OK;
}

static BSErr endDoc(XMLParser* parser)
{
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("end doc\n");
		return BSERR_OK;
	}
	return resultCallback(parser,parser->node,XMLPARSER_DOC);
}

static BSErr foundAtr(XMLParser* parser,unsigned char* string)
{
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("\t\t %s\n",(char*)string);
		return BSERR_OK;
	}
	if (parser->node) {
		QBNode* node = QBNodeLastChild(parser->node);
		if (node) {
			QBNodeSetStringWithKeyPath(node,(char*)string,".Attr");
		} else {
			return BSERR_MEM;
		}
	}
	return BSERR_OK;
}

static BSErr foundTag(XMLParser* parser,unsigned char* string)
{
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("\t %s\n",(char*)string);
		return BSERR_OK;
	}
	if (parser->node) {
		QBNode* node = QBNodeAppendChildWithNumberKey(parser->node);
		if (node) {
			QBNodeSetString(node,(char*)string);
		} else {
			return BSERR_MEM;
		}
	}
	return BSERR_OK;
}

static BSErr foundDoc(XMLParser* parser,unsigned char* string)
{
	XMLParserResultFunc resultCallback = (XMLParserResultFunc)parser->resultCallback;
	if (resultCallback == NULL) {
		printf("\t %s\n",(char*)string);
		return BSERR_OK;
	}
	if (parser->node) {
		QBNode* node = QBNodeAppendChildWithNumberKey(parser->node);
		if (node) {
			QBNodeSetString(node,(char*)string);
		} else {
			return BSERR_MEM;
		}
	}
	return BSERR_OK;
}

#pragma mark -

static unsigned char* stringPtr(XMLParser* parser)
{
	if (parser->string == NULL) return (unsigned char*)"";
	return parser->string;
}

static unsigned char* copyString(XMLParser* parser,unsigned char* string)
{
	if (string != NULL) {
		int len = strlen((char*)string)+1;
		unsigned char* mem = (unsigned char*)BSMalloc(QBNodePoolPtr(parser),len,"copyString");
		if (mem) {
			memcpy(mem,parser->string,len);
			return mem;
		}
	}
	return NULL;
}

static BSErr resetStr(XMLParser* parser)
{
	if (parser->string != NULL) {
		parser->string[0] = 0;
	}
	parser->strptr = 0;
	return BSERR_OK;
}

static BSErr appendChar(XMLParser* parser,unsigned char a)
{
	if (parser->string == NULL) {
		parser->string = (unsigned char*)BSCalloc(QBNodePoolPtr(parser),1000,"appendChar");
		parser->strptr = 0;
	}
	if (parser->string) {
		if (BSSize(parser->string)-1 <= parser->strptr) {
			unsigned char* mem = (unsigned char*)BSCalloc(QBNodePoolPtr(parser),BSSize(parser->string)+1000,"appendChar");
			if (mem) {
				strcpy((char*)mem,(char*)parser->string);
				BSFree(parser->string);
				parser->string = mem;
			} else {
				return BSERR_MEM;
			}
		}
		parser->string[parser->strptr++] = a;
		parser->string[parser->strptr] = 0;
	}
	return BSERR_OK;
}

static BSErr findXMLTag(XMLParser* parser)
{
	BSErr err = BSERR_OK;
	BSErr parseErr = BSERR_INVAILD;
	unsigned char c;

	while (err == BSERR_OK) {
		
		if ((err = readMore(parser)) != BSERR_OK) break;
		if (parser->size == 0) break;
		
		{
			c = (unsigned char)parser->buffer[parser->ptr++];

			if (c <= 0x20 && parser->openclose != XMLPARSER_OPEN) {
				
			} else
			if (c == '<') {
				//タグ内部の解析
				int space = 0;
				
				parseErr = BSERR_INVAILD;
				
				if ((err = startTag(parser)) != BSERR_OK) break;

				while (err == BSERR_OK) {

					if ((err = readMore(parser)) != BSERR_OK) break;
					if (parser->size == 0) break;
					
					c = (unsigned char)parser->buffer[parser->ptr++];
				
					if (c == '=') {
						if (parser->strptr > 0) {
							err = foundTag(parser,stringPtr(parser));
							resetStr(parser);
						}

						while (err == BSERR_OK) {

							if ((err = readMore(parser)) != BSERR_OK) break;
							if (parser->size == 0) break;

							c = (unsigned char)parser->buffer[parser->ptr++];
							
							if (c <= 0x20) {
								
							} else
							if (c == '\"') {
								
								while (err == BSERR_OK) {

									if ((err = readMore(parser)) != BSERR_OK) break;
									if (parser->size == 0) break;

									c = (unsigned char)parser->buffer[parser->ptr++];

									if (c == '\"') {
									
										if (parser->strptr > 0) {
											err = foundAtr(parser,stringPtr(parser));
											resetStr(parser);
										}
									
										break;
									} else {
										err = appendChar(parser,c);
									}

								}
								
								break;
								
							} else {
								
								err = BSERR_INVAILD;
								
							}

						}

					} else
					if (c == '?') {
						if (parser->strptr > 0) {
							err = foundTag(parser,stringPtr(parser));
							resetStr(parser);
						}
						err = appendChar(parser,c);
						err = foundTag(parser,stringPtr(parser));
						resetStr(parser);
					} else
					if (c == '/') {
						if (parser->strptr > 0) {
							err = foundTag(parser,stringPtr(parser));
							resetStr(parser);
						}
						err = appendChar(parser,c);
						err = foundTag(parser,stringPtr(parser));
						resetStr(parser);
					} else
					if (c <= 0x20) {
						if (space == 0) {
							if (parser->strptr > 0) {
								err = foundTag(parser,stringPtr(parser));
								resetStr(parser);
							}
						}
						space = 1;
					} else
					if (c == '>') {
						if (parser->strptr > 0) {
							err = foundTag(parser,stringPtr(parser));
							resetStr(parser);
						}
						err = endTag(parser);
						parseErr = BSERR_OK;
						break;
					} else {
						err = appendChar(parser,c);
						space = 0;
					}
				
				}
				
			} else {
				//文字列の解析
				int space = 0;

				parseErr = BSERR_INVAILD;

				if ((err = startDoc(parser)) != BSERR_OK) break;

				if (c <= 0x20) {
					space = 1;
				}

				parser->ptr --;

				while (err == BSERR_OK) {

					if ((err = readMore(parser)) != BSERR_OK) break;
					if (parser->size == 0) break;
					
					c = (unsigned char)parser->buffer[parser->ptr++];

					if (c <= 0x20) {
						if (parser->strptr > 0 && space == 0) {
							err = foundDoc(parser,stringPtr(parser));
							resetStr(parser);
						}
						space = 1;
						err = appendChar(parser,c);
					} else
					if (c == '<') {
						if (parser->strptr > 0) {
							err = foundDoc(parser,stringPtr(parser));
							resetStr(parser);
						}
						err = endDoc(parser);
						parser->ptr --;
						break;
					} else {
						if (parser->strptr > 0 && space == 1) {
							err = foundDoc(parser,stringPtr(parser));
							resetStr(parser);
						}
						err = appendChar(parser,c);
						space = 0;
					}
				
				}
			}
		}
	}
	
	if (parseErr != BSERR_OK) {
		return parseErr;
	}
	return err;
}

static BSErr readMore(XMLParser* parser)
{
	XMLParserReadFunc readCallback = (XMLParserReadFunc)parser->readCallback;
	if (readCallback == NULL) return BSERR_PARAM;
	{
		BSErr err = BSERR_OK;
		if (parser->ptr >= parser->size || parser->size == 0) {
			parser->size = sizeof(parser->buffer);
			err = readCallback(parser,parser->buffer,&parser->size);
			parser->ptr = 0;
		}
		return err;
	}
}

//static char* xmlsample = NULL;
//static long ptr = 0;
//static long level = 0;

static BSErr readFunc(XMLParser* parser,char* buffer,long* size)
{
	PPSAXHandler* handler = (PPSAXHandler*)parser->userData;
	long *p = (long*)&handler->ptr;//XMLParserUserData(parser);
	long i;
	if (handler->xmltext[*p] == 0) {
		*size = 0;
	} else {
		for (i=0;(i<*size) && (handler->xmltext[*p]!=0);i++,(*p)++) {
			buffer[i] = handler->xmltext[*p];
		}
		*size = i;
	}
	return BSERR_OK;
}

static void printtab(int level)
{
	int i;
	for (i=0;i<level;i++) {
		printf("    ");
	}
}

static BSErr resultDumpFunc(XMLParser* parser,QBNode* node,int openclose)
{
	PPSAXHandler* handler = (PPSAXHandler*)parser->userData;
	switch (openclose) {
	case XMLPARSER_HEAD:
		break;
	case XMLPARSER_DOC:
		printtab(handler->level);
		{
			int i;
			for (i=0;i<QBNodeChildNum(node);i++) {
				if (i > 0) printf(",");
				printf("%s",QBNodeString(QBNodeChild(node,i)));
			}
		}
		printf("\n");
		break;
	case XMLPARSER_OPEN:
		printtab(handler->level);
		printf("%s ",QBNodeString(QBNodeFirstChild(node)));
		{
			int i;
			for (i=1;i<QBNodeChildNum(node);i++) {
				if (i > 1) printf(",");
				printf("%s",QBNodeString(QBNodeChild(node,i)));
				if (QBNodeChildNum(QBNodeChild(node,i)) > 0) {
					printf("=%s",QBNodeString(QBNodeChild(QBNodeChild(node,i),0)));
				}
			}
		}
		printf("\n");
		handler->level ++;
		break;
	case XMLPARSER_OPEN_CLOSE:
		printtab(handler->level);
		printf("%s/ ",QBNodeString(QBNodeFirstChild(node)));
		{
			int i;
			for (i=1;i<QBNodeChildNum(node);i++) {
				if (i > 1) printf(",");
				printf("%s",QBNodeString(QBNodeChild(node,i)));
				if (QBNodeChildNum(QBNodeChild(node,i)) > 0) {
					printf("=%s",QBNodeString(QBNodeChild(QBNodeChild(node,i),0)));
				}
			}
		}
		printf("\n");
		break;
	case XMLPARSER_CLOSE:
		handler->level --;
		printtab(handler->level);
		printf("/%s\n",QBNodeString(QBNodeLastChild(node)));
		break;
	}
	return BSERR_OK;
}

static BSErr resultFunc(XMLParser* parser,QBNode* node,int openclose)
{
	PPSAXHandler* handler = (PPSAXHandler*)parser->userData;
	switch (openclose) {
	case XMLPARSER_HEAD:
		break;
	case XMLPARSER_DOC:
		{
			int i;
			for (i=0;i<QBNodeChildNum(node);i++) {
				char* s = QBNodeString(QBNodeChild(node,i));
				int len = strlen(s);
				handler->characters(handler->userdata,s,len);
			}
		}
		break;
	case XMLPARSER_OPEN:
	case XMLPARSER_OPEN_CLOSE:
		{
			char* name = QBNodeString(QBNodeFirstChild(node));
			char** attr = (char**)calloc(sizeof(char*),QBNodeChildNum(node)*2);
//printf("%s ",name);
			{
				int i;
				for (i=1;i<QBNodeChildNum(node);i++) {
					const char* key = NULL;
					const char* value = NULL;
					key = QBNodeString(QBNodeChild(node,i));
					if (QBNodeChildNum(QBNodeChild(node,i)) > 0) {
						value = QBNodeString(QBNodeChild(QBNodeChild(node,i),0));
					} else {
						value = "";
					}
					attr[(i-1)*2+0] = (char*)malloc(strlen(key)+1);
					strcpy(attr[(i-1)*2+0],key);
					attr[(i-1)*2+1] = (char*)malloc(strlen(value)+1);
					strcpy(attr[(i-1)*2+1],value);
//printf("%s=%s,",key,value);
				}
			}
//printf("\n");
			handler->startElement(handler->userdata,name,(const char**)attr);
			for (int i=0;attr[i];i+=2) {
				free(attr[i+0]);
				free(attr[i+1]);
			}
			free(attr);
			if (openclose == XMLPARSER_OPEN_CLOSE) {
				handler->endElement(handler->userdata,name);
			}
		}
		break;
	case XMLPARSER_CLOSE:
		{
			char* name = QBNodeString(QBNodeLastChild(node));
			handler->endElement(handler->userdata,name);
		}
		break;
	}
	return BSERR_OK;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
