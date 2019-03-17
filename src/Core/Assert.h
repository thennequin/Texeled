#ifndef __CORE_ASSERT_H__
#define __CORE_ASSERT_H__

namespace Core
{
	bool Assert(bool bCondition, const char* pCondition, const char* pFile, int iLine, const char* pFunction, const char* pFormat, ...);
}
//Core

#define CORE_ASSERT_STRINGIFY1(arg) #arg
#define CORE_ASSERT_STRINGIFY(arg) CORE_ASSERT_STRINGIFY1(arg)

#ifdef _DEBUG
#	define CORE_ASSERT(bCondition, ...) { if (Core::Assert((bCondition), CORE_ASSERT_STRINGIFY(bCondition), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__, "")) __debugbreak(); }
#else
#	define CORE_ASSERT(bCondition, ...)
#endif //_DEBUG

#define CORE_VERIFY(bCondition, ...) { if (Core::Assert((bCondition), CORE_ASSERT_STRINGIFY(bCondition), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__, "")) __debugbreak(); }
#define CORE_VERIFY_OK(bCondition, ...) { ErrorCode oRes = (bCondition); if (Core::Assert(oRes == ErrorCode::Ok, CORE_ASSERT_STRINGIFY(bCondition), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__, "%s", oRes.ToString())) __debugbreak(); }
#define CORE_NOT_IMPLEMENTED() { if (Core::Assert(false, "Not implemented", __FILE__, __LINE__, __FUNCTION__, "Not implemented")) __debugbreak(); }

#endif //__CORE_ASSERT_H__