
#include <string>

#define DBG( LEVEL , FMT, ... ) \
	Debug::instance()->print(LEVEL, #LEVEL ": " FMT, __func__, ## __VA_ARGS__) ;

using namespace std;

class Debug
{
public:
	enum DebugLevel
	{
		Error,
		Warning,
		Info,
		Dbg,
	};
	static Debug* instance();
	void init(string logFile, bool printToLog, bool printToStd, DebugLevel debugLevel);
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