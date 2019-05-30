#include "Graphics/TextureLoaders/TextureLoaderDDS.h"

#include "Graphics/DDS.h"
#include "Graphics/TextureLoader.h"

#include <stdlib.h>
#include <stdio.h>

namespace Graphics
{
	namespace TextureLoader
	{
		ErrorCode TextureLoaderDDS(Core::Stream* pStream, Texture* pTexture);

		void RegisterLoaderDDS()
		{
			Graphics::RegisterTextureLoader("DirectDraw Surface", "*.dds\0", Graphics::TextureLoader::TextureLoaderDDS);
		}


		ErrorCode TextureLoaderDDS(Core::Stream* pStream, Texture* pTexture)
		{
			Texture::Desc oDesc;
			uint32_t iDDSMagic;
			DDS_HEADER oDDSHeader;
			DDS_HEADER_DXT10 oDDSHeaderDX10;

			if (pStream->Read(&iDDSMagic) == false || iDDSMagic != DDS_MAGIC)
			{
				return ErrorCode::Fail;
				//return ErrorCode(1, "Not a DDS");
			}

			if (pStream->Read(&oDDSHeader) == false
				|| oDDSHeader.iSize != sizeof(DDS_HEADER)
				|| oDDSHeader.oPixelFormat.iSize != sizeof(DDS_PIXELFORMAT))
			{
				return ErrorCode(1, "Invalid DDS header");
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
					return ErrorCode(1, "Invalid DDS DX10 header");
				}

				if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_TYPELESS
				|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM
				|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_UINT)
				{
					oDesc.ePixelFormat = PixelFormatEnum::R10G10B10A2_UNORM;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT)
				{
					oDesc.ePixelFormat = PixelFormatEnum::RGBA16_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32_FLOAT )
				{
					oDesc.ePixelFormat = PixelFormatEnum::R32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32G32_FLOAT )
				{
					oDesc.ePixelFormat = PixelFormatEnum::RG32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32G32B32_FLOAT )
				{
					oDesc.ePixelFormat = PixelFormatEnum::RGB32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT )
				{
					oDesc.ePixelFormat = PixelFormatEnum::RGBA32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT )
				{
					oDesc.ePixelFormat = PixelFormatEnum::RGBA16_FLOAT;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
				{
					oDesc.ePixelFormat = PixelFormatEnum::R9G9B9E5_SHAREDEXP;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_SNORM)
				{
					oDesc.ePixelFormat = PixelFormatEnum::BC4;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_SNORM)
				{
					oDesc.ePixelFormat = PixelFormatEnum::BC6H;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_UF16
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_SF16)
				{
					oDesc.ePixelFormat = PixelFormatEnum::BC6H;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_TYPELESS
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_UNORM
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)
				{
					oDesc.ePixelFormat = PixelFormatEnum::BC7;
				}
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BGRA8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RGB8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_B8G8R8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BGR8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::R5G6B5_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BC1;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT2, sizeof(DDS_PIXELFORMAT)) == 0
				  || memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT3, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BC2;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT4, sizeof(DDS_PIXELFORMAT)) == 0
				|| memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DXT5, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BC3;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R16G16B16A16_UNORM.iFourCC)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RGBA16_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R16G16B16A16_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RGBA16_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = PixelFormatEnum::R32_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32G32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RG32_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32G32B32A32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RGBA32_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 8
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::R8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 8
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff)
			{
				oDesc.ePixelFormat = PixelFormatEnum::R8_UNORM;
				//TODO : copy channel R to GBA
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 16
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RG8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 16
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xffff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0) // Declaration seems weird
			{
				oDesc.ePixelFormat = PixelFormatEnum::RG8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff000000)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BGRA8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff000000)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RGBA8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::BGRA8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = PixelFormatEnum::RGBA8_UNORM;
			}

			if (oDesc.ePixelFormat == PixelFormatEnum::_NONE)
			{
				return ErrorCode(1, "Not supported pixel format");
			}

			ErrorCode oErr = pTexture->Create(oDesc);
			if (oErr != ErrorCode::Ok)
			{
				return oErr;
			}

			const PixelFormatInfos& oInfos = PixelFormatEnumInfos[oDesc.ePixelFormat];

			for (int iFace = 0; iFace < oDesc.iFaceCount; ++iFace)
			{
				for (int iMip = 0; iMip < oDesc.iMipCount; ++iMip)
				{
					const Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(iMip, iFace);

					uint32_t iBlockCountX, iBlockCountY;
					PixelFormat::GetBlockCount(oDesc.ePixelFormat, oFaceData.iWidth, oFaceData.iHeight, &iBlockCountX, &iBlockCountY);

					int iBlockCount = iBlockCountX * iBlockCountY;

					size_t iBlocksSize = iBlockCount * oInfos.iBlockSize;

					void* pBlocks = oFaceData.pData;

					if (oFaceData.iSize != iBlocksSize)
						return ErrorCode(2, "Internal : Invalid block size");

					size_t iRowSize = iBlockCountX * oInfos.iBlockSize;

					if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PITCH) != 0)
					{
						size_t iPitchSize = oDDSHeader.iPitchOrLinearSize;
						if (iRowSize > iPitchSize)
						{
							return ErrorCode(2, "Internal : Invalid pitch size");
						}

						size_t iDiff = iPitchSize - iRowSize;
						size_t iRemainingPitch = iPitchSize;
						for (int iLine = 0; iLine < iBlockCountY; ++iLine)
						{
							if (iRowSize > iRemainingPitch)
							{
								pStream->Seek(iRemainingPitch, Core::Stream::SeekModeEnum::OFFSET);
								iRemainingPitch = iPitchSize;
							}

							{
								if (pStream->Read((char*)pBlocks + iLine * iRowSize, iRowSize) != iRowSize)
								{
									return ErrorCode(1, "Incomplete file");
								}
								iRemainingPitch -= iRowSize;
							}
						}
					}
					else if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_LINEARSIZE) != 0)
					{
						/*size_t iLinearSize = oDDSHeader.iPitchOrLinearSize;
						if (iMip == 0 && (iBlockCountY * oInfos.iBlockSize) != iLinearSize)
						{
							return false;
						}*/
						if (pStream->Read(pBlocks, iBlocksSize) != iBlocksSize)
						{
							return ErrorCode(1, "Incomplete file");
						}
					}
					else
					{
						if (pStream->Read(pBlocks, iBlocksSize) != iBlocksSize)
						{
							return ErrorCode(1, "Incomplete file");
						}
					}
				}
			}

			return ErrorCode::Ok;
		}
	}
	//namespace TextureLoader
}
//namespace Graphics