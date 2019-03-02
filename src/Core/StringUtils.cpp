#include "Core/StringUtils.h"

#include <string.h> // strlen
#include <ctype.h> // toupper

namespace Core
{
	namespace StringUtils
	{
		bool EndsWith(const char* pString, const char* pEnd, bool bCaseSensitive)
		{
			if (pString == NULL || pEnd == NULL)
				return false;
			size_t iLenString = strlen(pString);
			size_t iLenEnd = strlen(pEnd);

			if (iLenString >= iLenEnd)
			{
				const char* pStart = pString + (iLenString - iLenEnd);

				if (bCaseSensitive)
				{
					while (*pStart == *pEnd && *pStart != 0)
					{
						++pStart;
						++pEnd;
					}
				}
				else
				{
					while (toupper(*pStart) == toupper(*pEnd) && *pStart != 0)
					{
						++pStart;
						++pEnd;
					}
				}

				return *pStart == 0;
			}

			return false;
		}
	}
	//namespace StringUtils
}
//namespace Core