#ifndef __GRAPHICS_TEXTURE_UTILS_H__
#define __GRAPHICS_TEXTURE_UTILS_H__

#include "Graphics/Texture.h"

namespace Graphics
{
	enum ECubemapFormat
	{
		E_CUBEMAPFORMAT_NONE = 0,
		E_CUBEMAPFORMAT_STRIP_VERTICAL,
		E_CUBEMAPFORMAT_STRIP_HORIZONTAL,
		E_CUBEMAPFORMAT_CROSS_VERTICAL,
		E_CUBEMAPFORMAT_CROSS_HORIZONTAL,
		E_CUBEMAPFORMAT_LATLONG,

		_E_CUBEMAPFORMAT_COUNT
	};
	extern const char* const ECubemapFormat_string[_E_CUBEMAPFORMAT_COUNT];

	ErrorCode		ConvertPixelFormat(const Texture* pTexture, Texture* pOutTexture, EPixelFormat eWantedPixelFormat);

	bool			IsPixelFormatResizable(EPixelFormat ePixelFormat);
	ErrorCode		ResizeTexture(const Texture* pTexture, Texture* pOutTexture, int iNewWidth, int iNewHeight);
	ErrorCode		GenerateMips(const Texture* pTexture, Texture* pOutTexture, bool bOnlyMissingMips);

	bool			DetermineCubemapFormatFromImageSize(int iWidth, int iHeight, ECubemapFormat* pOutFormat, int* pOutFaceSize);
	bool			GetCubemapFacePos(int iWidth, int iHeight, ECubemapFormat eFormat, Texture::EFace eFace, int* pOutX, int* pOutY);

	ErrorCode		ConvertTextureToCubemap(const Graphics::Texture& oTexture, Graphics::Texture* pOutCubemap);
}

#endif //__GRAPHICS_TEXTURE_UTILS_H__