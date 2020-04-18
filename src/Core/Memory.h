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
#	define CORE_PTR_CAST(Type,Data) ((Core::Pointer<Type>)(Data))
#	define CORE_PTR_CAST_VOID(Data) ((Core::PointerVoid)(Data))
#else
#	define CORE_PTR_VOID void*
#	define CORE_PTR(Type) Type*
#	define CORE_PTR_NULL (NULL)
#	define CORE_PTR_CAST(Type,Data) ((Type*)(Data))
#	define CORE_PTR_CAST_VOID(Data) ((void*)(Data))
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
		PointerVoid(const PointerVoid& oRight);

		template<typename T2>
		explicit PointerVoid(const Pointer<T2>& oRight)
		{
			m_iMemory = oRight.m_iMemory;
			m_iSize = oRight.m_iSize;
			m_iPos = oRight.m_iPos;
		}

		bool operator ==(intptr_t pRight) const;
		bool operator !=(intptr_t pRight) const;

		operator PointerVoid() const;

		operator void*() const;

		template<typename T>
		operator T*() const
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			return (T*)((char*)m_iMemory + m_iPos);
		}

		template<typename T>
		explicit operator Pointer<T>() const
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
		Pointer()
		{
			m_iMemory = -1;
			m_iSize = 0;
			m_iPos = 0;
		}

		Pointer(T* pMemory, size_t iSize, size_t iPos)
			: PointerVoid(pMemory, iSize, iPos)
		{
			CORE_ASSERT(iPos <= iSize - sizeof(T));
		}

		Pointer(const Pointer<T>& oRight)
		{
			m_iMemory = oRight.m_iMemory;
			m_iSize = oRight.m_iSize;
			m_iPos = oRight.m_iPos;
		}

		template<typename T2>
		explicit Pointer(const Pointer<T2>& oRight)
		{
			PointerVoid oRightVoid = (PointerVoid)oRight;
			PointerVoid* pLeftVoid = (PointerVoid*)this;
			*pLeftVoid = oRightVoid;
		}

		Pointer<T>& operator =(Pointer<T> oRight)
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			m_iMemory = oRight.m_iMemory;
			m_iSize = oRight.m_iSize;
			m_iPos = oRight.m_iPos;
			return *this;
		}

		Pointer<T> operator+(intptr_t iOffset)
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			CORE_ASSERT((m_iPos + iOffset * sizeof(T)) < m_iSize);
			Pointer<T> oNewPtr((T*)m_iMemory, m_iSize, m_iPos + iOffset * sizeof(T));
			return oNewPtr;
		}

		Pointer<T> operator-(intptr_t iOffset)
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			CORE_ASSERT((m_iPos - iOffset * sizeof(T)) >= 0);
			Pointer<T> oNewPtr((T*)m_iMemory, m_iSize, m_iPos - iOffset * sizeof(T));
			return oNewPtr;
		}

		T& operator[](size_t iPos)
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			CORE_ASSERT(m_iMemory != NULL);
			CORE_ASSERT((m_iPos + iPos * sizeof(T)) < m_iSize);
			return *((T*)*this + iPos);
		}

		operator T*() const
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			return (T*)((char*)m_iMemory + m_iPos);
		}

		explicit operator void*() const
		{
			CORE_ASSERT(m_iMemory != -1, "Using an uninitialized Pointer");
			return (void*)((char*)m_iMemory + m_iPos);
		}
	};

	CORE_PTR_VOID				Malloc(size_t iSize);
	void						Free(CORE_PTR_VOID pMemory);
	CORE_PTR_VOID				ToPointer(void* pMemory, size_t iSize);
	void						MemCpy(CORE_PTR_VOID pDest, CORE_PTR_VOID pSource, size_t iSize);
}

#endif // __CORE_MEMORY_H__