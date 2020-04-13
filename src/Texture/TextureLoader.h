#ifndef __TEXTURE_TEXTURE_LOADER_H__
#define __TEXTURE_TEXTURE_LOADER_H__

#include "Core/ErrorCode.h"
#include "Graphics/Texture.h"

namespace Texture
{
	typedef ErrorCode(*TextureLoaderFunc)(IO::Stream* pStream, Graphics::Texture* pTexture);

	typedef struct
	{
		const char*				pName;
		const char*				pExts;
		TextureLoaderFunc		pLoader;
	} TextureLoaderInfo;

	void						RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader);

	ErrorCode					LoadFromStream(Graphics::Texture* pTexture, IO::Stream* pStream, const TextureLoaderInfo* pUseLoader = NULL);
	ErrorCode					LoadFromFile(Graphics::Texture* pTexture, const char* pFilename, const TextureLoaderInfo* pUseLoader = NULL);
	void						GetTextureLoaders(const TextureLoaderInfo** pOutLoaders, int* pOutCount);
}
//namespace Texture

#endif //__TEXTURE_TEXTURE_LOADER_H__