#include <string>

/*
 * @Details		This macro is print easy printing to log and or to stdout
 * @Example		DBG(Debug::DBG_ERROR, "%s", "This is an example");
 */
#define DBG( LEVEL , FMT, ... ) \
	do\
	{\
		switch(LEVEL)\
		{\
		case(Debug::DBG_ERROR):\
			Debug::instance().print(LEVEL, "Error  " ": " FMT, ## __VA_ARGS__);\
			break;\
		case(Debug::DBG_WARNING):\
			Debug::instance().print(LEVEL, "Warning" ": " FMT, ## __VA_ARGS__);\
			break;\
		case(Debug::DBG_INFO):\
			Debug::instance().print(LEVEL, "Info   " ": " FMT, ## __VA_ARGS__);\
			break;\
		case(Debug::DBG_DEBUG):\
			Debug::instance().print(LEVEL, "Debug  " ": " FMT, ## __VA_ARGS__);\
			break;\
		default:\
			Debug::instance().print(LEVEL, "Error  " ": " FMT, ## __VA_ARGS__);\
		}\
	} while (0);

using namespace std;


class Debug
{
public:
	/**
	 * @Details		Debug level enum (error, warning, info, debug)
	 */
	enum DebugLevel
	{
		DBG_ERROR,
		DBG_WARNING,
		DBG_INFO,
		DBG_DEBUG,
		DBG_MAX
	};

	/**
	 * @Details		Get the instance (singleton)
	 */
	static Debug& instance();

	/**
	 * @Details		Init the debugger
	 * @param		logFile - name of the log file
	 * @param		printToLog - determines if printouts should be printed to log or not
	 * @param		printToStd - determines if printouts should be printed to stdoutor not
	 * @param		debugLevel - printouts from level equal or lower should be printed
	 */
	void init(string logFile, bool printToLog, bool printToStd, DebugLevel debugLevel);

	/**
	 * @Details		print function
	 * @param		debugLevel - level of current printout
	 * @param		fmt - format of the print
	 * @param		... - arguments for the format
	 */
	void print(DebugLevel debugLevel, const char* fmt, ...) const;

private:
	/**
	 * @Details		Init constructor with default values: 
	 *				m_logFile("game.log"), m_printToLog(true), m_printToStd(true), m_debugLevel(DBG_ERROR)
	 */
	Debug();

	~Debug();

	/**
	 * @Details		Deleted copy constructor
	 */
	Debug(const Debug&) = delete;

	/**
	 * @Details		ignore assignment
	 */
	Debug& operator=(const Debug&) = delete;

	Debug(Debug &&) = delete;

	string m_logFile; // path to the lo file
	bool m_printToLog; // print to log file or not
	bool m_printToStd; // print to std or not
	DebugLevel m_debugLevel; // minimum debug level
	FILE* m_pFile;


	/**
	 * @Details		assistent prin function
	 */
	void vprint(const char* fmt, va_list ap) const;
};
