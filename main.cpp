#include <iostream>
#include <locale>
#include <Windows.h>
#include "Game.h"
#include "Debug.h"
#include "BoardBuilder.h"
#include "Tournament.h"
#include <codecvt>

#define INI_IS_QUIET_DEFAULT (0)
#define INI_DELAY_DEFAULT (2000)
#define INI_NUM_THREADS_DEFAULT (4)

void parseIni(string& filesLocation, bool& isQuiet, int& delay, int& numThreads)
{
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wstring fileName = converter.from_bytes(filesLocation) + L"conf.ini";
	wstring generalSection = L"general";
	wstring isQuietKey = L"isQuiet";
	wstring delayKey = L"delay";
	wstring numThreadsKey = L"numThreads";
	isQuiet = GetPrivateProfileInt(generalSection.c_str(), isQuietKey.c_str(), INI_IS_QUIET_DEFAULT, fileName.c_str());
	delay = GetPrivateProfileInt(generalSection.c_str(), delayKey.c_str(), INI_DELAY_DEFAULT, fileName.c_str());
	numThreads = GetPrivateProfileInt(generalSection.c_str(), numThreadsKey.c_str(), INI_NUM_THREADS_DEFAULT, fileName.c_str());
}

void parseArgs(int argc, char* argv[], string& filesLocation, bool& isQuiet, int& delay, int& numThreads)
{
	bool isDelayInitialized = false;
	bool isQuietInitialized = false;
	bool isNumThreadsInitialized = false;

	// set defaults
	filesLocation = ".\\";

	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);
		if ("-quiet" == arg)
		{
			isQuiet = true;
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
			delay = stoi(delatStr);
			isDelayInitialized = true;
		}
		else if (false == isNumThreadsInitialized && "-threads" == arg)
		{
			if ((i + 1) == argc)
			{
				cout << "-threads configured with no value - using default" << endl;
			}
			i++;
			string threadsStr(argv[i]);
			delay = stoi(threadsStr);
			isNumThreadsInitialized = true;
		}
		else /* this is path */
		{
			filesLocation = arg;

			if (arg.at(arg.length() - 1) != '\\' && arg.at(arg.length() - 1) != '/')
				filesLocation = arg + "\\";
		}
	}

	// get conf from ini and replace default if needed
	bool isQuietIni;
	int delayIni;
	int numThreadsIni;
	parseIni(filesLocation, isQuietIni, delayIni, numThreadsIni);
	if (false == isQuietInitialized) isQuiet = isQuietIni;
	if (false == isDelayInitialized) delay = delayIni;
	if (false == isNumThreadsInitialized) numThreads = numThreadsIni;
}



int main(int argc, char* argv[])
{
	bool isQuiet;
	int delay;
	string filesLocation;
	int numThreads;
	parseArgs(argc, argv, filesLocation, isQuiet, delay, numThreads);

	// init log
	Debug::instance().init(filesLocation + "game.log", true, true, Debug::DBG_INFO);

	Tournament& tournament = Tournament::instance();
	
	tournament.init(filesLocation);
	tournament.startTournament(numThreads);
	
	
	/*Game& game = Game::instance();

	
	

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