#include "Graphics/TextureLoader.h"

#include "Core/StringUtils.h"
#include "Core/FileStream.h"

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

	ErrorCode LoadFromStream(Texture* pTexture, Core::Stream* pStream)
	{
		if (pTexture != NULL && pStream != NULL && pStream->IsReadable())
		{
			Texture oTemp;
			for (Core::Array<TextureLoaderInfo>::iterator it = s_oTextureLoaders.begin(), itEnd = s_oTextureLoaders.end(); it != itEnd; ++it)
			{
				pStream->Seek(0, Core::Stream::E_SEEK_MODE_BEGIN);
				if (it->pLoader(pStream, &oTemp))
				{
					pTexture->Swap(oTemp);
					return ErrorCode::Ok;
				}
			}
		}
		return ErrorCode::Fail;
	}

	ErrorCode LoadFromFile(Texture* pTexture, const char* pFilename)
	{
		if (pTexture != NULL)
		{
			Core::FileStream oFileStream;
			if (oFileStream.Open(pFilename, Core::FileStream::E_ACCESS_MODE_READ))
			{
				ErrorCode oErr = LoadFromStream(pTexture, &oFileStream);

				oFileStream.Close();
				return oErr;
			}
		}

		return ErrorCode::Fail;
	}

	void GetTextureLoaders(const TextureLoaderInfo** pOutLoaders, int* pOutCount)
	{
		*pOutLoaders = s_oTextureLoaders.begin();;
		*pOutCount = s_oTextureLoaders.size();
	}
}