#include "Core/FileStream.h"

#include <stdio.h>

namespace Core
{
	FileStream::FileStream()
		: m_pFile(NULL)
	{
	}

	FileStream::~FileStream()
	{
		Close();
	}

	bool FileStream::Open(const char* pFilname, EAccessMode eAccessMode)
	{
		Close();

		const char* pMode;
		switch (eAccessMode)
		{
		case E_ACCESS_MODE_READ:
			pMode = "rb";
			break;
		case E_ACCESS_MODE_WRITE:
			pMode = "wb";
			break;
		default:
			return false;
		}

		FILE* pFile = fopen(pFilname, pMode);
		if (pFile != NULL)
		{
			m_pFile = pFile;
			m_eAccessMode = eAccessMode;
			m_iPos = 0;
			return true;
		}
		return false;
	}

	bool FileStream::Close()
	{
		if (m_pFile != NULL)
		{
			fclose((FILE*)m_pFile);
			m_pFile = NULL;
			return true;
		}
		return false;
	}

	bool FileStream::IsValid() const
	{
		return m_pFile != NULL;
	}

	bool FileStream::IsSeekable() const
	{
		return m_eAccessMode == E_ACCESS_MODE_READ;
	}

	bool FileStream::IsEndOfStream() const
	{
		if (m_pFile != NULL && m_eAccessMode == E_ACCESS_MODE_READ)
		{
			return feof((FILE*)m_pFile) != 0;
		}
		return false;
	}

	bool FileStream::IsReadable() const
	{
		return m_eAccessMode == E_ACCESS_MODE_READ;
	}

	bool FileStream::IsWritable() const
	{
		return m_eAccessMode == E_ACCESS_MODE_WRITE;
	}

	bool FileStream::Seek(size_t iPos, ESeekMode eSeekMode)
	{
		if (m_pFile != NULL)
		{
			int iOrigin;
			switch (eSeekMode)
			{
			case E_SEEK_MODE_BEGIN:
				iOrigin = SEEK_SET;
				m_iPos = iPos;
				break;
			case E_SEEK_MODE_OFFSET:
				iOrigin = SEEK_CUR;
				m_iPos += iPos;
				break;
			case E_SEEK_MODE_END:
				iOrigin = SEEK_END;
				m_iPos = -iPos;
				break;
			default:
				return false;
			}

			return _fseeki64((FILE*)m_pFile, iPos, iOrigin) == 0;
		}
		return false;
	}

	size_t FileStream::Read(void* pBuffer, size_t iSize)
	{
		if (m_pFile != NULL)
		{
			m_iPos += iSize;
			return fread(pBuffer, 1, iSize, (FILE*)m_pFile);
		}
		return 0;
	}

	size_t FileStream::Write(void* pBuffer, size_t iSize)
	{
		if (m_pFile != NULL)
		{
			m_iPos += iSize;
			return fwrite(pBuffer, 1, iSize, (FILE*)m_pFile);
		}
		return 0;
	}
}