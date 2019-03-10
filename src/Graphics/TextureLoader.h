#ifndef __GRAPHICS_TEXTURE_LOADER_H__
#define __GRAPHICS_TEXTURE_LOADER_H__

#include "Graphics/Texture.h"

namespace Graphics
{
	typedef bool(*TextureLoaderFunc)(Core::Stream* pStream, Texture* pTexture);

	void			RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader);

	ErrorCode		LoadFromStream(Texture* pTexture, Core::Stream* pStream);
	ErrorCode		LoadFromFile(Texture* pTexture, const char* pFilename);
}

#endif //__GRAPHICS_TEXTURE_LOADER_H__