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

	ErrorCode		ConvertPixelFormat(const Texture* pTexture, Texture* pOutTexture, PixelFormatEnum eWantedPixelFormat);

	bool			IsPixelFormatResizable(PixelFormatEnum ePixelFormat);
	ErrorCode		ResizeTexture(const Texture* pTexture, Texture* pOutTexture, int iNewWidth, int iNewHeight);
	ErrorCode		GenerateMips(const Texture* pTexture, Texture* pOutTexture, uint16_t iMips = 0xFFFF);
	ErrorCode		GenerateMissingMips(const Texture* pTexture, Texture* pOutTexture);

	bool			DetermineCubemapFormatFromImageSize(int iWidth, int iHeight, ECubemapFormat* pOutFormat, int* pOutFaceSize);
	bool			GetCubemapFacePos(int iWidth, int iHeight, ECubemapFormat eFormat, Texture::FaceFlag eFace, int* pOutX, int* pOutY);

	ErrorCode		ConvertTextureToCubemap(const Graphics::Texture& oTexture, Graphics::Texture* pOutCubemap);

	ErrorCode		AssembleTextureMipmap(Texture* pOutTexture, const Texture* pTextures, uint8_t iTextureCount, bool bRegenerateIntermediate);

	ErrorCode		ExtractChannel(const Texture* pTexture, Texture* pOutTexture, ComponentFlag eChannel);
	// Generate an R8 Signed Distance Field texture
	ErrorCode		SignedDistanceField(Texture::SliceData& oInputSlice, uint8_t iSpread, uint8_t iThreshold = 127);
}

#endif //__GRAPHICS_TEXTURE_UTILS_H__