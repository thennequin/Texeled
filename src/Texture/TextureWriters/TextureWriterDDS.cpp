#include "Texture/TextureWriters/TextureWriterDDS.h"

#include "Texture/TextureWriter.h"

#include "Texture/DDS.h"

#include "Math/Arithmetic.h"

namespace Texture
{
	namespace TextureWriter
	{
		ESupportedWriter TextureWriterSupportedDDS(const Graphics::Texture* pTexture)
		{
			return E_SUPPORTED_WRITER_FULL;
		}

		bool TextureWriterDDS(const Graphics::Texture* pTexture, const void* /*pSettings*/, IO::Stream* pStream)
		{
			uint32_t iDDSMagic = DDS_MAGIC;
			DDS_HEADER oDDSHeader;
			DDS_HEADER_DXT10 oDDSHeaderDX10;
			memset(&oDDSHeader, 0, sizeof(oDDSHeader));
			memset(&oDDSHeaderDX10, 0, sizeof(oDDSHeaderDX10));
			bool bHasDX10Header = false;

			oDDSHeader.iSize = sizeof(DDS_HEADER);
			oDDSHeader.oPixelFormat.iSize = sizeof(DDS_PIXELFORMAT);

			oDDSHeader.iHeaderFlags = DDS_HEADER_FLAGS_CAPS
				| DDS_HEADER_FLAGS_WIDTH
				| DDS_HEADER_FLAGS_HEIGHT
				| DDS_HEADER_FLAGS_PIXELFORMAT;

			oDDSHeader.iSurfaceFlags |= DDS_SURFACE_FLAGS_TEXTURE;

			if (pTexture->GetMipCount() > 1)
			{
				oDDSHeader.iHeaderFlags |= DDS_HEADER_FLAGS_MIPMAPCOUNT;
				oDDSHeader.iSurfaceFlags |= DDS_SURFACE_FLAGS_MIPMAP;
			}

			oDDSHeader.iWidth = pTexture->GetWidth();
			oDDSHeader.iHeight = pTexture->GetHeight();
			oDDSHeader.iMipMapCount = pTexture->GetMipCount();
			if (pTexture->GetSliceCount() == 6)
			{
				oDDSHeader.iCubemapFlags = DDS_CUBEMAP_ALLFACES;
			}
			else if (pTexture->GetSliceCount() != 1)
			{
				oDDSHeader.iDepth = pTexture->GetSliceCount();
				oDDSHeader.iHeaderFlags |= DDS_HEADER_FLAGS_DEPTH;
				oDDSHeader.iSurfaceFlags |= DDS_SURFACE_FLAGS_VOLUME;
			}

			switch (pTexture->GetPixelFormat())
			{
			case Graphics::PixelFormatEnum::RGB8_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8, sizeof(DDS_PIXELFORMAT));
				break;
			case Graphics::PixelFormatEnum::RGBA8_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8A8, sizeof(DDS_PIXELFORMAT));
				break;
			case Graphics::PixelFormatEnum::BGR8_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_B8G8R8, sizeof(DDS_PIXELFORMAT));
				break;
			case Graphics::PixelFormatEnum::R5G6B5_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT));
				break;
			case Graphics::PixelFormatEnum::B5G6BR_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_B5G6R5, sizeof(DDS_PIXELFORMAT));
				break;

			case Graphics::PixelFormatEnum::BC1:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT));
				break;
			case Graphics::PixelFormatEnum::BC2:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DXT3, sizeof(DDS_PIXELFORMAT));
				break;
			case Graphics::PixelFormatEnum::BC3:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DXT5, sizeof(DDS_PIXELFORMAT));
				break;

			//DX10
			case Graphics::PixelFormatEnum::RGBA16_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;
			case Graphics::PixelFormatEnum::R32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32_FLOAT;
				break;
			case Graphics::PixelFormatEnum::RG32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case Graphics::PixelFormatEnum::RGB32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case Graphics::PixelFormatEnum::RGBA32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case Graphics::PixelFormatEnum::R10G10B10A2_UNORM:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
				break;
			case Graphics::PixelFormatEnum::R9G9B9E5_SHAREDEXP:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
				break;

			case Graphics::PixelFormatEnum::BC4:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC4_TYPELESS;
				break;
			case Graphics::PixelFormatEnum::BC5:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC5_TYPELESS;
				break;
			case Graphics::PixelFormatEnum::BC6H:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC6H_TYPELESS;
				break;
			case Graphics::PixelFormatEnum::BC7:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC7_TYPELESS;
				break;

			default:
				CORE_ASSERT(false, "Not supported");
				return false;
			}

			if (Graphics::PixelFormat::IsCompressed(pTexture->GetPixelFormat()))
			{
				oDDSHeader.iHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
				oDDSHeader.iPitchOrLinearSize = Graphics::PixelFormat::GetPitch(pTexture->GetPixelFormat(), pTexture->GetWidth());
			}
			else
			{
				oDDSHeader.iHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
				oDDSHeader.iPitchOrLinearSize = Graphics::PixelFormat::GetPitch(pTexture->GetPixelFormat(), pTexture->GetWidth());
			}

			if (bHasDX10Header)
			{
				oDDSHeader.iHeaderFlags |= DDS_FOURCC;
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DX10, sizeof(DDS_PIXELFORMAT));
			}

			if (pStream->Write(&iDDSMagic, sizeof(iDDSMagic)) != sizeof(iDDSMagic))
				return false;

			if (pStream->Write(&oDDSHeader, sizeof(oDDSHeader)) != sizeof(oDDSHeader))
				return false;

			if (bHasDX10Header)
			{
				if (pStream->Write(&oDDSHeaderDX10, sizeof(oDDSHeaderDX10)) != sizeof(oDDSHeaderDX10))
					return false;
			}

			for (int iFace = 0; iFace < pTexture->GetSliceCount(); ++iFace)
			{
				for (int iMip = 0; iMip < pTexture->GetMipCount(); ++iMip)
				{
					const Graphics::Texture::SliceData oSliceData = pTexture->GetSliceData(0, iMip, iFace);
					uint32_t iPadMipWidth = oSliceData.iWidth;
					uint32_t iPadMipHeight = oSliceData.iHeight;

					if (pStream->Write(oSliceData.pData, oSliceData.iSize) != oSliceData.iSize)
						return false;
				}
			}

			//CORE_ASSERT(false);
			return true;
		}

		void RegisterWriterDDS()
		{
			RegisterTextureWriter("DirectDraw Surface", "*.dds\0", TextureWriterDDS, TextureWriterSupportedDDS);
		}
	}
	//namespace TextureLoader
}
//namespace Texture