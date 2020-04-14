#include "Texture/TextureWriters/TextureWriterTGA.h"

#include "Texture/TextureWriter.h"
#include "Graphics/TextureUtils.h"

#include "stb_image_write.h"

namespace Texture
{
	namespace TextureWriter
	{
		ESupportedWriter TextureWriterSupportedTGA(Graphics::Texture* pTexture)
		{
			if (pTexture->GetMipCount() != 1 || pTexture->GetFaceCount() != 1)
			{
				return E_SUPPORTED_WRITER_PARTIAL;
			}

			Graphics::PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			int iComp = Graphics::PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if (ePixelFormat != Graphics::PixelFormatEnum::R8_UNORM
				&& ePixelFormat != Graphics::PixelFormatEnum::RGB8_UNORM
				&& ePixelFormat != Graphics::PixelFormatEnum::RGBA8_UNORM)
			{
				if( iComp == 1 )
				{
					ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
				}
				else if( iComp == 2 )
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
				}
				else if (iComp == 3)
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
				}
				else if (iComp == 4)
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
				}
				else
				{
					return E_SUPPORTED_WRITER_FALSE;
				}

				Graphics::PixelFormat::ConvertionFuncChain oConvertionFuncChain;
				int iConvertionChainLength;
				int iAdditionalBits;
				if (Graphics::PixelFormat::GetConvertionChain(pTexture->GetPixelFormat(), ePixelFormat, &oConvertionFuncChain, &iConvertionChainLength, &iAdditionalBits) == false)
				{
					return E_SUPPORTED_WRITER_FALSE;
				}

				return E_SUPPORTED_WRITER_PARTIAL;
			}
			return E_SUPPORTED_WRITER_FULL;
		}

		void WriteToStreamTGA(void* pContext, void* pData, int iSize)
		{
			IO::Stream* pStream = (IO::Stream*)pContext;
			CORE_VERIFY(pStream->Write(pData, iSize) == iSize);
		}

		bool TextureWriterTGA(Graphics::Texture* pTexture, const WriterSettings* /*pSettings*/, IO::Stream* pStream)
		{
			Graphics::Texture oNewTexture;

			Graphics::PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			int iComp = Graphics::PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if( iComp == 1 )
			{
				ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
			}
			else if( iComp == 2 )
			{
				ePixelFormat = Graphics::PixelFormatEnum::RG8_UNORM;
			}
			else if (iComp == 3)
			{
				ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
			}
			else if (iComp == 4)
			{
				ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
			}
			else
			{
				return false;
			}

			if (ePixelFormat!= pTexture->GetPixelFormat())
			{
				if (ConvertPixelFormat( pTexture, &oNewTexture, ePixelFormat ) != ErrorCode::Ok)
					return false;

				pTexture = &oNewTexture;
			}

			const Graphics::Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(0, 0);
			if (stbi_write_tga_to_func(WriteToStreamTGA, pStream, pTexture->GetWidth(), pTexture->GetHeight(), iComp, (void*)oFaceData.pData) == 1)
			{
				return true;
			}
			return false;
		}

		void RegisterWriterTGA()
		{
			RegisterTextureWriter("Truevision Targa", "*.tga\0", TextureWriterTGA, TextureWriterSupportedTGA);
		}
	}
	//namespace TextureLoader
}
//namespace Texture
