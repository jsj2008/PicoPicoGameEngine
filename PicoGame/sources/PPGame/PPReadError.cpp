/*-----------------------------------------------------------------------------------------------
	名前	PPReadError.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPReadError.h"
#include <string>

std::string* readerror=NULL;

void PPReadErrorReset()
{
	if (readerror) delete readerror;
	readerror = NULL;
}

void PPReadErrorSet(const char* filename)
{
	if (readerror) delete readerror;
	std::string e = filename;
	e = "fileread error : "+e;
	readerror = new std::string(e);
}

const char* PPReadError()
{
	if(readerror) return readerror->c_str();
	return NULL;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
