
#include "Debug.h"
#include <cstdarg>

Debug::Debug(): m_logFile("game.log"), m_printToLog(true), m_printToStd(true), m_debugLevel(Error){}

Debug* Debug::instance()
{
	static Debug *Instance = new Debug();
	return  Instance;
}

void Debug::init(string logFile, bool printToLog, bool printToStd, DebugLevel debugLevel)
{
	m_logFile = logFile;
	m_printToLog = printToLog;
	m_printToStd = printToStd;
	m_debugLevel = debugLevel;
}

void Debug::print(DebugLevel debugLevel, const char* fmt, ...)
{
	if (debugLevel > m_debugLevel)
	{
		return;
	}
	
	va_list ap;
	va_start(ap, fmt);
	vprint(fmt, ap);
	va_end(ap);
}

void Debug::vprint(const char *fmt, va_list ap)
{
	if (fmt == NULL || ap == NULL)
		return;

	if (true == m_printToStd)
	{
		vfprintf(stderr, fmt, ap);
		fputc('\n', stderr);
	}
	if (true == m_printToLog)
	{
		FILE * pFile;
		int rc = fopen_s(&pFile, m_logFile.c_str(), "w");
		// TODO handle error
		int nWritten = vfprintf(pFile, fmt, ap);
		if (nWritten >= 0)
		{
			fprintf(pFile, "\n");
		}
		fflush(pFile);
		fclose(pFile);
	}
}
