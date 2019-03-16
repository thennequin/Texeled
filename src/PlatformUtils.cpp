#include "PlatformUtils.h"

#include "Core/StringBuilder.h"

#include <Windows.h>

namespace PlatformUtils
{
	bool OpenFileDialog(const char* pTitle, const char* pExts, char* pInOut, int iOutSize, int* pInOutSelectedExt)
	{
		OPENFILENAME of;
		::memset(&of, 0, sizeof(of));

		of.lStructSize = sizeof(of);
		//of.hwndOwner       = (AfxGetMainWnd())->GetSafeHwnd( );
		of.hwndOwner = NULL;
		of.lpstrFile = pInOut;
		of.lpstrFilter = pExts;
		of.nMaxFile = iOutSize;
		of.nFilterIndex = (pInOutSelectedExt != NULL) ? *pInOutSelectedExt : 0;
		of.lpstrFileTitle = NULL;
		of.nMaxFileTitle = 0;
		of.lpstrInitialDir = pInOut;
		of.lpstrDefExt = "";
		of.Flags = /*OFN_EXPLORER | OFN_ENABLESIZING |*/ OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		of.lpstrTitle = pTitle;

		if (FALSE == GetOpenFileName(&of))
			return false;

		if (pInOutSelectedExt != NULL)
			*pInOutSelectedExt = of.nFilterIndex;

		return true;
	}

	bool SaveFileDialog(const char* pTitle, const char* pExts, char* pOut, int iOutSize, int* pInOutSelectedExt)
	{
		OPENFILENAME of;
		::memset(&of, 0, sizeof(of));

		of.lStructSize = sizeof(of);
		//of.hwndOwner       = (AfxGetMainWnd())->GetSafeHwnd( );
		of.hwndOwner = NULL;
		of.lpstrFile = pOut;
		of.lpstrFilter = pExts;
		of.nMaxFile = iOutSize;
		of.nFilterIndex = (pInOutSelectedExt != NULL) ? *pInOutSelectedExt : 0;
		of.lpstrFileTitle = NULL;
		of.nMaxFileTitle = 0;
		of.lpstrInitialDir = NULL;
		of.lpstrDefExt = "";
		of.Flags = /*OFN_EXPLORER | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST*/ OFN_NOCHANGEDIR;
		of.lpstrTitle = pTitle;

		if (FALSE == GetSaveFileNameA(&of))
			return false;

		if (pInOutSelectedExt != NULL)
			*pInOutSelectedExt = of.nFilterIndex - 1;

		return true;
	}
}
//namespace PlatformUtils