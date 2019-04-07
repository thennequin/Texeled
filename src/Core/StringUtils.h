#ifndef __CORE_STRING_UTILS_H__
#define __CORE_STRING_UTILS_H__

#include <stdarg.h>

namespace Core
{
	namespace StringUtils
	{
		size_t					StrLen(const char* pString, size_t iBufferSize);
		char*					StrDup(const char* pString);
		int						FindFirst(const char* pSource, size_t iSourceSize, const char* pSearch, size_t iSearchSize);
		int						FindLast(const char* pSource, size_t iSourceSize, const char* pSearch, size_t iSearchSize);

		int						SNPrintf(char* pBuffer, size_t iBufferSize, const char* pFormat, ...);
		int						VSNPrintf(char* pBuffer, size_t iBufferSize, const char* pFormat, va_list oArgs);

		bool					EndsWith(const char* pString, const char* pEnd, bool bCaseSensitive = true);
		bool					Wildcard(const char* pPattern, const char* pString);

		void					GetReadableSize(size_t iSize, char* pOutBuffer, size_t iOutBufferSize);
	}
	//namespace StringUtils
}
//namespace Core

#endif //__CORE_STRING_UTILS_H__