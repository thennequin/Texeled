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
		E_SUPPORTED_WRITER_FALSE,
		E_SUPPORTED_WRITER_PARTIAL,
		E_SUPPORTED_WRITER_FULL,
	};

	typedef ESupportedWriter(*TextureWriterSupportedFunc)(Texture* pTexture);
	typedef bool(*TextureWriterFunc)(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream);

	typedef struct
	{
		const char*					pName;
		const char*					pExt;
		TextureWriterFunc			pWriter;
		TextureWriterSupportedFunc	pTester;
	} TextureWriterInfo;

	void							RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester);

	ErrorCode						SaveToStream(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream, const char* pFilename, const TextureWriterInfo* pUseWriter = NULL);
	ErrorCode						SaveToFile(Texture* pTexture, const WriterSettings* pSettings, const char* pFilename, const TextureWriterInfo* pUseWriter = NULL);
	void							GetTextureWriters(const TextureWriterInfo** pOutWriters, int* pOutCount);
}
//namspace Graphics

#endif //__GRAPHICS_TEXTURE_WRITER_H__