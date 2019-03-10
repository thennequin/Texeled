#ifndef __CORE_STRING_BUILDER_H__
#define __CORE_STRING_BUILDER_H__

//#define __STRINGBUILDER_USE_STD_STRING__
#ifdef __STRINGBUILDER_USE_STD_STRING__
#include <string>
#endif // __STRINGBUILDER_USE_STD_STRING__

namespace Core
{

	class StringBuilder
	{
		typedef unsigned int uint32;

		static const uint32 c_iPoolSize = 2048;

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
		int					Size() const { return m_iPosition; }

		void				Append(char cChar);
		void				Append(const char* pString);
		void				CopyTo(char* pDest);
		char*				Export(); //Using malloc passed to constructor

		void				operator +=(char cChar);
		void				operator +=(const char* pString);

#ifdef __STRINGBUILDER_USE_STD_STRING__
		void				Append(const std::string& oString);
		void				CopyTo(std::string& sOut);

		void				operator +=(const std::string& oString);
#endif //__STRINGBUILDER_USE_STD_STRING__

	protected:
		void				AppendInternal(const char* pString, uint32 iLen);

		MallocFuncPtr		m_pMallocFunc;
		FreeFuncPtr			m_pFreeFunc;

		uint32				m_iPosition;
		Pool				m_oFirstPool;
		Pool*				m_pCurrentPool;
		int					m_iCurrentPoolIndex;
	};
}
//namespace Core

#endif //__CORE_STRING_BUILDER_H__