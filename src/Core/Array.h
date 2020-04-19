#ifndef __CORE_ARRAY_H__
#define __CORE_ARRAY_H__

#include <stdlib.h> // malloc/free
#include <string.h> // memcpy

#include "Core/Assert.h"

namespace Core
{
	template <typename T, bool WithConstructor = true>
	class Array
	{
	public:
		static const size_t		InvalidIndex = (size_t)-1;
	public:

		typedef T*				iterator;
		typedef const T*		const_iterator;

		Array();
		~Array();

		void					clear();
		bool 					resize(size_t iCount, bool bGrowth = true);
		bool 					reserve(size_t iCount);

		bool					empty() const					{ return m_iSize == 0; }
		size_t					size() const					{ return m_iSize; }
		size_t					capacity() const				{ return m_iCapacity; }

		iterator				begin()							{ return m_pData;}
		const_iterator			begin() const					{ return m_pData; }
		iterator				end()							{ return m_pData + m_iSize;}
		const_iterator			end() const						{ return m_pData + m_iSize; }

		T*						data()							{ return m_pData; }

		size_t					find(const T& oRight) const;

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

		template<bool WithConstructor_>
		inline static void		ctor(T* pMemory);
		template<bool WithConstructor_>
		inline static void		ctor_copy(T* pMemory, const T& oRight);
		template<bool WithConstructor_>
		inline static void		dtor(T* pMemory);

		// With constructor call
		template<>
		inline static void ctor<true>(T* pMemory)
		{
			new(pMemory) T();
		}

		template<>
		inline static void ctor_copy<true>(T* pMemory, const T& oRight)
		{
			new(pMemory) T(oRight);
		}

		template<>
		inline static void dtor<true>(T* pMemory)
		{
			pMemory->~T();
		}

		// Without constructor call
		template<>
		inline static void ctor<false>(T* /*pMemory*/)
		{
			// Do nothing
		}

		template<>
		inline static void ctor_copy<false>(T* pMemory, const T& oRight)
		{
			memcpy(pMemory, &oRight, sizeof(T));
		}

		template<>
		inline static void dtor<false>(T* /*pMemory*/)
		{
			//Do Nothing
		}
	};
	////////////////////////////////////////////////////////////////////////

	template <typename T, bool WithConstructor>
	Array<T, WithConstructor>::Array()
	{
		m_iSize = 0;
		m_iCapacity = 0;
		m_pData = NULL;
	}

	template <typename T, bool WithConstructor>
	Array<T, WithConstructor>::~Array()
	{
		clear();
		free(m_pData);
	}

	template <typename T, bool WithConstructor>
	void Array<T, WithConstructor>::clear()
	{
		resize(0);
	}

	template <typename T, bool WithConstructor>
	bool Array<T, WithConstructor>::resize(size_t iSize, bool bGrowth)
	{
		if (reserve(bGrowth ? growCapacity(iSize) : iSize))
		{
			if (WithConstructor)
			{
				if (m_iSize <= iSize)
				{
					for (size_t iIndex = m_iSize; iIndex < iSize; ++iIndex)
					{
						ctor<WithConstructor>(m_pData + iIndex);
					}
				}
				else
				{
					for (size_t iIndex = m_iSize; iIndex > iSize; --iIndex)
					{
						dtor<WithConstructor>(m_pData + iIndex - 1);
					}
				}
			}
			m_iSize = iSize;
			return true;
		}
		return false;
	}

	template <typename T, bool WithConstructor>
	bool Array<T, WithConstructor>::reserve(size_t iNewCapacity)
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

	template <typename T, bool WithConstructor>
	bool Array<T, WithConstructor>::push_back(const T& oValue)
	{
		if (reserve(m_iSize + 1))
		{
			ctor_copy<WithConstructor>(m_pData + m_iSize, oValue);
			m_iSize++;
			return true;
		}
		return false;
	}

	template <typename T, bool WithConstructor>
	void Array<T, WithConstructor>::pop_back()
	{
		CORE_ASSERT(m_iSize > 0);
		if (m_iSize > 0)
		{
			--m_iSize;
			dtor<WithConstructor>(m_pData + m_iSize);
		}
	}

	template <typename T, bool WithConstructor>
	size_t Array<T, WithConstructor>::find(const T& oRight) const
	{
		for (size_t iIndex = 0; iIndex < m_iSize; ++iIndex)
		{
			if (m_pData[iIndex] == oRight)
				return iIndex;
		}
		return InvalidIndex;
	}

	template <typename T, bool WithConstructor>
	size_t Array<T, WithConstructor>::growCapacity(size_t iSize) const
	{
		size_t iNewCapacity = (m_iCapacity > 0) ? m_iCapacity : 8; // minimum 8 values
		while (iNewCapacity < iSize)
			iNewCapacity *= 2;
		return iNewCapacity;
	}
} // namespace Core

#endif //__CORE_ARRAY_H__