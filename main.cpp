#include "Game.h"
#include "Debug.h"
#include <iostream>
#include <algorithm>
#include <locale>

void parseArgs(int argc, char* argv[], string& filesLocation, bool& isQuiet, int& delay)
{
	bool isPathInitialized = false;
	bool isDelayInitialized = false;
	bool isQuietInitialized = false;

	// set defaults
	filesLocation = "./";
	isQuiet = false;
	delay = 2000;

	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);
		if (false == isQuietInitialized && "-quiet" == arg)
		{
			isQuietInitialized = true;
		}
		else if (false == isDelayInitialized && "-delay" == arg)
		{
			if ((i+1) == argc)
			{
				cout << "-delay configured with no value - using default" << endl;
			}
			i++;
			string delatStr(argv[i]);
			int delay = std::stoi(delatStr);
			isDelayInitialized = true;
		}
		else /* this is path */
		{
			filesLocation = arg + "/";
		}
	}
}

int main(int argc, char* argv[])
{
	// init log
	Debug::instance()->init("game.log", false, true, Debug::DBG_DEBUG);

	Game& game = Game::getInstance();
	// add is initialize ?
	//string filesLocation("C:\\Users\\ormalkai\\Desktop\\AP_tests\\TestFiles\\Good Boards\\good_board_0.sboard");
	//string filesLocation("C:\\Users\\ormalkai\\Desktop\\AP_tests\\TestFiles\\Bad Boards\\bad_board_1.sboard");
	//string filesLocation("C:\\Users\\ormalkai\\Desktop\\AP_tests\\TestFiles\\Bad Boards");
	//string filesLocation("C:/Users/ormalkai/Desktop/AP_tests/TestFiles/Bad Boards/");

	string filesLocation;
	bool isQuiet;
	int delay;
	parseArgs(argc, argv, filesLocation, isQuiet, delay);

	game.init(filesLocation, isQuiet, delay);
	game.startGame();

	int number;
	std::cin >> number;

	return 0;
}