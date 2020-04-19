#include "Core/Logger.h"

#include "Core/Array.h"
#include "Core/StringUtils.h"

#include <stdio.h> // printf
#include <time.h>

namespace Core
{
	namespace Logger
	{
		Array<LoggerOutputer*, false> s_oOutputer;

		void RegisterLoggerOutputer(LoggerOutputer* pOutputer)
		{
			s_oOutputer.push_back(pOutputer);
		}

		void LogV(Category eCategory, const char* pName, const char* pMessage, va_list oArgs)
		{
			size_t iLen = StringUtils::VSNPrintf(NULL, 0, pMessage, oArgs);
			char pHeapBuffer[2048];
			char* pBuffer = pHeapBuffer;
			if (iLen >= sizeof(pBuffer))
			{
				pBuffer = (char*)malloc(iLen + 1);
			}
			StringUtils::VSNPrintf(pBuffer, iLen + 1, pMessage, oArgs);
			for (size_t iIndex = 0; iIndex < s_oOutputer.size(); ++iIndex)
			{
				s_oOutputer[iIndex]->Log(eCategory, pName, pBuffer);
			}

			if (iLen >= sizeof(pBuffer))
			{
				free(pBuffer);
			}
		}

		void Log(Category eCategory, const char* pName, const char* pMessage, ...)
		{
			va_list oArgs;
			va_start(oArgs, pMessage);
			LogV(eCategory, pName, pMessage, oArgs);
			va_end(oArgs);
		}
	};

	void LogDebug(const char* pName, const char* pMessage, ...)
	{
		va_list oArgs;
		va_start(oArgs, pMessage);
		Logger::LogV(Logger::Category::Debug, pName, pMessage, oArgs);
		va_end(oArgs);
	}

	void LogInfo(const char* pName, const char* pMessage, ...)
	{
		va_list oArgs;
		va_start(oArgs, pMessage);
		Logger::LogV(Logger::Category::Info, pName, pMessage, oArgs);
		va_end(oArgs);
	}

	void LogWarning(const char* pName, const char* pMessage, ...)
	{
		va_list oArgs;
		va_start(oArgs, pMessage);
		Logger::LogV(Logger::Category::Warning, pName, pMessage, oArgs);
		va_end(oArgs);
	}

	void LogError(const char* pName, const char* pMessage, ...)
	{
		va_list oArgs;
		va_start(oArgs, pMessage);
		Logger::LogV(Logger::Category::Error, pName, pMessage, oArgs);
		va_end(oArgs);
	}
}