#include "Graphics/TextureLoader.h"

#include "Core/StringUtils.h"

#include "IO/FileStream.h"

namespace Graphics
{
	static Core::Array<TextureLoaderInfo>	s_oTextureLoaders;

	void RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader)
	{
		CORE_ASSERT(pName != NULL);
		CORE_ASSERT(pExts != NULL && pExts[strlen(pExts)] == 0);
		CORE_ASSERT(pLoader != NULL);

		TextureLoaderInfo oInfo;
		oInfo.pName = pName;
		oInfo.pExts = pExts;
		oInfo.pLoader = pLoader;
		s_oTextureLoaders.push_back(oInfo);
	}

	ErrorCode LoadFromStream(Texture* pTexture, Core::Stream* pStream, const TextureLoaderInfo* pUseLoader)
	{
		if (pTexture != NULL && pStream != NULL && pStream->IsReadable())
		{
			Texture oTemp;
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
					pStream->Seek(0, Core::Stream::SeekModeEnum::BEGIN);
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

	ErrorCode LoadFromFile(Texture* pTexture, const char* pFilename, const TextureLoaderInfo* pUseLoader)
	{
		if (pTexture != NULL)
		{
			Core::FileStream oFileStream;
			if (oFileStream.Open(pFilename, Core::FileStream::AccessModeEnum::READ))
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