/*-----------------------------------------------------------------------------------------------
	名前	XMLParser.h
	説明		        
	作成	2007.01.06 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#pragma once

#ifndef __XMLPASER_H__
#define __XMLPASER_H__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeSystem.h"

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

enum {
	XMLPARSER_HEAD,
	XMLPARSER_DOC,
	XMLPARSER_OPEN,
	XMLPARSER_OPEN_CLOSE,
	XMLPARSER_CLOSE
};

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

typedef void (*startElementPPSAXMLFunc) (void *ctx,const char *name,const char **atts);

typedef void (*endElementPPSAXFunc) (void *ctx,const char *name);

typedef void (*charactersPPSAXFunc) (void *ctx,const char *ch,int len);

typedef struct _PPSAXHandler {
    startElementPPSAXMLFunc startElement;
    endElementPPSAXFunc endElement;
    charactersPPSAXFunc characters;
	void* userdata;
	int ptr;
	int level;
	char* xmltext;
} PPSAXHandler;

typedef struct _XMLParser {
	void* userData;
	void* readCallback;
	void* resultCallback;
	char buffer[256];
	long size;
	long ptr;
	unsigned char* string;
	long strptr;
	QBNode* node;
	QBNode* stack;
	int openclose;
} XMLParser;

#ifdef __cplusplus
extern "C" {
#endif

int PPSAXParser(PPSAXHandler* handler);

typedef BSErr (*XMLParserReadFunc) (XMLParser* parser,char* buffer,long* size);
typedef BSErr (*XMLParserResultFunc) (XMLParser* parser,QBNode* node,int openclose);

#ifdef __cplusplus
};
#endif

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

QBNode* XMLParserMakeQBNode(QBNodePool* pool,char* xmlstring);

XMLParser* XMLParserInit(QBNodePool* pool);
void* XMLParserUserData(XMLParser* parser);
BSErr XMLParserSetUserData(XMLParser* parser,void* userData);
BSErr XMLParserSetReadCallback(XMLParser* parser,XMLParserReadFunc func);
BSErr XMLParserSetResultCallback(XMLParser* parser,XMLParserResultFunc func);
BSErr XMLParserDo(XMLParser* parser);
void XMLParserDispose(XMLParser* parser);

#ifdef __cplusplus
};
#endif

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
