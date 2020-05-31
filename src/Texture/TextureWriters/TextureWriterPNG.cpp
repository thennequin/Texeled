#include "Texture/TextureWriters/TextureWriterPNG.h"

#include "Texture/TextureWriter.h"
#include "Graphics/TextureUtils.h"

#include "stb_image_write.h"

namespace Texture
{
	namespace TextureWriter
	{
		ESupportedWriter TextureWriterSupportedPNG(Graphics::Texture* pTexture)
		{
			if (pTexture->GetMipCount() != 1 || pTexture->GetSliceCount() != 1)
			{
				return E_SUPPORTED_WRITER_PARTIAL;
			}

			Graphics::PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			int iComp = Graphics::PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if (ePixelFormat != Graphics::PixelFormatEnum::RGB8_UNORM
				&& ePixelFormat != Graphics::PixelFormatEnum::RGBA8_UNORM)
			{
				if( iComp == 1 )
				{
					ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
				}
				else if (iComp == 2 || iComp == 3)
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

		void WriteToStream(void* pContext, void* pData, int iSize)
		{
			IO::Stream* pStream = (IO::Stream*)pContext;
			CORE_VERIFY(pStream->Write(pData, iSize) == iSize);
		}

		bool TextureWriterPNG(Graphics::Texture* pTexture, const void* /*pSettings*/, IO::Stream* pStream)
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
			else if( iComp == 3 )
			{
				ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
			}
			else if( iComp == 4 )
			{
				ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
			}
			else
			{
				return false;
			}

			if( ePixelFormat != pTexture->GetPixelFormat() )
			{
				if( ConvertPixelFormat( pTexture, &oNewTexture, ePixelFormat ) != ErrorCode::Ok )
					return false;

				pTexture = &oNewTexture;
			}

			const Graphics::Texture::SliceData oSliceData = pTexture->GetSliceData(0, 0, 0);
			if (stbi_write_png_to_func(WriteToStream, pStream, pTexture->GetWidth(), pTexture->GetHeight(), iComp, (void*)oSliceData.pData, 0) == 1)
			{
				return true;
			}
			return false;
		}


		TextureWriterHandle	s_hWriterInfoHandlePNG = TextureWriterHandleInvalid;
		void RegisterWriterPNG()
		{
			CORE_ASSERT(s_hWriterInfoHandlePNG == TextureWriterHandleInvalid, "TextureWriterPNG already registered");
			s_hWriterInfoHandlePNG = RegisterTextureWriter("Portable Network Graphics", "*.png\0", TextureWriterPNG, TextureWriterSupportedPNG);
		}

		const TextureWriterInfo* GetWriterPNG()
		{
			return GetTextureWriter(s_hWriterInfoHandlePNG);
		}
	}
	//namespace TextureLoader
}
//namespace Graphics
