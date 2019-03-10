#ifndef __PLATFORM_UTILS_H__
#define __PLATFORM_UTILS_H__

namespace PlatformUtils
{
	bool OpenFileDialog(const char* pTitle, const char* pExts, char* pInOut, int iOutSize, int* pOutSelectedExt);
	bool SaveFileDialog(const char* pTitle, const char* pExts, char* pInOut, int iOutSize, int* pOutSelectedExt);
}
//namespace PlatformUtils

#endif //__PLATFORM_UTILS_H__