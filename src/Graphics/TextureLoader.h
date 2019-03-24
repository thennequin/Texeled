#ifndef __GRAPHICS_TEXTURE_LOADER_H__
#define __GRAPHICS_TEXTURE_LOADER_H__

#include "Core/ErrorCode.h"
#include "Graphics/Texture.h"

namespace Graphics
{
	typedef ErrorCode(*TextureLoaderFunc)(Core::Stream* pStream, Texture* pTexture);

	typedef struct
	{
		const char*				pName;
		const char*				pExts;
		TextureLoaderFunc		pLoader;
	} TextureLoaderInfo;

	void						RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader);

	ErrorCode					LoadFromStream(Texture* pTexture, Core::Stream* pStream, const TextureLoaderInfo* pUseLoader = NULL);
	ErrorCode					LoadFromFile(Texture* pTexture, const char* pFilename, const TextureLoaderInfo* pUseLoader = NULL);
	void						GetTextureLoaders(const TextureLoaderInfo** pOutLoaders, int* pOutCount);
}

#endif //__GRAPHICS_TEXTURE_LOADER_H__