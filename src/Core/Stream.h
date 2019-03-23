#ifndef __CORE_STREAM_H__
#define __CORE_STREAM_H__

#include <stddef.h>

namespace Core
{
	class Stream
	{
	public:
		struct _SeekModeEnum
		{
			enum Enum
			{
				BEGIN,
				OFFSET,
				END
			};
		};
		typedef _SeekModeEnum::Enum SeekModeEnum;

		virtual bool			IsValid() const = 0;
		virtual bool			IsSeekable() const = 0;
		virtual bool			IsEndOfStream() const = 0;
		virtual bool			IsReadable() const = 0;
		virtual bool			IsWritable() const = 0;

		virtual bool			Seek(size_t iPos, SeekModeEnum eSeekMode) = 0;
		virtual size_t			Read(void* pBuffer, size_t iSize) = 0;
		virtual size_t			Write(void* pBuffer, size_t iSize) = 0;

		template<typename T>
		bool						Read(T* pOut)
		{
			return Read(pOut, sizeof(T));
		}
	};
}
//namespace Core

#endif //__CORE_STREAM_H__