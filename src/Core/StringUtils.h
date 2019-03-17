#ifndef __CORE_STRING_UTILS_H__
#define __CORE_STRING_UTILS_H__

namespace Core
{
	namespace StringUtils
	{
		char* StrDup(const char* pString);
		bool EndsWith(const char* pString, const char* pEnd, bool bCaseSensitive = true);
		bool Wildcard(const char* pPattern, const char* pString);
	}
	//namespace StringUtils
}
//namespace Core

#endif //__CORE_STRING_UTILS_H__