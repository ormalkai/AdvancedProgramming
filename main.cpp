#include "Game.h"
#include "Debug.h"
#include <iostream>
#include <locale>
#include "BoardBuilder.h"
#include "Tournament.h"

void parseArgs(int argc, char* argv[], string& filesLocation, bool& isQuiet, int& delay)
{
	bool isDelayInitialized = false;

	// set defaults
	filesLocation = ".\\";
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
			filesLocation = arg;

			if (arg.at(arg.length() - 1) != '\\' && arg.at(arg.length() - 1) != '/')
				filesLocation = arg + "\\";
		}
	}
}

int main(int argc, char* argv[])
{
	bool isQuiet;
	int delay;
	string filesLocation;
	parseArgs(argc, argv, filesLocation, isQuiet, delay);

	// init log
	Debug::instance().init("game.log", true, false, Debug::DBG_INFO);

	Tournament& tournament = Tournament::instance();
	
	tournament.init(filesLocation, 4);
	tournament.startTournament();
	
	
	Game& game = Game::instance();

	
	

	system("cls");
	
	BoardBuilder* bb = new BoardBuilder(filesLocation + "board.sboard");
	vector<vector<vector<char>>> v; 
	ReturnCode rc = bb->parseBoardFile(v);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}
	
	string dllFolder = "C:\\Users\\USER\\Documents\\GitHub\\Ex1\\Proj\\Ex1\\x64\\Debug\\";
	game.loadAllAlgoFromDLLs({ dllFolder + "or.dll", dllFolder + "gal.dll" });
	IBattleshipGameAlgo* ibg1 = get<1>(game.m_algoDLLVec[PLAYER_A])();
	unique_ptr<IBattleshipGameAlgo> p1(ibg1);

	IBattleshipGameAlgo* ibg2 = get<1>(game.m_algoDLLVec[PLAYER_B])();
	unique_ptr<IBattleshipGameAlgo> p2(ibg2);

	rc = game.init(v,move(p1), move(p2));
	game.startGame();
	
	/*ReturnCode rc = game.init(filesLocation, isQuiet, delay);
	if (RC_SUCCESS != rc)
		return RC_ERROR;

	game.startGame();
	*/
	int a;
	cin >> a;
	return 0;
}