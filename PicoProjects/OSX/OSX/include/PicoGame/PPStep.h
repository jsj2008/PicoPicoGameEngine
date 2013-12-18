/*-----------------------------------------------------------------------------------------------
	名前	PPStep.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPStep_h
#define picopico_PPStep_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdlib.h>

#define NEXT(method) stepNext((PP_SELECTOR)(&method),0)
#define WAIT(method,time) stepNext((PP_SELECTOR)(&method),time)

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPStep;

typedef void (PPStep::*PP_SELECTOR)();

class PPStep {
public:
	PPStep() {
		step = 0;
		ppstep_timer = 0;
		ppstep_waitTimer = 0;
		idleSelector = NULL;
	}
	virtual ~PPStep() {
	}
	
	virtual void start() {
		step = 0;
		ppstep_timer = 0;
		ppstep_waitTimer = 0;
		idleSelector = NULL;
	}

	virtual void idle() {
		if (idleSelector) {
			(this->*idleSelector)();
		}
	}
	
	virtual void setIdle(PP_SELECTOR selector) {
		idleSelector = selector;
	}

	virtual bool stepNext(PP_SELECTOR selector,float wait=0) {
		bool r = false;
		if (idleSelector != selector) {
			if (wait == 0) {
				idleSelector = selector;
				r = true;
				step = 0;
				ppstep_timer = 0;
				ppstep_waitTimer = 0;
			} else {
				ppstep_waitTimer += 1.0;
				if (ppstep_waitTimer >= wait) {
					ppstep_waitTimer = 0;
					idleSelector = selector;
					r = true;
					step = 0;
					ppstep_timer = 0;
				}
			}
		}
		return r;
	}

	int step;

protected:
	PP_SELECTOR idleSelector;
	
	int ppstep_timer;
	float ppstep_waitTimer;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
