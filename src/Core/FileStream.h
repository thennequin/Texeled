#ifndef __CORE_FILE_STREAM_H__
#define __CORE_FILE_STREAM_H__

#include "Core/Stream.h"

namespace Core
{
	class FileStream : public Stream
	{
	public:
		struct _AccessModeEnum
		{
			enum Enum
			{
				READ,
				WRITE,
				WRITE_SAFE
			};
		};
		typedef _AccessModeEnum::Enum AccessModeEnum;

								FileStream();
		virtual					~FileStream();

		bool					Open(const char* pFilename, AccessModeEnum eAccessMode);
		bool					Close();
		bool					Cancel(); //For WRITE_SAFE

		virtual bool			IsValid() const;
		virtual bool			IsSeekable() const;
		virtual bool			IsEndOfStream() const;
		virtual bool			IsReadable() const;
		virtual bool			IsWritable() const;

		virtual bool			Seek(size_t iPos, SeekModeEnum eSeekMode);
		virtual size_t			Read(void* pBuffer, size_t iSize);
		virtual size_t			Write(void* pBuffer, size_t iSize);
	protected:
		void*					m_pFile;
		void*					m_pSourceFile;
		AccessModeEnum			m_eAccessMode;
		size_t					m_iPos;
		char*					m_pFileName;
		char*					m_pSourceFileName;
	};
}
//namespace Core

#endif //__CORE_FILE_STREAM_H__