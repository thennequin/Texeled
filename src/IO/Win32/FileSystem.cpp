#include "IO/FileSystem.h"

#include "Core/StringUtils.h"
#include "Math/Arithmetic.h"

#include <Windows.h>

namespace IO
{
	namespace FileSystem
	{
		bool GetPreviousFile(const char* pFilePath, char* pOutPreviousFilename, size_t iPreviousFilenameSize)
		{
			if (pFilePath == NULL || pOutPreviousFilename == NULL || iPreviousFilenameSize == 0)
				return false;

			size_t iFilePathLen = Core::StringUtils::StrLen(pFilePath, (size_t)-1);

			int iSlashPos = Math::Max(Core::StringUtils::FindLast(pFilePath, iFilePathLen, "\\", 1), Core::StringUtils::FindLast(pFilePath, iFilePathLen, "/", 1));
			if (iSlashPos == -1)
			{
				return false;
			}

			char pParentFolder[1024];
			char pFileName[1024];
			size_t iParentFolderLen;
			size_t iFileNameLen;

			Core::StringUtils::SubString(pFilePath, iFilePathLen, pParentFolder, sizeof(pParentFolder), 0, iSlashPos + 1);
			iParentFolderLen = iSlashPos + 1;
			Core::StringUtils::SubString(pFilePath, iFilePathLen, pFileName, sizeof(pFileName), iSlashPos + 1);
			iFileNameLen = iFilePathLen - (iSlashPos + 1);

			char pQuery[1024];
			strcpy(pQuery, pParentFolder);
			pQuery[iSlashPos + 1] = '*';
			pQuery[iSlashPos + 2] = 0;

			WIN32_FIND_DATA oFindData[2];
			int iCount = 0;
			HANDLE hFind = FindFirstFileA(pQuery, &oFindData[iCount % 2]);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					const char* pCurrentFileName = oFindData[iCount % 2].cFileName;
					size_t iCurrentFileNameLen = Core::StringUtils::StrLen(pCurrentFileName, MAX_PATH);
					if (Core::StringUtils::StrCmpI(pCurrentFileName, iCurrentFileNameLen, pFileName, iFileNameLen) == 0) // Ignore case
					{
						if (iCount != 0)
						{
							int iPreviousModulo = iCount - 1;
							const char* pPreviousFileName = oFindData[iPreviousModulo % 2].cFileName;
							size_t iPreviousFileNameLen = Core::StringUtils::StrLen(pPreviousFileName, MAX_PATH);

							if (Core::StringUtils::StrCmp(pPreviousFileName, iPreviousFileNameLen, ".", 1) == 0
								|| Core::StringUtils::StrCmp(pPreviousFileName, iPreviousFileNameLen, "..", 2) == 0)
							{
								return false;
							}

							if ((iPreviousFileNameLen + iSlashPos) < iPreviousFilenameSize)
							{
								strcpy(pOutPreviousFilename, pParentFolder);
								strcpy(pOutPreviousFilename + iSlashPos + 1, oFindData[iPreviousModulo % 2].cFileName);
								FindClose(hFind);
								return true;
							}
						}
						FindClose(hFind);
						return false;
					}
					++iCount;
				} while (FindNextFileA(hFind, &oFindData[iCount % 2]));
				FindClose(hFind);
				return false;
			}
			return false;
		}

		bool GetNextFile(const char* pFilePath, char* pOutNextFilename, size_t iNextFilenameSize)
		{
			if (pFilePath == NULL || pOutNextFilename == NULL || iNextFilenameSize == 0)
				return false;

			size_t iFilePathLen = Core::StringUtils::StrLen(pFilePath, (size_t)-1);

			int iSlashPos = Math::Max(Core::StringUtils::FindLast(pFilePath, iFilePathLen, "\\", 1), Core::StringUtils::FindLast(pFilePath, iFilePathLen, "/", 1));
			if (iSlashPos == -1)
			{
				return false;
			}

			char pParentFolder[1024];
			char pFileName[1024];
			size_t iParentFolderLen;
			size_t iFileNameLen;

			Core::StringUtils::SubString(pFilePath, iFilePathLen, pParentFolder, sizeof(pParentFolder), 0, iSlashPos + 1);
			iParentFolderLen = iSlashPos + 1;
			Core::StringUtils::SubString(pFilePath, iFilePathLen, pFileName, sizeof(pFileName), iSlashPos + 1);
			iFileNameLen = iFilePathLen - (iSlashPos + 1);

			char pQuery[1024];
			strcpy(pQuery, pParentFolder);
			pQuery[iSlashPos + 1] = '*';
			pQuery[iSlashPos + 2] = 0;

			WIN32_FIND_DATA oFindData;
			HANDLE hFind = FindFirstFile(pQuery, &oFindData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					const char* pCurrentFileName = oFindData.cFileName;
					size_t iCurrentFileNameLen = Core::StringUtils::StrLen(pCurrentFileName, MAX_PATH);
					if (Core::StringUtils::StrCmpI(pCurrentFileName, iCurrentFileNameLen, pFileName, iFileNameLen) == 0)
					{
						if (FindNextFile(hFind, &oFindData))
						{
							const char* pNextFileName = oFindData.cFileName;
							size_t iNextFileNameLen = Core::StringUtils::StrLen(pNextFileName, MAX_PATH);

							if (Core::StringUtils::StrCmp(pNextFileName, iNextFileNameLen, ".", 1) == 0
								|| Core::StringUtils::StrCmp(pNextFileName, iNextFileNameLen, "..", 2) == 0)
							{
								return false;
							}

							if ((iNextFileNameLen + iSlashPos) < iNextFilenameSize)
							{
								strcpy(pOutNextFilename, pParentFolder);
								strcpy(pOutNextFilename + iSlashPos + 1, oFindData.cFileName);
								FindClose(hFind);
								return true;
							}
						}
						FindClose(hFind);
						return false;
					}
				} while (FindNextFile(hFind, &oFindData));
				FindClose(hFind);
				return false;
			}
			return false;
		}
	}
}
//namespace Core