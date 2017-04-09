
#include "Game.h"
#include "Debug.h"

int main() 
{
	// init log
	Debug::instance()->init("game.log", true, true, Debug::DBG_ERROR);
	DBG(Debug::DBG_ERROR, "hello world");

	//Game& game = Game::getInstance();
	// add is initialize ?
	//game.init();
	//game.startGame();

	return 0;
}

