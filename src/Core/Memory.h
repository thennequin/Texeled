#ifndef __CORE_MEMORY_H__
#define __CORE_MEMORY_H__

#include "Core/Assert.h"

#include <stdint.h> // intptr_t / size_t

#ifdef _DEBUG
#	define CORE_MEMORY_DEBUG
#endif

#ifdef CORE_MEMORY_DEBUG
#	define CORE_PTR_VOID Core::PointerVoid
#	define CORE_PTR(Type) Core::Pointer<Type>
#	define CORE_PTR_NULL (Core::PointerVoid(NULL, 0))
#	define CORE_PTR_CAST(Type,Data) ((Core::Pointer<Type>)Data)
#else
#	define CORE_PTR_VOID void*
#	define CORE_PTR(Type) Type*
#	define CORE_PTR_NULL (NULL)
#	define CORE_PTR_CAST(Type,Data) ((Type*)Data)
#endif

namespace Core
{
	class PointerVoid;
	template<typename T>
	class Pointer;

	class PointerVoid
	{
	public:
		PointerVoid();
		PointerVoid(void* pMemory, size_t iSize, size_t iPos = 0);

		bool operator ==(intptr_t pRight) const;
		bool operator !=(intptr_t pRight) const;

		operator PointerVoid() const;

		template<typename T>
		operator T*() const
		{
			CORE_ASSERT(m_iMemory != -1, "Using of an uninitialized Pointer");
			return (T*)((char*)m_iMemory + m_iPos);
		}

		template<typename T>
		operator Pointer<T>() const
		{
			return Pointer<T>((T*)m_iMemory, m_iSize, m_iPos);
		}

		template<typename T>
		PointerVoid& operator =(Pointer<T> oRight)
		{
			m_iMemory = oRight.m_iMemory;
			m_iSize = oRight.m_iSize;
			m_iPos = oRight.m_iPos;
			return *this;
		}

		bool					IsRootAllocation() const;
	protected:
		intptr_t				m_iMemory;
		size_t					m_iSize;
		size_t					m_iPos;
	};

	template<typename T>
	class Pointer : protected PointerVoid
	{
		friend class PointerVoid;
	public:
		Pointer(T* pMemory, size_t iSize, size_t iPos)
			: PointerVoid(pMemory, iSize, iPos)
		{
			CORE_ASSERT(iPos <= iSize - sizeof(T));
		}

		Pointer<T>& operator =(Pointer<T> oRight)
		{
			CORE_ASSERT(m_iMemory != -1, "Using of an uninitialized Pointer");
			m_iMemory = oRight.m_iMemory;
			m_iSize = oRight.m_iSize;
			m_iPos = oRight.m_iPos;
			return *this;
		}

		Pointer<T> operator+(intptr_t iOffset)
		{
			CORE_ASSERT(m_iMemory != -1, "Using of an uninitialized Pointer");
			CORE_ASSERT((m_iPos + iOffset * sizeof(T)) < m_iSize);
			Pointer<T> oNewPtr((T*)m_iMemory, m_iSize, m_iPos + iOffset * sizeof(T));
			return oNewPtr;
		}

		Pointer<T> operator-(intptr_t iOffset)
		{
			CORE_ASSERT(m_iMemory != -1, "Using of an uninitialized Pointer");
			CORE_ASSERT((m_iPos - iOffset * sizeof(T)) >= 0);
			Pointer<T> oNewPtr((T*)m_iMemory, m_iSize, m_iPos - iOffset * sizeof(T));
			return oNewPtr;
		}

		T& operator[](size_t iPos)
		{
			CORE_ASSERT(m_iMemory != -1, "Using of an uninitialized Pointer");
			CORE_ASSERT(m_iMemory != NULL);
			CORE_ASSERT((m_iPos + iPos * sizeof(T)) < m_iSize);
			return *((T*)*this + iPos);
		}

		operator T*() const
		{
			CORE_ASSERT(m_iMemory != -1, "Using of an uninitialized Pointer");
			return (T*)((char*)m_iMemory + m_iPos);
		}
	};

	CORE_PTR_VOID				Malloc(size_t iSize);
	void						Free(CORE_PTR_VOID pMemory);
	void						MemCpy(CORE_PTR_VOID pDest, CORE_PTR_VOID pSource, size_t iSize);
}

#endif // __CORE_MEMORY_H__