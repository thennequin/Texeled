#include "Core/LoggerOutputerConsole.h"

#include <stdio.h> // printf
#include <math.h> // lrint
#include <time.h> // strftime

//TODO : To move in another file
#ifdef _MSC_VER

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64

// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval * tp, struct timezone * /*tzp*/)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}

#endif //_MSC_VER

namespace Core
{
	void LoggerOutputerConsole::Log(Logger::Category eCategory, const char* pName, const char* pFormattedMessage)
	{
		struct timeval oTimeVal;
		gettimeofday(&oTimeVal, NULL);

		// Round to nearest millisec
		int iMs = lrint(oTimeVal.tv_usec / 1000.0);
		// Allow for rounding up to nearest second
		if (iMs >= 1000)
		{
			iMs -= 1000;
			oTimeVal.tv_sec++;
		}

		time_t iSeconds = oTimeVal.tv_sec;
		struct tm* oTimeInfo = localtime(&iSeconds);

		char pDateBuffer[26];
		strftime(pDateBuffer, 26, "%Y:%m:%d %H:%M:%S", oTimeInfo);

		WORD iColor;
		const char* pCategory;
		switch (eCategory)
		{
		case Logger::Category::Debug:
			pCategory = "Debug";
			iColor = FOREGROUND_INTENSITY;
			break;
		default:
		case Logger::Category::Info:
			pCategory = "Info";
			iColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		case Logger::Category::Warning:
			pCategory = "Warning";
			iColor = FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case Logger::Category::Error:
			pCategory = "Error";
			iColor = FOREGROUND_RED;
			break;
		}
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, iColor);
		printf("%s [%s] %s : %s\n", pDateBuffer, pCategory, pName, pFormattedMessage);
	};
}