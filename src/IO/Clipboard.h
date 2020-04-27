#ifndef __IO_CLIPBOARD_H__
#define __IO_CLIPBOARD_H__

#include "Core/String.h"

#include "Graphics/Texture.h"

namespace IO
{
	namespace Clipboard
	{
		bool		GetText(Core::String* pOutText);
		bool		SetText(const char* pText);

		bool		GetTexture(Graphics::Texture* pOutTexture);
	}
}
//namespace IO

#endif //__IO_CLIPBOARD_H__