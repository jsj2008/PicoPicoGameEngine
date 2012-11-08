/*-----------------------------------------------------------------------------------------------
	名前	PPAnimationMap.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPAnimationMap_h
#define picopico_PPAnimationMap_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

//#include "PPKeyFrame.h"
#include "PPGameMap.h"
#include "PPAnimation.h"
#include <string>

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPAnimationMap {
public:
	PPAnimationMap(const char* filename,const char* name=NULL);
	virtual ~PPAnimationMap();
	
	PPSize size(const char* name) {
		PPSize s(0,0);
		int o = offset(name);
		if (o >= 0) {
//			s = PPSize(array[o]->width,array[o]->height);
		}
		return s;
	}
	
	int offset(const char* name) {
		for (int i=0;i<count;i++) {
			if (event[i]) {
				if (event[i]->compare(name) == 0) {
					return i;
				}
			}
		}
		return -1;
	}
	
	static PPAnimationMap* getAnimationMap(const char* name);
	static void deleteAnimationMap(const char* name);

	virtual void setName(const char* name) {
		if (_name) delete _name;
		_name = new std::string(name);
	}
	virtual const char* name() {return _name->c_str();}
	
	int index;

private:
	int count;
	std::string** event;
	
	std::string* _name;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
