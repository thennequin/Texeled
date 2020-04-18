#ifndef __CORE_STRING_H__
#define __CORE_STRING_H__

#include <stdint.h>

namespace Core
{
	class String
	{
	public:
		static const size_t		npos = (size_t)-1;

		typedef char*			iterator;
		typedef const char*		const_iterator;
	public:
		String();
		~String();

		bool					resize(size_t iLength);
		size_t					size() const { return m_iSize; }

		const char*				c_str() const { return m_pData; }
		iterator				begin() { return m_pData; }
		iterator				end() { return m_pData + m_iSize + 1; }
		const_iterator			begin() const { return m_pData; }
		const_iterator			end() const { return m_pData + m_iSize + 1; }

		size_t					find(char cChar) const;

		String&					operator=(const char* pString);
		String&					operator=(const String& sString);

		bool					operator==(const char* pString);
		bool					operator==(const String& sString);
	protected:
		bool					_Alloc(size_t iLength);

		char*					m_pData;
		size_t					m_iSize;
		size_t					m_iCapacity;
	};
} // namespace Core

#endif //__CORE_STRING_H__