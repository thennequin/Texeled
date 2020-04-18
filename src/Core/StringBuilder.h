#ifndef __CORE_STRING_BUILDER_H__
#define __CORE_STRING_BUILDER_H__

#include <stdint.h> // size_t

#include "Core/String.h"

namespace Core
{

	class StringBuilder
	{
		static const size_t c_iPoolSize = 2048;

		struct Pool
		{
			char			m_oData[c_iPoolSize];
			Pool*			m_pNext;
		};
	public:
		typedef void*(*MallocFuncPtr)(size_t iSize);
		typedef void(*FreeFuncPtr)(void*);

		StringBuilder(MallocFuncPtr pMallocFunc = 0, FreeFuncPtr pFreeFunc = 0);
		~StringBuilder();

		void				Clear(bool bFreePools = false);
		size_t				Size() const { return m_iPosition; }

		void				Append(char cChar);
		void				Append(const char* pString);
		void				Append(const Core::String& oString);

		void				CopyTo(char* pDest);
		void				CopyTo(Core::String& sOut);

		char*				Export(); //Using malloc passed to constructor

		void				operator +=(char cChar);
		void				operator +=(const char* pString);
		void				operator +=(const Core::String& oString);

	protected:
		void				AppendInternal(const char* pString, size_t iLen);

		MallocFuncPtr		m_pMallocFunc;
		FreeFuncPtr			m_pFreeFunc;

		size_t				m_iPosition;
		Pool				m_oFirstPool;
		Pool*				m_pCurrentPool;
		int					m_iCurrentPoolIndex;
	};
}
//namespace Core

#endif //__CORE_STRING_BUILDER_H__