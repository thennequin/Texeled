#include "StringBuilder.h"

#include <stdlib.h> // for malloc/free
#include <string.h> // for strlen/memcpy

namespace Core
{
	StringBuilder::StringBuilder(MallocFuncPtr pMallocFunc, FreeFuncPtr pFreeFunc)
	{
		m_pMallocFunc = (pMallocFunc != NULL) ? pMallocFunc : malloc;
		m_pFreeFunc = (pFreeFunc != NULL) ? pFreeFunc : free;

		m_iPosition = 0;
		m_oFirstPool.m_pNext = NULL;
		m_pCurrentPool = &m_oFirstPool;
		m_iCurrentPoolIndex = 0;
	}

	StringBuilder::~StringBuilder()
	{
		Clear(true);
	}

	void StringBuilder::Clear(bool bFreePools)
	{
		m_iPosition = 0;
		m_pCurrentPool = &m_oFirstPool;
		m_iCurrentPoolIndex = 0;
		if (bFreePools)
		{
			Pool* pPool = m_oFirstPool.m_pNext;
			while (NULL != pPool)
			{
				Pool* pNextPool = pPool->m_pNext;
				m_pFreeFunc(pPool);
				pPool = pNextPool;
			}
		}
	}

	void StringBuilder::Append(char cChar)
	{
		AppendInternal(&cChar, 1);
	}

	void StringBuilder::Append(const char* pString)
	{
		AppendInternal(pString, (uint32)strlen(pString));
	}

	void StringBuilder::CopyTo(char* pDest)
	{
		if (pDest != NULL)
		{
			Pool* pCurrentPool = &m_oFirstPool;
			for (uint32 iPos = 0; iPos < m_iPosition; iPos += c_iPoolSize)
			{
				int iLen = m_iPosition - iPos;
				if (iLen > c_iPoolSize)
					iLen = c_iPoolSize;

				memcpy(pDest + iPos, pCurrentPool->m_oData, iLen);

				pCurrentPool = pCurrentPool->m_pNext;
			}
			*(pDest + m_iPosition) = 0;
		}
	}

	char* StringBuilder::Export()
	{
		if (m_iPosition == 0)
			return NULL;

		char* pOut = (char*)m_pMallocFunc(m_iPosition + 1);
		CopyTo(pOut);
		return pOut;
	}

	void StringBuilder::operator +=(char cChar)
	{
		Append(cChar);
	}

	void StringBuilder::operator +=(const char* pString)
	{
		Append(pString);
	}

#ifdef __STRINGBUILDER_USE_STD_STRING__
	void StringBuilder::Append(const std::string& oString)
	{
		AppendInternal(oString.c_str(), (uint32)oString.length());
	}

	void StringBuilder::CopyTo(std::string& sOut)
	{
		sOut.resize(m_iPosition);
		char* pOut = (char*)sOut.data();
		CopyTo(pOut);
	}

	void StringBuilder::operator +=(const std::string& oString)
	{
		Append(pString);
	}
#endif //__STRINGBUILDER_USE_STD_STRING__

	void StringBuilder::AppendInternal(const char* pString, uint32 iLen)
	{
		if (iLen == 0)
			return;

		// Copy pString to pools
		uint32 iPoolPos = m_iPosition - m_iCurrentPoolIndex * c_iPoolSize;
		uint32 iStrPos = 0;
		while (iStrPos < iLen)
		{
			if (iPoolPos >= c_iPoolSize)
			{
				if (m_pCurrentPool->m_pNext == NULL)
				{
					Pool* pNewPool = (Pool*)m_pMallocFunc(sizeof(Pool));
					pNewPool->m_pNext = NULL;
					m_pCurrentPool->m_pNext = pNewPool;
					m_pCurrentPool = pNewPool;
				}
				else
				{
					m_pCurrentPool = m_pCurrentPool->m_pNext;
				}

				++m_iCurrentPoolIndex;
				iPoolPos = iPoolPos - c_iPoolSize;
			}

			int iMaxCopySize = c_iPoolSize - iPoolPos;
			int iToCopySize = iLen - iStrPos;
			if (iToCopySize > iMaxCopySize)
				iToCopySize = iMaxCopySize;

			memcpy(m_pCurrentPool->m_oData + iPoolPos, pString + iStrPos, iToCopySize);

			iStrPos += iToCopySize;
			iPoolPos += iToCopySize;
		}

		m_iPosition += iLen;
	}

}