#ifndef __TEXTURE_TEXTURE_LOADER_PNG_H__
#define __TEXTURE_TEXTURE_LOADER_PNG_H__

#include "Texture/TextureLoader.h"

namespace Texture
{
	namespace TextureLoader
	{
		void RegisterLoaderPNG();
		const TextureLoaderInfo* GetLoaderPNG();
	}
	//namespace TextureLoader
}
//namespace Texture

#endif //__TEXTURE_TEXTURE_LOADER_PNG_H__
