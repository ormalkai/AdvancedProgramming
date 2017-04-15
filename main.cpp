#include "Game.h"
#include "Debug.h"
#include <iostream>
#include <locale>

void parseArgs(int argc, char* argv[], string& filesLocation, bool& isQuiet, int& delay)
{
	bool isPathInitialized = false;
	bool isDelayInitialized = false;

	// set defaults
	filesLocation = "./";
	isQuiet = DEFAULT_IS_QUIET_MODE;
	delay = DEFAULT_DELAY_ATTACK;

	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);
		if (false == isQuiet && "-quiet" == arg)
		{
			isQuiet = true;
		}
		else if (false == isDelayInitialized && "-delay" == arg)
		{
			if ((i+1) == argc)
			{
				cout << "-delay configured with no value - using default" << endl;
			}
			i++;
			string delatStr(argv[i]);
			delay = std::stoi(delatStr);
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
	Debug::instance()->init("game.log", true, false, Debug::DBG_MAX);

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

	ReturnCode rc = game.init(filesLocation, isQuiet, delay);
	if (RC_SUCCESS != rc)
		return RC_ERROR;

	game.startGame();

	int number;
	std::cin >> number;

	return 0;
}