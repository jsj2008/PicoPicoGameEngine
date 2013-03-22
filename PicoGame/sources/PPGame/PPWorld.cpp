/*-----------------------------------------------------------------------------------------------
	名前	PPWorld.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <iostream>
#include "PPWorld.h"
#include "PPObject.h"

//static PPWorld* instance=NULL;
//float PPWorld::density = 1.0;

//PPWorld* PPWorld::sharedWorld(PPWorld* object)
//{
//	if (object == NULL) {
//	} else
//	if (object != instance) {
//		instance = object;
//	}
//	return instance;
//}

PPWorld::PPWorld() : ot(NULL),projector(NULL),userdata(NULL),scale_factor(1)
{
}

PPWorld::~PPWorld()
{
//	instance = NULL;
}

unsigned long PPWorld::getKey()
{
	return 0;
}

void PPWorld::allocOT()
{
	if (ot==NULL) {
		ot = new PPObject*[depthOT()];
		for (int i=0;i<depthOT();i++) {
			ot[i] = new PPObject(this);
		}
		for (int i=0;i<depthOT()-1;i++) {
			ot[i]->__otnext = ot[i+1];
		}
	}
}

void PPWorld::resetOT()
{
	int i;
	allocOT();
	for (i=0;i<depthOT()-1;i++) {
		ot[i]->__otnext = ot[i+1];
	}
	ot[i]->__otnext = NULL;
}

void PPWorld::addOT(PPObject* object)
{
	allocOT();
	if (object->isAlive() && object->isVisible()) {
		int depth = object->depth;
		if (depth >= 0 && depth < depthOT()) {
			int d = depthOT()-depth-1;
			object->__otnext = ot[d]->__otnext;
			ot[d]->__otnext = object;
		}
	}
}

void PPWorld::drawOT()
{
	allocOT();
	{
		PPObject* o = ot[0];
		while (o) {
			o->draw();
			o = o->__otnext;
		}
	}
}

unsigned int PPGetCharBytesUTF8(const char* string)
{
    const char ch = string[0];

    // マルチバイト文字かどうか
    if( ch & 0x80 ){

        // 先頭文字かどうか
        if( ch & 0x40 ){
            if( !(ch & 0x20 ) ){
                return 2;
            }
            else if( !(ch & 0x10) ){
                return 3;
            }
            else {
                return 4;
            }
        } else {
            // 不正な文字です
            return 0;
        }
    }
    else {
        return 1;
    }
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
