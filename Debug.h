#include <string>

#define DBG( LEVEL , FMT, ... ) \
	do\
	{\
		switch(LEVEL)\
		{\
		case(Debug::DBG_ERROR):\
			Debug::instance()->print(LEVEL, "Error  " ": " FMT, ## __VA_ARGS__);\
			break;\
		case(Debug::DBG_WARNING):\
			Debug::instance()->print(LEVEL, "Warning" ": " FMT, ## __VA_ARGS__);\
			break;\
		case(Debug::DBG_INFO):\
			Debug::instance()->print(LEVEL, "Info   " ": " FMT, ## __VA_ARGS__);\
			break;\
		case(Debug::DBG_DEBUG):\
			Debug::instance()->print(LEVEL, "Debug  " ": " FMT, ## __VA_ARGS__);\
			break;\
		default:\
			Debug::instance()->print(LEVEL, "Error  " ": " FMT, ## __VA_ARGS__);\
		}\
	} while (0);

using namespace std;


class Debug
{
public:
	enum DebugLevel
	{
		DBG_ERROR,
		DBG_WARNING,
		DBG_INFO,
		DBG_DEBUG,
		DBG_MAX
	};
	static Debug* instance();
	void init(string logFile, bool printToLog, bool printToStd, DebugLevel debugLevel);
	void printWithDebugLevel(DebugLevel debugLevel, const char *fmt, ...);
	void print(DebugLevel debugLevel, const char *fmt, ...);

private:
	Debug();
	~Debug();
	Debug(const Debug&);
	void operator=(const Debug&);
	string	m_logFile;
	bool	m_printToLog;
	bool	m_printToStd;
	DebugLevel m_debugLevel;
	void vprint(const char *fmt, va_list ap);
};