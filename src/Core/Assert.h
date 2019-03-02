#ifndef __CORE_ASSERT_H__
#define __CORE_ASSERT_H__

namespace Core
{
	void Assert(bool bCondition, const char* pCondition, const char* pFile, int iLine, const char* pFunction, const char* pFormat, ...);
}
//Core

#define CORE_ASSERT_STRINGIFY1(arg) #arg
#define CORE_ASSERT_STRINGIFY(arg) CORE_ASSERT_STRINGIFY1(arg)

#ifdef _DEBUG
#	define CORE_ASSERT(bCondition, ...) { Core::Assert((bCondition), CORE_ASSERT_STRINGIFY(bCondition), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__, ""); }
#else
#	define CORE_ASSERT(bCondition, ...)
#endif //_DEBUG

#define CORE_VERIFY(bCondition, ...) { Core::Assert((bCondition), CORE_ASSERT_STRINGIFY(bCondition), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__, ""); }

#endif //__CORE_ASSERT_H__