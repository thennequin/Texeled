#include "Graphics/TextureLoaders/TextureLoaderSTBI.h"

#include "Graphics/TextureLoader.h"

#include "stb_image.h"

namespace Graphics
{
	namespace TextureLoader
	{
		ErrorCode TextureLoaderSTBI(IO::Stream* pStream, Texture* pTexture);

		void RegisterLoaderSTBI()
		{
			RegisterTextureLoader("Joint Photographic Experts Group", "*.jpg\0*.jpeg\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Truevision Targa", "*.tga\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Bitmap", "*.bmp\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("HDR", "*.hdr\0", Graphics::TextureLoader::TextureLoaderSTBI);
			RegisterTextureLoader("Photoshop", "*.psd\0", Graphics::TextureLoader::TextureLoaderSTBI);
		}

		int StreamRead(void* pUser, char* pData, int iSize)
		{
			IO::Stream* pStream = (IO::Stream*)pUser;
			return (int)pStream->Read(pData, (size_t)iSize);
		}

		void StreamSkip(void* pUser, int n)
		{
			IO::Stream* pStream = (IO::Stream*)pUser;
			pStream->Seek(n, IO::Stream::SeekModeEnum::OFFSET);
		}

		int StreamEof(void* pUser)
		{
			IO::Stream* pStream = (IO::Stream*)pUser;
			return pStream->IsEndOfStream() ? 1 : 0;
		}

		ErrorCode TextureLoaderSTBI(IO::Stream* pStream, Texture* pTexture)
		{
			stbi_io_callbacks oCallbacks;
			oCallbacks.read = StreamRead;
			oCallbacks.skip = StreamSkip;
			oCallbacks.eof = StreamEof;

			Texture::Desc oDesc;
			int iComponentCount = 0;
			if (stbi_info_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount) == 0)
			{
				return ErrorCode::Fail;
			}

			if (pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN)
				&& stbi_is_hdr_from_callbacks(&oCallbacks, pStream))
			{
				pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN);

				float* pImage = stbi_loadf_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 3:
						oDesc.ePixelFormat = PixelFormatEnum::RGB32_FLOAT;
						break;
					case 4:
						oDesc.ePixelFormat = PixelFormatEnum::RGBA32_FLOAT;
						break;
					}
					oDesc.pData[0][0] = pImage;
				}
			}
			/*else if (pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN)
				&& stbi_is_16_bit_from_callbacks(&oCallbacks, pStream))
			{
				pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN);
				stbi_us* pImage = stbi_load_16_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 3:
						oDesc.ePixelFormat = PixelFormatEnum::RGB16_UNORM;
						break;
					case 4:
						oDesc.ePixelFormat = PixelFormatEnum::RGBA16_UNORM;
						break;
					}
					oDesc.pData[0][0] = pImage;
				}
			}*/
			else
			{
				pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN);
				stbi_uc* pImage = stbi_load_from_callbacks(&oCallbacks, pStream, &oDesc.iWidth, &oDesc.iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 3:
						oDesc.ePixelFormat = PixelFormatEnum::RGB8_UNORM;
						break;
					case 4:
						oDesc.ePixelFormat = PixelFormatEnum::RGBA8_UNORM;
						break;
					}
					oDesc.pData[0][0] = pImage;
				}
			}

			if (oDesc.ePixelFormat != PixelFormatEnum::_NONE && oDesc.pData[0][0] != NULL)
			{
				ErrorCode oErr = pTexture->Create(oDesc);
				stbi_image_free((void*)oDesc.pData[0][0]);
				return oErr;
			}
			const char* pError = stbi_failure_reason();
			return ErrorCode(1, "%s", pError);
		}
	}
	//namespace TextureLoader
}
//namespace Graphics