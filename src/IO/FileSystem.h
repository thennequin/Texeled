#ifndef __IO_FILE_SYSTEM_H__
#define __IO_FILE_SYSTEM_H__

namespace IO
{
	namespace FileSystem
	{
		bool			GetPreviousFile(const char* pFilename, char* pOutPreviousFilename, size_t iPreviousFilnameSize);
		bool			GetNextFile(const char* pFilename, char* pOutNextFilename, size_t iNextFilnameSize);
	}
}
//namespace IO

#endif //__IO_FILE_SYSTEM_H__