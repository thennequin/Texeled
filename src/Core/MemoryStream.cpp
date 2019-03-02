#include "Core/MemoryStream.h"

#include <string> //memcpy

namespace Core
{
	MemoryStream::MemoryStream(void* pMemory, size_t iSize)
		: m_pMemory(pMemory)
		, m_iSize(iSize)
		, m_iCursor(0)
		, m_bWritable(true)
	{
	}

	MemoryStream::MemoryStream(const void* pMemory, size_t iSize)
		: m_pMemory((void*)pMemory)
		, m_iSize(iSize)
		, m_iCursor(0)
		, m_bWritable(false)
	{
	}

	MemoryStream::~MemoryStream()
	{
	}

	bool MemoryStream::IsValid() const
	{
		return m_pMemory != NULL && m_iSize != 0;
	}

	bool MemoryStream::IsSeekable() const
	{
		return true;
	}

	bool MemoryStream::IsEndOfStream() const
	{
		return m_iCursor == m_iSize;
	}

	bool MemoryStream::IsReadable() const
	{
		return true;
	}

	bool MemoryStream::IsWritable() const
	{
		return m_bWritable;
	}

	bool MemoryStream::Seek(size_t iPos, ESeekMode eSeekMode)
	{
		if (iPos < 0)
			return false;

		if (eSeekMode == E_SEEK_MODE_BEGIN)
		{
			if (iPos < m_iSize)
			{
				m_iCursor = iPos;
				return true;
			}
		}
		else if (eSeekMode == E_SEEK_MODE_BEGIN)
		{
			if ((m_iCursor + iPos) < m_iSize)
			{
				m_iCursor += iPos;
				return true;
			}
		}
		else if (eSeekMode == E_SEEK_MODE_END)
		{
			if (((m_iSize - 1) - iPos) >= 0)
			{
				m_iCursor = (m_iSize -1) - iPos;
				return true;
			}
		}
		return false;
	}

	size_t MemoryStream::Read(void* pBuffer, size_t iSize)
	{
		size_t iReadSize = iSize;
		if (iReadSize > (m_iSize - m_iCursor))
			iReadSize = m_iSize - m_iCursor;
		if (iReadSize > 0)
		{
			memcpy(pBuffer, (char*)m_pMemory + m_iCursor, iSize);
		}
		return iReadSize;
	}

	size_t MemoryStream::Write(void* pBuffer, size_t iSize)
	{
		size_t iWriteSize = iSize;
		if (iWriteSize > (m_iSize - m_iCursor))
			iWriteSize = m_iSize - m_iCursor;
		if (iWriteSize > 0)
		{
			memcpy((char*)m_pMemory + m_iCursor, pBuffer, iSize);
		}
		return iWriteSize;
	}
}
//namespace Core
