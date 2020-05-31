#include "Texture/TextureWriters/TextureWriterEXR.h"

#include "Texture/TextureWriter.h"
#include "Graphics/TextureUtils.h"

#include "tinyexr.h"

namespace Texture
{
	namespace TextureWriter
	{
		ESupportedWriter TextureWriterSupportedEXR(Graphics::Texture* pTexture)
		{
			if (pTexture->GetMipCount() != 1 || pTexture->GetSliceCount() != 1)
			{
				return E_SUPPORTED_WRITER_PARTIAL;
			}

			Graphics::PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			int iComp = Graphics::PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if( ePixelFormat != Graphics::PixelFormatEnum::RGB16_FLOAT
				&& ePixelFormat != Graphics::PixelFormatEnum::RGBA16_FLOAT
				&& ePixelFormat != Graphics::PixelFormatEnum::RGB32_FLOAT
				&& ePixelFormat != Graphics::PixelFormatEnum::RGBA32_FLOAT )
			{
				if (iComp == 3)
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGB32_FLOAT;
				}
				else if (iComp == 4)
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGBA32_FLOAT;
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

		bool TextureWriterEXR(Graphics::Texture* pTexture, const void* /*pSettings*/, IO::Stream* pStream)
		{
			Graphics::Texture oNewTexture;

			Graphics::PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();
			const int iComp = Graphics::PixelFormatEnumInfos[ePixelFormat].iComponentCount;

			if (ePixelFormat != Graphics::PixelFormatEnum::RGB16_FLOAT
				&& ePixelFormat != Graphics::PixelFormatEnum::RGBA16_FLOAT
				&& ePixelFormat != Graphics::PixelFormatEnum::RGB32_FLOAT
				&& ePixelFormat != Graphics::PixelFormatEnum::RGBA32_FLOAT )
			{
				if (iComp == 3)
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGB32_FLOAT;
				}
				else if (iComp == 4)
				{
					ePixelFormat = Graphics::PixelFormatEnum::RGBA32_FLOAT;
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

			const Graphics::Texture::SliceData oSliceData = pTexture->GetSliceData(0, 0, 0);

			bool bHalf = (ePixelFormat == Graphics::PixelFormatEnum::RGB16_FLOAT) || (ePixelFormat == Graphics::PixelFormatEnum::RGBA16_FLOAT);
			EXRHeader header;
			InitEXRHeader(&header);

			header.num_channels = iComp;

			EXRChannelInfo oChannelsInfos[4];
			oChannelsInfos[0].name[0] = 'B'; oChannelsInfos[0].name[1] = '\0';
			oChannelsInfos[1].name[0] = 'G'; oChannelsInfos[1].name[1] = '\0';
			oChannelsInfos[2].name[0] = 'R'; oChannelsInfos[2].name[1] = '\0';
			oChannelsInfos[3].name[0] = 'A'; oChannelsInfos[3].name[1] = '\0';
			header.channels = oChannelsInfos;

			int iPixelTypesFloat[4] = { TINYEXR_PIXELTYPE_FLOAT, TINYEXR_PIXELTYPE_FLOAT, TINYEXR_PIXELTYPE_FLOAT, TINYEXR_PIXELTYPE_FLOAT };
			int iPixelTypesHalf[4] = { TINYEXR_PIXELTYPE_HALF, TINYEXR_PIXELTYPE_HALF, TINYEXR_PIXELTYPE_HALF, TINYEXR_PIXELTYPE_HALF };

			header.pixel_types = bHalf ? iPixelTypesHalf : iPixelTypesFloat;
			header.requested_pixel_types = bHalf ? iPixelTypesHalf : iPixelTypesFloat;

			EXRImage image;
			InitEXRImage(&image);

			image.num_channels = iComp;
			image.width = oSliceData.iWidth;
			image.height = oSliceData.iHeight;

			const int iRGBAtoBGRA[4] = { 2, 1, 0, 3 };
			Core::Array<unsigned char> oChannels[4];
			unsigned char* pChannelsPtrs[4];
			int iChannelSize = bHalf ? 2 : 4;
			for (int i= 0; i < iComp; ++i)
			{
				oChannels[i].resize( image.width * image.height * iChannelSize );
				pChannelsPtrs[i] = oChannels[i].begin();
				for( int iY = 0; iY < image.height; ++iY )
				{
					for( int iX = 0; iX < image.width; ++iX )
					{
						unsigned char* pDest = pChannelsPtrs[i] + (iY * image.width + iX) * iChannelSize;
						unsigned char* pSource = ( (unsigned char*)oSliceData.pData ) + ((iY * image.width + iX) * iComp + iRGBAtoBGRA[i]) * iChannelSize;
						memcpy( pDest, pSource, iChannelSize );
					}
				}
			}

			image.images = pChannelsPtrs;

			unsigned char* pMemory = NULL;
			const char* pErr = NULL;
			size_t iMemSize = SaveEXRImageToMemory(&image, &header, &pMemory, &pErr);
			if( iMemSize == 0 )
				return false;

			bool bOk = true;
			if( pStream->Write( pMemory, iMemSize ) != iMemSize )
				bOk = false;

			free(pMemory);

			return bOk;
		}

		void RegisterWriterEXR()
		{
			RegisterTextureWriter("OpenEXR", "*.exr\0", TextureWriterEXR, TextureWriterSupportedEXR);
		}
	}
	//namespace TextureLoader
}
//namespace Texture
