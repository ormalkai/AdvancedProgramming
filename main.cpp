
#include "Game.h"
#include "Debug.h"

int main() 
{
	// init log
	Debug::instance()->init("game.log", true, true, Debug::Error);
	DBG(Debug::Error, "hello world");

	//Game& game = Game::getInstance();
	// add is initialize ?
	//game.init();
	//game.startGame();

	return 0;
}

