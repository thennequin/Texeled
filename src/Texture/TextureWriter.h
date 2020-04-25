#ifndef __TEXTURE_TEXTURE_WRITER_H__
#define __TEXTURE_TEXTURE_WRITER_H__

#include "Graphics/Texture.h"

namespace Texture
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

	typedef ESupportedWriter(*TextureWriterSupportedFunc)(Graphics::Texture* pTexture);
	typedef bool(*TextureWriterFunc)(Graphics::Texture* pTexture, const WriterSettings* pSettings, IO::Stream* pStream);

	typedef struct
	{
		const char*					pName;
		const char*					pExt;
		TextureWriterFunc			pWriter;
		TextureWriterSupportedFunc	pTester;
	} TextureWriterInfo;

	typedef size_t TextureWriterHandle;
	const TextureWriterHandle TextureWriterHandleInvalid = (TextureWriterHandle)-1;

	TextureWriterHandle				RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester);

	const TextureWriterInfo*		GetTextureWriter(TextureWriterHandle hWriter);

	ErrorCode						SaveToStream(Graphics::Texture* pTexture, const WriterSettings* pSettings, IO::Stream* pStream, const char* pFilename, const TextureWriterInfo* pUseWriter = NULL);
	ErrorCode						SaveToFile(Graphics::Texture* pTexture, const WriterSettings* pSettings, const char* pFilename, const TextureWriterInfo* pUseWriter = NULL);
	void							GetTextureWriters(const TextureWriterInfo** pOutWriters, int* pOutCount);
}
//namspace Texture

#endif //__TEXTURE_TEXTURE_WRITER_H__