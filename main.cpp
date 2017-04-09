
#include "Game.h"
#include "Debug.h"
#include <iostream>

int main(int argc, char** argv) 
{
	// init log
	Debug::instance()->init("game.log", false, true, Debug::DBG_ERROR);
	DBG(Debug::DBG_ERROR, "hello world");

	Game& game = Game::getInstance();
	// add is initialize ?
	//string filesLocation("C:\\Users\\ormalkai\\Desktop\\AP_tests\\TestFiles\\Good Boards\\good_board_0.sboard");
	string filesLocation("C:\\Users\\ormalkai\\Desktop\\AP_tests\\TestFiles\\Bad Boards\\bad_board_1.sboard");
	game.init(filesLocation);
	//game.startGame();

	int number;
	std::cin >> number;

	return 0;
}

