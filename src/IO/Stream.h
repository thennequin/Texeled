#ifndef __IO_STREAM_H__
#define __IO_STREAM_H__

#include <stddef.h>

namespace IO
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

		Stream();
		virtual ~Stream();

		virtual bool			IsValid() const = 0;
		virtual bool			IsSeekable() const = 0;
		virtual bool			IsEndOfStream() const = 0;
		virtual bool			IsReadable() const = 0;
		virtual bool			IsWritable() const = 0;

		/*
			iPos is backward for SeekModeEnum::END
		*/
		virtual bool			Seek(size_t iPos, SeekModeEnum eSeekMode) = 0;
		virtual size_t			Tell() = 0;
		virtual size_t			Read(void* pBuffer, size_t iSize) = 0;
		virtual size_t			Write(const void* pBuffer, size_t iSize) = 0;

		template<typename T>
		bool					Read(T* pOut)
		{
			return Read(pOut, sizeof(T));
		}

		template<typename T>
		bool					Write(const T& oIn)
		{
			return Write(&oIn, sizeof(T));
		}
	};
}
//namespace IO

#endif //__IO_STREAM_H__