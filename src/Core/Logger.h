#ifndef __CORE_LOGGER_H__
#define __CORE_LOGGER_H__

#include <stdarg.h> // va_list

namespace Core
{
	namespace Logger
	{
		struct _Category
		{
			enum Enum
			{
				Debug,
				Info,
				Warning,
				Error
			};
		};
		typedef _Category::Enum Category;

		class LoggerOutputer
		{
		public:
			virtual void Log(Category eCategory, const char* pName, const char* pFormattedMessage) = 0;
		};

		void RegisterLoggerOutputer(LoggerOutputer* pOutputer);

		void LogV(Category eCategory, const char* pName, const char* pMessage, va_list args);
		void Log(Category eCategory, const char* pName, const char* pMessage, ...);
	};

	void LogDebug(const char* pName, const char* pMessage, ...);
	void LogInfo(const char* pName, const char* pMessage, ...);
	void LogWarning(const char* pName, const char* pMessage, ...);
	void LogError(const char* pName, const char* pMessage, ...);
}

#endif // __CORE_LOGGER_H__