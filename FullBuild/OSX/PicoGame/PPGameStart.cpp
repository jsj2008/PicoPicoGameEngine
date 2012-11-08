#include <PicoGame/PPGameStart.h>
#include <PicoGame/PPScriptGame.h>

QBGame* PPGameStart()
{
	return new PPScriptGame();
}
