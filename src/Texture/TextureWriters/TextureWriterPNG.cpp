#include "Graphics/TextureWriters/TextureWriterPNG.h"

#include "Graphics/TextureWriter.h"
#include "Graphics/TextureUtils.h"

#include "stb_image_write.h"

namespace Graphics
{
	namespace TextureWriter
	{
		ESupportedWriter TextureWriterSupportedPNG(Texture* pTexture);
		bool TextureWriterPNG(Texture* pTexture, const WriterSettings* pSettings, IO::Stream* pStream);

		void RegisterWriterPNG()
		{
			RegisterTextureWriter("Portable Network Graphics", "*.png\0", TextureWriterPNG, TextureWriterSupportedPNG);
		}

		ESupportedWriter TextureWriterSupportedPNG(Texture* pTexture)
		{
			if (pTexture->GetMipCount() != 1 || pTexture->GetFaceCount() != 1)
			{
				return E_SUPPORTED_WRITER_PARTIAL;
			}

			PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			int iComp = PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if (ePixelFormat != PixelFormatEnum::RGB8_UNORM
				&& ePixelFormat != PixelFormatEnum::RGBA8_UNORM)
			{
				if (iComp == 3)
				{
					ePixelFormat = PixelFormatEnum::RGB8_UNORM;
				}
				else if (iComp == 4)
				{
					ePixelFormat = PixelFormatEnum::RGBA8_UNORM;
				}
				else
				{
					return E_SUPPORTED_WRITER_FALSE;
				}

				PixelFormat::ConvertionFuncChain oConvertionFuncChain;
				int iConvertionChainLength;
				int iAdditionalBits;
				if (PixelFormat::GetConvertionChain(pTexture->GetPixelFormat(), ePixelFormat, &oConvertionFuncChain, &iConvertionChainLength, &iAdditionalBits) == false)
				{
					return E_SUPPORTED_WRITER_FALSE;
				}

				return E_SUPPORTED_WRITER_PARTIAL;
			}
			return E_SUPPORTED_WRITER_FULL;
		}

		void WriteToStream(void* pContext, void* pData, int iSize)
		{
			IO::Stream* pStream = (IO::Stream*)pContext;
			CORE_VERIFY(pStream->Write(pData, iSize) == iSize);
		}

		bool TextureWriterPNG(Texture* pTexture, const WriterSettings* pSettings, IO::Stream* pStream)
		{
			Texture oNewTexture;

			PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			int iComp = PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if (ePixelFormat != PixelFormatEnum::RGB8_UNORM
				&& ePixelFormat != PixelFormatEnum::RGBA8_UNORM)
			{
				if (iComp == 3)
				{
					ePixelFormat = PixelFormatEnum::RGB8_UNORM;
				}
				else if (iComp == 4)
				{
					ePixelFormat = PixelFormatEnum::RGBA8_UNORM;
				}
				else
				{
					return false;
				}

				if (ConvertPixelFormat(pTexture, &oNewTexture, ePixelFormat) != ErrorCode::Ok)
				{
					return false;
				}
				pTexture = &oNewTexture;
			}

			const Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(0, 0);
			if (stbi_write_png_to_func(WriteToStream, pStream, pTexture->GetWidth(), pTexture->GetHeight(), iComp, (void*)oFaceData.pData, 0) == 1)
			{
				return true;
			}
			return false;
		}
	}
	//namespace TextureLoader
}
//namespace Graphics
