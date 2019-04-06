#include "Core/StringUtils.h"

#include <string.h> // strlen
#include <ctype.h> // toupper
#include <stdlib.h> // malloc
#include <stdio.h> // vsnprintf

namespace Core
{
	namespace StringUtils
	{
		size_t StrLen(const char* pString, size_t iBufferSize)
		{
			if (pString == 0)
				return 0;
			size_t iSize = 0;
			while (*pString != 0 && iSize < iBufferSize)
			{
				iSize++;
				pString++;
			}
			return iSize;
		}

		char* StrDup(const char* pString)
		{
			char* pDup = (char*)malloc(strlen(pString) + 1);
			if (pDup != NULL)
			{
				strcpy(pDup, pString);
			}
			return pDup;
		}

		int SNPrintf(char* pBuffer, size_t iBufferSize, const char* pFormat, ...)
		{
			va_list oArgs;
			va_start(oArgs, pFormat);
			int iLen = vsnprintf(pBuffer, iBufferSize, pFormat, oArgs);
			va_end(oArgs);
			return iLen;
		}

		int	VSNPrintf(char* pBuffer, size_t iBufferSize, const char* pFormat, va_list oArgs)
		{
			return vsnprintf(pBuffer, iBufferSize, pFormat, oArgs);
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

		void GetReadableSize(int iSize, char* pOutBuffer, size_t iOutBufferSize)
		{
			static const char* const pSizes[] = { "bytes", "Kb", "Mb", "Gb" };
			int iDiv = 0;
			int iRem = 0;

			while (iSize >= 1024 && iDiv < (sizeof pSizes / sizeof *pSizes)) {
				iRem = (iSize % 1024);
				iDiv++;
				iSize /= 1024;
			}

			SNPrintf(pOutBuffer, iOutBufferSize, "%.1f %s\n", (float)iSize + (float)iRem / 1024.0, pSizes[iDiv]);
		}

	}
	//namespace StringUtils
}
//namespace Core