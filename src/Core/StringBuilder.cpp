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
		AppendInternal(pString, (size_t)strlen(pString));
	}

	void StringBuilder::Append(const Core::String& oString)
	{
		AppendInternal(oString.c_str(), (size_t)oString.size());
	}

	void StringBuilder::CopyTo(char* pDest)
	{
		if (pDest != NULL)
		{
			Pool* pCurrentPool = &m_oFirstPool;
			for (size_t iPos = 0; iPos < m_iPosition; iPos += c_iPoolSize)
			{
				size_t iLen = m_iPosition - iPos;
				if (iLen > c_iPoolSize)
					iLen = c_iPoolSize;

				memcpy(pDest + iPos, pCurrentPool->m_oData, iLen);

				pCurrentPool = pCurrentPool->m_pNext;
			}
			*(pDest + m_iPosition) = 0;
		}
	}

	void StringBuilder::CopyTo(Core::String& sOut)
	{
		sOut.resize(m_iPosition);
		char* pOut = (char*)sOut.c_str();
		CopyTo(pOut);
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

	void StringBuilder::operator +=(const Core::String& oString)
	{
		Append(oString);
	}

	void StringBuilder::AppendInternal(const char* pString, size_t iLen)
	{
		if (iLen == 0)
			return;

		// Copy pString to pools
		size_t iPoolPos = m_iPosition - m_iCurrentPoolIndex * c_iPoolSize;
		size_t iStrPos = 0;
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

			size_t iMaxCopySize = c_iPoolSize - iPoolPos;
			size_t iToCopySize = iLen - iStrPos;
			if (iToCopySize > iMaxCopySize)
				iToCopySize = iMaxCopySize;

			memcpy(m_pCurrentPool->m_oData + iPoolPos, pString + iStrPos, iToCopySize);

			iStrPos += iToCopySize;
			iPoolPos += iToCopySize;
		}

		m_iPosition += iLen;
	}

}