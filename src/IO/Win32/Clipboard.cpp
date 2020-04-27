#include "IO/Clipboard.h"

#include "Core/Assert.h"

#include <string.h> // strlen / memcpy

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace IO
{
	namespace Clipboard
	{
		bool GetText(Core::String* pOutText)
		{
			CORE_ASSERT(pOutText != NULL);
			if (pOutText == NULL)
			{
				return false;
			}

			if (OpenClipboard(NULL) == FALSE)
			{
				return false;
			}

			HANDLE hClipboardTextData;
			if ((hClipboardTextData = GetClipboardData(CF_UNICODETEXT)) != NULL)
			{
				const wchar_t* pWideText = (const wchar_t*)GlobalLock(hClipboardTextData);
				if (pWideText == NULL)
				{
					GlobalUnlock(hClipboardTextData);
					CloseClipboard();
					return false;
				}

				size_t iTextDataSize = GlobalSize(hClipboardTextData);
				size_t iWideTextLen = wcsnlen_s(pWideText, iTextDataSize);
				size_t iUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pWideText, iWideTextLen, NULL, 0, NULL, NULL);

				pOutText->resize(iUtf8Len);

				WideCharToMultiByte(CP_UTF8, 0, pWideText, iWideTextLen, pOutText->begin(), iUtf8Len, NULL, NULL);
			}
			else if ((hClipboardTextData = GetClipboardData(CF_TEXT)) != NULL)
			{
				const char* pText = (const char*)GlobalLock(hClipboardTextData);
				if (pText == NULL)
				{
					GlobalUnlock(hClipboardTextData);
					CloseClipboard();
					return false;
				}

				size_t iTextDataSize = GlobalSize(hClipboardTextData);
				size_t pTextLen = Core::StringUtils::StrLen(pText, iTextDataSize);

				pOutText->resize(pTextLen);
				memcpy(pOutText->begin(), pText, pTextLen);
			}
			else
			{
				CloseClipboard();
				return false;
			}

			GlobalUnlock(hClipboardTextData);
			CloseClipboard();
			return true;
		}

		bool SetText(const char* pText)
		{
			if (pText == NULL)
			{
				return false;
			}

			size_t iLen = strlen(pText);

			if (OpenClipboard(NULL) == FALSE)
			{
				return false;
			}

			HGLOBAL hClipboardTextData = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)(iLen + 1));
			if (hClipboardTextData == NULL)
			{
				CloseClipboard();
				return false;
			}

			char* pClipboardText = (char*)GlobalLock(hClipboardTextData);
			if (pClipboardText == NULL)
			{
				GlobalUnlock(hClipboardTextData);
				CloseClipboard();
				return false;
			}

			memcpy(pClipboardText, pText, iLen + 1);

			GlobalUnlock(hClipboardTextData);

			if (EmptyClipboard() == false)
			{
				GlobalFree(hClipboardTextData);
				CloseClipboard();
				return false;
			}

			SetClipboardData(CF_TEXT, hClipboardTextData);

			CloseClipboard();
			return true;
		}
	}
}
//namespace IO