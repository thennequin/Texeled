#include "Texture/TextureLoader.h"

#include "Core/StringUtils.h"

#include "IO/FileStream.h"

namespace Texture
{
	static Core::Array<TextureLoaderInfo>	s_oTextureLoaders;

	TextureLoaderHandle RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader)
	{
		CORE_ASSERT(pName != NULL);
		CORE_ASSERT(pExts != NULL);
		CORE_ASSERT(pLoader != NULL);

		if (pName == NULL || pExts == NULL || pLoader == NULL )
			return TextureLoaderHandleInvalid;

		TextureLoaderInfo oInfo;
		oInfo.pName = pName;
		oInfo.pExts = pExts;
		oInfo.pLoader = pLoader;
		s_oTextureLoaders.push_back(oInfo);
		return s_oTextureLoaders.size() - 1;
	}

	const TextureLoaderInfo* GetTextureLoader(TextureLoaderHandle hLoader)
	{
		if (hLoader == TextureLoaderHandleInvalid)
			return NULL;

		CORE_ASSERT(hLoader < s_oTextureLoaders.size());

		return &s_oTextureLoaders[hLoader];
	}

	ErrorCode LoadFromStream(Graphics::Texture* pTexture, IO::Stream* pStream, const TextureLoaderInfo* pUseLoader)
	{
		if (pTexture != NULL && pStream != NULL && pStream->IsReadable())
		{
			Graphics::Texture oTemp;
			if (pUseLoader != NULL)
			{
				ErrorCode oErr = pUseLoader->pLoader(pStream, &oTemp);
				if (oErr == ErrorCode::Ok)
				{
					pTexture->Swap(oTemp);
					return ErrorCode::Ok;
				}
				return ErrorCode(oErr);
			}
			else
			{
				for (Core::Array<TextureLoaderInfo>::iterator it = s_oTextureLoaders.begin(), itEnd = s_oTextureLoaders.end(); it != itEnd; ++it)
				{
					pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN);
					ErrorCode oErr = it->pLoader(pStream, &oTemp);
					if (oErr == ErrorCode::Ok)
					{
						pTexture->Swap(oTemp);
						return ErrorCode::Ok;
					}
					else if (oErr != ErrorCode::Fail)
					{
						return ErrorCode(oErr);
					}
				}
			}
		}
		return ErrorCode::Fail;
	}

	ErrorCode LoadFromFile(Graphics::Texture* pTexture, const char* pFilename, const TextureLoaderInfo* pUseLoader)
	{
		if (pTexture != NULL)
		{
			IO::FileStream oFileStream;
			if (oFileStream.Open(pFilename, IO::FileStream::AccessModeEnum::READ))
			{
				ErrorCode oErr = LoadFromStream(pTexture, &oFileStream, pUseLoader);

				oFileStream.Close();
				return oErr;
			}
			return ErrorCode(1, "Can't open file");
		}

		return ErrorCode::Fail;
	}

	void GetTextureLoaders(const TextureLoaderInfo** pOutLoaders, int* pOutCount)
	{
		*pOutLoaders = s_oTextureLoaders.begin();;
		*pOutCount = (int)s_oTextureLoaders.size();
	}
}