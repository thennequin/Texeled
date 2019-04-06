#include "PlatformUtils.h"

#include "Core/Assert.h"
#include "Core/StringBuilder.h"
#include "Core/StringUtils.h"

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
			*pInOutSelectedExt = of.nFilterIndex - 1;

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

	MessageBoxReturnEnum MsgBox(MessageBoxStyleEnum eStyle, MessageBoxTypeEnum eType, const char* pTitle, const char* pMessage)
	{
		UINT iType = 0;
		switch (eStyle)
		{
		case MessageBoxStyleEnum::DEFAULT:
			//No icon
			break;
		case MessageBoxStyleEnum::INFORMATION:
			iType |= MB_ICONASTERISK;
			break;
		case MessageBoxStyleEnum::QUESTION:
			iType |= MB_ICONQUESTION;
			break;
		case MessageBoxStyleEnum::WARNING:
			iType |= MB_ICONEXCLAMATION;
			break;
		case MessageBoxStyleEnum::CRITICAL:
			iType |= MB_ICONHAND;
			break;
		}

		switch (eType)
		{
		case MessageBoxTypeEnum::OK:
			iType |= MB_OK;
			break;
		case MessageBoxTypeEnum::OK_CANCEL:
			iType |= MB_OKCANCEL;
			break;
		case MessageBoxTypeEnum::YES_NO:
			iType |= MB_YESNO;
			break;
		case MessageBoxTypeEnum::YES_NO_CANCEL:
			iType |= MB_YESNOCANCEL;
			break;
		}

		int iRet = MessageBoxA(NULL, pMessage, pTitle, iType);
		switch (iRet)
		{
		case IDOK:
			return MessageBoxReturnEnum::OK;
		case IDCANCEL:
			return MessageBoxReturnEnum::CANCEL;
		case IDYES:
			return MessageBoxReturnEnum::YES;
		case IDNO:
			return MessageBoxReturnEnum::NO;
		default:
			CORE_ASSERT(false, "MessageBox : Unknown return value '%d'", iRet);
			return MessageBoxReturnEnum::OK;
		}
	}

	MessageBoxReturnEnum MsgBoxFormat(MessageBoxStyleEnum eStyle, MessageBoxTypeEnum eType, const char* pTitle, const char* pMessageFormat, ...)
	{
		char pBuffer[4096];
		va_list oArgs;
		va_start(oArgs, pMessageFormat);
		Core::StringUtils::VSNPrintf(pBuffer, sizeof(pBuffer), pMessageFormat, oArgs);
		va_end(oArgs);

		return MsgBox(eStyle, eType, pTitle, pBuffer);
	}
}
//namespace PlatformUtils