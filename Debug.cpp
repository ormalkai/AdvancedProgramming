#include <cstdarg>
#include "Debug.h"

Debug::Debug(): m_logFile("game.log"), m_printToLog(false), m_printToStd(false), m_debugLevel(DBG_ERROR), m_pFile(nullptr)
{
}

Debug::~Debug()
{
	if (nullptr != m_pFile)
	{
		fclose(m_pFile);
	}
}

Debug& Debug::instance()
{
	static Debug Instance;
	return Instance;
}

void Debug::init(string logFile, bool printToLog, bool printToStd, DebugLevel debugLevel)
{
	m_logFile = logFile;
	m_printToLog = printToLog;
	m_printToStd = printToStd;
	m_debugLevel = debugLevel;
	if (true == m_printToLog)
	{
		int rc = fopen_s(&m_pFile, m_logFile.c_str(), "a");
		if (0 != rc)
		{
			m_printToLog = false;
		}
		else
		{
			DBG(DBG_MSG, "========================= Starting Tournament =========================");
		}
	}
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

void Debug::vprint(const char* fmt, va_list ap)
{
	lock_guard<std::mutex> lock(m_printMutex);
	if (fmt == nullptr || ap == nullptr)
		return;

	if (true == m_printToStd)
	{
		vfprintf(stderr, fmt, ap);
		fputc('\n', stderr);
	}
	if (true == m_printToLog)
	{
		int nWritten = vfprintf(m_pFile, fmt, ap);
		if (nWritten >= 0)
		{
			fprintf(m_pFile, "\n");
		}
		fflush(m_pFile);
	}
}
