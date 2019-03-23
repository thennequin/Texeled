#ifndef __CORE_MEMORY_STREAM_H__
#define __CORE_MEMORY_STREAM_H__

#include "Core/Stream.h"

namespace Core
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream(void* pMemory, size_t iSize);
		MemoryStream(const void* pMemory, size_t iSize);
		virtual					~MemoryStream();

		virtual bool			IsValid() const;
		virtual bool			IsSeekable() const;
		virtual bool			IsEndOfStream() const;
		virtual bool			IsReadable() const;
		virtual bool			IsWritable() const;

		virtual bool			Seek(size_t iPos, SeekModeEnum eSeekMode);
		virtual size_t			Read(void* pBuffer, size_t iSize);
		virtual size_t			Write(void* pBuffer, size_t iSize);
	protected:
		void*					m_pMemory;
		size_t					m_iSize;
		size_t					m_iCursor;
		bool					m_bWritable;
	};
}
//namespace Core

#endif //__CORE_MEMORY_STREAM_H__