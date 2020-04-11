#ifndef __CORE_LOGGEROUTPUTERCONSOLE_H__
#define __CORE_LOGGEROUTPUTERCONSOLE_H__

#include "Core/Logger.h"

namespace Core
{
	class LoggerOutputerConsole : public Logger::LoggerOutputer
	{
	public:
		virtual void Log(Logger::Category eCategory, const char* pName, const char* pFormattedMessage);
	};
}

#endif //__CORE_LOGGEROUTPUTERCONSOLE_H__