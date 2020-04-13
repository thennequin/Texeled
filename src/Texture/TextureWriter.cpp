#include "Texture/TextureWriter.h"

#include "Core/StringUtils.h"

#include "IO/FileStream.h"

namespace Texture
{
	static Core::Array<TextureWriterInfo>	s_oTextureWriters;

	void RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester)
	{
		TextureWriterInfo oInfo;
		oInfo.pName = pName;
		oInfo.pExt = pExts;
		oInfo.pWriter = pWriter;
		oInfo.pTester = pWriterTester;
		s_oTextureWriters.push_back(oInfo);
	}

	ErrorCode SaveToStream(Graphics::Texture* pTexture, const WriterSettings* pSettings, IO::Stream* pStream, const char* pFilename, const TextureWriterInfo* pUseWriter)
	{
		if (pTexture != NULL)
		{
			if (pUseWriter == NULL)
			{
				for (Core::Array<TextureWriterInfo>::iterator it = s_oTextureWriters.begin(), itEnd = s_oTextureWriters.end(); it != itEnd; ++it)
				{
					if (Core::StringUtils::Wildcard(it->pExt, pFilename))
					{
						pUseWriter = &*it;
						break;
					}
				}
			}

			if (pUseWriter != NULL)
			{
				if (pUseWriter->pTester != NULL && pUseWriter->pTester(pTexture) == E_SUPPORTED_WRITER_FALSE)
					return ErrorCode(1, "Texture not supported by writer");

				if (pUseWriter->pWriter(pTexture, pSettings, pStream))
				{
					return ErrorCode::Ok;
				}
				else
				{
					return ErrorCode::Fail;
				}
			}
			else
			{
				return ErrorCode(1, "Extension not supported");
			}
		}
		return ErrorCode(1, "Invalid arguments");
	}

	ErrorCode SaveToFile(Graphics::Texture* pTexture, const WriterSettings* pSettings, const char* pFilename, const TextureWriterInfo* pUseWriter)
	{
		if (pTexture != NULL)
		{
			IO::FileStream oFileStream;
			if (oFileStream.Open(pFilename, IO::FileStream::AccessModeEnum::WRITE_SAFE))
			{
				ErrorCode oErr = SaveToStream(pTexture, pSettings, &oFileStream, pFilename, pUseWriter);

				if (oErr == ErrorCode::Ok)
				{
					oFileStream.Close();
				}
				else
				{
					oFileStream.Cancel();
				}

				return oErr;
			}
		}
		return ErrorCode(1, "Invalid arguments");
	}

	void GetTextureWriters(const TextureWriterInfo** pOutWriters, int* pOutCount)
	{
		*pOutWriters = s_oTextureWriters.begin();
		*pOutCount = (int)s_oTextureWriters.size();
	}
}