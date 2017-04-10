
#include "Game.h"
#include "Debug.h"
#include <iostream>

using ::Debug;

int main() 
{
	string boardPathFile = "Board.txt";

	// init log
	Debug::instance()->init("game.log", true, true, Debug::Error);
	DBG(Debug::Error, "hello world");

	Game& game = Game::getInstance();
	// add is initialize ?
	game.init(boardPathFile);
	game.startGame();
	int a;
	cin >> a;
	return 0;
}

