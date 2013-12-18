//
//  PicoGame.h
//  PicoGame
//
//  Created by 山口 博光 on 2013/11/13.
//
//

#ifndef __PicoGame__PicoGame__
#define __PicoGame__PicoGame__

#include <iostream>
#import <PicoGame/PPScriptGame.h>

class PicoGame : public PPScriptGame {
public:
	PicoGame();
	virtual ~PicoGame();

	virtual void initScript();
};


#endif /* defined(__PicoGame__PicoGame__) */
