#include "Graphics/TextureLoaders/TextureLoaderSTBI.h"

#include "Graphics/TextureLoader.h"

#include "stb_image.h"

namespace Graphics
{
	namespace TextureLoader
	{
		bool TextureLoaderSTBI(Core::Stream* pStream, Texture* pTexture);

		void RegisterLoaderSTBI()
		{
			RegisterTextureLoader("Portable Network Graphics", "*.png\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Jpeg", "*.jpg\0*.jpeg\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Targa", "*.tga\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Bitmap", "*.bmp\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("HDR", "*.hdr\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Photoshop", "*.psd\0", Graphics::TextureLoader::TextureLoaderSTBI);
		}

		int StreamRead(void* pUser, char* pData, int iSize)
		{
			Core::Stream* pStream = (Core::Stream*)pUser;
			return (int)pStream->Read(pData, (size_t)iSize);
		}

		void StreamSkip(void* pUser, int n)
		{
			Core::Stream* pStream = (Core::Stream*)pUser;
			pStream->Seek(n, Core::Stream::E_SEEK_MODE_OFFSET);
		}

		int StreamEof(void* pUser)
		{
			Core::Stream* pStream = (Core::Stream*)pUser;
			return pStream->IsEndOfStream() ? 1 : 0;
		}

		bool TextureLoaderSTBI(Core::Stream* pStream, Texture* pTexture)
		{
			stbi_io_callbacks oCallbacks;
			oCallbacks.read = StreamRead;
			oCallbacks.skip = StreamSkip;
			oCallbacks.eof = StreamEof;

			Texture::Desc oDesc;

			if (stbi_is_hdr_from_callbacks(&oCallbacks, pStream))
			{
				pStream->Seek(0, Core::Stream::E_SEEK_MODE_BEGIN);
				int iComponentCount = 0;
				float* pImage = stbi_loadf_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 3:
						oDesc.ePixelFormat = E_PIXELFORMAT_RGB32_FLOAT;
						break;
					case 4:
						oDesc.ePixelFormat = E_PIXELFORMAT_RGBA32_FLOAT;
						break;
					}
					oDesc.pData[0][0] = pImage;
				}
			}
			else if (stbi_is_16_bit_from_callbacks(&oCallbacks, pStream))
			{
				pStream->Seek(0, Core::Stream::E_SEEK_MODE_BEGIN);
				int iComponentCount = 0;
				stbi_us* pImage = stbi_load_16_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 3:
						//oDesc.ePixelFormat = Texture::E_PIXELFORMAT_RGB16_UINT;
						break;
					case 4:
						//oDesc.ePixelFormat = Texture::E_PIXELFORMAT_RGBA16_UINT;
						break;
					}
					oDesc.pData[0][0] = pImage;
				}
			}
			else
			{
				pStream->Seek(0, Core::Stream::E_SEEK_MODE_BEGIN);
				int iComponentCount = 0;
				stbi_uc* pImage = stbi_load_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 3:
						oDesc.ePixelFormat = E_PIXELFORMAT_RGB8_UNORM;
						break;
					case 4:
						oDesc.ePixelFormat = E_PIXELFORMAT_RGBA8_UNORM;
						break;
					}
					oDesc.pData[0][0] = pImage;
				}
			}

			if (oDesc.ePixelFormat != E_PIXELFORMAT_NONE && oDesc.pData[0][0] != NULL)
			{
				ErrorCode oErr = pTexture->Create(oDesc);
				stbi_image_free((void*)oDesc.pData[0][0]);
				return oErr == ErrorCode::Ok;
			}
			const char* pError = stbi_failure_reason();
			return false;
		}
	}
	//namespace TextureLoader
}
//namespace Graphics