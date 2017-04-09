
#include "Game.h"
#include "Debug.h"
#include <iostream>

int main(int argc, char** argv) 
{
	// init log
	Debug::instance()->init("game.log", true, true, Debug::DBG_ERROR);
	DBG(Debug::DBG_ERROR, "hello world");
	int number;
	std::cin >> number;

	//Game& game = Game::getInstance();
	// add is initialize ?
	//game.init();
	//game.startGame();

	return 0;
}

