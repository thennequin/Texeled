#ifndef __GRAPHICS_TEXTURE_LOADER_STBI_H__
#define __GRAPHICS_TEXTURE_LOADER_STBI_H__

#include "Graphics/Texture.h"

namespace Graphics
{
	namespace TextureLoader
	{
		void RegisterLoaderSTBI();
		bool TextureLoaderSTBI(Core::Stream* pStream, Texture* pTexture);
	}
	//namespace TextureLoader
}
//namespace Graphics

#endif //__GRAPHICS_TEXTURE_LOADER_STBI_H__