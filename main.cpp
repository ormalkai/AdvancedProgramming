#include <iostream>
#include <locale>
#include <Windows.h>
#include <codecvt>
#include "Debug.h"
#include "Tournament.h"

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
	isQuiet = GetPrivateProfileInt(generalSection.c_str(), isQuietKey.c_str(), INI_IS_QUIET_DEFAULT, fileName.c_str()) != 0;
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
			if ((i + 1) == argc)
			{
				cout << "-delay configured with no value - using default" << endl;
			}
			i++;
			string delatStr(argv[i]);
			try
			{
				delay = stoi(delatStr);
			}
			catch (exception&)
			{
				isDelayInitialized = false;
				continue;
			}
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
			try
			{
				numThreads = stoi(threadsStr);
			}
			catch (exception&)
			{
				isNumThreadsInitialized = false;
				continue;
			}
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
	Debug::instance().init(filesLocation + "game.log", true, false, Debug::DBG_DEBUG);
	DBG(Debug::DBG_INFO, "path [%s]", filesLocation.c_str());
	DBG(Debug::DBG_INFO, "threads [%d]", numThreads);
	Tournament& tournament = Tournament::instance();


	ReturnCode rc = tournament.init(filesLocation);
	if (RC_SUCCESS != rc)
	{
		DBG(Debug::DBG_ERROR, "Failed to init tournament");
		return rc;
	}
	tournament.startTournament(numThreads);

	int a;
	cin >> a;
	return 0;
}
