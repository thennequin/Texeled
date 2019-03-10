#ifndef __GRAPHICS_TEXTURE_WRITER_H__
#define __GRAPHICS_TEXTURE_WRITER_H__

#include "Graphics/Texture.h"

namespace Graphics
{
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

	typedef ESupportedWriter(*TextureWriterSupportedFunc)(Texture* pTexture);
	typedef bool(*TextureWriterFunc)(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream);

	void			RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester);

	ErrorCode		SaveToStream(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream, const char* pFilename);
	ErrorCode		SaveToFile(Texture* pTexture, const WriterSettings* pSettings, const char* pFilename);
}
//namspace Graphics

#endif //__GRAPHICS_TEXTURE_WRITER_H__