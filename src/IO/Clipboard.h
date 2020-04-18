#ifndef __IO_CLIPBOARD_H__
#define __IO_CLIPBOARD_H__

#include "Core/String.h"

namespace IO
{
	namespace Clipboard
	{
		bool		GetText(Core::String* pOutText);
		bool		SetText(const char* pText);
	}
}
//namespace IO

#endif //__IO_CLIPBOARD_H__