#include "Core/StringUtils.h"

#include "Core/Assert.h"

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

		void SubString(const char* pSource, size_t iSourceSize, char* pDest, size_t iDestSize, int iStart, int iLength)
		{
			size_t iSourceLen = StrLen(pSource, iSourceSize);
			if (iStart < 0)
			{
				iStart = iSourceLen + iStart + 1;
			}

			if (iLength < 0)
			{
				iLength = iSourceLen - iStart + iLength + 1;
			}

			CORE_ASSERT((size_t)(iStart + iLength) <= iSourceLen);
			CORE_ASSERT((size_t)(iLength) < iDestSize);

			const char* pStart = pSource + iStart;
			const char* pEnd = pSource + iStart + iLength;

			while (pStart != pEnd)
			{
				*pDest = *pStart;
				++pDest;
				++pStart;
			}
			*pDest = 0;
		}

		int FindFirst(const char* pSource, size_t iSourceSize, const char* pSearch, size_t iSearchSize)
		{
			if (pSource == NULL || iSourceSize == 0 || pSearch == NULL || iSearchSize == 0)
				return -1;

			const size_t iSourceLen = StrLen(pSource, iSourceSize);
			const size_t iSearchLen = StrLen(pSearch, iSearchSize);

			if (iSourceLen < iSearchLen)
				return -1;

			const size_t iMaxPos = iSourceLen - iSearchLen;
			size_t iPos = 0;
			while (iPos <= iMaxPos)
			{
				if (pSource[iPos] == 0)
					return -1;

				if (pSource[iPos] == pSearch[0])
				{
					size_t iSearchPos = 0;
					while (iSearchPos <= iSearchLen)
					{
						if (pSearch[iSearchPos] == 0)
							return iPos;
						else if (pSource[iPos + iSearchPos] != pSearch[iSearchPos])
							break;

						++iSearchPos;
					}
				}

				++iPos;
			}
			return -1;
		}

		int FindLast(const char* pSource, size_t iSourceSize, const char* pSearch, size_t iSearchSize)
		{
			if (pSource == NULL || iSourceSize == 0 || pSearch == NULL || iSearchSize == 0)
				return -1;

			const size_t iSourceLen = StrLen(pSource, iSourceSize);
			const size_t iSearchLen = StrLen(pSearch, iSearchSize);

			if (iSourceLen < iSearchLen)
				return -1;

			const size_t iMaxPos = iSourceLen - iSearchLen;
			size_t iPos = iMaxPos;
			while (iPos >= 0)
			{
				if (pSource[iPos] == 0)
					return -1;

				if (pSource[iPos] == pSearch[0])
				{
					size_t iSearchPos = 0;
					while (iSearchPos <= iSearchLen)
					{
						if (pSearch[iSearchPos] == 0)
							return iPos;
						else if (pSource[iPos + iSearchPos] != pSearch[iSearchPos])
							break;

						++iSearchPos;
					}
				}

				--iPos;
			}
			return -1;
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

		void GetReadableSize(size_t iSize, char* pOutBuffer, size_t iOutBufferSize)
		{
			static const char* const pSizes[] = { "bytes", "Kb", "Mb", "Gb" };
			size_t iDiv = 0;
			size_t iRem = 0;

			while (iSize >= 1024 && iDiv < (sizeof pSizes / sizeof *pSizes)) {
				iRem = (iSize % 1024);
				iDiv++;
				iSize /= 1024;
			}

			SNPrintf(pOutBuffer, iOutBufferSize, "%.1f %s\n", (double)iSize + (double)iRem / 1024.0, pSizes[iDiv]);
		}

	}
	//namespace StringUtils
}
//namespace Core