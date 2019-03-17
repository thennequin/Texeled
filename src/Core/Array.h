#ifndef __CORE_ARRAY_H__
#define __CORE_ARRAY_H__

#include <stdlib.h> // malloc/free
#include <string.h> // memcpy

#include "Core/Assert.h"

namespace Core
{
	template <typename T>
	class Array
	{
	public:
		typedef T*				iterator;
		typedef const T*		const_iterator;

		Array();
		~Array();

		void					clear();
		bool 					resize(size_t iCount);
		bool 					reserve(size_t iCount);

		bool					empty() const					{ return m_iSize == 0; }
		size_t					size() const					{ return m_iSize; }
		size_t					capacity() const				{ return m_iCapacity; }

		iterator				begin()							{ return m_pData;}
		const_iterator			begin() const					{ return m_pData; }
		iterator				end()							{ return m_pData + m_iSize;}
		const_iterator			end() const						{ return m_pData + m_iSize; }

		bool					push_back(const T& oValue);
		void					pop_back();

		T&						front()							{ CORE_ASSERT(m_iSize > 0); return m_pData[0]; }
		const T&				front() const					{ CORE_ASSERT(m_iSize > 0); return m_pData[0]; }
		T&						back()							{ CORE_ASSERT(m_iSize > 0); return m_pData[m_iSize - 1]; }
		const T&				back() const					{ CORE_ASSERT(m_iSize > 0); return m_pData[m_iSize - 1]; }

		T&						operator[](size_t iIndex)		{ CORE_ASSERT(iIndex < m_iSize); return m_pData[iIndex]; }
		const T&				operator[](size_t iIndex) const	{ CORE_ASSERT(iIndex < m_iSize); return m_pData[iIndex]; }

	protected:
		size_t					growCapacity(size_t iSize) const;

		size_t 					m_iSize;
		size_t 					m_iCapacity;
		T*						m_pData;
	};

	////////////////////////////////////////////////////////////////////////

	template <typename T>
	Array<T>::Array()
	{
		m_iSize = 0;
		m_iCapacity = 0;
		m_pData = NULL;
	}

	template <typename T>
	Array<T>::~Array()
	{
		clear();
		free(m_pData);
	}

	template <typename T>
	void Array<T>::clear()
	{
		resize(0);
	}

	template <typename T>
	bool Array<T>::resize(size_t iSize)
	{
		if (reserve(growCapacity(iSize)))
		{
			m_iSize = iSize;
			return true;
		}
		return false;
	}

	template <typename T>
	bool Array<T>::reserve(size_t iNewCapacity)
	{
		if (iNewCapacity <= m_iCapacity)
			return true;

		if (iNewCapacity < m_iSize)
			return false;

		if (m_pData == NULL)
		{
			T* pNewData = (T*)malloc(iNewCapacity * sizeof(T));
			if (pNewData != NULL)
			{
				m_pData = pNewData;
				m_iCapacity = iNewCapacity;
				return true;
			}
		}
		else
		{
			T* pNewData = (T*)realloc(m_pData, iNewCapacity * sizeof(T));
			if (pNewData != NULL)
			{
				m_pData = pNewData;
				m_iCapacity = iNewCapacity;
				return true;
			}
			else
			{
				pNewData = (T*)malloc(iNewCapacity * sizeof(T));
				if (m_pData != NULL && pNewData != NULL)
				{
					memcpy(pNewData, m_pData, iNewCapacity * sizeof(T));
					free(m_pData);
					m_pData = pNewData;
					m_iCapacity = iNewCapacity;
					return true;
				}
			}
		}
		return false;
	}

	template <typename T>
	bool Array<T>::push_back(const T& oValue)
	{
		if (reserve(m_iSize + 1))
		{
			m_pData[m_iSize++] = oValue;
			return true;
		}
		return false;
	}

	template <typename T>
	void Array<T>::pop_back()
	{
		CORE_ASSERT(m_iSize > 0);
		if (m_iSize > 0)
		{
			--m_iSize;
		}
	}

	template <typename T>
	size_t Array<T>::growCapacity(size_t iSize) const
	{
		size_t iNewCapacity = (m_iCapacity > 0) ? m_iCapacity : 8; // minimum 8 values
		while (iNewCapacity < iSize)
			iNewCapacity *= 2;
		return iNewCapacity;
	}
} // namespace Core

#endif //__CORE_ARRAY_H__