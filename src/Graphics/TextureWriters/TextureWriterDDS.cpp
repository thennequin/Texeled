#include "Graphics/TextureWriters/TextureWriterDDS.h"

#include "Graphics/TextureWriter.h"

#include "Graphics/DDS.h"

#include "Math/Math.h"

namespace Graphics
{
	namespace TextureWriter
	{
		ESupportedWriter TextureWriterSupportedDDS(Texture* pTexture);
		bool TextureWriterDDS(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream);

		void RegisterWriterDDS()
		{
			RegisterTextureWriter("DirectDraw Surface", "*.dds\0", TextureWriterDDS, TextureWriterSupportedDDS);
		}


		ESupportedWriter TextureWriterSupportedDDS(Texture* pTexture)
		{
			return E_SUPPORTED_WRITER_FULL;
		}

		bool TextureWriterDDS(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream)
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
			if (pTexture->GetFaceCount() == 6)
			{
				oDDSHeader.iCubemapFlags = DDS_CUBEMAP_ALLFACES;
			}
			else if (pTexture->GetFaceCount() != 1)
			{
				CORE_ASSERT(false, "Not supported");
				return false;
			}

			switch (pTexture->GetPixelFormat())
			{
			case PixelFormatEnum::RGB8_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8, sizeof(DDS_PIXELFORMAT));
				break;
			case PixelFormatEnum::RGBA8_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8A8, sizeof(DDS_PIXELFORMAT));
				break;
			case PixelFormatEnum::BGR8_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_B8G8R8, sizeof(DDS_PIXELFORMAT));
				break;
			case PixelFormatEnum::R5G6B5_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT));
				break;
			case PixelFormatEnum::B5G6BR_UNORM:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_B5G6R5, sizeof(DDS_PIXELFORMAT));
				break;

			case PixelFormatEnum::BC1:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT));
				break;
			case PixelFormatEnum::BC2:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DXT3, sizeof(DDS_PIXELFORMAT));
				break;
			case PixelFormatEnum::BC3:
				memcpy(&oDDSHeader.oPixelFormat, &DDSPF_DXT5, sizeof(DDS_PIXELFORMAT));
				break;

			//DX10
			case PixelFormatEnum::RGBA16_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;
			case PixelFormatEnum::R32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32_FLOAT;
				break;
			case PixelFormatEnum::RG32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case PixelFormatEnum::RGB32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case PixelFormatEnum::RGBA32_FLOAT:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case PixelFormatEnum::R10G10B10A2_UNORM:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
				break;
			case PixelFormatEnum::R9G9B9E5_SHAREDEXP:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
				break;

			case PixelFormatEnum::BC4:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC4_TYPELESS;
				break;
			case PixelFormatEnum::BC5:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC5_TYPELESS;
				break;
			case PixelFormatEnum::BC6H:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC6H_TYPELESS;
				break;
			case PixelFormatEnum::BC7:
				bHasDX10Header = true;
				oDDSHeaderDX10.oDxgiFormat = DXGI_FORMAT_BC7_TYPELESS;
				break;

			default:
				CORE_ASSERT(false, "Not supported");
				return false;
			}

			if (PixelFormat::IsCompressed(pTexture->GetPixelFormat()))
			{
				oDDSHeader.iHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
				oDDSHeader.iPitchOrLinearSize = PixelFormat::GetPitch(pTexture->GetPixelFormat(), pTexture->GetWidth());
			}
			else
			{
				oDDSHeader.iHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
				oDDSHeader.iPitchOrLinearSize = PixelFormat::GetPitch(pTexture->GetPixelFormat(), pTexture->GetWidth());
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

			for (int iFace = 0; iFace < pTexture->GetFaceCount(); ++iFace)
			{
				for (int iMip = 0; iMip < pTexture->GetMipCount(); ++iMip)
				{
					const Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(iMip, iFace);
					uint32_t iPadMipWidth = oFaceData.iWidth;
					uint32_t iPadMipHeight = oFaceData.iHeight;

					if (pStream->Write(oFaceData.pData, oFaceData.iSize) != oFaceData.iSize)
						return false;
				}
			}

			//CORE_ASSERT(false);
			return true;
		}
	}
	//namespace TextureLoader
}
//namespace Graphics