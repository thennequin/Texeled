#include "Texture/TextureLoaders/TextureLoaderSTBI.h"

#include "Texture/TextureLoader.h"

#include "stb_image.h"

namespace Texture
{
	namespace TextureLoader
	{
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

		ErrorCode TextureLoaderSTBI(IO::Stream* pStream, Graphics::Texture* pTexture)
		{
			stbi_io_callbacks oCallbacks;
			oCallbacks.read = StreamRead;
			oCallbacks.skip = StreamSkip;
			oCallbacks.eof = StreamEof;

			Graphics::Texture::Desc oDesc;
			int iWidth, iHeight;
			int iComponentCount = 0;
			if (stbi_info_from_callbacks(&oCallbacks, pStream, &iWidth, &iHeight, &iComponentCount) == 0)
			{
				return ErrorCode::Fail;
			}

			oDesc.iWidth = (uint16_t)iWidth;
			oDesc.iHeight = (uint16_t)iHeight;

			CORE_PTR_VOID pImageVoid = CORE_PTR_NULL;
			if (pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN)
				&& stbi_is_hdr_from_callbacks(&oCallbacks, pStream))
			{
				pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN);

				float* pImage = stbi_loadf_from_callbacks(&oCallbacks, pStream, &iWidth, &iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 1:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::R32_FLOAT;
						break;
					case 2:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG32_FLOAT;
						break;
					case 3:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB32_FLOAT;
						break;
					case 4:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA32_FLOAT;
						break;
					}
					pImageVoid = Core::ToPointer(pImage, oDesc.iWidth * oDesc.iHeight * iComponentCount * 4);
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
				stbi_uc* pImage = stbi_load_from_callbacks(&oCallbacks, pStream, &iWidth, &iHeight, &iComponentCount, 0);
				if (pImage != NULL)
				{
					switch (iComponentCount)
					{
					case 1:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
						break;
					case 2:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG8_UNORM;
						break;
					case 3:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
						break;
					case 4:
						oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
						break;
					}
					pImageVoid = Core::ToPointer(pImage, oDesc.iWidth * oDesc.iHeight * iComponentCount * 1);
				}
			}

			if (oDesc.ePixelFormat != Graphics::PixelFormatEnum::_NONE && pImageVoid != NULL)
			{
				ErrorCode oErr = pTexture->Create(oDesc);
				if (oErr == ErrorCode::Ok)
				{
					Graphics::Texture::SliceData oData = pTexture->GetSliceData(0, 0, 0);
					Core::MemCpy(oData.pData, pImageVoid, oData.iSize);
				}
				stbi_image_free(pImageVoid);
				return oErr;
			}
			const char* pError = stbi_failure_reason();
			return ErrorCode(1, "%s", pError);
		}

		void RegisterLoaderSTBI()
		{
			RegisterTextureLoader("Joint Photographic Experts Group", "*.jpg\0*.jpeg\0", TextureLoaderSTBI);
			RegisterTextureLoader("Truevision Targa", "*.tga\0", TextureLoaderSTBI);
			RegisterTextureLoader("Bitmap", "*.bmp\0", TextureLoaderSTBI);
			RegisterTextureLoader("HDR", "*.hdr\0", TextureLoaderSTBI);
			RegisterTextureLoader("Photoshop", "*.psd\0", TextureLoaderSTBI);
		}
	}
	//namespace TextureLoader
}
//namespace Texture