#include "Graphics/TextureWriter.h"

#include "Core/StringUtils.h"
#include "Core/FileStream.h"

namespace Graphics
{
	typedef struct
	{
		const char*				pName;
		const char*				pExt;
		TextureWriterFunc				pWriter;
		TextureWriterSupportedFunc		pTester;
	} TextureWriterInfo;

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

	ErrorCode SaveToStream(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream, const char* pFilename)
	{
		if (pTexture != NULL)
		{
			for (Core::Array<TextureWriterInfo>::iterator it = s_oTextureWriters.begin(), itEnd = s_oTextureWriters.end(); it != itEnd; ++it)
			{
				if (Core::StringUtils::EndsWith(pFilename, it->pExt, false))
				{
					WriterSettings oSettings;
					if (it->pWriter(pTexture, pSettings, pStream))
					{
						return ErrorCode::Ok;
					}
					else
					{
						return ErrorCode::Fail;
					}
				}
			}
		}
		return ErrorCode::Fail;
	}

	ErrorCode SaveToFile(Texture* pTexture, const WriterSettings* pSettings, const char* pFilename)
	{
		if (pTexture != NULL)
		{
			Core::FileStream oFileStream;
			if (oFileStream.Open(pFilename, Core::FileStream::E_ACCESS_MODE_WRITE))
			{
				ErrorCode oErr = SaveToStream(pTexture, pSettings, &oFileStream, pFilename);

				oFileStream.Close();
				return oErr;
			}
		}
		return ErrorCode::Fail;
	}
}