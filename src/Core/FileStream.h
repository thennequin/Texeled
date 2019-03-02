#ifndef __CORE_FILE_STREAM_H__
#define __CORE_FILE_STREAM_H__

#include "Core/Stream.h"

namespace Core
{
	class FileStream : public Stream
	{
	public:
		enum EAccessMode
		{
			E_ACCESS_MODE_READ,
			E_ACCESS_MODE_WRITE
		};

								FileStream();
		virtual					~FileStream();

		bool					Open(const char* pFilname, EAccessMode eAccessMode);
		bool					Close();

		virtual bool			IsValid() const;
		virtual bool			IsSeekable() const;
		virtual bool			IsEndOfStream() const;
		virtual bool			IsReadable() const;
		virtual bool			IsWritable() const;

		virtual bool			Seek(size_t iPos, ESeekMode eSeekMode);
		virtual size_t			Read(void* pBuffer, size_t iSize);
		virtual size_t			Write(void* pBuffer, size_t iSize);
	protected:
		void*					m_pFile;
		EAccessMode				m_eAccessMode;
		size_t					m_iPos;
	};
}
//namespace Core

#endif //__CORE_FILE_STREAM_H__