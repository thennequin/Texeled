#include "Graphics/TextureLoaders/TextureLoaderDDS.h"

#include "Graphics/DDS.h"
#include "Graphics/TextureUtils.h"

#include <stdlib.h>
#include <stdio.h>

namespace Graphics
{
	namespace TextureLoader
	{
		void RegisterLoaderDDS()
		{
			Graphics::RegisterTextureLoader("DDS", ".DDS\0", Graphics::TextureLoader::TextureLoaderDDS);
		}

		bool TextureLoaderDDS(Core::Stream* pStream, Texture* pTexture)
		{
			Texture::Desc oDesc;
			uint32_t iDDSMagic;
			DDS_HEADER oDDSHeader;
			DDS_HEADER_DXT10 oDDSHeaderDX10;

			if (pStream->Read(&iDDSMagic) == false)
			{
				return false;
			}

			if (iDDSMagic != DDS_MAGIC)
			{
				return false;
			}

			if (pStream->Read(&oDDSHeader) == false
				|| oDDSHeader.iSize != sizeof(DDS_HEADER)
				|| oDDSHeader.oPixelFormat.iSize != sizeof(DDS_PIXELFORMAT))
			{
				return false;
			}

			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_CAPS) != 0)
				printf("DDS_HEADER_FLAGS_CAPS\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_HEIGHT) != 0)
				printf("DDS_HEADER_FLAGS_HEIGHT\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_WIDTH) != 0)
				printf("DDS_HEADER_FLAGS_WIDTH\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PITCH) != 0)
				printf("DDS_HEADER_FLAGS_PITCH\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PIXELFORMAT) != 0)
				printf("DDS_HEADER_FLAGS_PIXELFORMAT\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_MIPMAPCOUNT) != 0)
				printf("DDS_HEADER_FLAGS_MIPMAPCOUNT\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_LINEARSIZE) != 0)
				printf("DDS_HEADER_FLAGS_LINEARSIZE\n");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_DEPTH) != 0)
				printf("DDS_HEADER_FLAGS_DEPTH\n");

			char pFourCC[5] = "0000";
			pFourCC[0] = oDDSHeader.oPixelFormat.iFourCC & 0xff;
			pFourCC[1] = (oDDSHeader.oPixelFormat.iFourCC >> 8) & 0xff;
			pFourCC[2] = (oDDSHeader.oPixelFormat.iFourCC >> 16) & 0xff;
			pFourCC[3] = (oDDSHeader.oPixelFormat.iFourCC >> 24) & 0xff;

			oDesc.iWidth = oDDSHeader.iWidth;
			oDesc.iHeight = oDDSHeader.iHeight;
			oDesc.iMipCount = ((oDDSHeader.iSurfaceFlags & DDS_SURFACE_FLAGS_MIPMAP) && (oDDSHeader.iMipMapCount > 0)) ? oDDSHeader.iMipMapCount : 1;

			int iCubemapFace = 0;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_POSITIVEX)
				++iCubemapFace;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_NEGATIVEX)
				++iCubemapFace;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_POSITIVEY)
				++iCubemapFace;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_NEGATIVEY)
				++iCubemapFace;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_POSITIVEZ)
				++iCubemapFace;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_NEGATIVEZ)
				++iCubemapFace;

			oDesc.iFaceCount = iCubemapFace > 0 ? iCubemapFace : 1;

			if ((oDDSHeader.iHeaderFlags & DDS_FOURCC)
				&& oDDSHeader.oPixelFormat.iFourCC == DDSPF_DX10.iFourCC
				//&& memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DX10, sizeof(DDS_PIXELFORMAT)) == 0
				)
			{
				if (pStream->Read(&oDDSHeaderDX10) == false)
				{
					return false;
				}

				if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_TYPELESS
				|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM
				|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_UINT)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_R10G10B10A2_UNORM;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGBA16_SFLOAT;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_R9G9B9E5_SHAREDEXP;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_SNORM)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC4;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_SNORM)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC6H;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_UF16
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_SF16)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC6H;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_TYPELESS
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_UNORM
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)
				{
					oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC7;
				}
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BGRA8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGB8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_B8G8R8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BGR8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC1;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT2, sizeof(DDS_PIXELFORMAT)) == 0
				  || memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT3, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC2;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT4, sizeof(DDS_PIXELFORMAT)) == 0
				|| memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT5, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BC3;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R16G16B16A16_UNORM.iFourCC)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGBA16_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R16G16B16A16_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGBA16_SFLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32G32B32A32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGBA32_SFLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 8
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_R8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 8
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_R8_UNORM;
				//TODO : copy channel R to GBA
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 16
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RG8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 16
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xffff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0) // Declaration seems weird
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RG8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff000000)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BGRA8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff000000)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGBA8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_BGRA8_UNORM;
				//TODO: do something for the alpha?
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = EPixelFormat::E_PIXELFORMAT_RGBA8_UNORM;
				//TODO: do something for the alpha?
			}

			if (oDesc.ePixelFormat != EPixelFormat::E_PIXELFORMAT_NONE)
			{
				if (pTexture->Create(oDesc) != ErrorCode::Ok)
				{
					return false;
				}

				uint32_t iBPP = PixelFormat::BitPerPixel(oDesc.ePixelFormat);
				uint32_t iBlockSize = PixelFormat::BlockSize(oDesc.ePixelFormat);

				for (int iFace = 0; iFace < oDesc.iFaceCount; ++iFace)
				{
					for (int iMip = 0; iMip < oDesc.iMipCount; ++iMip)
					{
						const Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(iMip, iFace);
						uint32_t iPadMipWidth = oFaceData.iWidth;
						uint32_t iPadMipHeight = oFaceData.iHeight;

						uint32_t iPaddingX, iPaddingY;
						PixelFormat::GetDataSize(oDesc.ePixelFormat, &iPadMipWidth, &iPadMipHeight, &iPaddingX, &iPaddingY);

						int iBlockCount = iPadMipWidth * iPadMipHeight / iPaddingX / iPaddingY;

						size_t iBlocksSize = iBlockCount * iBlockSize;

						void* pBlocks = oFaceData.pData;

						if (oFaceData.iSize != iBlocksSize)
							return false;

						size_t iRowSize = iPadMipWidth * iBPP / 8 /*/ iPaddingX*/;

						if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PITCH) != 0)
						{
							size_t iPitchSize = oDDSHeader.iPitchOrLinearSize;
							if (iRowSize > iPitchSize)
							{
								return false;
							}

							size_t iDiff = iPitchSize - iRowSize;
							size_t iRemainingPitch = iPitchSize;
							for (int iLine = 0; iLine < iPadMipHeight; iLine += iPaddingY)
							{
								if (iRowSize > iRemainingPitch)
								{
									pStream->Seek(iRemainingPitch, Core::Stream::E_SEEK_MODE_OFFSET);
									iRemainingPitch = iPitchSize;
								}

								{
									if (pStream->Read((char*)pBlocks + iLine * iRowSize, iRowSize) != iRowSize)
									{
										return false;
									}
									iRemainingPitch -= iRowSize;
								}
							}
						}
						else if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_LINEARSIZE) != 0)
						{
							size_t iLinearSize = oDDSHeader.iPitchOrLinearSize;
							if (iMip == 0 && iBlocksSize != iLinearSize)
							{
								return false;
							}
							if (pStream->Read(pBlocks, iBlocksSize) != iBlocksSize)
							{
								return false;
							}
						}
						else
						{
							if (pStream->Read(pBlocks, iBlocksSize) != iBlocksSize)
							{
								return false;
							}
						}
					}
				}

				return true;
			}

			return false;
		}
	}
	//namespace TextureLoader
}
//namespace Graphics