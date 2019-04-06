#ifndef __PLATFORM_UTILS_H__
#define __PLATFORM_UTILS_H__

namespace PlatformUtils
{
	struct _MessageBoxStyleEnum
	{
		enum Enum
		{
			DEFAULT,
			INFORMATION,
			QUESTION,
			WARNING,
			CRITICAL
		};
	};
	typedef _MessageBoxStyleEnum::Enum MessageBoxStyleEnum;

	struct _MessageBoxTypeEnum
	{
		enum Enum
		{
			OK,
			OK_CANCEL,
			YES_NO,
			YES_NO_CANCEL
		};
	};
	typedef _MessageBoxTypeEnum::Enum MessageBoxTypeEnum;

	struct _MessageBoxReturnEnum
	{
		enum Enum
		{
			OK,
			CANCEL,
			YES,
			NO
		};
	};
	typedef _MessageBoxReturnEnum::Enum MessageBoxReturnEnum;


	MessageBoxReturnEnum		MsgBox(MessageBoxStyleEnum eStyle, MessageBoxTypeEnum eType, const char* pTitle, const char* pMessage);
	MessageBoxReturnEnum		MsgBoxFormat(MessageBoxStyleEnum eStyle, MessageBoxTypeEnum eType, const char* pTitle, const char* pMessageFormat, ...);

	bool						OpenFileDialog(const char* pTitle, const char* pExts, char* pInOut, int iOutSize, int* pInOutSelectedExt);
	bool						SaveFileDialog(const char* pTitle, const char* pExts, char* pInOut, int iOutSize, int* pInOutSelectedExt);
}
//namespace PlatformUtils

#endif //__PLATFORM_UTILS_H__