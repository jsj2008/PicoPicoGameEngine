/*-----------------------------------------------------------------------------------------------
	名前	PPGameRunlength.c
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PPGameRunlength.h"

#define ALIAN_CHG_SHORT(x) (x)
#define ABtoL(x) (x)

void *PPGameRunlength_Compress(unsigned short *ptr,long *size)
{
	unsigned short *buf;
	long i,j;
	unsigned short pat;
	unsigned long patct=0;
	int step;
	int msize = (int)(*size);

	buf = (unsigned short *)calloc(1,msize*2);

	j = 0;
	step = 0;
	((long *)buf)[0] = *size;	
	j += 4;

	for (i=0;i<*size/sizeof(short);i++) {
		switch (step) {
		case 0:
			pat = ptr[i];
			patct = 1;
			step = 1;
			break;
		case 1:
			if (ptr[i] == pat) {
				patct ++;
				if (patct == 0x8000-1) {
					unsigned short tmp;
					tmp = patct | 0x8000;
					tmp = ALIAN_CHG_SHORT(tmp);
					buf[j++] = tmp;
					buf[j++] = pat;
					patct = 0;
				}
			} else {
				if (patct > 1) {
					unsigned short tmp;
					tmp = patct | 0x8000;
					tmp = ALIAN_CHG_SHORT(tmp);
					buf[j++] = tmp;
					if (j >= msize*2) {
printf("err\n");
						break;
					}
				}
				buf[j++] = pat;
				if (j >= msize*2) {
printf("err\n");
					break;
				}
				pat = ptr[i];
				patct = 1;
			}
			break;
		}
	}
	if (patct > 1) {
		unsigned short tmp;
		tmp = patct | 0x8000;
		tmp = ALIAN_CHG_SHORT(tmp);
		buf[j++] = tmp;
		if (j >= msize*2) {
printf("err\n");
		}
	}
	buf[j++] = pat;
	if (j >= msize*2) {
printf("err\n");
	}
	if (j % 2) j ++;
	*size = j*sizeof(short);
	((long *)buf)[1] = *size;	
	return buf;
}

void *PPGameRunlength_Decompress(unsigned short *ptr,long *datsize)
{
	unsigned short *buf;
	long size;
	long i,j,k;
	unsigned short pat;
	unsigned short patct;

	size = *datsize = ((signed int *)ptr)[0];
	
	buf = (unsigned short *)malloc(size);
	
	for (i=0,j=4;i<size/sizeof(short);) {
		pat = ptr[j++];
		pat = ALIAN_CHG_SHORT(pat);
		if (pat & 0x8000) {
			patct = pat & (~0x8000);
			pat = ptr[j++];
			for (k=0;k<patct;k++) {
				buf[i++] = pat;
				if (i >= size/sizeof(short)) {
					break;
				}
			}
		} else {
			buf[i++] = ALIAN_CHG_SHORT(pat);
		}
		if (i >= size/sizeof(short)) {
			break;
		}
	}
	return buf;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
