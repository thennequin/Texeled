#include "Core/String.h"

#include "Core/Assert.h"

#include <stdlib.h> // malloc / realloc / free
#include <string.h> // memcpy / memset / strlen

namespace Core
{
	String::String()
		: m_pData(m_oHeapData)
		, m_iSize(0)
		, m_iCapacity(sizeof(m_oHeapData))
	{
	}

	String::String(const String& sRight)
		: m_pData(m_oHeapData)
		, m_iSize(0)
		, m_iCapacity(sizeof(m_oHeapData))
	{
		*this = sRight;
	}

	String::String(const char* pString)
		: m_pData(m_oHeapData)
		, m_iSize(0)
		, m_iCapacity(sizeof(m_oHeapData))
	{
		*this = pString;
	}

	String::~String()
	{
		if (m_pData != NULL && m_iCapacity > sizeof(m_oHeapData))
		{
			free(m_pData);
		}
	}

	bool String::resize(size_t iLength)
	{
		size_t iPreviousSize = m_iSize;
		if (_Alloc(iLength))
		{
			m_iSize = iLength;
			if (m_pData != NULL)
			{
				if (iPreviousSize <= m_iSize)
				{
					// Add whitespace until end
					memset(m_pData + iPreviousSize, ' ', m_iSize - iPreviousSize);
				}
				m_pData[m_iSize] = 0;
			}
			return true;
		}
		return false;
	}

	void String::clear()
	{
		m_iSize = 0;
		m_pData[0] = 0;
	}

	size_t String::find(char cChar) const
	{
		for (size_t i = 0; i < m_iSize; ++i)
		{
			if (m_pData[i] == cChar)
				return i;
		}
		return npos;
	}

	String& String::operator=(const char* pString)
	{
		size_t iLen = (pString != 0) ? strlen(pString) : 0;
		resize(iLen);
		if (iLen > 0)
		{
			memcpy(m_pData, pString, iLen);
		}
		m_pData[iLen] = 0;
		return *this;
	}

	String& String::operator=(const String& sString)
	{
		size_t iLen = sString.size();
		if (iLen > 0)
		{
			resize(iLen);
			memcpy(m_pData, sString.m_pData, iLen);
		}
		if (m_pData != NULL)
		{
			m_pData[iLen] = 0;
		}
		return *this;
	}

	bool String::operator==(const char* pString)
	{
		const char* pThis = m_pData;
		while (*pThis == *pString && *pThis != 0 && *pString != 0)
		{
			++pThis;
			++pString;
		}
		return *pThis == 0 && *pString == 0;
	}

	bool String::operator==(const String& sString)
	{
		return *this == sString.c_str();
	}

	bool String::_Alloc(size_t iLength)
	{
		if (m_iCapacity >= (iLength + 1))
			return true;

		char* pNewData = NULL;
		// Try realloc
		if (m_iCapacity > sizeof(m_oHeapData))
		{
			pNewData = (char*)realloc(m_pData, (iLength + 1) * sizeof(char));
		}

		if (pNewData != NULL)
		{
			m_pData = pNewData;
			m_iCapacity = iLength;
			return true;
		}
		else
		{
			size_t iNewCapacity = iLength + 1;
			pNewData = (char*)malloc(iNewCapacity);
			if (m_pData != NULL)
			{
				memcpy(pNewData, m_pData, m_iSize + 1);
				if (m_iCapacity > sizeof(m_oHeapData))
				{
					free(m_pData);
				}
				m_pData = pNewData;
				m_iCapacity = iNewCapacity;
				return true;
			}
		}
		return false;
	}
}