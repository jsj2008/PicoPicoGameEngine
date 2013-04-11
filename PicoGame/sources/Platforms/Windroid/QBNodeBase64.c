/*-----------------------------------------------------------------------------------------------
	名前	QBNodeBase64.c
	説明		        
	作成	2006.12.29 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "QBNodeBase64.h"

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	型の定義
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	プロトタイプ宣言
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	局所変数
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	外部に公開するプログラム
-----------------------------------------------------------------------------------------------*/

/*
メモ
	base64
		3バイトを6bitに区切って下記のルールに沿って4文字に変換。
			0  - 25    ->    A - Z
			26 - 51    ->    a - z
			52 - 61    ->    0 - 9
			62         ->    +
			63         ->    /
*/

static char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

long base64encodeBufferSize(long length)
{
	long r = ((length+2)/3)*4;
	return r+1;
}

char* base64encode(QBNodePool* pool,unsigned char* inBuffer,long inLength)
{
	return base64encodeWithSpace(pool,inBuffer,inLength,0);
}

char* base64encodeWithSpace(QBNodePool* pool,unsigned char* inBuffer,long inLength,int space)
{
	char* buffer = NULL;
	if (inBuffer != NULL) {
		long size = base64encodeBufferSize(inLength)-1;
		buffer = (char*)BSMalloc(pool,size+(size / 68)*(space+1)+1,"base64encode");
		if (buffer) {
			long i;
			long j;
			long bj;
			int r=1;
			unsigned char c = 0;
			unsigned char a = 0;
			long len = (inLength+2)/3*3;
			for (i=0,j=0,bj=0;i<len;i++) {
				if (r) {
					int k;
					if (j > 0) buffer[j++] = '\n';
					for (k=0;k<space;k++) buffer[j++] = ' ';
					r = 0;
				}
				if (i < inLength) {
					c = inBuffer[i];
				} else {
					c = 0;
				}
				switch (i%3) {
				case 0:
					a = c;
					buffer[j++] = table[(a >> 2)];
					bj++;
					if (bj % 68 == 0) {r=1;}
					a <<= 6;
					break;
				case 1:
					a = (a & 0xc0) | (c>>2);
					buffer[j++] = table[(a >> 2)];
					bj++;
					if (bj % 68 == 0) {r=1;}
					a = c;
					a <<= 4;
					break;
				case 2:
					a = (a & 0xf0) | (c>>4);
					buffer[j++] = table[(a >> 2)];
					bj++;
					if (bj % 68 == 0) {r=1;}
					if (i < inLength) {
						a = c;
						buffer[j++] = table[(a & 63)];
						bj++;
						if (bj % 68 == 0) {r=1;}
					}
					break;
				}
				if (i == inLength) {
					break;
				}
			}
			for (;bj<size;) {
				buffer[j++] = '=';
				bj++;
			}
			buffer[j++] = 0;
			//if (outLength) *outLength = size;
		}
	}
	return buffer;
}

unsigned char* base64decode(QBNodePool* pool,char* string,long length)
{
	long i;
	unsigned char* buffer = NULL;
	long size = 0;
	if (string == NULL) return NULL;
	if (length == 0) length = strlen(string);
	while (1) {
	
		/* バッファサイズの計算 */
		for (i=0;i<length;i++) {
			if ((string[i] >= 'A' && string[i] <= 'Z')
			 || (string[i] >= 'a' && string[i] <= 'z')
			 || (string[i] >= '0' && string[i] <= '9')
			 || (string[i] == '+' || string[i] == '/')) {
				size ++;
			} else
			if (string[i] == '=' || string[i] == 0) {
				break;
			}
		}
		
		if (buffer==NULL) {
			long tsize = (size / 4)*3;
			switch (size % 4) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				tsize += 1;
				break;
			case 3:
				tsize += 2;
				break;
			}
			buffer = (unsigned char*)BSMalloc(pool,tsize+1,"base64decode");
			if (buffer == NULL) break;
		}
		
		/* エンコード */
		{
			long j;
			int s=0;
			for (i=0,j=0;i<length;i++) {
				int c = -1;
				char a = string[i];
			
				if (a >= 'A' && a <= 'Z') {
					c = 0+a-'A';
				} else
				if (a >= 'a' && a <= 'z') {
					c = 26+a-'a';
				} else
				if (a >= '0' && a <= '9') {
					c = 52+a-'0';
				} else
				if (a == '+') {
					c = 62;
				} else
				if (a == '/') {
					c = 63;
				} else
				if (a == '=' || a == 0) {
					break;
				}
				
				if (c >= 0) {
					switch (s%4) {
					case 0:
						buffer[j] = c << 2;
						break;
					case 1:
						buffer[j] |= c >> 4;
						j ++;
						buffer[j] = c << 4;
						break;
					case 2:
						buffer[j] |= c >> 2;
						j ++;
						buffer[j] = c << 6;
						break;
					case 3:
						buffer[j] |= c;
						j ++;
						break;
					}
					s ++;
				}
			}
		}
		
		break;
	}
	return buffer;
}


/*-----------------------------------------------------------------------------------------------
	外部に公開しないプログラム
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
