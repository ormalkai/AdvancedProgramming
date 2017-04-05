
#include "Game.h"

int main() 
{

	Game& game = Game::getInstance();
	// add is initialize ?
	game.init();
	game.startGame();

	return 0;
}

