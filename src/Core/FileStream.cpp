#include "Core/FileStream.h"

#include "Core/StringUtils.h"

#include <stdio.h> //fopen/fread/fwrite/fclose/rename/unlink
#include <time.h> //time
#include <stdlib.h> //srand/rand

namespace Core
{
	FileStream::FileStream()
		: m_pFile(NULL)
		, m_pSourceFile(NULL)
		, m_pFileName(NULL)
		, m_pSourceFileName(NULL)
	{
	}

	FileStream::~FileStream()
	{
		Close();
	}

	bool FileStream::Open(const char* pFilename, AccessModeEnum eAccessMode)
	{
		Close();

		const char* pMode;
		switch (eAccessMode)
		{
		case AccessModeEnum::READ:
			pMode = "rb";
			break;
		case AccessModeEnum::WRITE:
		case AccessModeEnum::WRITE_SAFE:
			pMode = "wb";
			break;
		default:
			return false;
		}

		if (eAccessMode == AccessModeEnum::WRITE_SAFE)
		{
			srand((unsigned int)time(NULL));
			int iRand = rand();
			const int c_iBufferSize = 2048; // should be enough for a filename
			char pNewName[c_iBufferSize];
			snprintf(pNewName, c_iBufferSize, "%s.tmp%d", pFilename, iRand);

			FILE* pFile = fopen(pNewName, pMode);
			FILE* pSourceFile = fopen(pFilename, "a");
			if (pFile != NULL && pSourceFile != NULL)
			{
				m_pFile = pFile;
				m_pSourceFile = pSourceFile;
				m_eAccessMode = eAccessMode;
				m_iPos = 0;
				m_pFileName = StringUtils::StrDup(pNewName);
				m_pSourceFileName = StringUtils::StrDup(pFilename);
				return true;
			}
			if (pFile != NULL)
				fclose(pFile);
			if (pSourceFile != NULL)
				fclose(pSourceFile);
		}
		else
		{
			FILE* pFile = fopen(pFilename, pMode);
			if (pFile != NULL)
			{
				m_pFile = pFile;
				m_eAccessMode = eAccessMode;
				m_iPos = 0;
				m_pFileName = StringUtils::StrDup(pFilename);

				return true;
			}
		}

		return false;
	}

	bool FileStream::Close()
	{
		if (m_pFile != NULL)
		{
			bool bRet = true;
			fclose((FILE*)m_pFile);
			m_pFile = NULL;
			if (m_pSourceFile != NULL)
			{
				fclose((FILE*)m_pSourceFile);
				m_pSourceFile = NULL;
			}

			if (m_eAccessMode == AccessModeEnum::WRITE_SAFE)
			{
				unlink(m_pSourceFileName);
				if (rename(m_pFileName, m_pSourceFileName) != 0)
				{
					bRet = false;
				}
			}

			free(m_pFileName);
			free(m_pSourceFileName);

			return bRet;
		}
		return false;
	}

	bool FileStream::Cancel()
	{
		if (m_eAccessMode == AccessModeEnum::WRITE_SAFE && m_pFile != NULL && m_pSourceFile != NULL)
		{
			fclose((FILE*)m_pFile);
			m_pFile = NULL;
			fclose((FILE*)m_pSourceFile);
			m_pSourceFile = NULL;

			unlink(m_pFileName);

			free(m_pFileName);
			free(m_pSourceFileName);

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
		return m_eAccessMode == AccessModeEnum::READ;
	}

	bool FileStream::IsEndOfStream() const
	{
		if (m_pFile != NULL && m_eAccessMode == AccessModeEnum::READ)
		{
			return feof((FILE*)m_pFile) != 0;
		}
		return false;
	}

	bool FileStream::IsReadable() const
	{
		return m_eAccessMode == AccessModeEnum::READ;
	}

	bool FileStream::IsWritable() const
	{
		return m_eAccessMode == AccessModeEnum::WRITE || m_eAccessMode == AccessModeEnum::WRITE_SAFE;
	}

	bool FileStream::Seek(size_t iPos, SeekModeEnum eSeekMode)
	{
		if (m_pFile != NULL)
		{
			int iOrigin;
			switch (eSeekMode)
			{
			case SeekModeEnum::BEGIN:
				iOrigin = SEEK_SET;
				m_iPos = iPos;
				break;
			case SeekModeEnum::OFFSET:
				iOrigin = SEEK_CUR;
				m_iPos += iPos;
				break;
			case SeekModeEnum::END:
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

	size_t FileStream::Tell()
	{
		if (m_pFile != NULL)
		{
			return _ftelli64((FILE*)m_pFile);
		}
		return 0;
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