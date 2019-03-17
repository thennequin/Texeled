#include "Core/StringUtils.h"

#include <string.h> // strlen
#include <ctype.h> // toupper
#include <stdlib.h> // malloc

namespace Core
{
	namespace StringUtils
	{
		char* StrDup(const char* pString)
		{
			char* pDup = (char*)malloc(strlen(pString) + 1);
			if (pDup != NULL)
			{
				strcpy(pDup, pString);
			}
			return pDup;
		}

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

		//https://github.com/clibs/wildcardcmp/
		bool Wildcard(const char* pPattern, const char* pString)
		{
			const char* pLastStart = NULL; // last `*`
			const char* pLastChar = NULL; // last checked char

			//Invalid
			if (pPattern == NULL || pString == NULL)
				return false;

			// loop 1 char at a time
			while (1)
			{
				if (*pString == NULL)
				{
					if (*pPattern == NULL)
						return true;
					if (*pLastChar == NULL)
						return false;
					pString = pLastChar++;
					pPattern = pLastStart;
					continue;
				}
				else
				{
					if (*pPattern != *pString)
					{
						if ('*' == *pPattern)
						{
							pLastStart = ++pPattern;
							pLastChar = pString;
							// "*" -> "foobar"
							if (*pPattern != NULL)
								continue;
							return false;
						}
						else if (pLastStart != NULL)
						{
							pString++;
							// "*ooba*" -> "foobar"
							continue;
						}
						return false;
					}
				}

				pString++;
				pPattern++;
			}

			return true;
		}
	}
	//namespace StringUtils
}
//namespace Core