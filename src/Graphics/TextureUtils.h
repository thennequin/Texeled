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

	ErrorCode		ConvertPixelFormat(const Texture& oTexture, Texture* pOutTexture, EPixelFormat eWantedPixelFormat);

	bool			DetermineCubemapFormatFromImageSize(int iWidth, int iHeight, ECubemapFormat* pOutFormat, int* pOutFaceSize);
	bool			GetCubemapFacePos(int iWidth, int iHeight, ECubemapFormat eFormat, Texture::EFace eFace, int* pOutX, int* pOutY);

	ErrorCode		ConvertTextureToCubemap(const Graphics::Texture& oTexture, Graphics::Texture* pOutCubemap);

	struct WriterSettings
	{
		unsigned char							iQuality; // 0-100
	};

	enum ESupportedWriter
	{
		E_SUPPORTED_WRTIER_FALSE,
		E_SUPPORTED_WRTIER_PARIAL,
		E_SUPPORTED_WRTIER_FULL,
	};

	typedef bool(*TextureLoaderFunc)(Core::Stream* pStream, Texture* pTexture);

	typedef ESupportedWriter(*TextureWriterSupportedFunc)(Texture* pTexture);
	typedef bool(*TextureWriterFunc)(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream);

	void			RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader);
	void			RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester);

	ErrorCode		LoadFromStream(Texture* pTexture, Core::Stream* pStream);
	ErrorCode		LoadFromFile(Texture* pTexture, const char* pFilename);

	ErrorCode		SaveToStream(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream, const char* pFilename);
	ErrorCode		SaveToFile(Texture* pTexture, const WriterSettings* pSettings, const char* pFilename);
}

#endif //__GRAPHICS_TEXTURE_UTILS_H__