#ifndef __TEXTURE_TEXTURE_WRITER_PNG_H__
#define __TEXTURE_TEXTURE_WRITER_PNG_H__

#include "Texture/TextureWriter.h"

namespace Texture
{
	namespace TextureWriter
	{
		void RegisterWriterPNG();
		const TextureWriterInfo* GetWriterPNG();
	}
	//namespace TextureLoader
}
//namespace Texture

#endif //__TEXTURE_TEXTURE_WRITER_PNG_H__